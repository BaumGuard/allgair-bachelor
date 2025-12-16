#include "plane.h"

#include "../utils.h"
#include "../status_codes.h"

#include <cmath>

/*---------------------------------------------------------------*/

int Plane::createPlaneFromPoints ( Vector& p1, Vector& p2, Vector& p3 ) {
    Vector
        v1 = p2 - p1,
        v2 = p3 - p1;

    if ( v1.length() == 0.0 || v2.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }
    if ( v1.linearDependant(v2) ) {
        return VECTORS_LINEAR_DEPENDANT;
    }

    toNormalizedCoordinateForm( p1, v1, v2 );

    return SUCCESS;
} /* createPlaneFromPoints() */

/*---------------------------------------------------------------*/

int Plane::createPlaneFromBaseAndVectors ( Vector& base, Vector& v1, Vector& v2 ) {
    if ( v1.length() == 0.0 || v2.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }
    if ( v1.linearDependant(v2) ) {
        return VECTORS_LINEAR_DEPENDANT;
    }

    toNormalizedCoordinateForm( base, v1, v2 );

    return SUCCESS;
} /* createPlaneFromBaseAndVectors() */

/*---------------------------------------------------------------*/

int Plane::createPlaneFromCoordinates ( double x, double y, double z, double n ) {

    if ( x == 0.0 && y == 0.0 && z == 0.0 ) {
        return ALL_COORDINATES_ZERO;
    }

    this->x = x;
    this->y = y;
    this->z = z;
    this->n = n;

    double vector_length = sqrt( x*x + y*y + z*z );
    x /= vector_length;
    y /= vector_length;
    z /= vector_length;
    n /= vector_length;

    return SUCCESS;
} /* createPlaneFromCoordinates() */

/*---------------------------------------------------------------*/

int Plane::createPlaneFromBaseAndNormalVector ( Vector& base, Vector& normal_vector ) {
    if ( normal_vector.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }

    x = normal_vector.getX();
    y = normal_vector.getY();
    z = normal_vector.getZ();
    n = -base.getX()*x - base.getY()*y - base.getZ()*z;

    double normal_vector_length = normal_vector.length();

    x /= normal_vector_length;
    y /= normal_vector_length;
    z /= normal_vector_length;
    n /= normal_vector_length;

    return SUCCESS;
} /* createPlaneFromBaseAndNormalVector() */

/*---------------------------------------------------------------*/

Vector Plane::normalVector() const {
    return Vector( x, y, z );
} /* normalVector() */

/*---------------------------------------------------------------*/

void Plane::toNormalizedCoordinateForm( Vector& base, Vector& v1, Vector& v2 ) {
    double
        x_a = v1.getX(),
        y_a = v1.getY(),
        z_a = v1.getZ(),

        x_b = v2.getX(),
        y_b = v2.getY(),
        z_b = v2.getZ();

    x = y_a * z_b - z_a * y_b;
    y = z_a * x_b - x_a * z_b;
    z = x_a * y_b - y_a * x_b;
    n = -base.getX() * x - base.getY() * y - base.getZ() * z;

    double vector_length = sqrt( x*x + y*y + z*z );
    x /= vector_length;
    y /= vector_length;
    z /= vector_length;
    n /= vector_length;
} /* toCoordinateForm() */

/*---------------------------------------------------------------*/

bool Plane::isPointOnPlane ( Vector& p ) const {
    double result = x * p.getX() + y * p.getY() + z * p.getZ() + n;

    if ( fabs(result) < 0.01 ) {
        return true;
    }
    return false;
} /* isPointOnPlane() */

/*---------------------------------------------------------------*/

int Plane::lineIntersection ( Line& l, Vector& intersect, double* factor ) const {
    // Get the base point and direction vector of the line
    Vector
        base_vec = l.getBaseVector(),
        dir_vec  = l.getDirectionVector();

    double
        x_a = base_vec.getX(),
        y_a = base_vec.getY(),
        z_a = base_vec.getZ(),

        x_b = dir_vec.getX(),
        y_b = dir_vec.getY(),
        z_b = dir_vec.getZ();

    // Calculate the denominator of the line's scalar u to determine
    // whether an intersection exists (if u != 0)
    double u_denom = x_b*x + y_b*y + z_b*z;
    if ( fabs(u_denom) < 0.001 && isPointOnPlane(base_vec) ) {
        return LINE_ON_PLANE;
    }
    if ( fabs(u_denom) < 0.001 ) {
        return LINE_PARALLEL_TO_PLANE;
    }

    // Calculate the numerator of u and u itself
    double u_num = -x_a*x - y_a*y - z_a*z - n;
    double u = u_num / u_denom;

    // Use u to calculate the intersection point
    intersect = base_vec + u * dir_vec;

    if ( factor != nullptr ) {
        *factor = u;
    }

    return INTERSECTION_FOUND;
} /* lineIntersection() */

/*---------------------------------------------------------------*/

int Plane::reflectLine ( Line& old_line, Line& new_line ) const {
    Vector old_intersect;

    // Factor of the direction vector in the line
    // Plugged into the line equation it will evaluate to the
    // intersection point
    double factor;

    // Find the intersection of the given line with the plane
    int status = lineIntersection( old_line, old_intersect, &factor );
    if ( status != INTERSECTION_FOUND ) {
        return status;
    }

    // Change the factor to find an auxiliary point on the line which
    // is not the intersection with the plane
    factor += 1.0;

    Vector plumb_line_base_point =
        old_line.getBaseVector() + factor * new_line.getDirectionVector();

    Vector normal_vector = normalVector();

    // Create a plumb line starting at the auxiliary point
    // This is used to reflect a chosen point on the plane
    Line plumb_line;
    plumb_line.createLineFromBaseAndVector( plumb_line_base_point, normal_vector );

    Vector plumb_line_intersect;
    lineIntersection( plumb_line, plumb_line_intersect );

    // Vector between the auxiliary point and the plumb line intersection
    Vector delta = plumb_line_intersect - plumb_line_base_point;

    // Reflect the point on the plane
    Vector new_line_p = plumb_line_base_point + 2.0 * delta;

    // Create the reflected line
    new_line.createLineFromTwoPoints( old_intersect, new_line_p );

    return SUCCESS;
} /* reflectLine() */

/*---------------------------------------------------------------*/

double Plane::slope () {
    // Calculate the angle of the normal vector when being
    // projected onto the xy plane
    double alpha = -atan2( y, x );

    // Normal vector of the plane
    Vector nv( x, y, z );

    // Rotate the normal vector around the z axis in order to
    // eliminate the y coordinate
    Vector rot_nv = nv.rotateVector( alpha, 0.0 );

    // Calculate the angle between the plane and the xy plane
    double angle_nv = atan( rot_nv.getZ() / rot_nv.getX() );

    // Calculate the slope depending on whether the angle of the
    // plane is positive or negative
    if ( angle_nv > 0.0 ) {
        return -angle_nv + 0.5 * M_PI;
    }
    return angle_nv + 0.5 * M_PI;
} /* slope() */

/*---------------------------------------------------------------*/

double Plane::getX () const {
    return x;
} /* getX() */

double Plane::getY () const {
    return y;
} /* getY() */

double Plane::getZ () const {
    return z;
} /* getZ() */

double Plane::getN () const {
    return n;
} /* getN() */

/*---------------------------------------------------------------*/

bool Plane::operator == ( Plane& plane ) {
    if (
        x == plane.getX() && y == plane.getY() &&
        z == plane.getZ() && n == plane.getN()
    ) {
        return true;
    }
    return false;
} /* operator == */

bool Plane::operator != ( Plane& plane ) {
    return !( *this == plane );
} /* operator != */

/*---------------------------------------------------------------*/

double Plane::distanceOfPointToPlane ( Vector& p ) const {
    return fabs( x*p.getX() + y*p.getY() + y*p.getZ() + n );
} /* distanceOfPointToPlane() */

/*---------------------------------------------------------------*/

void Plane::printPlane() const {
    printMessage( NORMAL, "x=%f y=%f z=%f n=%f\n", x, y, z, n );
} /* printPlane() */
