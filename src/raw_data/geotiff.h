#ifndef GEOTIFF_H
#define GEOTIFF_H

#include "../utils.h"

/*
Class to read and save the content of a GeoTIFF file
*/
class GeoTiffFile {
public:
    /* CONSTRUCTOR */

    /*
    Create a GeoTiffFile object from a GeoTIFF file

    Args:
     - file_path : File path to the GeoTIFF file
    */
    GeoTiffFile( std::string file_path );

    /* DESTRUCTOR */
    ~GeoTiffFile();


    /* GETTERS */

    /*
    Return the array with the data from the GeoTIFF file
    */
    float* getData ();

    /*
    Return the tile name
    */
    std::string getTileName ();

    /*
    Return the width of the tile
    */
    uint getTileWidth ();

private:
    float* data;
    uint tile_width;

    std::string tile_name;
};
#endif
