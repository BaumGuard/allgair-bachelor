#ifndef PRECALC_H
#define PRECALC_H

#include "../geometry/vector.h"
#include "../geometry/polygon.h"

#include <vector>

// Select methods
enum SelectMethods {
    BY_MIN_DISTANCE,
    BY_MAX_AREA
};


/*
Find all polygons in the Fresnel zone between the start and the end point
that satisfy the following condition:
For every polygon in the Fresnel zone the algorithm first creates a line
between the polygon's centroid and the end point and uses the direction
vector of this line and the end point to create a plane perpendicular to
the line.
The algorithm proceeds by reflecting every corner point of the polygon
and the centroid onto the newly created plane.
If the reflected centroid is inside the reflected polygon, the algorithm
adds the polygon to the list of selected polygons.

Args:
 - start_point   : Start point as UTM coordinates and altitude
 - end_points    : List of end points as UTM coordinates and altitude
 - select_method : Choose which of the polygons should be returned:
                    - BY_MIN_DISTANCE : The polygon that is closest to
                                        the starting point
                    - BY_MAX_AREA     : The polygon with the largest area
 - fresnel_zone  : Number of the Fresnel zone (Default: 2)
 - freq          : Frequency of the signal in Hz

Returns:
 - List of polygons that satisfy the condition above
*/
int precalculate (
    Polygon& selected_polygon,
    Vector& start_point,
    Vector& end_point,
    int select_method,
    int fresnel_zone = 2,
    double freq = 868.0e6,
    int threads = 10
);

#endif
