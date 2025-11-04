#ifndef POLYGON_H
#define POLYGON_H

#include "vector.h"
#include "plane.h"
#include "../utils.h"

#include <vector>

/*
Class to represent polygons lying on a plane
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
    The points should be given in a clockwise order

    Args:
     - point : A point of the polygon as Vector to add

    Returns:
     - Status code:
        - POINT_NOT_ON_BASE_PLANE
          (Failure: A point of the polygon must be on the base plane)
        - POINT_DUPLICATE
          (Failure: All points must differ from eachother)
        - POINT_ADDED (Success)
    */
    int addPoint ( Vector point );

    /*
    Check if the point p is inside or outside the polygon

    Args:
     - p : Point as Vector to check
    */
    bool isPointInPolygon ( Vector& p );


    /* GETTERS */

    /*
    Return the base plane of the polygon as a Plane object
    */
    Plane getBasePlane();

    /*
    Return a reference to the point list of the polygon
    */
    std::vector<Vector> getPoints ();


    int lineIntersection ( Line& l, Vector& intersect );

    uint size () {return points.size();}

private:
    std::vector<Vector> points;
    Plane base_plane;

    bool initialized = false;

    /*
    Check if a point is already in the polygon
    (Used by the function addPoint)

    If true, addPoint will return the status code POINT_DUPLICATE
    */
    bool isPointAlreadyInPolygon ( Vector& p );
};

#endif
