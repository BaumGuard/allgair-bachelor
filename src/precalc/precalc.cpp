#include "precalc.h"

#include "../geometry/line.h"
#include "../geometry/plane.h"
#include "fresnel_zone.h"

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

Polygon precalculate (
    Vector& start_point, std::vector<Vector>& end_points,
    int select_method,
    int fresnel_zone, double freq
) {
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

    switch ( select_method ) {
        case BY_MIN_DISTANCE:
            return getPolygonWithMinDistance( start_point, selected_polygons );

        case BY_MAX_AREA:
            return getPolygonWithMaxArea( selected_polygons );
    }

    return selected_polygons[0];
} /* precalculate() */
