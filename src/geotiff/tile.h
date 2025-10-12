#ifndef TILE_H
#define TILE_H

/*
Class to represent a tile (e.g. from a GeoTIFF file)
*/
class Tile {
public:
    /* CONSTRUCTORS */
    Tile ( unsigned int width );
    Tile ();

    /* DESTRUCTOR */
    ~Tile ();

    /*
    Create a Tile object from a GeoTIFF file

    Args:
     - file_path : File path of the GeoTIFF file
    */
    int createTileFromGeoTIFF ( char* file_path );

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
    float getValue ( unsigned int x, unsigned int y );
    void setValue ( unsigned int x, unsigned int y, float value );

    int getTileWidth ();

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
