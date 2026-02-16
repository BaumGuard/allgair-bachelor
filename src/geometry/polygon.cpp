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

bool Polygon::isPointInPolygon ( Vector& p, bool two_d ) const {
    // Rotated polygon points
    std::vector<Vector> mapped_points;

    // Number of points in the polygon
    size_t n_points = points.size();

    // Rotated point p
    Vector p_rot;

    // y intercept of the probing ray
    double t_ray;

    // 3D mode -> Rotate the polygon so that it becomes parallel
    // to the xy plane
    if ( !two_d ) {
        if ( !base_plane.isPointOnPlane(p) ) {
            return false;
        }

        // Normal vector of the polygon's base plane
        Vector normal_vector = base_plane.normalVector();

        double
            x_nv = normal_vector.getX(),
            y_nv = normal_vector.getY(),
            z_nv = normal_vector.getZ();

        // Find the rotation angles alpha and beta to rotate the base plane
        // so that it becomes parallel to the xy plane

        // Rotation angles around the z axis (alpha) and y axis (beta)
        double
            alpha = atan2( y_nv, x_nv ),
            beta  = 0.5*M_PI - atan2( z_nv, sqrt(x_nv*x_nv+y_nv*y_nv) );

        // Rotate all points of the polygon around the angles alpha and beta
        // to map the base plane to the xy plane
        for ( size_t i = 0; i < n_points; i++ ) {
            mapped_points.push_back( points[i].rotateVector(-alpha, -beta) );
        }

        // Rotating the point p around alpha and beta (mapping to the xy plane)
        p_rot = p.rotateVector( -alpha, -beta );

        // The y intercept of the probing ray is set to the y coordinate
        // of the point p
        // The probing ray is a horizontal line and starts at the point p
        // and stretches towards positive infinity
        t_ray = p_rot.getY();
    }

    // 2D mode -> Don't rotate the polygon and just use the x and y coordinate
    else {
        // Add all points to the list mapped_points without rotation
        for ( size_t i = 0; i < n_points; i++ ) {
            mapped_points.push_back( points[i] );
        }

        // Leave the point p unrotated
        p_rot = p;

        // The y intercept of the probing ray is set to the y coordinate
        // of the point p
        // The probing ray is a horizontal line and starts at the point p
        // and stretches towards positive infinity
        t_ray = p_rot.getY();
    }


    // Adjacent points in the polygon
    Vector p1, p2;
    double x_p1, y_p1, x_p2, y_p2;

    // Slope and y intercept of a polygon's edge
    double m_edge, t_edge;

    double
        x,          // x coordinate of the intersection
        y = t_ray,  // y coordinate of the intersection
        x_diff;     // Difference between x of p and x of the intersection

    // Count the number of intersections betweeen an edge and the ray
    int intersect_count = 0;

    for ( size_t i = 0; i < n_points; i++ ) {
        // Get two adjacent points from the polygon
        // Wrap back to the beginning at the end of the loop
        p1 = mapped_points[i];
        p2 = mapped_points[(i+1)%n_points];

        x_p1 = p1.getX();
        y_p1 = p1.getY();
        x_p2 = p2.getX();
        y_p2 = p2.getY();

        // Calculate the slope and the y intercept of the current edge
        m_edge = ( y_p2 - y_p1 ) / ( x_p2 - x_p1 );
        t_edge = y_p1 - m_edge * x_p1;

        // Find the intersection between the polygon's edge and the ray
        // if the rays aren't parallel to eachother.
        if ( m_edge != 0.0 ) {
            // x coordinate of the intersection
            if ( std::isfinite(m_edge) ) {
                // If the edge is not aligned vertically (m_edge finite)
                // calculate the the intersection using the two lines
                x = ( t_ray - t_edge ) / m_edge;
            }
            else {
                // If the edge is aligned vertically (m_edge finite)
                // use the x coordinate of the first corner as the
                // x coordinate of the intersection
                x = x_p1;
            }

            x_diff = x - p_rot.getX();

            // Continue with the next point pair if the intersection
            // is located on the left of the point p (x_diff < 0.0)
            if ( x_diff < 0.0 ) {
                continue;
            }
        }
        else {
            continue;
        }

        // Distances between
        //  - the two corners of the edge
        //  - the first corner of the edge and the intersection
        //  - the second corner of the edge and the intersection
        double
            len_p1_p2 = sqrt( pow(x_p2-x_p1, 2) + pow(y_p2-y_p1, 2) ),
            len_p1_intersect = sqrt( pow(x-x_p1, 2) + pow(y-y_p1, 2) ),
            len_intersect_p2 = sqrt( pow(x_p2-x, 2) + pow(y_p2-y, 2) );

        // Test if the intersection is located between the two corners
        if ( equalWithThreshold(len_intersect_p2 + len_p1_intersect, len_p1_p2, 0.001) ) {
        //if ( len_intersect_p2 + len_p1_intersect == len_p1_p2 ) {
            intersect_count++;
        }
    }

    // If the number of intersections is even, the point p is located
    // outside the polygon and if it is odd, the point p is located inside
    // the polygon
    return intersect_count % 2;
} /* isPointInPolygon() */


/*---------------------------------------------------------------*/

Plane& Polygon::getBasePlane () {
    return base_plane;
} /* getBasePlane() */

/*---------------------------------------------------------------*/

std::vector<Vector>& Polygon::getPoints () {
    return points;
} /* getPoints() */

/*---------------------------------------------------------------*/

uint Polygon::getSurfaceType () const {
    return surface_type;
} /* getSurfaceType () */

// Don't introduce initialization checking here!
// Overways the binary file parsing won't work here!'
void Polygon::setSurfaceType ( uint surface_type ) {
    this->surface_type = surface_type;
} /* setSurfaceType () */

/*---------------------------------------------------------------*/

std::string Polygon::getID () const {
    return id;
} /* getID () */

void Polygon::setID ( std::string id ) {
    this->id = id;
    hasID = true;
} /* setID () */

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

/*---------------------------------------------------------------*/

Vector Polygon::getCentroid () {
    if ( !centroid_calculated ) {
        double sum_x = 0.0, sum_y = 0.0, sum_z = 0.0;

        uint len = points.size();
        for ( uint i = 0; i < len; i++ ) {
            sum_x += points[i].getX();
            sum_y += points[i].getY();
            sum_z += points[i].getZ();
        }

        centroid = Vector(
            sum_x / (double)len,
            sum_y / (double)len,
            sum_z / (double)len
        );

        centroid_calculated = true;
    }

    return centroid;
} /* getCentroid() */

/*---------------------------------------------------------------*/

void Polygon::setCentroid ( Vector& centroid ) {
    this->centroid = centroid;
    centroid_calculated = true;
} /* setCentroid () */

/*---------------------------------------------------------------*/

#if 0
double Polygon::polygonArea () {
    Vector centroid = getCentroid();

    double area = 0.0;

    uint len = points.size();
    for ( uint i = 0; i < len; i++ ) {
        // Vector between the current point and the adjacent point
        Vector v1 = points[(i+1)%len] - points[i];

        // Vector between the current point and the centroid
        Vector v2 = centroid - points[i];

        // The length of the vector resulting from the cross product
        // of v1 and v2 is the area of the parallelogram created by
        // the vectors v1 and v2
        Vector product = v1.crossProduct( v2 );
        area += product.length();
    }

    // Divide by 2 to get the area of the triangles created by v1 and v2
    // instead of the parallelograms
    return area / 2.0;
} /* polygonArea () */
#endif
/*---------------------------------------------------------------*/

void Polygon::setArea ( double area ) {
    this->area = area;
} /* setArea() */

double Polygon::getArea () {
    return area;
} /* getArea() */

/*---------------------------------------------------------------*/

bool Polygon::pointListsEqual( std::vector<Vector>& other_list ) {
    uint len_local = points.size();
    uint len_other_list = other_list.size();

    if ( len_local != len_other_list ) {
        return false;
    }

    uint len = points.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( points[i] != other_list[i] ) {
            return false;
        }
    }
    return true;
} /* pointListsEqual() */

bool Polygon::operator == ( Polygon& polygon ) {
    if (
        pointListsEqual( polygon.getPoints() )   &&
        base_plane   == polygon.getBasePlane()   &&
        surface_type == polygon.getSurfaceType()
    ) {
        return true;
    }
    return false;
} /* operator == */

bool Polygon::operator != ( Polygon& polygon ) {
    return !( *this == polygon );
} /* operator != */

/*---------------------------------------------------------------*/

void Polygon::printPolygon () {
    printf( "POLYGON\n" );
/*
    if ( hasID ) {
        printf( "\tPolygon ID\n" );
        printf( "\t%s\n\n", id.data() );
    }
    else {
        printf( "\tPolygon ID\n" );
        printf( "(NONE)" );
    }
*/
    printf( "\tBase plane:\n" );
    printf(
        "\tx = %f y = %f z = %f n = %f\n\n",
        base_plane.getX(),
        base_plane.getY(),
        base_plane.getZ(),
        base_plane.getN()
    );

    printf( "\tPoints:\n" );
    uint len = points.size();
    for ( size_t i = 0; i < len; i++ ) {

        printf(
            "\tx = %f y = %f z = %f\n",
            points[i].getX(),
            points[i].getY(),
            points[i].getZ()
        );

        //printf("%f, %f\n", points[i].getX(), points[i].getY());
    }

    printf( "\n" );
} /* printPolygon() */
