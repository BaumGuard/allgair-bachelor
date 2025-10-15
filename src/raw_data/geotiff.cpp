#include "geotiff.h"

#include "../status_codes.h"

#include <stdint.h>
#include <tiffio.h>

/*---------------------------------------------------------------*/

union tiff_data {
    float f32;

    uint8_t bytes [8];
};

/*---------------------------------------------------------------*/

GeoTiffFile::GeoTiffFile ( char* file_path ) {
    uint32_t
        width,
        buf_size;

    TIFF* tif = TIFFOpen( file_path, "r" );
    if ( !tif ) {
        return FILE_NOT_FOUND;
    }

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );

    buf_size = TIFFScanlineSize(tif);
    uint8_t* buf = new uint8_t[buf_size];

    data = new float [width*width];

    union tiff_data pixel;

    int pixel_it = 0;
    int x = 0;

    int dst_it = 0;

    for ( uint32_t i=0; i<width; i++ ) {
        TIFFReadScanline( tif, buf, i, 0 );

        for ( uint32_t j=0; j<buf_size; j++ ) {
            pixel.bytes[pixel_it] = buf[j];
            pixel_it++;

            if ( pixel_it == 4 ) {
                data[dst_it] = pixel.f32;
                dst_it++;
                pixel_it = 0;
                x++;
            }
        }

        x = 0;
    }

    TIFFClose( tif );
    delete[] buf;

    return CREATION_SUCCEEDED;
} /* GeoTiffFile::GeoTiffFile ( char* file_path ) */

/*---------------------------------------------------------------*/

GeoTiffFile::~GeoTiffFile () {
    delete[] data;
}

/*---------------------------------------------------------------*/

float* GeoTiffFile::getData () {
    return data;
}

/*---------------------------------------------------------------*/

unsigned int getTileWidth () {
    return tile_width;
}
