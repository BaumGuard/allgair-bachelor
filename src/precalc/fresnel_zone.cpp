#include "fresnel_zone.h"

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../utils.h"
#include "../status_codes.h"
#include "../raw_data/gmlfile.h"
#include "../tile/vector_tile.h"
#include "../tile/load_tile.h"

#include <cmath>
#include <pthread.h>

#define LIGHT_SPEED 300000000.0

int MAX_THREADS;

/*---------------------------------------------------------------*/

Polygon fresnelZone (
    Vector& start_point, Vector& end_point,
    int nth_zone,
    double freq,
    uint n_samples
) {
    Vector diff = end_point - start_point;
    Vector ellipse_center = start_point + diff / 2.0;
    double distance = diff.length();

    double radius = sqrt( (nth_zone*LIGHT_SPEED/freq*distance) / 4.0 );

    double e = distance / 2.0;
    double b = radius;
    double a = sqrt( b*b + e*e );

    double step = 4.0 * a / (double)n_samples;

    Polygon ground_area;
    Plane base_plane;
    base_plane.createPlaneFromCoordinates( 0.0, 0.0, 1.0, 0.0 );
    ground_area.initPolygonWithPlane( base_plane );

    for ( double x = -a; x < a; x += step ) {
        double y = b / a * sqrt( a*a - x*x );
        Vector sample( x, y, 0.0 );
        ground_area.addPoint( sample );
    }
    for ( double x = a; x > -a; x -= step ) {
        double y = -b / a * sqrt( a*a - x*x );
        Vector sample( x, y, 0.0 );
        ground_area.addPoint( sample );
    }

    double
        delta_x = diff.getX(),
        delta_y = diff.getY();

    double rotation_angle = atan2( delta_y, delta_x );

    std::vector<Vector>& points = ground_area.getPoints();
    uint len = points.size();
    double x, y;
    for ( uint i = 0; i < len; i++ ) {
        x = points[i].getX() * cos(rotation_angle) -
            points[i].getY() * sin(rotation_angle);

        y = points[i].getX() * sin(rotation_angle) +
            points[i].getY() * cos(rotation_angle);

        x += ellipse_center.getX();
        y += ellipse_center.getY();

        points[i].setX( x );
        points[i].setY( y );
    }

    return ground_area;
} /* fresnelZone() */


/*---------------------------------------------------------------*/

bool listContains ( std::vector<std::string>& list, std::string str ) {
    uint len = list.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( list[i] == str ) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> tilesInGroundArea ( Polygon& ground_area ) {
    std::vector<Vector> ground_area_points = ground_area.getPoints();
    double
        min_utmx = ground_area_points[0].getX(),
        max_utmx = ground_area_points[0].getX(),
        min_utmy = ground_area_points[0].getY(),
        max_utmy = ground_area_points[0].getY();

    uint len = ground_area_points.size();
    for ( uint i = 1; i < len; i++ ) {
        if ( ground_area_points[i].getX() < min_utmx ) {
            min_utmx = ground_area_points[i].getX();
        }
        if ( ground_area_points[i].getX() > max_utmx ) {
            max_utmx = ground_area_points[i].getX();
        }
        if ( ground_area_points[i].getY() < min_utmy ) {
            min_utmy = ground_area_points[i].getY();
        }
        if ( ground_area_points[i].getY() > max_utmy ) {
            max_utmy = ground_area_points[i].getY();
        }
    }

    min_utmx -= fmod( min_utmx, 2000.0 );
    max_utmx -= fmod( max_utmx, 2000.0 );
    min_utmy -= fmod( min_utmy, 2000.0 );
    max_utmy -= fmod( max_utmy, 2000.0 );

    Vector
        coord_lower_left,
        coord_lower_right,
        coord_upper_left,
        coord_upper_right;

    std::vector<std::string> tiles_in_ground_area;


    uint utmx_km, utmy_km;

    for ( double utmy = min_utmy; utmy <= max_utmy; utmy += 2000.0 ) {
        for ( double utmx = min_utmx; utmx <= max_utmx; utmx += 2000.0 ) {
            coord_lower_left  = Vector( utmx,        utmy,        0.0 );
            coord_lower_right = Vector( utmx+2000.0, utmy,        0.0 );
            coord_upper_left  = Vector( utmx,        utmy+2000.0, 0.0 );
            coord_upper_right = Vector( utmx+2000.0, utmy+2000.0, 0.0 );

            if (
                ground_area.isPointInPolygon( coord_lower_left,  true )  ||
                ground_area.isPointInPolygon( coord_lower_right, true )  ||
                ground_area.isPointInPolygon( coord_upper_left,  true )  ||
                ground_area.isPointInPolygon( coord_upper_right, true )
            ) {
                utmx_km = (uint)( utmx / 1000.0 ),
                utmy_km = (uint)( utmy / 1000.0 );

                std::string tile_name = buildTileName( utmx_km, utmy_km );
                tiles_in_ground_area.push_back( tile_name );
            }
        }
    }

    len = ground_area_points.size();
    for ( uint i = 0; i < len; i++ ) {
        utmx_km = (uint) ground_area_points[i].getX(),
        utmy_km = (uint) ground_area_points[i].getY();

        utmx_km = ( utmx_km - utmx_km % 2000 ) / 1000;
        utmy_km = ( utmy_km - utmy_km % 2000 ) / 1000;

        std::string tile_name = buildTileName( utmx_km, utmy_km );

        if ( !listContains( tiles_in_ground_area, tile_name ) ) {
            tiles_in_ground_area.push_back( tile_name );
        }
    }

    return tiles_in_ground_area;
} /* tilesInGroundArea() */

/*---------------------------------------------------------------*/

Polygon* ground_area_global;
std::vector<Polygon>* polygon_list;
pthread_mutex_t polygon_list_mutex;

void* Thread_getPolygonsInGroundArea ( void* arg ) {
    std::string* tile_name = (std::string*) arg;

    VectorTile vector_tile;

    uint len_polygons;

    Vector test_point;


    getVectorTile( vector_tile, *tile_name );

    std::vector<Polygon>& tile_polygons = vector_tile.getPolygons();
    len_polygons = tile_polygons.size();

    for ( uint j = 0; j < len_polygons; j++ ) {
        std::vector<Vector>& points = tile_polygons[j].getPoints();

        uint len_points = points.size();
        if ( len_points > 0 ) {
            for ( uint k = 0; k < len_points; k++ ) {
                test_point = points[k];
                test_point.setZ( 0.0 );

                if ( ground_area_global->isPointInPolygon(test_point, true) ) {
                    pthread_mutex_lock( &polygon_list_mutex );
                    polygon_list->push_back( tile_polygons[j] );
                    pthread_mutex_unlock( &polygon_list_mutex );
                    break;
                }
            }

        }

    }

    return NULL;
}

int getPolygonsInGroundArea (
    std::vector<Polygon>& polygons,
    Polygon& ground_area,
    int n_threads
) {
    std::vector<std::string> tile_names = tilesInGroundArea( ground_area );

    int n_tiles = tile_names.size();
    int n_thread_blocks = n_tiles / MAX_THREADS + 1;

    int list_index = 0;

    polygon_list = &polygons;
    ground_area_global = &ground_area;
    pthread_mutex_init( &polygon_list_mutex, NULL );
    MAX_THREADS = n_threads;

    for ( int i = 0; i < n_thread_blocks; i++ ) {
        if ( n_tiles / MAX_THREADS > 0 ) {
            n_threads = MAX_THREADS;
        }
        else {
            n_threads = n_tiles;
        }


        pthread_t* threads = new pthread_t [n_threads];

        for ( int j = 0; j < n_threads; j++ ) {
            pthread_create(
                &threads[j], NULL,
                Thread_getPolygonsInGroundArea, (void*)&tile_names[list_index]
            );
            list_index++;
        }
        for ( int j = 0; j < n_threads; j++ ) {
            pthread_join( threads[j], NULL );
        }

        delete[] threads;
    }

    /*
    VectorTile vector_tile;

    uint len_tiles = tile_names.size();
    uint len_polygons;
    int status;

    Vector test_point;

    for ( uint i = 0; i < len_tiles; i++ ) {
        status = getVectorTile( vector_tile, tile_names[i] );
        if ( status != SUCCESS ) {
            return status;
        }

        std::vector<Polygon>& tile_polygons = vector_tile.getPolygons();
        len_polygons = tile_polygons.size();

        for ( uint j = 0; j < len_polygons; j++ ) {
            std::vector<Vector>& points = tile_polygons[j].getPoints();

            uint len_points = points.size();
            if ( len_points > 0 ) {
                for ( uint k = 0; k < len_points; k++ ) {
                    test_point = points[k];
                    test_point.setZ( 0.0 );

                    if ( ground_area.isPointInPolygon(test_point) ) {
                        polygons.push_back( tile_polygons[j] );
                        break;
                    }
                }

            }

        }
    }
    */

    return SUCCESS;
} /* getPolygonsInGroundArea() */
