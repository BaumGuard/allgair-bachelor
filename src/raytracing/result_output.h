#ifndef RESULT_OUTPUT_H
#define RESULT_OUTPUT_H

#include <ctime>
#include <cstdio>
#include <vector>

#include "../geometry/vector.h"
#include "../geometry/polygon.h"


/*
Create a JSON result file for raytracing with reflection
The result files will be written into /results
The name pattern for the files is as follows (strftime notation):
    Result_%Y%m%d_%H%M%S.json

Args:
 - start_point        : Starting point of the raytracing
 - end_points         : List of end points of the raytracing
 - reflection_point   : Point on a polygon on which the point
                        was reflected to reach the end point
 - reflecting_polygon : Reference to the polygon that reflected
                        the ray to the end point
 - distance           : Sum of the distances in meters between
                         - Start point and reflection point
                         - Reflection point and end point
 - vegetation_count   : Vegetation count
 - ground_count       : Ground count
 - selection_method   : Polygon selection method used in the
                        precalculator (see enum SelectionMethods)
                         - MAX_AREA
                         - MIN_DISTANCE

Returns:
 - Status code
    - SUCCESS

    - FILE_NOT_CREATABLE
*/
int createResultFile_WithReflection (
    Vector& start_point, std::vector<Vector> end_points,
    Vector& reflection_point,
    Polygon& reflecting_polygon,
    float distance,
    uint vegetation_count, uint ground_count,
    uint selection_method,
    uint fresnel_zone
);


/*
Create a JSON result file for direct raytracing
The result files will be written into /results
The name pattern for the files is as follows (strftime notation):
    Result_%Y%m%d_%H%M%S.json

Args:
 - start_point        : Starting point of the raytracing
 - end_points         : List of end points of the raytracing
 - distance           : Sum of the distances in meters between
                         - Start point and reflection point
                         - Reflection point and end point
 - vegetation_count   : Vegetation count
 - ground_count       : Ground count

Returns:
 - Status code
    - SUCCESS

    - FILE_NOT_CREATABLE
*/
int createResultFile_Direct (
    Vector& start_point, std::vector<Vector> end_points,
    float distance,
    uint vegetation_count, uint ground_count
);

#endif
