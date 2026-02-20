#include "precalc.h"

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
} /* isPolygonInList() */

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
} /* getPolygonWithMinDistance() */

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
} /* getPolygonWithMaxArea() */
