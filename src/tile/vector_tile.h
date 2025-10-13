#ifndef VECTOR_TILE_H
#define VECTOR_TILE_H

#include "../geometry/polygon.h"
#include "../raw_data/gmlfile.h"

#include <vector>

/*
Class to save vector data (surfaces) read from a GML file
*/
class VectorTile {
public:
    /* CONSTRUCTORS */

    /*
    Read vector data from a GML file and convert it to an
    std::vector of Polygon objects

    Args:
     - gmlfile : GmlFile instance
    */
    VectorTile ( GmlFile& gmlfile );
    VectorTile ();

private:
    std::vector<Polygon> polygons;
};

#endif
