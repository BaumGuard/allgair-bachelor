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

    /*
    Default constructor
    */
    VectorTile ();


    /* GETTERS */

    /*
    Return the lower left corner of the tile as a Vector
    */
    Vector getLowerCorner ();

    /*
    Return the upper right corner of the tile as a Vector
    */
    Vector getUpperCorner ();

    /*
    Return a reference to the std::vector with the Polygon objects
    */
    std::vector<Polygon>& getPolygons ();

private:
    std::vector<Polygon> polygons;

    Vector
        lower_corner,
        upper_corner;
};

#endif
