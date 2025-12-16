#ifndef PRECALC_H
#define PRECALC_H

#include "../geometry/vector.h"
#include "../geometry/polygon.h"

#include <vector>

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
 - start_point : Start point as UTM coordinates and altitude
 - end_point   : End point as UTM coordinates and altitude

Returns:
 - List of polygons that satisfy the condition above
*/
std::vector<Polygon> precalculate ( Vector& start_point, Vector& end_point );

#endif
