#include "polygon.h"

/*---------------------------------------------------------------*/

void Polygon::initPolygonWithPlane ( Plane p ) {
    base_plane = p;
} /* void Polygon::initPolygonWithPlane ( Plane p ) */

/*---------------------------------------------------------------*/

bool Polygon::isPointAlreadyInPolygon ( Vector& p ) {
    int size = points.size();

    Vector current_point;

    for ( int i=0; i<size; i++ ) {
        current_point = points[i];

        if ( current_point == p ) {
            return true;
        }
    }

    return false;
} /* bool Polygon::isPointAlreadyInPolygon ( Vector& p ) */

/*---------------------------------------------------------------*/

int Polygon::addPoint ( Vector point ) {
    if ( !base_plane.isPointOnPlane(point) ) {
        return POINT_NOT_ON_BASE_PLANE;
    }
    if ( isPointAlreadyInPolygon(point) ) {
        return POINT_DUPLICATE;
    }
    points.push_back( point );

    return POINT_ADDED;
} /* int Polygon::addPoint ( Vector point ) */

/*---------------------------------------------------------------*/

bool Polygon::isPointInPolygon ( Vector& p ) {
    if ( !base_plane.isPointOnPlane(p) ) {
        return false;
    }

    Vector dir_vec = base_plane.getVector1();


    Line edge_line;

    Line ray;
    ray.createLineFromBaseAndVector( p, dir_vec );

    Vector p1, p2, intersect;

    int size = points.size();

    int status;
    double factor;

    double
        length_p1_p2,
        length_p1_intersect,
        length_frac;

    int intersect_count = 0;

    for ( int i=0; i<size; i++ ) {
        p1 = points[i];
        p2 = points[(i+1)%size];

        edge_line.createLineFromTwoPoints( p1, p2 );



        status = edge_line.lineIntersect( ray, intersect, &factor );

        if ( status == LINES_INTERSECT && factor > 0.0 ) {
            length_p1_p2 = ( p2 - p1 ).length();
            length_p1_intersect = ( intersect - p1 ).length();

            length_frac = length_p1_intersect / length_p1_p2;

            if ( length_frac > 0.0 && length_frac < 1.0 ) {
                intersect_count++;
            }
        }
    }

    return intersect_count % 2 == 1;
} /* bool Polygon::isPointInPolygon ( Vector& p ) */
