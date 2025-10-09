#ifndef PLANE_H
#define PLANE_H

#include "vector.h"
#include "line.h"
#include "status_codes.h"

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
        - VECTOR_LENGTH_0
          (Failure: At least two points are identical -> Must be different)
        - VECTORS_LINEAR_DEPENDANT
          (Failure: At least two points are on a line -> Points must form a triangle)
        - CREATION_SUCCEEDED (Success)
    */
    int buildPlaneFromPoints ( Vector& p1, Vector& p2, Vector& p3 );

    /*
    Create plane from a base point and a vector

    Args:
     - p  : Base point as Vector
     - v1 : Direction vector 1 as Vector
     - v2 : Direction vector 2 as Vector

    Returns:
     - Status code
        - VECTOR_LENGTH_0
          (Failure: At least one vector has a length of 0 -> Length must be >0)
        - VECTORS_LINEAR_DEPENDANT
          (Failure: Vectors v1 and v2 are linear dependant -> Must be linear independant)
        - CREATION_SUCCEEDED (Success)
    */
    int buildPlaneFromBaseAndVectors ( Vector& base, Vector& v1, Vector& v2 );

    /*
    Return the normal vector of the plane as Vector
    */
    Vector normalVector();

    /*
    Check if a point p is on the plane

    Args:
     - p : Point to check as Vector

    Returns:
     - true  : Point p is on the plane
     - false : Point p is not on the plane
    */
    bool isPointOnPlane ( Vector& p );

    /*
    Determine the intersection of a Line l with the plane

    Args:
     - l         : Line to determine the intersection with the plane
     - intersect : Reference to the Vector to save the intersection to
     - factor    : Pointer to the variable to save the line factor for l

    Returns:
     - Status code:
        - LINE_ON_PLANE (Failure: Infinitely many intersection points)
        - LINE_PARALLEL_TO_PLANE (Failure: No intersection with the plane)
        - LINE_INTERSECTS_PLANE (Success)
    */
    int lineIntersection ( Line& l, Vector& intersect, double* factor = NULL );

    /*
    Reflect a line on the plane

    Args:
     - old_line : Line to reflect
     - new_line : Reference to the Line object to save the reflected line

    Returns:
     - LINE_ON_PLANE (Failure: No reflection possible)
     - LINE_PARALLEL_TO_PLANE (Failure: No reflection possible)
     - CREATION_SUCCEEDED (Success)
    */
    int reflectLine ( Line& old_line, Line& new_line );

    /* GETTERS */
    Vector getBaseVector() const;
    Vector getVector1() const;
    Vector getVector2() const;

private:
    double
        x,
        y,
        z,
        n;

    Vector
        base,
        v1,
        v2;

    /*
    Convert the parameter form to the coordinate form internally
    for better computation
    */
    void toCoordinateForm ( void );
};

#endif
