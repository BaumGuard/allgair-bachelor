#include "precalc.h"

#include "../status_codes.h"
#include "../geometry/line.h"
#include "../geometry/plane.h"
#include "fresnel_zone.h"
#include "../raytracing/selection_methods.h"

#include <pthread.h>

bool isPolygonInList ( std::vector<Polygon>& polygon_list, Polygon& polygon ) {
    uint len = polygon_list.size();
    if ( len == 0 ) {
        return false;
    }

    for ( uint i = 0; i < len; i++ ) {
        if ( polygon_list[i] != polygon ) {
            return false;
        }
    }
    return true;
}

Polygon getPolygonWithMinDistance ( Vector& start_point, std::vector<Polygon>& polygons ) {
    uint len_polygons = polygons.size();
    double length = ( polygons[0].getCentroid() - start_point ).length();
    double min_length = length;
    uint min_length_index = 0;

    for ( uint i = 1; i < len_polygons; i++ ) {
        length = ( polygons[i].getCentroid() - start_point ).length();
        if ( length < min_length ) {
            min_length = length;
            min_length_index = i;
        }
    }

    return polygons[min_length_index];
}

Polygon getPolygonWithMaxArea ( std::vector<Polygon>& polygons ) {
    uint len_polygons = polygons.size();
    double area = polygons[0].getArea();
    double max_area = area;
    uint max_area_index = 0;

    for ( uint i = 1; i < len_polygons; i++ ) {
        area = polygons[i].getArea();
        if ( area > max_area ) {
            max_area = area;
            max_area_index = i;
        }
    }

    return polygons[max_area_index];
}


pthread_mutex_t selected_polygons_mutex;
int n_threads;
double part_size;
std::vector<Polygon> polygons;
std::vector<Polygon> global_selected_polygons;

Vector _start_point, _end_point;

void* Thread_precalculate ( void* arg ) {
    int thread_nr = *(int*) arg;

    uint
        start_idx = (uint)( (thread_nr-1) * part_size ),
        end_idx   = (uint)( thread_nr * part_size );

    for ( uint i = start_idx; i < end_idx; i++ ) {
        // Calculate the center point (centroid) of the current polygon
        Vector centroid = polygons[i].getCentroid();

        // Create a line between the polygon's centroid and the end point
        Line center_ray;
        center_ray.createLineFromTwoPoints( centroid, _end_point );

        // Create a plane with the end point on it and the direction
        // vector of center_ray being the normal vector
        Plane destination_plane;
        Vector dest_plane_normal_vector = center_ray.getDirectionVector();
        destination_plane.createPlaneFromBaseAndNormalVector(
            _end_point, dest_plane_normal_vector );

        // Get a list of the points of the current polygon
        std::vector<Vector>& points = polygons[i].getPoints();

        // Get the base plane of the current polygon
        Plane polygon_base_plane = polygons[i].getBasePlane();

        // Create a Polygon object for the reflected polygon which
        // is on the plane destination_plane
        Polygon reflected_polygon;
        reflected_polygon.initPolygonWithPlane( destination_plane );

        uint len_points = points.size();
        for ( uint j = 0; j < len_points; j++ ) {
            // Ray from the starting point to the jth point of the polygon
            Line first_ray;
            first_ray.createLineFromTwoPoints( _start_point, points[j] );

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

        polygon_base_plane.reflectLine( center_ray, reflected_center_ray );
        destination_plane.lineIntersection( reflected_center_ray, reflected_center_point );

        // If the intersection with the destination plane of the reflected center ray
        // is inside the reflected polygon add it to the list of the selected polygons
        if (
            reflected_polygon.isPointInPolygon( reflected_center_point )/* &&
            !isPolygonInList(selected_polygons, polygons[i])*/
        ) {
            pthread_mutex_lock( &selected_polygons_mutex );
            global_selected_polygons.push_back( polygons[i] );
            polygons[i].printPolygon();
            pthread_mutex_unlock( &selected_polygons_mutex );
        }
    }

    return NULL;
}


int precalculate (
    std::vector<Polygon>& selected_polygons,
    Vector& start_point, Vector& end_point,
    int select_method,
    int fresnel_zone, double freq, int n_threads
) {
    Polygon ground_area = fresnelZone( start_point, end_point, fresnel_zone, 868.0e6, 16 );
    getPolygonsInGroundArea( polygons, ground_area );

    _start_point = start_point;
    _end_point = end_point;

    part_size = (double)polygons.size() / (double)n_threads;


    pthread_mutex_init( &selected_polygons_mutex, NULL );

    int* thread_idx = new int [n_threads];

    pthread_t* precalc_threads = new pthread_t [n_threads];
    for ( int i = 0; i < n_threads; i++ ) {
        thread_idx[i] = i;
        pthread_create( &precalc_threads[i], NULL, Thread_precalculate, (void*)&thread_idx[i] );
    }

    for ( int i = 0; i < n_threads; i++ ) {
        pthread_join( precalc_threads[i], NULL );
    }

    pthread_mutex_destroy( &selected_polygons_mutex );
    delete[] thread_idx;

#if 0
    uint len_end_points = end_points.size();

    // List to store all the polygons selected by the precalculator
    std::vector<Polygon> selected_polygons;

    for ( uint h = 0; h < len_end_points; h++ ) {
        // Calculate the ground area of the Fresnel zone
        Polygon ground_area = fresnelZone( start_point, end_points[h], fresnel_zone, 868.0e6, 16 );
        // Get all polygons inside the ground area
        std::vector<Polygon> polygons;
        getPolygonsInGroundArea( polygons, ground_area );

        // Iterate over all polygons in the ground area
        uint len_polygons = polygons.size();
        for ( uint i = 0; i < len_polygons; i++ ) {
            // Calculate the center point (centroid) of the current polygon
            Vector centroid = polygons[i].getCentroid();

            // Create a line between the polygon's centroid and the end point
            Line center_ray;
            center_ray.createLineFromTwoPoints( centroid, end_points[h] );

            // Create a plane with the end point on it and the direction
            // vector of center_ray being the normal vector
            Plane destination_plane;
            Vector dest_plane_normal_vector = center_ray.getDirectionVector();
            destination_plane.createPlaneFromBaseAndNormalVector(
                end_points[h], dest_plane_normal_vector );

            // Get a list of the points of the current polygon
            std::vector<Vector>& points = polygons[i].getPoints();

            // Get the base plane of the current polygon
            Plane polygon_base_plane = polygons[i].getBasePlane();

            // Create a Polygon object for the reflected polygon which
            // is on the plane destination_plane
            Polygon reflected_polygon;
            reflected_polygon.initPolygonWithPlane( destination_plane );

            uint len_points = points.size();
            for ( uint j = 0; j < len_points; j++ ) {
                // Ray from the starting point to the jth point of the polygon
                Line first_ray;
                first_ray.createLineFromTwoPoints( start_point, points[j] );

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

            polygon_base_plane.reflectLine( center_ray, reflected_center_ray );
            destination_plane.lineIntersection( reflected_center_ray, reflected_center_point );

            // If the intersection with the destination plane of the reflected center ray
            // is inside the reflected polygon add it to the list of the selected polygons
            if (
                reflected_polygon.isPointInPolygon( reflected_center_point ) &&
                !isPolygonInList(selected_polygons, polygons[i])
            ) {
                    selected_polygons.push_back( polygons[i] );
                    polygons[i].printPolygon();
            }
        }
    }
#endif

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
