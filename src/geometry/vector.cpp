#include "vector.h"

#include <cmath>
#include <cstdio>

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

#if 0
bool Vector::linearDependant ( Vector v, int precision ) {
    double
        div_sum = 0.0,
        div_avg;

    int coord = 0;
    int count = 0;

    if ( !(x == 0.0 && v.getX() == 0.0) ) {
        count++;
    }
    if ( !(y == 0.0 && v.getY() == 0.0) ) {
        count++;
    }
    if ( !(z == 0.0 && v.getZ() == 0.0) ) {
        count++;
    }

    if ( v.getX() != 0.0 ) {
        div_sum += x / v.getX();
        coord = 1;
    }
    if ( v.getY() != 0.0 ) {
        div_sum += y / v.getY();
        coord = 2;
    }
    if ( v.getZ() != 0.0 ) {
        div_sum += z / v.getZ();
        coord = 3;
    }

    div_avg = div_sum / (double)count;


    switch ( coord ) {
        case 1:
            return equalWithThreshold( div_avg, x / v.getX(), 0.01 );
        case 2:
            return equalWithThreshold( div_avg, y / v.getY(), 0.01 );
        case 3:
            return equalWithThreshold( div_avg, z / v.getZ(), 0.01 );
    }



    /*
    switch ( coord ) {
        case 1:
            return div_avg == x / v.getX();
        case 2:
            return div_avg == y / v.getY();
        case 3:
            return div_avg == z / v.getZ();
    }
    */


    Vector zero_vec ( 0.0, 0.0, 0.0 );

    return *this == zero_vec;
} /* bool Vector::linearDependant ( Vector v ) */
#endif

bool Vector::linearDependant ( Vector v ) {

    double
        x1 = x,
        y1 = y,
        z1 = z,
        x2 = v.getX(),
        y2 = v.getY(),
        z2 = v.getZ();

    double
        normalize1 = sqrt( x1*x1 + y1*y1 + z1*z1 ),
        normalize2 = sqrt( x2*x2 + y2*y2 + z2*z2 );

    x1 /= normalize1;
    y1 /= normalize1;
    z1 /= normalize1;
    x2 /= normalize2;
    y2 /= normalize2;
    z2 /= normalize2;

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
