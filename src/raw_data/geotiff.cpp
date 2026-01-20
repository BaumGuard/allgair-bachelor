#include "geotiff.h"

#include "../status_codes.h"
#include "../utils.h"
#include "../tile/field.h"

#include <tiffio.h>
#include <gdal.h>
#include <iostream>
#include <cmath>

/*---------------------------------------------------------------*/

#define FLOAT_SIZE 4

union sample {
    uint8_t bytes [FLOAT_SIZE];
    float f32;
};

/*---------------------------------------------------------------*/

int GeoTiffFile::readGeoTiffFile ( std::string file_path, int tile_type ) {

    if (
        tile_type != DGM1  && tile_type != DGM20 &&
        tile_type != DOM20 && tile_type != DOM1
    ) {
        return INVALID_TILE_TYPE;
    }

    TIFF* tiff = TIFFOpen( file_path.data(), "r" );
    if ( tiff == NULL ) {
        return FILE_NOT_FOUND;
    }

    std::string file_name = extractFilename( file_path );
    //tile_name = removeFileEnding( file_name );

    uint32_t
        image_width,
        buf_size;

    TIFFGetField( tiff, TIFFTAG_IMAGEWIDTH, &image_width );

    uint8_t* buf;
    data = new float [image_width * image_width];

    //tile_width = image_width;

    union sample current_sample;

    int
        pixel_it = 0,
        dst_it = 0,
        x = 0;


    // In case of a DGM1 GeoTIFF file, the data is presented in lines
    // from top to bottom
    if ( tile_type == DGM1 || tile_type == DGM20 ) {
        buf_size = TIFFScanlineSize(tiff);
        buf = new uint8_t [buf_size];

        for ( size_t i = 0; i < image_width; i++ ) {
            TIFFReadScanline( tiff, buf, i, 0 );

            for ( size_t j = 0; j < buf_size; j++ ) {
                current_sample.bytes[pixel_it] = buf[j];
                pixel_it++;

                if ( pixel_it == FLOAT_SIZE ) {
                    data[dst_it] = current_sample.f32;
                    dst_it++;
                    pixel_it = 0;
                    x++;
                }
            }
            x = 0;
        }
    }

    // In case of a DOM20 GeoTIFF file, the data is presented as a grid
    // of tiles
    if ( tile_type == DOM20 || tile_type == DOM1 ) {
        uint32_t tiff_tile_width;
        TIFFGetField( tiff, TIFFTAG_TILEWIDTH, &tiff_tile_width );

        uint32_t tiles_per_line =
            (uint32_t) ceil( (float)image_width / (float)tiff_tile_width );

        buf_size = tiff_tile_width * tiff_tile_width * sizeof(float);
        buf = new uint8_t [buf_size];


        uint32_t
            x_grid,
            y_grid,
            index;

        for ( size_t y = 0; y < tiles_per_line; y++ ) {
            for ( size_t x = 0; x < tiles_per_line; x++ ) {
                TIFFReadTile( tiff, buf, x, y, 0, 0 );

                for ( size_t i = 0; i < tiff_tile_width; i++ ) {
                    for ( size_t j = 0; j < tiff_tile_width; j++ ) {
                        x_grid = x * tiff_tile_width + j;
                        y_grid = y * tiff_tile_width + i;

                        if ( x_grid >= image_width || y_grid >= image_width ) {
                            break;
                        }

                        memcpy(
                            current_sample.bytes,
                            &buf[i*tiff_tile_width*FLOAT_SIZE+j*FLOAT_SIZE],
                            FLOAT_SIZE
                        );

                        index = y_grid * image_width + x_grid;

                        data[index] = current_sample.f32;
                    }
                }
            }
        }
    }

    TIFFClose( tiff );
    delete[] buf;


    GDALAllRegister();

    GDALDatasetH hDataset = GDALOpen( file_path.data(), GA_ReadOnly );
    if ( hDataset == NULL ) {
        return FILE_NOT_FOUND;
    }

    double gt[6];
    if ( GDALGetGeoTransform(hDataset, gt) != CE_None ) {
        GDALClose( hDataset );
        return FILE_CORRUPT;
    }

    int height = GDALGetRasterYSize( hDataset );

    utm_origin_x = (uint)( gt[0] + 0 * gt[1] + height * gt[2] );
    utm_origin_y = (uint)( gt[3] + 0 * gt[4] + height * gt[5] );
    tile_width = height;

    tile_name = buildTileName( utm_origin_x/1000, utm_origin_y/1000 );

    data_memalloc = true;
    return SUCCESS;
} /* readGeoTiffFile () */

/*---------------------------------------------------------------*/

GeoTiffFile::GeoTiffFile() {}

GeoTiffFile::GeoTiffFile( const GeoTiffFile& old_geotiff ) {
    tile_width = old_geotiff.getTileWidth();
    utm_origin_x = old_geotiff.getUtmOriginX();
    utm_origin_y = old_geotiff.getUtmOriginY();

    uint len = tile_width * tile_width;

    data = new float [len];
    memcpy( data, old_geotiff.getData(), len*FLOAT_SIZE );

    tile_name = old_geotiff.getTileName();

    data_memalloc = true;
} /* GeoTiffFile() */

GeoTiffFile::~GeoTiffFile () {
    if ( data_memalloc ) {
        delete[] data;
    }
} /* ~GeoTiffFile () */

/*---------------------------------------------------------------*/

float* GeoTiffFile::getData () const {
    return data;
} /* getData () */

/*---------------------------------------------------------------*/

std::string GeoTiffFile::getTileName () const {
    return tile_name;
} /* getTileName () */

/*---------------------------------------------------------------*/

uint GeoTiffFile::getTileWidth () const {
    return tile_width;
} /* getTileWidth () */

uint GeoTiffFile::getUtmOriginX () const {
    return utm_origin_x;
} /* getUtmOriginX () */

uint GeoTiffFile::getUtmOriginY () const {
    return utm_origin_y;
} /* getUtmOriginY () */
