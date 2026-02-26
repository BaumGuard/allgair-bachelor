#include "field.h"

#include "../geometry/line.h"
#include "load_tile.h"
#include "../raytracing/fresnel_zone.h"
#include "../raytracing/selection_methods.h"
#include "tile_types.h"
#include "../status_codes.h"
#include "../raw_data/surface.h"

#include <unistd.h>
#include <iostream>
#include <cmath>
#include <gdal.h>
#include <time.h>


std::vector<std::string> tilesOnRay (
    double x_start, double y_start,
    double x_end, double y_end,
    uint tile_width_km
) {
    // Convert meters to kilometers (used in the tile name)
    x_start /= 1000.0;
    y_start /= 1000.0;
    x_end   /= 1000.0;
    y_end   /= 1000.0;

    // Calculate m and t of the line running through the start and end
    // coordinate
    double m = (double)( y_end - y_start ) / (double)( x_end - x_start );
    double t = y_start - m * x_start;

    double
        x_start_f = x_start,
        x_end_f   = x_end;


    if ( x_end < x_start ) {
        x_start_f = x_end;
        x_end_f   = x_start;
    }

    // Set the x coordinates to the left edge of the tile
    int
        x_start_i = (int) floor( x_start_f ),
        x_end_i   = (int) floor( x_end_f );

    // If the tile width is larger than 1 km, set the coordinates
    // to the closest left edge
    x_start_i -= x_start_i % tile_width_km;
    x_end_i   -= x_end_i % tile_width_km;

    std::string current_tile_name;
    std::vector<std::string> tile_names;

    int
        y_bound1,   // y coordinate where the line starts in the
                    // current interation
        y_bound2;   // y coordinate where the line ends in the
                    // current iteration

    double tmp;

    for ( int i = x_start_i; i < x_end_i; i += tile_width_km ) {
        y_bound1 = (int) floor( m * i + t );
        y_bound2 = (int) floor( m * (i+1) + t );

        y_bound1 -= y_bound1 % tile_width_km;
        y_bound2 -= y_bound2 % tile_width_km;

        // Swap y_bound1 and y_bound2 if the line declines
        if ( y_bound2 < y_bound1 ) {
            tmp = y_bound1;
            y_bound1 = y_bound2;
            y_bound2 = tmp;
        }

        // Add all tiles through which the line runs to the list
        for ( int j = y_bound1; j <= y_bound2; j += tile_width_km ) {
            current_tile_name = buildTileName( i, j );
            tile_names.push_back( current_tile_name );
        }
    }

    if ( x_start < x_end ) {
        current_tile_name = buildTileName(
            (int)floor(x_end) - (int)floor(x_end) % tile_width_km,
            (int)floor(y_end) - (int)floor(y_end) % tile_width_km
        );
        tile_names.push_back( current_tile_name );
    }
    else {
        current_tile_name = buildTileName(
            (int)floor(x_start) - (int)floor(x_start) % tile_width_km,
            (int)floor(y_start) - (int)floor(y_start) % tile_width_km
        );
        tile_names.push_back( current_tile_name );
    }

    return tile_names;
} /* tilesOnRay() */



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
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                pthread_mutex_lock( &dgm_mutex );
                if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                    loadTile( tile_name, tile_type );
                }
                pthread_mutex_unlock( &dgm_mutex );
            }
            break;

        case DOM:
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                pthread_mutex_lock( &dom_mutex );
                if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                    loadTile( tile_name, tile_type );
                }
                pthread_mutex_unlock( &dom_mutex );
            }
            break;

        case DOM_MASKED:
            if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                pthread_mutex_lock( &dom_masked_mutex );
                if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
                    loadTile( tile_name, tile_type );
                }
                pthread_mutex_unlock( &dom_masked_mutex );
            }
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



int Field::bresenhamPseudo3D (
    Vector& start,
    Vector& end,
    float ground_level_threshold,
    std::vector<bool>* decision_arrays_united,
    int tile_type,
    bool cancel_on_ground
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
        x_step = (double)dx / (double)MAX_THREADS,
        y_step = (double)dy / (double)MAX_THREADS,
        z_step = (double)dz / (double)MAX_THREADS;

    double x_start_f = 0.0, y_start_f = 0.0, z_start_f = 0.0;

    bool intersection_found = false;

    for ( int i = 0; i < MAX_THREADS; i++ ) {
        bresenham_data[i].x_start = (int) x_start_f + x_start;
        bresenham_data[i].y_start = (int) y_start_f + y_start;
        bresenham_data[i].z_start = (int) z_start_f + z_start;

        x_start_f += x_step;
        y_start_f += y_step;
        z_start_f += z_step;

        if ( i < MAX_THREADS-1 ) {
            bresenham_data[i].x_end = (int) x_start_f + x_start - 1;
            bresenham_data[i].y_end = (int) y_start_f + y_start - 1;
            bresenham_data[i].z_end = (int) z_start_f + z_start - 1;
        }
        else {
            bresenham_data[i].x_end = (int) x_start_f + x_start;
            bresenham_data[i].y_end = (int) y_start_f + y_start;
            bresenham_data[i].z_end = (int) z_start_f + z_start;
        }

        bresenham_data[i].ground_level_threshold = ground_level_threshold;
        bresenham_data[i].tile_type = tile_type;
        bresenham_data[i].intersection_found = &intersection_found;

        bresenham_data[i].cancel_on_ground = cancel_on_ground;

        bresenham_data[i].h_curve_correction = h_curve_correction;

        decision_arrays[i].clear();
        bresenham_data[i].decision_array = &decision_arrays[i];
        bresenham_data[i].field = this;

        pthread_create( &bresenham_threads[i], NULL, Thread_bresenhamPseudo3D, (void*)&bresenham_data[i] );
    }

    for ( int i = 0; i < MAX_THREADS; i++ ) {
        pthread_join( bresenham_threads[i], NULL );
    }

    for ( uint i = 0; i < MAX_THREADS; i++ ) {
        uint len_decision_array = decision_arrays[i].size();
        for ( uint j = 0; j < len_decision_array; j++ ) {
            decision_arrays_united->push_back( decision_arrays[i][j] );
        }
    }

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


        utm_x = x * GRID_RESOLUTION;
        utm_y = y * GRID_RESOLUTION;

        altitude = z * GRID_RESOLUTION;

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

int Field::precalculate (
    std::vector<Polygon>& selected_polygons,
    Vector& start_point, Vector& end_point,
    int select_method,
    int fresnel_zone, double freq
) {
    std::vector<Polygon> polygons_in_ground_area;
    std::vector<Polygon> global_selected_polygons;


    struct timespec start, end;
    clock_gettime( CLOCK_MONOTONIC, &start );

    Polygon ground_area = fresnelZone( start_point, end_point, fresnel_zone, 868.0e6, 16 );

    clock_gettime( CLOCK_MONOTONIC, &end );
    double time_elapsed = (double)end.tv_sec + (double)end.tv_nsec / 1.0e9;
    time_elapsed -= (double)start.tv_sec + (double)start.tv_nsec / 1.0e9;
    //printf("TIME SEGMENT\n");
    //printf("TIME - Fresnel zone: %.10f\n", time_elapsed);
    fresnel_time = time_elapsed;


    clock_gettime( CLOCK_MONOTONIC, &start );

    getPolygonsInGroundArea( polygons_in_ground_area, ground_area );

    clock_gettime( CLOCK_MONOTONIC, &end );
    time_elapsed = (double)end.tv_sec + (double)end.tv_nsec / 1.0e9;
    time_elapsed -= (double)start.tv_sec + (double)start.tv_nsec / 1.0e9;
    //printf("TIME - PolygonsInGroundArea: %.10f\n", time_elapsed);
    ground_area_time = time_elapsed;



    double part_size = (double)polygons_in_ground_area.size() / (double)MAX_THREADS;

    double start_idx = 0.0;


    clock_gettime( CLOCK_MONOTONIC, &start );

    for ( int i = 0; i < MAX_THREADS; i++ ) {
        precalc_data[i].start_idx = (uint) start_idx;
        start_idx += part_size;
        precalc_data[i].end_idx = (uint) start_idx;

        precalc_data[i].start_point = start_point;
        precalc_data[i].end_point = end_point;

        precalc_data[i].polygons = &polygons_in_ground_area;
        precalc_data[i].selected_polygons = &global_selected_polygons;

        precalc_data[i].selected_polygons_mutex = &selected_polygons_mutex;


        pthread_create( &precalc_threads[i], NULL, Thread_precalculate, (void*)&precalc_data[i] );
    }

    for ( int i = 0; i < MAX_THREADS; i++ ) {
        pthread_join( precalc_threads[i], NULL );
    }

    clock_gettime( CLOCK_MONOTONIC, &end );
    time_elapsed = (double)end.tv_sec + (double)end.tv_nsec / 1.0e9;
    time_elapsed -= (double)start.tv_sec + (double)start.tv_nsec / 1.0e9;
    //printf("TIME - Precalculate: %.10f\n", time_elapsed);
    precalc_time = time_elapsed;

    selected_polygons = global_selected_polygons;

    return SUCCESS;
} /* precalculate() */


void* Thread_precalculate ( void* arg ) {
    struct Precalculate_Thread_Data* data = (struct Precalculate_Thread_Data*) arg;

    for ( uint i = data->start_idx; i < data->end_idx; i++ ) {
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
        if ( tile_polygons[i].getSurfaceType() == GROUND ) {
            continue;
        }

        Vector centroid = tile_polygons[i].getCentroid();

        if (
            centroid.getX() < data->ground_area->getMinX() ||
            centroid.getX() > data->ground_area->getMaxX() ||
            centroid.getY() < data->ground_area->getMinY() ||
            centroid.getY() > data->ground_area->getMaxY()
        ) {
            continue;
        }

        if ( data->ground_area->isPointInPolygon( centroid, true ) ) {
            data->polygon_list->push_back( tile_polygons[i] );
        }
    }

#if 0
    std::string utm_parts [2];
    splitString( data->tile_name, utm_parts, '_' );

    double fresnel_min_y, fresnel_max_y;

    double tile_lower = std::stoi( utm_parts[1] ) * 1000.0;
    double tile_upper = tile_lower + 2000.0;
    double tile_left  = std::stoi( utm_parts[0] ) * 1000.0;
    double tile_right = tile_left  + 2000.0;

    Plane ground_plane;
    ground_plane.createPlaneFromCoordinates( 0.0, 0.0, 1.0, 0.0 );

    Polygon tile_polygon;
    tile_polygon.initPolygonWithPlane( ground_plane );

    tile_polygon.addPoint( Vector(tile_left, tile_lower, 0.0) );
    tile_polygon.addPoint( Vector(tile_left, tile_upper, 0.0) );
    tile_polygon.addPoint( Vector(tile_right, tile_upper, 0.0) );
    tile_polygon.addPoint( Vector(tile_right, tile_lower, 0.0) );

    std::vector<Vector>& fresnel_points = data->ground_area->getPoints();
    uint len_fresnel_points = fresnel_points.size();

    fresnel_min_y = fresnel_points[0].getY();
    fresnel_max_y = fresnel_points[0].getY();


    for ( uint i = 1; i < len_fresnel_points; i++ ) {
        if ( tile_polygon.isPointInPolygon( fresnel_points[i], true ) ) {
            if ( fresnel_points[i].getY() < fresnel_min_y ) {
                fresnel_min_y = fresnel_points[i].getY();
            }
            if ( fresnel_points[i].getY() > fresnel_max_y ) {
                fresnel_max_y = fresnel_points[i].getY();
            }
        }
    }

    fresnel_min_y -= tile_lower;
    fresnel_max_y -= tile_lower;

    printf("MIN_Y %f MAX_Y %f\n", fresnel_min_y, fresnel_max_y);

    uint start_row = (uint)( fresnel_min_y / 100.0 );
    uint end_row =   (uint)( fresnel_max_y / 100.0 );

    printf("START_ROW %d END_ROW %d\n", start_row, end_row);


    uint len_section_starts = section_starts.size();
    printf("LEN_SECTION_STARTS %d\n", len_section_starts);
    for ( uint i = start_row; i <= end_row; i++ ) {
        int section_end;
        if ( i < len_section_starts - 1 ) {
            section_end = section_starts[i+1];
        }
        else {
            section_end = tile_polygons.size();
        }

        uint j = section_starts[i];
        for ( ; j < section_end; j++ ) {
            if ( data->ground_area->isPointInPolygon( tile_polygons[j].getCentroid(), true ) ) {
                //printf("Polygon found\n");
                pthread_mutex_lock( data->polygon_list_mutex );
                data->polygon_list->push_back( tile_polygons[j] );
                pthread_mutex_unlock( data->polygon_list_mutex );
                break;
            }
        }

        j++;

        uint outside_count = 0;
        for ( ; j < section_end; j++ ) {
            if ( data->ground_area->isPointInPolygon( tile_polygons[j].getCentroid(), true ) ) {
                pthread_mutex_lock( data->polygon_list_mutex );
                data->polygon_list->push_back( tile_polygons[j] );
                pthread_mutex_unlock( data->polygon_list_mutex );
            }
            /*
            else if ( outside_count < 1 ) {
                outside_count++;
                //break;
            }
            */
            else {
                break;
            }
        }
    }
#endif
    return NULL;
} /* Thread_getPolygonsInGroundArea() */


int Field::getPolygonsInGroundArea (
    std::vector<Polygon>& polygons,
    Polygon& ground_area
) {
    std::vector<std::string> tile_names = tilesInGroundArea( ground_area );

    int n_tiles = tile_names.size();
    int n_thread_blocks = n_tiles / MAX_THREADS;
    if ( n_tiles < MAX_THREADS ) {
        n_thread_blocks += 1;
    }

    int list_index = 0;


    int n_threads = MAX_THREADS;


    for ( int i = 0; i < MAX_THREADS; i++ ) {
        ground_area_polygons[i].clear();
    }

    for ( int i = 0; i < n_thread_blocks; i++ ) {
        if ( n_tiles / MAX_THREADS > 0 ) {
            n_threads = MAX_THREADS;
        }
        else {
            n_threads = n_tiles;
        }


        for ( int j = 0; j < n_threads; j++ ) {
            ground_area_data[j].field = this;
            ground_area_data[j].ground_area = &ground_area;
            ground_area_data[j].polygon_list = &ground_area_polygons[j];
            ground_area_data[j].tile_name = tile_names[list_index++];

            pthread_create(
                &ground_area_threads[j], NULL,
                Thread_getPolygonsInGroundArea, (void*)&ground_area_data[j]
            );
        }
        for ( int j = 0; j < n_threads; j++ ) {
            pthread_join( ground_area_threads[j], NULL );
        }

    }

    for ( int i = 0; i < MAX_THREADS; i++ ) {
         polygons.insert( polygons.end(), ground_area_polygons[i].begin(), ground_area_polygons[i].end() );
    }

    return SUCCESS;
} /* getPolygonsInGroundArea() */
