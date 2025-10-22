#ifndef LINE_H
#define LINE_H

#include "vector.h"

/*
Class to represent lines in 3D space which consist of a base point
and a direction vector
*/

class Line {
public:

    /*
    Create a line when two points are given

    Args:
     - p1 : Point 1 as Vector
     - p2 : Point 2 as Vector

    Returns:
     - Status code
        - POINTS_IDENTICAL (Failure -> Two different points must be given)
        - CREATION_SUCCEEDED (Success)
    */
    int createLineFromTwoPoints ( Vector& p1, Vector& p2 );

    /*
    Create a line using a base point and a direction vector

    Args:
     - p : Base point as Vector
     - v : Direction vector

    Returns:
     - Status code
        - VECTOR_LENGTH_0 (The vector v must have a length greater than 0)
        - CREATION_SUCCEEDED (Success)
    */
    int createLineFromBaseAndVector ( Vector& p, Vector& v );


    /*
    Return the base vector of the line as Vector
    */
    Vector getBaseVector() const;

    /*
    Return the direction vector of the line as Vector
    */
    Vector getDirectionVector() const;


    /*
    Check if the point p is on the line

    Args:
     - p : Point to check if it is on the line

    Returns:
     - false (The point is not on the line)
     - true  (The point is on the line)
    */
    bool pointIsOnLine ( Vector p );

    /*
    Find the intersection between this line and another line

    Args:
     - l         : Line to find the intersection with the local line
     - intersect : Reference to the vector to save the intersection point it
     - factor    : Pointer to the variable to save the factor of the second line

    Returns:
     - Status code
        - LINES_IDENTICAL (Failure -> Infinitely many intersections)
        - LINES_PARALLEL  (Failure -> No intersection)
        - LINES_NO_INTERSECT (Failure -> No intersection)
        - LINES_INTERSECT (Success)
    */
    int lineIntersect ( Line& l, Vector& intersect, double* factor = nullptr );

    /*
    Check if two lines are equal
    */
    bool operator == ( Line& l );


    /*
    Print a description of the line
    */
    void printLine ();

private:
    Vector
        base,
        direction;
};

#endif
