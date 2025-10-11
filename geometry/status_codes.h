#ifndef STATUS_CODES_H
#define STATUS_CODES_H

enum Statuscodes {
    // Line
    LINES_INTERSECT          =  0, // If two lines intersect
    LINES_NO_INTERSECT       =  1, // If two lines don't intersect'
    LINES_IDENTICAL          =  2, // If two lines are mathematically identical
    LINES_PARALLEL           =  3, // If two lines are parallel and not identical
    POINTS_IDENTICAL         =  4, // If two points are identical

    // Line / Plane
    VECTOR_LENGTH_0          =  5, // If a vector has length 0
    VECTORS_LINEAR_DEPENDANT =  6, // If vectors are linear dependant
    LINE_INTERSECTS_PLANE    =  7, // If a line intersects the plane
    LINE_PARALLEL_TO_PLANE   =  8, // If a line is parallel to the plane
    LINE_ON_PLANE            =  9, // If a line is located on a plane
    CREATION_SUCCEEDED       = 10, // Success

    // Polygon
    POINT_DUPLICATE         =  11, // If a point is already present in the point list
    POINT_ADDED             =  12, // Point added successfully to the point list
    POINT_NOT_ON_BASE_PLANE =  13, // If a point is not located on the base plane
    POLYGON_NOT_INITIALIZED =  14, // If a Polygon object is not initialized with a plane yet
    TOO_FEW_POLYGON_POINTS  =  15  // Too few points for a polygon (must be at least 3)
};

#endif
