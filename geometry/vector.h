#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

/*
Class to represent 3D vectors with vector arithmetics
*/
class Vector {

public:
    /* CONSTRUCTORS */
    Vector ( const double x, const double y, const double z );
    Vector ();

    /* SETTERS */
    void setX ( const double x );
    void setY ( const double y );
    void setZ ( const double z );

    /* GETTERS */
    double getX ( void ) const;
    double getY ( void ) const;
    double getZ ( void ) const;

    /* OPERATORS */
    Vector operator + ( const Vector& v ) const;
    void operator += ( Vector& v );

    Vector operator - ( const Vector& v ) const;
    void operator -= ( Vector& v );

    Vector operator * ( const double n ) const;
    void operator *= ( const double n );

    Vector operator / ( const double n ) const;
    void operator /= ( const double n );

    friend Vector operator * ( double n, const Vector& v );
    friend Vector operator / ( double n, const Vector& v );

    Vector& operator = ( const Vector& v ) = default;

    bool operator == ( Vector& v );
    bool operator != ( Vector & v );

    /*
    Check if a vector is linear dependant on the local vector

    Args:
     - v : Vector to check for linear dependancy

    Returns:
     - true  : Vector v is linear dependant on the local vector
     - false : Vector v is not linear dependant on the local vector
    */
    bool linearDependant ( Vector v );

    /*
    Length of the local vector

    Returns:
     - Length of the local vector
    */
    double length ();

private:
    double
        x = 0.0,
        y = 0.0,
        z = 0.0;
};



#endif
