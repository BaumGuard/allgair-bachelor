#ifndef GMLFILE_H
#define GMLFILE_H

#include <vector>
#include <string>

#include "../geometry/vector.h"
#include "surface.h"
#include "../geometry/polygon.h"

class GmlFile {
public:
    /*
    CONSTRUCTOR

    Extract the necessary data from a given GML file

    Args:
    - file_path : Path to the GML file
    */
    GmlFile ( std::string file_path );


    /* GETTERS */

    /*
    Return the std::vector with the surfaces read from the
    GML file
    */
    std::vector<Surface>& getSurfaces ();

    /*
    Return the lower left corner of the tile as a Vector
    */
    Vector getLowerCorner ();

    /*
    Return the upper right corner of the tile as a Vector
    */
    Vector getUpperCorner ();

private:
    Vector
        lower_corner,
        upper_corner;

    std::vector<Surface> surfaces;
};

#endif
