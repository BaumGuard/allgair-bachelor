#include "vector.h"

#include <cmath>

#include "../utils.h"

/*---------------------------------------------------------------*/

Vector::Vector ( const double x, const double y, const double z ):
    x(x), y(y), z(z)
{} /* Vector ( const double x, const double y, const double z ) */

Vector::Vector () {}

/*---------------------------------------------------------------*/

void Vector::setX ( const double x ) {
    this->x = x;
} /* setX() */
void Vector::setY ( const double y ) {
    this->y = y;
} /* setY() */
void Vector::setZ ( const double z ) {
    this->z = z;
} /* setZ() */

/*---------------------------------------------------------------*/

double Vector::getX ( void ) const {
    return x;
} /* getX() */
double Vector::getY ( void ) const {
    return y;
} /* getY() */
double Vector::getZ ( void ) const {
    return z;
} /* getZ() */

/*---------------------------------------------------------------*/

Vector Vector::operator + ( const Vector& v ) const {
    Vector v_res;

    v_res.setX( this->x + v.getX() );
    v_res.setY( this->y + v.getY() );
    v_res.setZ( this->z + v.getZ() );

    return v_res;
} /* operator + */

void Vector::operator += ( Vector& v ) {
    x += v.getX();
    y += v.getY();
    z += v.getZ();
} /* operator += */

Vector Vector::operator - ( const Vector& v ) const {
    Vector v_res;

    v_res.setX( this->x - v.getX() );
    v_res.setY( this->y - v.getY() );
    v_res.setZ( this->z - v.getZ() );

    return v_res;
} /* operator - */

void Vector::operator -= ( Vector& v ) {
    x -= v.getX();
    y -= v.getY();
    z -= v.getZ();
} /* operator -= */

Vector Vector::operator * ( const double n ) {
    return Vector( x*n, y*n, z*n );
} /* operator * */

void Vector::operator *= ( const double n ) {
    this->x *= n;
    this->y *= n;
    this->z *= n;
} /* operator *= */

Vector Vector::operator / ( const double n ) const {
    return Vector( x/n, y/n, z/n );
} /* operator / */

void Vector::operator /= ( const double n ) {
    this->x /= n;
    this->y /= n;
    this->z /= n;
} /* operator /= */

bool Vector::operator == ( Vector& v ) const {
    return
        equalWithThreshold( x, v.getX(), 0.001 ) &&
        equalWithThreshold( y, v.getY(), 0.001 ) &&
        equalWithThreshold( z, v.getZ(), 0.001 );
} /* operator == */

bool Vector::operator != ( Vector& v ) const {
    return
        !(
            equalWithThreshold( x, v.getX(), 0.001 ) &&
            equalWithThreshold( y, v.getY(), 0.001 ) &&
            equalWithThreshold( z, v.getZ(), 0.001 )
        );
} /* operator != */


Vector operator * ( double n, const Vector& v ) {
    double
        x = v.getX() * n,
        y = v.getY() * n,
        z = v.getZ() * n;

    return Vector( x, y, z );
} /* operator * */

/*---------------------------------------------------------------*/

Vector Vector::crossProduct( Vector v ) {
    return Vector (
        y * v.getZ() - z * v.getY(),
        z * v.getX() - x * v.getZ(),
        x * v.getY() - y * v.getX()
    );
} /* crossProduct() */

/*---------------------------------------------------------------*/

bool Vector::linearDependant ( Vector v ) const {

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
} /* linearDependant() */

/*---------------------------------------------------------------*/

double Vector::length () const {
    return sqrt( x*x + y*y + z*z );
} /* length() */

/*---------------------------------------------------------------*/

void Vector::toUnitVector () {
    if ( x == 0.0 && y == 0.0 && z == 0.0 ) return;

    double divisor = sqrt( x*x + y*y + z*z );

    x /= divisor;
    y /= divisor;
    z /= divisor;
} /* toUnitVector() */

/*---------------------------------------------------------------*/

Vector Vector::getUnitVector () const {
    if ( x == 0.0 && y == 0.0 && z == 0.0 ) {
        return Vector( 0.0, 0.0, 0.0 );
    }

    double divisor = sqrt( x*x + y*y + z*z );

    return Vector( x / divisor, y / divisor, z / divisor );
} /* getUnitVector() */

/*---------------------------------------------------------------*/

void Vector::printVector () const {
    printMessage( NORMAL, "x=%f y=%f z=%f\n", x, y, z );
} /* printVector() */

/*---------------------------------------------------------------*/

Vector Vector::rotateVector( double alpha, double beta ) const {
    Vector rotated_vector;

    double
        sin_alpha = sin( alpha ),
        sin_beta = sin( beta ),
        cos_alpha = cos( alpha ),
        cos_beta = cos( beta );

    double
        x_rot = x*cos_alpha*cos_beta - y*sin_alpha*cos_beta + z*sin_beta,
        y_rot = x*sin_alpha + y*cos_alpha,
        z_rot = -x*cos_alpha*sin_beta + y*sin_alpha*sin_beta + z*cos_beta;

    rotated_vector.setX( x_rot );
    rotated_vector.setY( y_rot );
    rotated_vector.setZ( z_rot );

    return rotated_vector;
} /* rotateVector () */
