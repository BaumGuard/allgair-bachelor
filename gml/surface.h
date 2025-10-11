#ifndef SURFACE_H
#define SURFACE_H


#include <string>
#include <vector>
#include <iostream>

#include "../geometry/vector.h"

/*---------------------------------------------------------------*/

/*
Types of surfaces
*/
enum SurfaceTypes {
    GROUND,
    ROOF,
    WALL,
    OTHER
};

/*---------------------------------------------------------------*/

/*
struct Surface to store surface properties
*/
typedef struct {
    std::string id;

    int surface_type;

    double
        area,
        z_max,
        z_max_asl,
        z_min,
        z_min_asl;

    double
        roof_tilt,
        roof_orientation;

    std::vector<Vector> pos_list;
} Surface;

/*---------------------------------------------------------------*/

/*
Print a description of the Surface struct to the console

Args:
 - surface : Pointer to a Surface struct
*/
void printSurfaceDescription ( Surface* surface );

#endif
