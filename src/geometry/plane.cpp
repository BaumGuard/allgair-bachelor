#include "plane.h"

#include "../status_codes.h"

/*---------------------------------------------------------------*/

int Plane::buildPlaneFromPoints ( Vector& p1, Vector& p2, Vector& p3 ) {
    base = p1;
    v1 = p2 - p1;
    v2 = p3 - p1;

    if ( v1.length() == 0.0 || v2.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }
    if ( v1.linearDependant(v2) ) {
        return VECTORS_LINEAR_DEPENDANT;
    }

    toCoordinateForm();

    return CREATION_SUCCEEDED;
} /* int Plane::buildPlaneFromPoints ( Vector& p1, Vector& p2, Vector& p3 ) */

/*---------------------------------------------------------------*/

int Plane::buildPlaneFromBaseAndVectors ( Vector& base, Vector& v1, Vector& v2 ) {
    if ( v1.length() == 0.0 || v2.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }
    if ( v1.linearDependant(v2) ) {
        return VECTORS_LINEAR_DEPENDANT;
    }
    this->base = base;
    this->v1 = v1;
    this->v2 = v2;

    toCoordinateForm();

    return CREATION_SUCCEEDED;
} /* int Plane::buildPlaneFromBaseAndVectors ( Vector& base, Vector& v1, Vector& v2 ) */

/*---------------------------------------------------------------*/

Vector Plane::normalVector() {
    Vector nv;

    double
        x_a = v1.getX(),
        y_a = v1.getY(),
        z_a = v1.getZ(),

        x_b = v2.getX(),
        y_b = v2.getY(),
        z_b = v2.getZ();

    double
        x_nv = y_a * z_b - z_a * y_b,
        y_nv = z_a * x_b - x_a * z_b,
        z_nv = x_a * y_b - y_a * x_b;

    nv.setX( x_nv );
    nv.setY( y_nv );
    nv.setZ( z_nv );

    return nv;
} /* Vector Plane::normalVector() */

/*---------------------------------------------------------------*/

void Plane::toCoordinateForm() {
    Vector nv = normalVector();

    x = nv.getX();
    y = nv.getY();
    z = nv.getZ();
    n = -base.getX() * x - base.getY() * y - base.getZ() * z;
} /* void Plane::toCoordinateForm() */

/*---------------------------------------------------------------*/

bool Plane::isPointOnPlane ( Vector& p ) {
    double res = x * p.getX() + y * p.getY() + z * p.getZ() + n;

    if ( res == 0.0 ) {
        return true;
    }
    return false;
} /* bool Plane::isPointOnPlane ( Vector& p ) */

/*---------------------------------------------------------------*/

int Plane::lineIntersection ( Line& l, Vector& intersect, double* factor ) {
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

    double u_denom = x_b*x + y_b*y + z_b*z;
    if ( u_denom == 0.0 && isPointOnPlane(base_vec) ) {
        return LINE_ON_PLANE;
    }
    if ( u_denom == 0.0 ) {
        return LINE_PARALLEL_TO_PLANE;
    }

    double u_num = -x_a*x - y_a*y - z_a*z - n;

    double u = u_num / u_denom;

    intersect = base_vec + u * dir_vec;

    if ( factor != nullptr ) {
        *factor = u;
    }

    return LINE_INTERSECTS_PLANE;
} /* int Plane::lineIntersection ( Line& l, Vector& intersect, double* factor ) */

/*---------------------------------------------------------------*/

int Plane::reflectLine ( Line& old_line, Line& new_line ) {
    Vector old_intersect;
    double factor;

    int status = lineIntersection( old_line, old_intersect, &factor );
    if ( status != LINE_INTERSECTS_PLANE ) {
        return status;
    }

    factor += 1.0;

    Vector plumb_line_base_point =
        old_line.getBaseVector() + factor * new_line.getDirectionVector();

    Vector normal_vector = normalVector();

    Line plumb_line;
    plumb_line.createLineFromBaseAndVector( plumb_line_base_point, normal_vector );

    Vector plumb_line_intersect;
    lineIntersection( plumb_line, plumb_line_intersect );

    Vector delta = plumb_line_intersect - plumb_line_base_point;

    Vector new_line_p = plumb_line_base_point + 2.0 * delta;

    new_line.createLineFromTwoPoints( old_intersect, new_line_p );

    return CREATION_SUCCEEDED;
} /* int Plane::reflectLine ( Line& old_line, Line& new_line ) */

/*---------------------------------------------------------------*/

Vector Plane::getBaseVector() const {
    return base;
} /* Vector Plane::getBaseVector() */

Vector Plane::getVector1() const {
    return v1;
} /* Vector Plane::getVector1() const */

Vector Plane::getVector2() const {
    return v2;
} /* Vector Plane::getVector2() const */
