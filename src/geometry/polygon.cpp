#include "polygon.h"

#include "line.h"
#include "../status_codes.h"

#include <iostream>
#include <cmath>

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
#if 0
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

        if ( status == INTERSECTION_FOUND && factor > 0.0 ) {
            // Check if the intersection is located between the two
            // adjacent points
            length_p1_p2 = ( p2 - p1 ).length();
            length_p1_intersect = ( intersect - p1 ).length();
            length_intersect_p2 = ( p2 - intersect ).length();



            //double test = ( intersect.getX() - p.getX() ) / dir_vec.getX();

            if ( length_p1_intersect + length_intersect_p2 == length_p1_p2 /*&& test >= 0.0*/ ) {

                p1.printVector();
                p2.printVector();
                intersect.printVector();
                printf("LENGTHS %f %f %f\n", length_p1_p2, length_p1_intersect, length_intersect_p2);

                edge_line.printLine();
                intersect_count++;
            }
        }
    }
    if ( intersect_count % 2 == 1 ) {printf("INTERSECTION\n"); ray.printLine();}
    return intersect_count % 2 == 1;

} /* isPointInPolygon() */
#endif


bool Polygon::isPointInPolygon ( Vector& p ) const {
    if ( !base_plane.isPointOnPlane(p) ) {
        return false;
    }

    Vector normal_vector = base_plane.normalVector();

    std::vector<Vector> mapped_points;

    double alpha = -atan2( normal_vector.getY(), normal_vector.getX() );
    Vector nv_rot = p.rotateVector( alpha, 0.0 );

    double beta  = -( atan2(nv_rot.getZ(), -nv_rot.getX()) - 0.5*M_PI );

    size_t n_points = points.size();
    for ( size_t i = 0; i < n_points; i++ ) {
        mapped_points.push_back( points[i].rotateVector(alpha, beta) );
    }

    Vector p1, p2;

    double x_p1, y_p1, x_p2, y_p2;
    double m_edge;
    double t_edge, t_ray;

    Vector p_rot = p.rotateVector( alpha, beta );

    t_ray = p_rot.getY();

    double x, y = t_ray;
    int intersect_count = 0;

    for ( size_t i = 0; i < n_points; i++ ) {
        p1 = mapped_points[i];
        p2 = mapped_points[(i+1)%n_points];

        x_p1 = p1.getX();
        y_p1 = p1.getY();
        x_p2 = p2.getX();
        y_p2 = p2.getY();

        m_edge = ( y_p2 - y_p1 ) / ( x_p2 - x_p1 );
        t_edge = y_p1 - m_edge * x_p1;

        if ( m_edge != 0.0 ) {
            x = ( t_ray - t_edge ) / m_edge;
        }
        else {
            continue;
        }

        double
            len_p1_p2 = sqrt( pow(x_p2-x_p1, 2) + pow(y_p2-y_p1, 2) ),
            len_p1_intersect = sqrt( pow(x-x_p1, 2) + pow(y-y_p1, 2) ),
            len_intersect_p2 = sqrt( pow(x_p2-x, 2) + pow(y_p2-y, 2) );

        if ( equalWithThreshold(len_intersect_p2 + len_p1_intersect, len_p1_p2, 0.01) ) {
            intersect_count++;
        }
    }

    return intersect_count % 2;
}


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


void Polygon::printPolygon () {
    printf( "POLYGON\n" );
    printf( "\tBase plane:\n" );
    printf( "\tx = %f y = %f z = %f n = %f\n\n", base_plane.getX(), base_plane.getY(), base_plane.getZ(), base_plane.getN() );

    printf( "\tPoints:\n" );
    uint len = points.size();
    for ( size_t i = 0; i < len; i++ ) {
        printf( "\tx = %f y = %f z = %f\n", points[i].getX(), points[i].getY(), points[i].getZ() );
    }

    printf( "\n" );
}
