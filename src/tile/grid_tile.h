#ifndef TILE_H
#define TILE_H

/*
Class to represent a tile (e.g. from a GeoTIFF file)
*/
class GridTile {
public:
    /* CONSTRUCTORS */
    GridTile ( unsigned int width );
    GridTile ( unsigned int width, float* values );
    GridTile ();

    /* DESTRUCTOR */
    ~GridTile ();

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
