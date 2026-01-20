#ifndef TILE_H
#define TILE_H

#include "vector_tile.h"
#include "../raw_data/geotiff.h"

#if 0
/* Downsample methods to pass to the function resampleTile */
float max ( float* block, uint width );
float min ( float* block, uint width );
float avg ( float* block, uint width );
#endif


enum DownsamplingMethods {
    AVG,
    MIN,
    MAX
};

/*
Class to represent a tile (e.g. from a GeoTIFF file)
*/
class GridTile {
public:
    /* CONSTRUCTORS */

    // Default constructor
    GridTile ();

    // Copy constructor
    GridTile ( const GridTile& old_gridtile );

    /*
    Create an empty grid tile

    Args:
    - width : Width and height of the grid
    */
    void emptyGridTileWithWidth ( uint width );

    /*
    Create a grid from float values (read from a GeoTIFF file)

    Args:
     - geotiff : GeoTiffFile object to transform into a GridTile
    */
    void fromGeoTiffFile ( GeoTiffFile& geotiff );

    /*
    Transform a vector tile into a grid tile using raycasting

    Args:
     - vector_tile : Reference to the vector tile to be transformed
     - width       : Width and height of the grid
    */
    void fromVectorTile ( VectorTile& vector_tile, uint width );


    /* DESTRUCTOR */
    ~GridTile ();

#if 0
    /*
    Upsample or downsample a tile
    Reduce a square of pixels into one or turn one pixel into a
    square of pixels using interpolation

    Args:
     - factor  : Resampling factor
                  - <1 : Downsample
                  - >1 : Upsample
     - *method : Function pointer to the function that computes
                 the maximum, minimum or average pixel value of
                 the subblock (only in case of downsampling)
                 Default method: max
                  - min
                  - max
                  - avg
    */
    void resampleTile (
        float factor,
        float (*method)(float*,uint) = max
    );
#endif

    /*
    Upsample or downsample a tile
    Reduce a block of pixels into one (downsampling) or turn one pixel
    into a block of pixels (upsampling) using lienar interpolation

    Args:
     - factor :              Factor by which the tile should be rescaled
                              -  < 1.0 -> Downsampling
                              - >= 1.0 -> Upsampling
     - downsampling_method : Method to accumulate a block of pixels to find
                             the value of the new pixel when downsampling
                              - AVG : Average value of the block
                              - MIN : Minimum value within the block
                              - MAX : Maximum value within the block
                              (Default: MAX)

    Returns:
     - Status code
        - SUCCESS

        - INVALID_RESAMPLING_FACTOR
    */
    int resampleTile ( float factor, int downsampling_method = MAX );


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
    Return the name of the tile

    Returns:
     - Name of the tile
    */
    std::string getTileName () const;

    /*
    Return the pointer to the tile data

    Returns:
     - Pointer to the array 'tile'
    */
    float* getData () const;

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


    /*
    Create a tif file from the tile

    Args:
     - file_path : File path of the output tif

    Returns:
     - Status code:
        - SUCCESS

        - FILE_NOT_CREATABLE
    */
    int createTifFile ( std::string file_path );


private:
    float* tile;
    bool tile_memalloc = false;

    uint width;

    std::string tile_name;

#if 0
    void getBlock (
        float* block_buf,
        uint block_width,
        uint x, uint y
    ) const;
#endif

    /*
    Accumulate a block of pixels for downsampling using
    the specified downsampling method

    Args:
     - x_start             : Starting x coordinate of the block (lower left corner)
     - y_start             : Starting y coordinate of the block (lower left corner)
     - x_end               : End x coordinate of the block
                             (upper right corner) (not in iteration included)
     - y_end               : End y coordinate of the block
                             (upper right corner) (not in iteration included)
     - downsampling_method : Method to accumulate a block of pixels to find
                             the value of the new pixel when downsampling
                              - AVG : Average value of the block
                              - MIN : Minimum value within the block
                              - MAX : Maximum value within the block
                              (Default: MAX)
                              -> See enum DownsamplingMethods

    Returns:
     - Downsampled pixel block as a float value
    */
    float block_accumulate (
        int x_start, int y_start,
        int x_end, int y_end,
        int downsampling_method
    );
};

#endif
