#ifndef VECTOR_H
#define VECTOR_H

#define LINDEP_THRESHOLD 0.05

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

    Vector operator * ( const double n );
    void operator *= ( const double n );

    Vector operator / ( const double n ) const;
    void operator /= ( const double n );

    friend Vector operator * ( double n, const Vector& v );

    Vector& operator = ( const Vector& v ) = default;

    bool operator == ( Vector& v ) const;
    bool operator != ( Vector& v ) const;


    /*
    Calculate the cross product of the local vector and vector v

    Args:
     - v : Second factor of the cross product

    Returns:
     - Cross product of the local vector and v
    */
    Vector crossProduct( Vector v );

    /*
    Check if a vector is linear dependant on the local vector

    Args:
     - v : Vector to check for linear dependence

    Returns:
     - true  : Vector v is linear dependent on the local vector
     - false : Vector v is not linear dependent on the local vector
    */
    bool linearDependant ( Vector v ) const;

    /*
    Length of the local vector

    Returns:
     - Length of the local vector
    */
    double length () const;

    /*
    Transform the local vector to a unit vector
    */
    void toUnitVector ();

    /*
    Return the unit vector of the local vector
    */
    Vector getUnitVector () const;

    /*
    Rotate a vector around the angles alpha (around the z axis)
    and beta (around the y axis)

    Args:
     - alpha : Angle around the z axis / in the x/y plane
     - beta  : Angle around the y axis / in the x/z plane

    Returns:
     - Vector rotated by alpha and beta
    */
    Vector rotateVector( double alpha, double beta ) const;

    /*
    Print a description of the vector
    */
    void printVector () const;

private:
    double
        x = 0.0,
        y = 0.0,
        z = 0.0;
};

#endif
