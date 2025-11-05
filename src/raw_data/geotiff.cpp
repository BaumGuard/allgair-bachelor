#include "geotiff.h"

#include "../status_codes.h"
#include "../utils.h"

#include <tiffio.h>
#include <iostream>

/*---------------------------------------------------------------*/

union tiff_data {
    float f32;

    uint8_t bytes [8];
};

/*---------------------------------------------------------------*/

GeoTiffFile::GeoTiffFile ( const char* file_path ) {
    char file_name [32];
    extractFilepath( file_name, (char*)file_path );

    removeFileEnding( tile_name, file_name, 8 );

    uint32_t
        width,
        buf_size;

    TIFF* tif = TIFFOpen( file_path, "r" );

    if ( tif == NULL ) {
        std::cout << "TIF not found\n";
        //return FILE_NOT_FOUND;
    }

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );
    tile_width = width;

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

    //return CREATION_SUCCEEDED;
} /* GeoTiffFile ( const char* file_path ) */

/*---------------------------------------------------------------*/

GeoTiffFile::~GeoTiffFile () {
    delete[] data;
} /* ~GeoTiffFile() */

/*---------------------------------------------------------------*/

float* GeoTiffFile::getData () {
    return data;
} /* getData() */

/*---------------------------------------------------------------*/

char* GeoTiffFile::getTileName () {
    return tile_name;
} /* getTileName() */

/*---------------------------------------------------------------*/

uint GeoTiffFile::getTileWidth () {
    return tile_width;
} /* getTileWidth() */
