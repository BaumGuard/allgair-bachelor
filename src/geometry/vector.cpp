#include "vector.h"

#include <cmath>

#include "../utils.h"

/*---------------------------------------------------------------*/

Vector::Vector ( const double x, const double y, const double z ):
    x(x), y(y), z(z)
{} /* Vector::Vector ( const double x, const double y, const double z ) */

Vector::Vector () {}

/*---------------------------------------------------------------*/

void Vector::setX ( const double x ) {
    this->x = x;
} /* void Vector::setX ( const double x ) */
void Vector::setY ( const double y ) {
    this->y = y;
} /* void Vector::setY ( const double y ) */
void Vector::setZ ( const double z ) {
    this->z = z;
} /* void Vector::setZ ( const double z ) */

/*---------------------------------------------------------------*/

double Vector::getX ( void ) const {
    return x;
} /* double Vector::getX ( void ) const */
double Vector::getY ( void ) const {
    return y;
} /* double Vector::getY ( void ) const */
double Vector::getZ ( void ) const {
    return z;
} /* double Vector::getZ ( void ) const */

/*---------------------------------------------------------------*/

Vector Vector::operator + ( const Vector& v ) const {
    Vector v_res;

    v_res.setX( this->x + v.getX() );
    v_res.setY( this->y + v.getY() );
    v_res.setZ( this->z + v.getZ() );

    return v_res;
} /* Vector Vector::operator + ( const Vector& v ) const */

void Vector::operator += ( Vector& v ) {
    x += v.getX();
    y += v.getY();
    z += v.getZ();
} /* void Vector::operator += ( Vector& v ) */

Vector Vector::operator - ( const Vector& v ) const {
    Vector v_res;

    v_res.setX( this->x - v.getX() );
    v_res.setY( this->y - v.getY() );
    v_res.setZ( this->z - v.getZ() );

    return v_res;
} /* Vector Vector::operator - ( const Vector& v ) const */

void Vector::operator -= ( Vector& v ) {
    x -= v.getX();
    y -= v.getY();
    z -= v.getZ();
} /* void Vector::operator -= ( Vector& v ) */

Vector Vector::operator * ( const double n ) const {
    return Vector( x*n, y*n, z*n );
} /* Vector Vector::operator * ( const double n ) const */

void Vector::operator *= ( const double n ) {
    this->x *= n;
    this->y *= n;
    this->z *= n;
} /* void Vector::operator *= ( const double n ) */

Vector Vector::operator / ( const double n ) const {
    return Vector( x/n, y/n, z/n );
} /* Vector Vector::operator / ( const double n ) const */

void Vector::operator /= ( const double n ) {
    this->x /= n;
    this->y /= n;
    this->z /= n;
} /* void Vector::operator /= ( const double n ) */

bool Vector::operator == ( Vector& v ) {
    return x == v.getX() && y == v.getY() && z == v.getZ();
} /* bool Vector::operator == ( Vector& v ) */

bool Vector::operator != ( Vector& v ) {
    return !( x == v.getX() && y == v.getY() && z == v.getZ() );
} /* bool Vector::operator != ( Vector& v ) */


Vector operator * ( double n, const Vector& v ) {
    double
        x = v.getX() * n,
        y = v.getY() * n,
        z = v.getZ() * n;

    return Vector( x, y, z );
} /* Vector operator * ( double n, const Vector& v ) */


/*---------------------------------------------------------------*/

bool Vector::linearDependant ( Vector v ) {

    Vector
        v1 = getUnitVector(),
        v2 = v.getUnitVector();

    double
        x1 = v1.getX(),
        y1 = v1.getY(),
        z1 = v1.getZ(),
        x2 = v2.getX(),
        y2 = v2.getY(),
        z2 = v2.getZ();

    if ( inRange(x1, 0.02) && inRange(x2, 0.02) ) {
        x1 = fabs( x1 );
        x2 = fabs( x2 );
    }
    if ( inRange(y1, 0.02) && inRange(y2, 0.02) ) {
        y1 = fabs( y1 );
        y2 = fabs( y2 );
    }
    if ( inRange(z1, 0.02) && inRange(z2, 0.02) ) {
        z1 = fabs( z1 );
        z2 = fabs( z2 );
    }

    double
        x1_x2 = x1 * x2,
        y1_y2 = y1 * y2,
        z1_z2 = z1 * z2;

    return (
        (
            ( x1_x2 >= 0.0 && y1_y2 >= 0.0 && z1_z2 >= 0.0 ) ||
            ( x1_x2 <= 0.0 && y1_y2 <= 0.0 && z1_z2 <= 0.0 )
        )
        &&
        (
            equalWithThreshold( fabs(x1), fabs(x2), LINDEP_THRESHOLD ) &&
            equalWithThreshold( fabs(y1), fabs(y2), LINDEP_THRESHOLD ) &&
            equalWithThreshold( fabs(z1), fabs(z2), LINDEP_THRESHOLD )
        )
    );
}

/*---------------------------------------------------------------*/

double Vector::length () {
    return sqrt( x*x + y*y + z*z );
} /* double Vector::length () */

/*---------------------------------------------------------------*/

void Vector::toUnitVector () {
    if ( x == 0.0 && y == 0.0 && z == 0.0 ) return;

    double divisor = sqrt( x*x + y*y + z*z );

    x /= divisor;
    y /= divisor;
    z /= divisor;
} /* void Vector::toUnitVector () */

/*---------------------------------------------------------------*/

Vector Vector::getUnitVector () {
    if ( x == 0.0 && y == 0.0 && z == 0.0 ) {
        return Vector( 0.0, 0.0, 0.0 );
    }

    double divisor = sqrt( x*x + y*y + z*z );

    return Vector( x / divisor, y / divisor, z / divisor );
} /* void Vector::getUnitVector () */

/*---------------------------------------------------------------*/

void Vector::printVector () {
    printMessage( NORMAL, "x=%f y=%f z=%f\n", x, y, z );
} /* void Vector::printVector () */
