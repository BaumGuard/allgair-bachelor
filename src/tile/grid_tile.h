#ifndef TILE_H
#define TILE_H

#include "vector_tile.h"
#include "../raw_data/geotiff.h"

enum PixelTypes {
    INT,
    FLOAT
};

/*
Class to represent a tile (e.g. from a GeoTIFF file)
*/
class GridTile {
public:
    /* CONSTRUCTORS */

    /*
    Create an empty grid tile

    Args:
    - width : Width and height of the grid
    */
    GridTile ( unsigned int width );

    /*
    Create a grid from float values (read from a GeoTIFF file)

    Args:
     - geotiff : GeoTiffFile object to transform into a GridTile
     - width   : Width and height of the grid
    */
    GridTile ( GeoTiffFile& geotiff );

    /*
    Transform a vector tile into a grid tile using raycasting

    Args:
     - vector_tile : Reference to the vector tile to be transformed
     - width       : Width and height of the grid
    */
    GridTile ( VectorTile& vector_tile, unsigned int width );

    /*
    Default constructor
    */
    GridTile ();

    /* DESTRUCTOR */
    ~GridTile ();


    /* INPUT / OUTPUT */

    /*
    Create a binary file of the grid data

    Args:
     - file_path    : File path of the output file
     - output_type  : Data type of the pixels
                       - INT   : 16 bit signed integer
                       - FLOAT : 32 bit float

    Returns:
    - Status code
       - BINARY_FILE_NOT_CREATABLE
       - BINARY_FILE_CREATED (Success)
    */
    int writeBinaryFile ( const char* file_path, int output_type );

    /*
    Read a binary grid file

    Args:
     - file_path : File path of the binary grid file

    Returns:
     - FILE_NOT_FOUND
     - CORRUPT_BINARY_FILE
     - READ_BINARY_FILE_SUCCESS (Success)
    */
    int readBinaryFile ( const char* file_path );



    /*
    Reduce the number of pixels in the tile by a given factor
    and with a given method

    Args:
     - factor  : The width of the subblocks that should be
                 combined to one pixel
     - *method : Function pointer to the function that computes
                 the maximum, minimum or average pixel value of
                 the subblock
    */
    int downsampleTile (
        unsigned int factor,
        float (*method)(float*,unsigned int)
    );


    /* GETTERS AND SETTERS */

    /*
    Return the value in the position (x,y) of the grid

    Args:
     - x : x coordinate
     - y : y coordinate
    */
    float getValue ( unsigned int x, unsigned int y );

    /*
    Return the width of the tile
    */
    unsigned int getTileWidth ();

    /*
    Set the value in the grid in the position (x,y)

    Args:
     - x     : x coordinate
     - y     : y coordinate
     - value : Value to set (x,y) to
    */
    void setValue ( unsigned int x, unsigned int y, float value );


private:
    float* tile;
    unsigned int width;

    char tile_name [9];

    void getBlock (
        float* block_buf,
        unsigned int block_width,
        unsigned int x, unsigned int y
    );
};

/*---------------------------------------------------------------*/

/* Downsample methods to pass to the function downsampleTile */
float max ( float* block, unsigned int width );
float min ( float* block, unsigned int width );
float avg ( float* block, unsigned int width );

#endif
