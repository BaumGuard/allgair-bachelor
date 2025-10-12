#include "tile.h"

#include "../status_codes.h"

#include <stdint.h>
#include <stdlib.h>
#include <tiffio.h>

/*---------------------------------------------------------------*/

union tiff_data {
    float f32;

    uint8_t bytes [8];
};

/*---------------------------------------------------------------*/

Tile::Tile ( unsigned int width ) {
    this->width = width;

    tile = new float [width*width];
} /* Tile::Tile ( unsigned int width ) */

Tile::Tile () {}

/*---------------------------------------------------------------*/

Tile::~Tile () {
    delete tile;
} /* Tile::~Tile () */

/*---------------------------------------------------------------*/

int Tile::createTileFromGeoTIFF ( char* file_path ) {
    uint32_t
        width,
        buf_size;

    TIFF* tif = TIFFOpen( file_path, "r" );
    if ( !tif ) {
        return FILE_NOT_FOUND;
    }

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );

    tile = new float [width*width];

    buf_size = TIFFScanlineSize(tif);

    uint8_t* buf = (uint8_t*)malloc( buf_size );

    union tiff_data pixel;

    int pixel_it = 0;
    int x = 0;

    for ( uint32_t i=0; i<width; i++ ) {
        TIFFReadScanline( tif, buf, i, 0 );

        for ( uint32_t j=0; j<buf_size; j++ ) {
            pixel.bytes[pixel_it] = buf[j];
            pixel_it++;

            if ( pixel_it == 4 ) {
                setValue( x, i, pixel.f32 );
                pixel_it = 0;
                x++;
            }
        }

        x = 0;
    }

    TIFFClose( tif );
    free( buf );

    return CREATION_SUCCEEDED;
} /* int Tile::createTileFromGeoTIFF ( char* file_path ) */

/*---------------------------------------------------------------*/

float Tile::getValue ( unsigned int x, unsigned int y ) {
    return tile[width*y+x];
} /* float Tile::getValue ( unsigned int x, unsigned int y ) */

void Tile::setValue ( unsigned int x, unsigned int y, float value ) {
    tile[width*y+x] = value;
} /* void Tile::setValue ( unsigned int x, unsigned int y, float value ) */

/*---------------------------------------------------------------*/

int Tile::getTileWidth () {
    return width;
} /* int Tile::getTileWidth () */

/*---------------------------------------------------------------*/

void Tile::getBlock (
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
} /* void Tile::getBlock */

/*---------------------------------------------------------------*/

int Tile::downsampleTile (
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
} /* int Tile::downsampleTile */

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
