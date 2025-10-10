#include "vector.h"

#include <cmath>


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
    double
        div_x = x / v.getX(),
        div_y = y / v.getY(),
        div_z = z / v.getZ();

    if ( div_x == div_y && div_x == div_z && div_y == div_z ) {
        return true;
    }
    return false;
} /* bool Vector::linearDependant ( Vector v ) */

/*---------------------------------------------------------------*/

double Vector::length () {
    return sqrt( x*x + y*y + z*z );
} /* double Vector::length () */
