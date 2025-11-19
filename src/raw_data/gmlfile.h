#ifndef GMLFILE_H
#define GMLFILE_H

#include <vector>
#include <string>

#include "../geometry/vector.h"
#include "surface.h"


class GmlFile {
public:
    /*
    Read a GML file and extract the surfaces and the coordinates
    of the lower and upper corner

    Args:
    - file_path : Path to the GML file

    Returns:
     - Status code
        - SUCCESS

        - NO_SURFACES
    */
    int readGmlFile ( std::string file_path );


    /* GETTERS */

    /*
    Return the std::vector with the surfaces read from the
    GML file
    */
    std::vector<Surface>& getSurfaces ();

    /*
    Return the lower left corner of the tile as a Vector
    */
    Vector getLowerCorner () const;

    /*
    Return the upper right corner of the tile as a Vector
    */
    Vector getUpperCorner () const;

private:
    Vector
        lower_corner,
        upper_corner;

    std::vector<Surface> surfaces;
};

#endif
