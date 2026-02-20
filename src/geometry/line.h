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
    Create a line out of two non-identical points

    Args:
     - p1 : Point 1 as a Vector object
     - p2 : Point 2 as a Vector object

    Returns:
     - Status code
        - SUCCESS

        - POINTS_IDENTICAL (Failure -> Two different points must be given)
    */
    int createLineFromTwoPoints ( Vector& p1, Vector& p2 );

    /*
    Create a line using a base point and a direction vector

    Args:
     - p : Base point as Vector
     - v : Direction vector

    Returns:
     - Status code
        - SUCCESS

        - VECTOR_LENGTH_0 (The vector v must have a length greater than 0)
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
    bool pointIsOnLine ( Vector p ) const;

    /*
    Calculate the distance from a point to the line

    Args:
     - p : Point to calculate the distance from the line
    */
    double distanceOfPointToLine ( Vector& p );

    /*
    Find the intersection between this line and another line

    Args:
     - l         : Line to find the intersection with the local line
     - intersect : Reference to the vector to save the intersection point in

    Returns:
     - Status code
        - INTERSECTION_FOUND

        - LINES_IDENTICAL (Failure -> Infinitely many intersections)
        - LINES_PARALLEL  (Failure -> No intersection)
        - NO_INTERSECTION_FOUND (Failure -> No intersection)
    */
    int lineIntersect ( Line& l, Vector& intersect ) const;

    /*
    Check if two lines are equal
    */
    bool operator == ( Line& l ) const;


    /*
    Print a description of the line
    */
    void printLine () const;

private:
    Vector
        base,
        direction;
};

#endif
