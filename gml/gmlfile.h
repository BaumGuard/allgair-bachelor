#ifndef GMLFILE_H
#define GMLFILE_H

#include <vector>
#include <string>

#include "../geometry/vector.h"
#include "surface.h"

class GmlFile {
public:
    /*
    CONSTRUCTOR

    Extract the necessary data from a given GML file

    Args:
    - file_path : Path to the GML file
    */
    GmlFile ( std::string file_path );

private:
    Vector
        lower_corner,
        upper_corner;

    std::vector<Surface> surfaces;
};

#endif
