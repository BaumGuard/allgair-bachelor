#ifndef SIGNAL_CONE_H
#define SIGNAL_CONE_H

#include "../geometry/line.h"
#include "../geometry/polygon.h"

/*
Determine the intersection area of a signal cone with the ground plane
by intersecting rays on the outer surface of the cone with the ground plane
Therefore the function doesn't determine the full ellipse on the ground
but rather calculates some samples on the edge of the intersection ellipse
and stores them in a Polygon passed to the function as a reference.

Args:
 - center_ray  : Line in the middle of the cone
                 The function assumes that the base point of center_ray
                 is the starting point of the ray and therefore also the
                 tip of the cone
 - cone_angle  : Angle between the center line of the cone and its outer
                 surface measured at the tip of the cone (in radians)
                 Range: [0, 0.5*PI[
 - n_samples   : Number of points to sample on the intersection ellipse
 - ground_area : Reference to the Polygon to store all sampled points in

Returns:
 - Status code:
    - SUCCESS

    - INVALID_SIGNAL_CONE_ANGLE
    - LINE_ON_PLANE
    - LINE_PARALLEL_TO_PLANE
    - POINT_NOT_ON_BASE_PLANE
    - POINT_DUPLICATE
*/
int signalConeGroundArea (
    Line& center_ray, double cone_angle, int n_samples, Polygon& ground_area );

#endif
