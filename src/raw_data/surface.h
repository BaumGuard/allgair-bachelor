#ifndef SURFACE_H
#define SURFACE_H


#include <string>
#include <vector>

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


/*
Check if two Surface objects are equal (ignoring the ID)

Args:
 - surface1 : Reference to the first Surface object
 - surface2 : Reference to the second Surface object to compare
              surface1 to

Returns:
 - Are surface1 and surface2 equal?
*/
bool surfacesEqual ( Surface& surface1, Surface& surface2 );


/*
Check if a Surface object is already in the surfaces list

Args:
 - surfaces_list   : std::vector of Surface objects
 - compare_surface : Surface object to check if it is already in
                     surfaces_list

Returns:
 - Is compare_surface in surfaces_list?
*/
bool surfaceAlreadyInList (
    std::vector<Surface>& surfaces_list, Surface& compare_surface );


/*
Print a description of the Surface struct to the console

Args:
 - surface : Pointer to a Surface struct
*/
void printSurfaceDescription ( Surface* surface );

#endif
