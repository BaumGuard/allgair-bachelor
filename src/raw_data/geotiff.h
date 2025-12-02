#ifndef GEOTIFF_H
#define GEOTIFF_H

#include "../utils.h"

/*
Class to read and save the content of a GeoTIFF file
*/
class GeoTiffFile {
public:
    /* CONSTRUCTORS */

    // Default constructor
    GeoTiffFile();

    /* Copy constructor */
    GeoTiffFile( const GeoTiffFile& old_geotiff );

    /* DESTRUCTOR */
    ~GeoTiffFile();

    /*
    Create a GeoTiffFile object from a GeoTIFF file

    Args:
     - file_path : File path to the GeoTIFF file

    Returns:
     - Status code
        - SUCCESS

        - INVALID_TILE_TYPE
        - FILE_NOT_FOUND
    */
    int readGeoTiffFile( std::string file_path, int tile_type );

    /* GETTERS */

    /*
    Return the array with the data from the GeoTIFF file
    */
    float* getData () const;

    /*
    Return the tile name
    */
    std::string getTileName () const;

    /*
    Return the width of the tile
    */
    uint getTileWidth () const;

private:
    float* data;
    uint tile_width;

    bool data_memalloc = false;

    std::string tile_name;
};
#endif
