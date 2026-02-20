#include "field.h"

#include "../geometry/line.h"
#include "../geometry/polygon.h"
#include "load_tile.h"
#include "../utils.h"
#include "../precalc/fresnel_zone.h"
#include "../precalc/precalc.h"
#include "../raytracing/selection_methods.h"

#include <unistd.h>
#include <iostream>
#include <cmath>
#include <gdal.h>
#include <time.h>

/*---------------------------------------------------------------*/

Field::Field ( double grid_resolution ) {
    this->grid_resolution = grid_resolution;

    pthread_mutex_init( &dgm_mutex, NULL );
    pthread_mutex_init( &dom_mutex, NULL );
    pthread_mutex_init( &dom_masked_mutex, NULL );

    GDALAllRegister();
} /* Field() */

/*---------------------------------------------------------------*/

int Field::loadTile ( std::string tile_name, int tile_type ) {
    GridTile grid_tile;
    VectorTile vector_tile;

    int status;
    if ( tile_type == DGM || tile_type == DOM || tile_type == DOM_MASKED ) {
        double resample_factor;

        clock_t start, end;

        switch ( tile_type ) {
            case DGM:
                status = getGridTile( grid_tile, tile_name, DGM1 );
                if ( status != SUCCESS ) {
                    return status;
                }
                resample_factor = 1.0 / grid_resolution;
                grid_tile.resampleTile( resample_factor );
                grid_tiles_dgm.insert( {tile_name, grid_tile} );

                break;

            case DOM:
                status = getGridTile( grid_tile, tile_name, DOM20 );
                if ( status != SUCCESS ) {
                    return status;
                }
                resample_factor = 0.2 / grid_resolution;
                grid_tile.resampleTile( resample_factor );
                grid_tiles_dom.insert( {tile_name, grid_tile} );

                break;

            case DOM_MASKED:
                status = getGridTile( grid_tile, tile_name, DOM20_MASKED );
                if ( status != SUCCESS ) {
                    return status;
                }
                resample_factor = 0.2 / grid_resolution;
                grid_tile.resampleTile( resample_factor );
                grid_tiles_dom_masked.insert( {tile_name, grid_tile} );

                break;
        }

        return SUCCESS;
    }
    else if ( tile_type == LOD2 ) {
        status = getVectorTile( vector_tile, tile_name );
        if ( status != SUCCESS ) {
            return status;
        }

        vector_tiles_lod2.insert( {tile_name, vector_tile} );

        return SUCCESS;
    }
    else {
        return TILE_NOT_AVAILABLE;
    }
} /* loadTile() */

/*---------------------------------------------------------------*/

bool Field::tileAlreadyLoaded ( std::string tile_name, int tile_type ) {
    switch ( tile_type ) {
        case DGM:
            return grid_tiles_dgm.contains( tile_name );

        case DOM:
            return grid_tiles_dom.contains( tile_name );

        case DOM_MASKED:
            return grid_tiles_dom_masked.contains( tile_name );

        case LOD2:
            return vector_tiles_lod2.contains( tile_name );

        default:
            return false;
    }
} /* tileAlreadyLoaded() */

/*---------------------------------------------------------------*/

double Field::getAltitudeAtXY ( double x, double y, int tile_type ) {
    uint
        tile_x = (uint)( x / 1000.0 ),
        tile_y = (uint)( y / 1000.0 );

    std::string tile_name = buildTileName( tile_x, tile_y );

    switch ( tile_type ) {
        case DGM:
            pthread_mutex_lock( &dgm_mutex );
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                loadTile( tile_name, tile_type );
            }
            pthread_mutex_unlock( &dgm_mutex );

            break;

        case DOM:
            pthread_mutex_lock( &dom_mutex );
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                loadTile( tile_name, tile_type );
            }
            pthread_mutex_unlock( &dom_mutex );

            break;

        case DOM_MASKED:
            pthread_mutex_lock( &dom_masked_mutex );
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                loadTile( tile_name, tile_type );
            }
            pthread_mutex_unlock( &dom_masked_mutex );

            break;
    }


    GridTile* tile;
    switch ( tile_type ) {
        case DGM:
            tile = &grid_tiles_dgm[tile_name];
            break;

        case DOM:
            tile = &grid_tiles_dom[tile_name];
            break;

        case DOM_MASKED:
            tile = &grid_tiles_dom_masked[tile_name];
            break;
    }

    uint
        easting  = (uint)( (fmod(x, 1000.0) / 1000.0) * tile->getTileWidth() ),
        northing = (uint)( (fmod(y, 1000.0) / 1000.0) * tile->getTileWidth() );

    float value;
    tile->getValue( easting, northing, value );

    return value;
} /* getAltitudeAtXY () */

/*---------------------------------------------------------------*/

struct Bresenham_Thread_Data {
    int
        x_start, x_end,
        y_start, y_end,
        z_start, z_end;

    float ground_level_threshold;
    int tile_type;
    bool* intersection_found;

    bool cancel_on_ground;

    double grid_resolution;

    double h_curve_correction;

    std::vector<bool>* decision_array;

    Field* field;
};

int Field::bresenhamPseudo3D (
    Vector& start,
    Vector& end,
    float ground_level_threshold,
    std::vector<bool>* decision_arrays_united,
    int tile_type,
    bool cancel_on_ground,
    int n_threads
)
{
    if ( tile_type != DGM && tile_type != DOM && tile_type != DOM_MASKED ) {
        return INVALID_TILE_TYPE;
    }

    // Cast start and end values to integers
    int
        x_start = (int) ( start.getX() / grid_resolution ),
        y_start = (int) ( start.getY() / grid_resolution ),
        z_start = (int) ( round( start.getZ() ) / grid_resolution ),
        x_end   = (int) ( end.getX() / grid_resolution ),
        y_end   = (int) ( end.getY() / grid_resolution ),
        z_end   = (int) ( round( end.getZ() ) / grid_resolution );

    // Distances between the start and end coordinate
    int
        dx = abs( x_end - x_start ),
        dy = abs( y_end - y_start ),
        dz = abs( z_end - z_start );

    // Altitude correction
    double line_length_2d = sqrt( dx*dx + dy*dy );
    double h_curve_correction =
        EARTH_RADIUS_EFFECTIVE -
        sqrt(
            EARTH_RADIUS_EFFECTIVE * EARTH_RADIUS_EFFECTIVE -
            line_length_2d * line_length_2d
        );

    // Length of the ray parts to perform the Bresenham algorithm on
    double
        x_step = (double)dx / (double)n_threads,
        y_step = (double)dy / (double)n_threads,
        z_step = (double)dz / (double)n_threads;

    double x_start_f = 0.0, y_start_f = 0.0, z_start_f = 0.0;

    std::vector<bool>* decision_arrays = new std::vector<bool> [n_threads];

    pthread_t* threads = new pthread_t [n_threads];
    struct Bresenham_Thread_Data* data = new Bresenham_Thread_Data [n_threads];

    bool intersection_found = false;

    struct timespec start_time, finish;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for ( int i = 0; i < n_threads; i++ ) {
        data[i].x_start = (int) x_start_f + x_start;
        data[i].y_start = (int) y_start_f + y_start;
        data[i].z_start = (int) z_start_f + z_start;

        x_start_f += x_step;
        y_start_f += y_step;
        z_start_f += z_step;

        if ( i < n_threads-1 ) {
            data[i].x_end = (int) x_start_f + x_start - 1;
            data[i].y_end = (int) y_start_f + y_start - 1;
            data[i].z_end = (int) z_start_f + z_start - 1;
        }
        else {
            data[i].x_end = (int) x_start_f + x_start;
            data[i].y_end = (int) y_start_f + y_start;
            data[i].z_end = (int) z_start_f + z_start;
        }

        data[i].ground_level_threshold = ground_level_threshold;
        data[i].tile_type = tile_type;
        data[i].intersection_found = &intersection_found;
        data[i].cancel_on_ground = cancel_on_ground;

        data[i].grid_resolution = grid_resolution;
        data[i].h_curve_correction = h_curve_correction;

        data[i].decision_array = &decision_arrays[i];
        data[i].field = this;

        pthread_create( &threads[i], NULL, Thread_bresenhamPseudo3D, (void*)&data[i] );
    }

    for ( int i = 0; i < n_threads; i++ ) {
        pthread_join( threads[i], NULL );
    }

    for ( uint i = 0; i < n_threads; i++ ) {
        uint len_decision_array = decision_arrays[i].size();
        for ( uint j = 0; j < len_decision_array; j++ ) {
            decision_arrays_united->push_back( decision_arrays[i][j] );
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    double elapsed = (finish.tv_sec - start_time.tv_sec);
    elapsed += (finish.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    printf("Bresenham execution time: %f s\n", elapsed);

    delete[] decision_arrays;
    delete[] threads;
    delete[] data;

    if ( intersection_found ) {
        return INTERSECTION_FOUND;
    }
    return NO_INTERSECTION_FOUND;
} /* bresenhamPseudo3D() */


void* Thread_bresenhamPseudo3D ( void* arg ) {
    clock_t start, end;
    start = clock();
    Bresenham_Thread_Data* data = (Bresenham_Thread_Data*) arg;

    // Cast start and end values to integers
    int
        x_start = data->x_start,
        y_start = data->y_start,
        z_start = data->z_start,
        x_end   = data->x_end,
        y_end   = data->y_end,
        z_end   = data->z_end;

    // Distances between the start and end coordinate
    int
        dx = abs( x_end - x_start ),
        dy = abs( y_end - y_start ),
        dz = abs( z_end - z_start );

    double utm_x, utm_y, altitude;

    // Axes
    enum IterationAxes {
        ITERATION_AXIS_X,
        ITERATION_AXIS_Y,
        ITERATION_AXIS_Z
    };

    // Find the largest distance
    // The corresponding axis will become the iteration axis
    int axis;
    if ( dx > dy && dx > dz ) axis = ITERATION_AXIS_X;
    else if ( dy > dz ) axis = ITERATION_AXIS_Y;
    else axis = ITERATION_AXIS_Z;

    int
        e1, e2,                // Bresenham error (if 0 increment/decrement dep1/dep2)
        it1, it2,              // Iterators to subtract from e1/e2
        corr,                  // Correction value to add to e1/e2 when they become negative
        start_it, end_it,      // Start/End value on the iteration axis
        dep1, dep2,            // Values dependant on the iteration axis (the other two axes)
        start_dep1, end_dep1,  // Start/End value on the first dependant axis
        start_dep2, end_dep2;  // Start/End value on the second dependant axis


    int x, y, z;

    // Map the x, y and z values to iteration and dependant coordinates
    switch ( axis ) {
        case ITERATION_AXIS_X:
            e1          = dx;
            e2          = dx;
            start_it    = x_start;
            end_it      = x_end;
            start_dep1  = y_start;
            end_dep1    = y_end;
            start_dep2  = z_start;
            end_dep2    = z_end;
            it1         = 2 * dy;
            it2         = 2 * dz;
            corr        = 2 * dx;
            dep1        = y_start;
            dep2        = z_start;
            break;

        case ITERATION_AXIS_Y:
            e1          = dy;
            e2          = dy;
            start_it    = y_start;
            end_it      = y_end;
            start_dep1  = x_start;
            end_dep1    = x_end;
            start_dep2  = z_start;
            end_dep2    = z_end;
            it1         = 2 * dx;
            it2         = 2 * dz;
            corr        = 2 * dy;
            dep1        = x_start;
            dep2        = z_start;
            break;

        case ITERATION_AXIS_Z:
            e1          = dz;
            e2          = dz;
            start_it    = z_start;
            end_it      = z_end;
            start_dep1  = x_start;
            end_dep1    = x_end;
            start_dep2  = y_start;
            end_dep2    = y_end;
            it1         = 2 * dx;
            it2         = 2 * dy;
            corr        = 2 * dz;
            dep1        = x_start;
            dep2        = y_start;
            break;
    } /* switch ( axis ) */


    int it = start_it;

    // Find an intersection between the ray and the ground
    // using Bresenham's algorithm modified for 3D
    while ( it != end_it ) {
        if ( data->cancel_on_ground && (*data->intersection_found) ) {
            break;
        }

        e1 -= it1;
        e2 -= it2;

        if ( e1 < 0 ) {
            if ( start_dep1 < end_dep1 ) {
                dep1++;
            }
            else {
                dep1--;
            }
            e1 += corr;
        }

        if ( e2 < 0 ) {
            if ( start_dep2 < end_dep2 ) {
                dep2++;
            }
            else {
                dep2--;
            }
            e2 += corr;
        }


        if ( start_it < end_it ) {
            it++;
        }
        else {
            it--;
        }


        // Map back to x, y and z values
        switch ( axis ) {
            case ITERATION_AXIS_X:
                x = it;
                y = dep1;
                z = dep2;
                break;

            case ITERATION_AXIS_Y:
                x = dep1;
                y = it;
                z = dep2;
                break;

            case ITERATION_AXIS_Z:
                x = dep1;
                y = dep2;
                z = it;
                break;
        } /* switch ( axis ) */


        utm_x = x * data->grid_resolution;
        utm_y = y * data->grid_resolution;

        altitude = z * data->grid_resolution;

        // Get the altitude at the current x/y position
        float altitude_at_xy =
            data->field->getAltitudeAtXY(utm_x, utm_y, data->tile_type) - data->ground_level_threshold;

        // If the value of z is equal or smaller than the altitude
        // at x/y they ray has hit the ground
        if ( altitude - data->h_curve_correction <= altitude_at_xy ) {
            data->decision_array->push_back( true );
            *(data->intersection_found) = true;
        } /* if ( z <= altitude_at_xy ) */
        else {
            data->decision_array->push_back( false );
        }
    } /* while ( it != end_it ) */

    end = clock();

    return NULL;
} /* Thread_bresenhamPseudo3D() */

/*---------------------------------------------------------------*/

struct Precalculate_Thread_Data {
    Vector start_point;
    Vector end_point;

    uint start_idx;
    uint end_idx;

    std::vector<Polygon>* polygons;
    std::vector<Polygon>* selected_polygons;

    pthread_mutex_t* selected_polygons_mutex;
};

int Field::precalculate (
    std::vector<Polygon>& selected_polygons,
    Vector& start_point, Vector& end_point,
    int select_method,
    int fresnel_zone, double freq
) {
    std::vector<Polygon> polygons_in_ground_area;
    std::vector<Polygon> global_selected_polygons;

    Polygon ground_area = fresnelZone( start_point, end_point, fresnel_zone, 868.0e6, 16 );
    getPolygonsInGroundArea( polygons_in_ground_area, ground_area );

    double part_size = (double)polygons_in_ground_area.size() / (double)MAX_THREADS;

    pthread_mutex_t selected_polygons_mutex;
    pthread_mutex_init( &selected_polygons_mutex, NULL );

    struct Precalculate_Thread_Data* data =
        new struct Precalculate_Thread_Data [MAX_THREADS];

    double start_idx = 0.0;

    pthread_t* precalc_threads = new pthread_t [MAX_THREADS];
    for ( int i = 0; i < MAX_THREADS; i++ ) {
        data[i].start_idx = (uint) start_idx;
        start_idx += part_size;
        data[i].end_idx = (uint) start_idx;

        data[i].start_point = start_point;
        data[i].end_point = end_point;

        data[i].polygons = &polygons_in_ground_area;
        data[i].selected_polygons = &global_selected_polygons;

        data[i].selected_polygons_mutex = &selected_polygons_mutex;


        pthread_create( &precalc_threads[i], NULL, Thread_precalculate, (void*)&data[i] );
    }

    for ( int i = 0; i < MAX_THREADS; i++ ) {
        pthread_join( precalc_threads[i], NULL );
    }

    pthread_mutex_destroy( &selected_polygons_mutex );
    delete[] data;
    delete[] precalc_threads;

    if ( global_selected_polygons.size() > 0 ) {
        switch ( select_method ) {
            case BY_MIN_DISTANCE:
                selected_polygons.push_back(
                    getPolygonWithMinDistance( start_point, global_selected_polygons )
                );
                return SUCCESS;

            case BY_MAX_AREA:
                selected_polygons.push_back( getPolygonWithMaxArea( global_selected_polygons ) );
                return SUCCESS;

            case ALL:
                selected_polygons = global_selected_polygons;
                return SUCCESS;
        }
    }

    return NO_POLYGON_FOUND;
} /* precalculate() */


void* Thread_precalculate ( void* arg ) {
    struct Precalculate_Thread_Data* data = (struct Precalculate_Thread_Data*) arg;

    for ( uint i = data->start_idx; i < data->end_idx; i++ ) {
        // Ignore ground surface (only use walls and roofs)
        if ( (*data->polygons)[i].getSurfaceType() == GROUND ) {
            continue;
        }

        // Get the center point (centroid) of the current polygon
        Vector centroid = (*data->polygons)[i].getCentroid();

        // Create a line between the polygon's centroid and the end point
        Line center_ray;
        center_ray.createLineFromTwoPoints( centroid, data->end_point );

        // Create a plane with the end point on it and the direction
        // vector of center_ray being the normal vector
        Plane destination_plane;
        Vector dest_plane_normal_vector = center_ray.getDirectionVector();
        destination_plane.createPlaneFromBaseAndNormalVector(
            data->end_point, dest_plane_normal_vector );

        // Get a list of the points of the current polygon
        std::vector<Vector>& points = (*data->polygons)[i].getPoints();

        // Get the base plane of the current polygon
        Plane polygon_base_plane = (*data->polygons)[i].getBasePlane();

        // Create a Polygon object for the reflected polygon which
        // is on the plane destination_plane
        Polygon reflected_polygon;
        reflected_polygon.initPolygonWithPlane( destination_plane );

        uint len_points = points.size();
        for ( uint j = 0; j < len_points; j++ ) {
            // Ray from the starting point to the jth point of the polygon
            Line first_ray;
            first_ray.createLineFromTwoPoints( data->start_point, points[j] );

            // Reflect first_ray on the polygon's base plane
            Line second_ray;
            polygon_base_plane.reflectLine( first_ray, second_ray );

            // Find the intersection between the reflected ray and the
            // destination plane
            // This will become a corner of the polygon
            Vector dest_intersect;
            destination_plane.lineIntersection( second_ray, dest_intersect );

            reflected_polygon.addPoint( dest_intersect );
        }

        // Reflect the center ray on the polygon's base plane and find the intersection
        // with the destination plane
        Vector reflected_center_point;
        Line reflected_center_ray;

        //polygon_base_plane.reflectLine( center_ray, reflected_center_ray );
        //destination_plane.lineIntersection( reflected_center_ray, reflected_center_point );

        // If the end point is located inside the reflection polygon, add the polygon to
        // the list of the selected polygons
        if (
            reflected_polygon.isPointInPolygon( data->end_point )/* &&
            !isPolygonInList(selected_polygons, polygons[i])*/
        ) {
            pthread_mutex_lock( data->selected_polygons_mutex );
            data->selected_polygons->push_back( (*data->polygons)[i] );
            pthread_mutex_unlock( data->selected_polygons_mutex );
        }
    }

    return NULL;
} /* Thread_precalculate() */

/*---------------------------------------------------------------*/

struct PolygonsInGroundArea_Thread_Data {
    Field* field;
    Polygon* ground_area;

    std::string tile_name;
    pthread_mutex_t* polygon_list_mutex;
    std::vector<Polygon>* polygon_list;
};


void* Thread_getPolygonsInGroundArea ( void* arg ) {
    struct PolygonsInGroundArea_Thread_Data* data =
        (struct PolygonsInGroundArea_Thread_Data*) arg;

    if ( !data->field->tileAlreadyLoaded( data->tile_name, LOD2 ) ) {
        data->field->loadTile( data->tile_name, LOD2 );
    }
    VectorTile& vector_tile = data->field->vector_tiles_lod2[data->tile_name];

    uint len_polygons;

    Vector test_point;

    std::vector<Polygon>& tile_polygons = vector_tile.getPolygons();
    std::vector<uint>& section_starts = vector_tile.getSectionStarts();
    len_polygons = tile_polygons.size();

    for ( uint i = 0; i < len_polygons; i++ ) {
        if ( data->ground_area->isPointInPolygon( tile_polygons[i].getCentroid(), true ) ) {
            pthread_mutex_lock( data->polygon_list_mutex );
            data->polygon_list->push_back( tile_polygons[i] );
            pthread_mutex_unlock( data->polygon_list_mutex );
        }
    }

    return NULL;
} /* Thread_getPolygonsInGroundArea() */


int Field::getPolygonsInGroundArea (
    std::vector<Polygon>& polygons,
    Polygon& ground_area
) {
    std::vector<std::string> tile_names = tilesInGroundArea( ground_area );

    std::vector<Polygon> polygon_list;

    int n_tiles = tile_names.size();
    int n_thread_blocks = n_tiles / MAX_THREADS;
    if ( n_tiles < MAX_THREADS ) {
        n_thread_blocks += 1;
    }

    int list_index = 0;

    pthread_mutex_t polygon_list_mutex;
    pthread_mutex_init( &polygon_list_mutex, NULL );

    int n_threads = MAX_THREADS;



    for ( int i = 0; i < n_thread_blocks; i++ ) {
        if ( n_tiles / MAX_THREADS > 0 ) {
            n_threads = MAX_THREADS;
        }
        else {
            n_threads = n_tiles;
        }

        pthread_t* threads = new pthread_t [n_threads];
        struct PolygonsInGroundArea_Thread_Data* data =
            new struct PolygonsInGroundArea_Thread_Data [n_threads];


        for ( int j = 0; j < n_threads; j++ ) {
            data[j].field = this;
            data[j].ground_area = &ground_area;
            data[j].polygon_list = &polygon_list;
            data[j].tile_name = tile_names[list_index++];
            data[j].polygon_list_mutex = &polygon_list_mutex;

            pthread_create(
                &threads[j], NULL,
                Thread_getPolygonsInGroundArea, (void*)&data[j]
            );
        }
        for ( int j = 0; j < n_threads; j++ ) {
            pthread_join( threads[j], NULL );
        }


        delete[] threads;
        delete[] data;
    }
    polygons = polygon_list;

    return SUCCESS;
} /* getPolygonsInGroundArea() */
