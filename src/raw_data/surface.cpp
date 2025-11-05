#include "surface.h"

#include <iostream>

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
