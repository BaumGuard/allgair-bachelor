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
     - tile_type : Tile type (DGM1, DOM20, DOM20_MASKED)

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

    /*
    Return the x coordinate of tile's coordinate origin (lower left corner)
    */
    uint getUtmOriginX () const;

    /*
    Return the y coordinate of tile's coordinate origin (lower left corner)
    */
    uint getUtmOriginY () const;

private:
    float* data;
    uint tile_width;

    uint
        utm_origin_x,
        utm_origin_y;

    bool data_memalloc = false;

    std::string tile_name;
};
#endif
