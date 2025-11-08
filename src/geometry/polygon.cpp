#include "polygon.h"

#include "line.h"
#include "../status_codes.h"

#include <iostream>

/*---------------------------------------------------------------*/

void Polygon::initPolygonWithPlane ( Plane p ) {
    base_plane = p;
    initialized = true;
} /* initPolygonWithPlane() */

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
} /* isPointAlreadyInPolygon() */

/*---------------------------------------------------------------*/

int Polygon::addPoint ( Vector point ) {

    if ( !initialized ) {
        return POLYGON_NOT_INITIALIZED;
    }
    if ( !base_plane.isPointOnPlane(point) ) {
        return POINT_NOT_ON_BASE_PLANE;
    }

    if ( isPointAlreadyInPolygon(point) ) {
        return POINT_DUPLICATE;
    }

    points.push_back( point );

    return POINT_ADDED;
} /* addPoint() */

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
        length_intersect_p2,
        length_frac;

    int intersect_count = 0;

    for ( int i=0; i<size; i++ ) {
        p1 = points[i];
        p2 = points[(i+1)%size];

        edge_line.createLineFromTwoPoints( p1, p2 );

        status = edge_line.lineIntersect( ray, intersect, &factor );

        if ( status == LINES_INTERSECT /*&& factor > 0.0*/ ) {
            length_p1_p2 = ( p2 - p1 ).length();
            length_p1_intersect = ( intersect - p1 ).length();
            length_intersect_p2 = ( p2 - intersect ).length();

            float test = ( intersect.getX() - p.getX() ) / dir_vec.getX();

            if ( length_p1_intersect + length_intersect_p2 == length_p1_p2 && test >= 0.0 ) {
                intersect_count++;
            }
        }
    }

    return intersect_count % 2 == 1;

} /* isPointInPolygon() */

/*---------------------------------------------------------------*/

Plane Polygon::getBasePlane () {
    return base_plane;
} /* getBasePlane() */

/*---------------------------------------------------------------*/

std::vector<Vector> Polygon::getPoints () {
    return points;
} /* getPoints() */

/*---------------------------------------------------------------*/

int Polygon::lineIntersection ( Line& l, Vector& intersect ) {
    Vector plane_intersect;
    int status = base_plane.lineIntersection( l, plane_intersect );


    if ( status != LINE_INTERSECTS_PLANE ) {
        return POLYGON_LINE_NO_INTERSECT;
    }

    if ( !isPointInPolygon(plane_intersect) ) {
        return POLYGON_LINE_NO_INTERSECT;
    }

    intersect = plane_intersect;
    return LINE_INTERSECTS_POLYGON;
} /* lineIntersection() */
