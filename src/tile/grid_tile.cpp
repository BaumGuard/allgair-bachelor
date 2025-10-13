#include "grid_tile.h"

#include "../exceptions/exception.h"
#include "../status_codes.h"

#include <stdint.h>
#include <stdlib.h>

/*---------------------------------------------------------------*/

GridTile::GridTile ( unsigned int width ) {
    this->width = width;

    tile = new float [width*width];
} /* GridTile::GridTile ( unsigned int width ) */

GridTile::GridTile ( unsigned int width, float* values ) {
    this->width = width;

    tile = new float [width*width];

    int len = width * width;
    for ( int i=0; i<len; i++ ) {
        tile[i] = values[i];
    }
} /* GridTile::GridTile ( unsigned int width, float* values ) */

GridTile::GridTile () {}

/*---------------------------------------------------------------*/

GridTile::~GridTile () {
    delete tile;
} /* GridTile::~GridTile () */

/*---------------------------------------------------------------*/

float GridTile::getValue ( unsigned int x, unsigned int y ) {
    if ( x >= width || y >= width ) {
        throw OutsideOfTileException( "test" );
    }
    return tile[width*y+x];
} /* float GridTile::getValue ( unsigned int x, unsigned int y ) */

void GridTile::setValue ( unsigned int x, unsigned int y, float value ) {
    if ( x >= width || y >= width ) {
        //throw OutsideOfGridException();
    }
    tile[width*y+x] = value;
} /* void GridTile::setValue ( unsigned int x, unsigned int y, float value ) */

/*---------------------------------------------------------------*/

unsigned int GridTile::getTileWidth () {
    return width;
} /* int GridTile::getGridTileWidth () */

/*---------------------------------------------------------------*/

void GridTile::getBlock (
    float* block_buf, unsigned int block_width,
    unsigned int x, unsigned int y
) {
    uint32_t buf_it = 0;

    for ( uint32_t i=y; i<y+block_width; i++ ) {
        for ( uint32_t j=x; j<x+block_width; j++ ) {
            block_buf[buf_it] = getValue( j, i );
            buf_it++;
        }
    }
} /* void GridTile::getBlock */

/*---------------------------------------------------------------*/

int GridTile::downsampleTile (
    unsigned int factor,
    float (*method)(float*,unsigned int)
) {
    if ( width % factor != 0 ) {
        return 1;
    }

    uint32_t step = width / factor;

    float* subblock = (float*) malloc( step*step*sizeof(float) );

    static float* new_tile = new float [step*step];
    int new_tile_it = 0;

    float new_value;

    for ( unsigned int i=0; i<width; i+=step ) {
        for ( unsigned int j=0; j<width; j+=step ) {
            getBlock( subblock, step, j, i );

            new_value = method( subblock, step );
            new_tile[new_tile_it] = new_value;
            new_tile_it++;
        }
    }


    delete tile;
    tile = new_tile;
    width = step;

    free( subblock );

    return 0;
} /* int GridTile::downsampleGridTile */

/*---------------------------------------------------------------*/

float max ( float* block, unsigned int width ) {
    int len = width * width;

    float max_value = block[0];

    for ( int i=1; i<len; i++ ) {
        if ( block[i] > max_value ) {
            max_value = block[i];
        }
    }

    return max_value;
} /* float max ( float* block, unsigned int width ) */

float min ( float* block, unsigned int width ) {
    int len = width * width;

    float min_value = block[0];

    for ( int i=1; i<len; i++ ) {
        if ( block[i] < min_value ) {
            min_value = block[i];
        }
    }

    return min_value;
} /* float min ( float* block, unsigned int width */

float avg ( float* block, unsigned int width ) {
    int len = width * width;

    float sum = 0.0;

    for ( int i=0; i<len; i++ ) {
        sum += block[i];
    }

    return sum / (float)len;
} /* float avg ( float* block, unsigned int width ) */
