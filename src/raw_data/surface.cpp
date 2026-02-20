#include "surface.h"

#include <iostream>

/*---------------------------------------------------------------*/

bool surfacesEqual ( Surface& surface1, Surface& surface2 ) {
    if (
        surface1.surface_type       != surface2.surface_type        ||
        surface1.area               != surface2.area                ||
        surface1.z_max              != surface2.z_max               ||
        surface1.z_max_asl          != surface2.z_max_asl           ||
        surface1.z_min              != surface2.z_min               ||
        surface1.z_min_asl          != surface2.z_min_asl           ||
        surface1.roof_tilt          != surface2.roof_tilt           ||
        surface1.roof_orientation   != surface2.roof_orientation
    ) {
        return false;
    }

    if ( surface1.pos_list.size() != surface2.pos_list.size() ) {
        return false;
    }

    uint len = surface1.pos_list.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( surface1.pos_list[i] != surface2.pos_list[i] ) {
            return false;
        }
    }

    return true;
} /* surfacesEqual() */

/*---------------------------------------------------------------*/

bool surfaceAlreadyInList (
    std::vector<Surface>& surfaces_list, Surface& compare_surface ) {

    uint len = surfaces_list.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( surfacesEqual(surfaces_list[i], compare_surface) ) {
            return true;
        }
    }
    return false;
} /* surfaceAlreadyInList() */

/*---------------------------------------------------------------*/

void printSurfaceDescription ( Surface* surface ) {
    std::string surface_type;

    switch ( surface->surface_type ) {
        case ROOF:
            surface_type = "Roof";
            break;

        case WALL:
            surface_type = "Wall";
            break;

        case GROUND:
            surface_type = "Ground";
            break;

        default:
            surface_type = "Other";
    }

    std::cout
        << "------------------------------------------------\n"
        << "Surface ID   : " << surface->id    << '\n'
        << "Surface Type : " << surface_type   << '\n'
        << "\n"
        << "Area         : " << surface->area  << " m²\n"
        << "Z_MAX        : " << surface->z_max << '\n'
        << "Z_MAX_ASL    : " << surface->z_max_asl << '\n'
        << "Z_MIN        : " << surface->z_min << '\n'
        << "Z_MIN_ASL    : " << surface->z_min_asl << "\n\n";

    std::cout << "Points:\n";

    unsigned long len = surface->pos_list.size();
    for ( unsigned long i=0; i<len; i++ ) {
        std::cout
            << "    ( " << surface->pos_list[i].getX() << ", "
            << surface->pos_list[i].getY() << ", "
            << surface->pos_list[i].getZ() << " )\n";
    }

    if ( surface->surface_type == ROOF ) {
        std::cout
            << "Roof tilt : " << surface->roof_tilt << '\n'
            << "Roof orientation : " << surface->roof_orientation << "\n\n";
    }

} /* printSurfaceDescription() */
