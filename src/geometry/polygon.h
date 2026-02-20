#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "plane.h"
#include "../utils.h"

#include <vector>
#include <cstdint>

/*
Class to represent polygons located on a plane
*/
class Polygon {
public:

    /*
    Initialize a Polygon object with a plane to
    put the points on

    Args:
     - p : Plane on which the polygon should be
    */
    void initPolygonWithPlane ( Plane p );

    /*
    Add a point of the polygon

    IMPORTANT:
    The points should be given in a clockwise or counter-clockwise
    order

    Args:
     - point : A point of the polygon as Vector to add

    Returns:
     - Status code:
        - SUCCESS

        - POLYGON_NOT_INITIALIZED
        - POINT_NOT_ON_BASE_PLANE
          (Failure: A point of the polygon must be on the base plane)
        - POINT_DUPLICATE
          (Failure: All points must differ from eachother)
    */
    int addPoint ( Vector point );

    /*
    Check if the point p is inside or outside the polygon

    Args:
     - p     : Point as Vector to check
     - two_d : 2D mode (don't rotate the polygon so that it becomes
               parallel to the xy plane, just use the x and y coordinates
               and ignore the z coordinate)
    */
    bool isPointInPolygon ( Vector p, bool two_d = false ) const;


    /* GETTERS */

    /*
    Return the base plane of the polygon as a Plane object
    */
    Plane& getBasePlane();

    /*
    Return the point list of the polygon
    */
    std::vector<Vector>& getPoints ();

    /*
    Return the surface type of the polygon
    (defined in enum SurfaceTypes - surface.h)
    */
    uint getSurfaceType () const;

    /*
    Set the surface type of the polygon
    */
    void setSurfaceType ( uint surface_type );


    /*
    Return the ID of the polygon
    */
    std::string getID () const;

    /*
    Set the ID of the polygon
    */
    void setID ( std::string id );


    /*
    Find the intersection between a polygon and a line

    Args:
     - l          : Line to calculate the intersection with the
                    polygon if it exists
     - intersect  : Reference to the Vector object to save the
                    intersection with the polygon in when an
                    intersection exists

    Returns:
     - Status code
        - INTERSECTION_FOUND

        - NO_INTERSECTION_FOUND
    */
    int lineIntersection ( Line& l, Vector& intersect ) const;

    /*
    Calculate the centroid of the polygon by averaging
    the x, y and z coordinates of the points and return it
    as a Vector object

    WARNING
    This function assumes that all points have already been added
    to the Polygon instance. The centroid will be calculated once
    when this function is called for the first time and is saved
    for later calls.

    Returns:
     - Centroid point of the polygon
    */
    Vector getCentroid();

    /*
    Set the centroid of the polygon

    Args:
     - centroid : Centroid as Vector
    */
    void setCentroid( Vector& centroid );

    /*
    Set the area of the polygon

    Args:
     - area : Polygon area in m²
    */
    void setArea ( double area );

    /*
    Return the area of the polygon in m²
    */
    double getArea ();

    /*
    Check if two Polygon objects are equal or unequal
    */
    bool operator == ( Polygon& polygon );
    bool operator != ( Polygon& polygon );

    /*
    Print a description of the polygon with the base plane
    and the points
    */
    void printPolygon ();

private:
    std::vector<Vector> points;
    Plane base_plane;

    bool initialized = false;
    bool hasID = false;

    uint surface_type = 0;

    bool centroid_calculated = false;
    Vector centroid;
    double area = 0.0;

    std::string id = "";


    /*
    Check if a point is already in the polygon
    (Used by the function addPoint)

    If true, addPoint will return the status code POINT_DUPLICATE
    */
    bool isPointAlreadyInPolygon ( Vector& p ) const;

    /*
    Check if two std::vector<Vector> of points are equal
    */
    bool pointListsEqual( std::vector<Vector>& other_list );
};

#endif
