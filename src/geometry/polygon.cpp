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

bool Polygon::isPointAlreadyInPolygon ( Vector& p ) const {
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

    return SUCCESS;
} /* addPoint() */

/*---------------------------------------------------------------*/

bool Polygon::isPointInPolygon ( Vector& p ) const {
    if ( !base_plane.isPointOnPlane(p) ) {
        return false;
    }

    Vector dir_vec = base_plane.getVector1();


    Line edge_line;

    // Create a ray from the probing point and one of the
    // direction vectors of the polygon's base plane
    Line ray;
    ray.createLineFromBaseAndVector( p, dir_vec );

    Vector p1, p2, intersect;

    int status;
    double factor;

    double
        length_p1_p2,
        length_p1_intersect,
        length_intersect_p2;

    int intersect_count = 0;

    int size = points.size();

    // Create edge lines for every pair of adjacent points and try
    // to intersect it with the ray
    // If the number of intersections of the ray with the edge lines
    // is odd, the point is inside the polygon and if it's even it is
    // located outside of it.
    for ( int i=0; i<size; i++ ) {
        p1 = points[i];
        p2 = points[(i+1)%size];

        edge_line.createLineFromTwoPoints( p1, p2 );

        status = edge_line.lineIntersect( ray, intersect, &factor );

        if ( status == INTERSECTION_FOUND /*&& factor > 0.0*/ ) {
            // Check if the intersection is located between the two
            // adjacent points
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

Plane Polygon::getBasePlane () const {
    return base_plane;
} /* getBasePlane() */

/*---------------------------------------------------------------*/

std::vector<Vector> Polygon::getPoints () const {
    return points;
} /* getPoints() */

/*---------------------------------------------------------------*/

int Polygon::getSurfaceType () const {
    return surface_type;
} /* getSurfaceType () */

/*---------------------------------------------------------------*/

void Polygon::setSurfaceType ( int surface_type ) {
    this->surface_type = surface_type;
} /* setSurfaceType () */

/*---------------------------------------------------------------*/

int Polygon::lineIntersection ( Line& l, Vector& intersect ) const {
    Vector plane_intersect;
    int status = base_plane.lineIntersection( l, plane_intersect );


    if ( status != INTERSECTION_FOUND ) {
        return NO_INTERSECTION_FOUND;
    }

    if ( !isPointInPolygon(plane_intersect) ) {
        return NO_INTERSECTION_FOUND;
    }

    intersect = plane_intersect;
    return INTERSECTION_FOUND;
} /* lineIntersection() */
