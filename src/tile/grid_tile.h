#ifndef TILE_H
#define TILE_H

#include "vector_tile.h"
#include "../raw_data/geotiff.h"

enum PixelTypes {
    INT_PIXEL,
    FLOAT_PIXEL
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
    GridTile ( uint width );

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
    GridTile ( VectorTile& vector_tile, uint width );

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
       - SUCCESS

       - FILE_NOT_CREATABLE
    */
    int writeBinaryFile ( const char* file_path, int output_type );

    /*
    Read a binary grid file

    Args:
     - file_path : File path of the binary grid file

    Returns:
     - Status code
        - SUCCESS

        - FILE_NOT_FOUND
        - CORRUPT_BINARY_FILE
    */
    int fromBinaryFile ( const char* file_path );



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
        uint factor,
        float (*method)(float*,uint)
    );


    /* GETTERS AND SETTERS */

    /*
    Return the value in the position (x,y) of the grid

    Args:
     - x     : x coordinate
     - y     : y coordinate
     - value : Reference to the float variable to store the value in

    Returns:
     - Status code
        - SUCCESS

        - COORDINATES_OUTSIDE_TILE
    */
    int getValue ( uint x, uint y, float& value ) const;

    /*
    Return the width of the tile
    */
    uint getTileWidth () const;

    /*
    Set the value in the grid in the position (x,y)

    Args:
     - x     : x coordinate
     - y     : y coordinate
     - value : Value to set (x,y) to

    Returns:
     - Status code
        - SUCCESS

        - COORDINATES_OUTSIDE_TILE
    */
    int setValue ( uint x, uint y, float value );


private:
    float* tile;
    uint width;

    char tile_name [9];

    void getBlock (
        float* block_buf,
        uint block_width,
        uint x, uint y
    ) const;
};

/*---------------------------------------------------------------*/

/* Downsample methods to pass to the function downsampleTile */
float max ( float* block, uint width );
float min ( float* block, uint width );
float avg ( float* block, uint width );

#endif
