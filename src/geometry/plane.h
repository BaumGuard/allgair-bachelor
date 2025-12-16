#ifndef PLANE_H
#define PLANE_H

#include "vector.h"
#include "line.h"

/*
Class to represent planes in 3D space
*/

class Plane {
public:
    /*
    Create plane from three points

    Args:
     - p1 : Point 1 as Vector
     - p2 : Point 2 as Vector
     - p3 : Point 3 as Vector

    Returns:
     - Status code
        - SUCCESS

        - VECTOR_LENGTH_0
          (Failure: At least two points are identical -> Must be different)
        - VECTORS_LINEAR_DEPENDANT
          (Failure: All three points are on a line -> Points must form a triangle)
    */
    int createPlaneFromPoints ( Vector& p1, Vector& p2, Vector& p3 );

    /*
    Create plane from a base point and a vector

    Args:
     - p  : Base point as Vector
     - v1 : Direction vector 1 as Vector
     - v2 : Direction vector 2 as Vector

    Returns:
     - Status code
        - SUCCESS

        - VECTOR_LENGTH_0
          (Failure: At least one vector has a length of 0 -> Length must be >0)
        - VECTORS_LINEAR_DEPENDANT
          (Failure: Vectors v1 and v2 are linear dependant -> Must be linear independant)
    */
    int createPlaneFromBaseAndVectors ( Vector& base, Vector& v1, Vector& v2 );

    /*
    Create a plane from the coordinates x, y, z, n

    Args:
     - x : x coordinate
     - y : y coordinate
     - z : z coordinate
     - n : n coordinate

    Returns:
     - Status code
        - SUCCESS

        - ALL_COORDINATES_ZERO
          (At least one of the coordinates x, y and z must be != 0)
    */
    int createPlaneFromCoordinates ( double x, double y, double z, double n );

    /*
    Create a plane from a base point and a normal vector

    Args:
     - base          : Base point on the plane
     - normal_vector : Normal vector of the plane

    Returns:
     - Status code
        - SUCCESS

        - VECTOR_LENGTH_0
    */
    int createPlaneFromBaseAndNormalVector ( Vector& base, Vector& normal_vector );

    /*
    Return the normal vector of the plane as Vector
    */
    Vector normalVector() const;

    /*
    Check if a point p is on the plane

    Args:
     - p : Point to check as Vector

    Returns:
     - true  : Point p is on the plane
     - false : Point p is not on the plane
    */
    bool isPointOnPlane ( Vector& p ) const;

    /*
    Determine the intersection of a Line l with the plane

    Args:
     - l         : Line to determine the intersection with the plane
     - intersect : Reference to the Vector to save the intersection to
     - factor    : Pointer to the variable to save the line factor for l

    Returns:
     - Status code:
        - INTERSECTION_FOUND

        - LINE_ON_PLANE (Failure: Infinitely many intersection points)
        - LINE_PARALLEL_TO_PLANE (Failure: No intersection with the plane)
    */
    int lineIntersection ( Line& l, Vector& intersect, double* factor = nullptr ) const;

    /*
    Reflect a line on the plane

    Args:
     - old_line : Line to reflect
     - new_line : Reference to the Line object to save the reflected line

    Returns:
     - Status code
        - SUCCESS

        - LINE_ON_PLANE (Failure: No reflection possible)
        - LINE_PARALLEL_TO_PLANE (Failure: No reflection possible)
    */
    int reflectLine ( Line& old_line, Line& new_line ) const;


    /*
    Calculate and return the slope of the plane (in radians)

    Returns:
     - Slope of the plane in radians
    */
    double slope ();

    /*
    Calculate the distance from a point to the plane

    Args:
     - p : Point as Vector
    */
    double distanceOfPointToPlane ( Vector& p ) const;

    /* GETTERS */
    double getX () const;
    double getY () const;
    double getZ () const;
    double getN () const;


    /* OPERATORS */

    /*
    Check if two Plane objects are equal or unequal
    */
    bool operator == ( Plane& plane );
    bool operator != ( Plane& plane );

    /*
    Print a description of the plane
    */
    void printPlane() const;

private:
    double
        x,
        y,
        z,
        n;

    /*
    Convert the parameter form to the normalized coordinate form internally
    for easier computation

    Args:
     - base : Base vector
     - v1   : First direction vector
     - v2   : Second direction vector
    */
    void toNormalizedCoordinateForm ( Vector& base, Vector& v1, Vector& v2 );
};

#endif
