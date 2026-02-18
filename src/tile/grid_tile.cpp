#include "grid_tile.h"

#include "../status_codes.h"
#include "../utils.h"
#include "../config/global_config.h"

#include <cstdint>
#include <iostream>
#include <cstring>
#include <tiffio.h>
#include <cmath>
#include <pthread.h>

/*---------------------------------------------------------------*/

GridTile::GridTile () {}

GridTile::GridTile( const GridTile& old_gridtile ) {
    width = old_gridtile.getTileWidth();
    tile_name = old_gridtile.getTileName();

    tile_origin = old_gridtile.getOrigin();

    uint len = width * width;
    tile = new float [len];
    memcpy( tile, old_gridtile.getData(), len*4 );

    tile_memalloc = true;
} /* GridTile() */

GridTile::~GridTile () {
    if ( tile_memalloc ) {
        delete[] tile;
    }
} /* ~GridTile() */


/*---------------------------------------------------------------*/

void GridTile::emptyGridTileWithWidth ( uint width ) {
    this->width = width;

    tile = new float [width*width];
    tile_memalloc = true;
} /* emptyGridTileWithWidth () */

void GridTile::fromGeoTiffFile ( GeoTiffFile& geotiff ) {
    this->width = geotiff.getTileWidth();
    float* values = geotiff.getData();
    tile_name = geotiff.getTileName();

    tile_origin.setX( (double)geotiff.getUtmOriginX() );
    tile_origin.setY( (double)geotiff.getUtmOriginY() );

    int len = width * width;

    tile = new float [len];

    // Reverse the order of the lines
    // In TIFF files the lines are presented from top to bottom
    // whereas in our case we access them from bottom to top
    // since the latitude increases from the equator towards the
    // poles.
    for ( int i=0; i<len; i++ ) {
        tile[(width-(i/width)-1)*width+(i%width)] = values[i];
    }


    tile_memalloc = true;
} /* fromGeoTiffFile () */
#if 0
void GridTile::fromVectorTile ( VectorTile& vector_tile, uint width ) {
    this->width = width;
    tile = new float [width*width];

    Vector
        lower_corner = vector_tile.getLowerCorner(),
        upper_corner = vector_tile.getUpperCorner();

    double
        x_step = ( upper_corner.getX() - lower_corner.getX() ) / (double)width,
        y_step = ( upper_corner.getY() - lower_corner.getY() ) / (double)width;

    double
        x_end = upper_corner.getX(),
        y_end = upper_corner.getY();

    int
        x_it = 0,
        y_it = 0;

    Line ray;

    Vector
        ray_start,
        ray_dir ( 0.0, 0.0, 1.0 );

    std::vector<Polygon>& polygons = vector_tile.getPolygons();
    int len_polygons = polygons.size();

    int status;
    Vector intersect;

    Plane base_plane;

    for ( double y=0.0f; y<y_end; y+=y_step ) {
        for ( double x=0.0f; x<x_end; x+=x_step ) {
            for ( int i=0; i<len_polygons; i++ ) {
                ray_start = Vector( x, y, 0.0 );

                ray.createLineFromBaseAndVector( ray_start, ray_dir );

                base_plane = polygons[i].getBasePlane();

                status = base_plane.lineIntersection( ray, intersect );

                if ( status == INTERSECTION_FOUND ) {
                    if ( polygons[i].isPointInPolygon(intersect) ) {
                        tile[y_it*width+x_it] = intersect.getZ();
                        break;
                    }
                }
                else {
                    tile[y_it*width+x_it] = -1.0;
                }
            }
            x_it++;
        }
        y_it++;
    }

    tile_memalloc = true;
} /* fromVectorTile() */
#endif
/*---------------------------------------------------------------*/

union output_data {
    float f32;
    int16_t i16;

    char bytes [4];
};

/*---------------------------------------------------------------*/

int GridTile::getValue ( uint x, uint y, float& value ) const {
    if ( x >= width || y >= width ) {
        return COORDINATES_OUTSIDE_TILE;
    };
    value = tile[y*width+x];

    return SUCCESS;
} /* getValue () */

int GridTile::setValue ( uint x, uint y, float value ) {
    if ( x >= width || y >= width ) {
        return COORDINATES_OUTSIDE_TILE;
    }

    tile[width*y+x] = value;

    return SUCCESS;
} /* setValue() */

/*---------------------------------------------------------------*/

Vector GridTile::getOrigin () const {
    return tile_origin;
} /* getOrigin() */

/*---------------------------------------------------------------*/

uint GridTile::getTileWidth () const {
    return width;
} /* getGridTileWidth() */


/*---------------------------------------------------------------*/


struct indices {
    uint start;
    uint end;
};

GridTile* global_grid_tile;
VectorTile* global_vector_tile;

void* Thread_maskTile ( void* arg ) {
    struct indices* thread_data = (struct indices*) arg;

    std::vector<Polygon>& polygon_list = global_vector_tile->getPolygons();

    Vector tile_origin = global_grid_tile->getOrigin();

    for ( uint i = thread_data->start; i < thread_data->end; i++ ) {
        if ( polygon_list[i].getSurfaceType() != GROUND ) {
            continue;
        }

        double
            min_utmx = polygon_list[i].getPoints()[0].getX(),
            max_utmx = polygon_list[i].getPoints()[0].getX(),
            min_utmy = polygon_list[i].getPoints()[0].getY(),
            max_utmy = polygon_list[i].getPoints()[0].getY();

        uint len_point_list = polygon_list[i].getPoints().size();
        for ( uint j = 1; j < len_point_list; j++ ) {
            if ( polygon_list[i].getPoints()[j].getX() < min_utmx ) {
                min_utmx = polygon_list[i].getPoints()[j].getX();
            }
            if ( polygon_list[i].getPoints()[j].getX() > max_utmx ) {
                max_utmx = polygon_list[i].getPoints()[j].getX();
            }
            if ( polygon_list[i].getPoints()[j].getY() < min_utmy ) {
                min_utmy = polygon_list[i].getPoints()[j].getY();
            }
            if ( polygon_list[i].getPoints()[j].getY() > max_utmy ) {
                max_utmy = polygon_list[i].getPoints()[j].getY();
            }
        }


        double
            start_x = min_utmx - fmod( min_utmx, 0.2 ),
            end_x   = max_utmx - fmod( max_utmx, 0.2 ),
            start_y = min_utmy - fmod( min_utmy, 0.2 ),
            end_y   = max_utmy - fmod( max_utmy, 0.2 );

        Vector test_point( 0.0, 0.0, 0.0 );

        for ( double y = start_y; y <= end_y; y += 0.2 ) {
            for ( double x = start_x; x <= end_x; x += 0.2 ) {
                test_point.setX( x );
                test_point.setY( y );

                if ( polygon_list[i].isPointInPolygon(test_point, true) ) {
                    Vector coord_in_tile = ( test_point - tile_origin ) / 0.2;

                    int
                        tile_x = (uint) coord_in_tile.getX(),
                        tile_y = (uint) coord_in_tile.getY();

                    if ( tile_x < 0 || tile_y < 0 ) {
                        continue;
                    }

                    for ( int y_kernel = tile_y-10; y_kernel <= tile_y+10; y_kernel++ ) {
                        for ( int x_kernel = tile_x-10; x_kernel <= tile_x+10; x_kernel++ ) {
                            global_grid_tile->setValue( x_kernel, y_kernel, -9999.0 );
                        }
                    }

                }
            }
        }
    }
    return NULL;
} /* Thread_maskTile() */

void GridTile::maskTile ( VectorTile& vector_tile, int n_threads ) {
    if ( n_threads <= 1 ) {
        n_threads = MAX_THREADS;
    }

    global_grid_tile = this;
    global_vector_tile = &vector_tile;
    uint len_polygon_list = vector_tile.getPolygons().size();

    double part_size = (double) len_polygon_list / (double) n_threads;
    double start = 0.0;

    struct indices* data = new struct indices [n_threads];
    pthread_t* threads = new pthread_t [n_threads];

    for ( int i = 0; i < n_threads; i++ ) {
        data[i].start = start;
        start += part_size;
        data[i].end = start;

        pthread_create( &threads[i], NULL, Thread_maskTile, (void*)&data[i] );
    }

    for ( int i = 0; i < n_threads; i++ ) {
        pthread_join( threads[i], NULL );
    }

    delete[] data;
    delete[] threads;
}

/*---------------------------------------------------------------*/
#if 0
void GridTile::getBlock (
    float* block_buf, uint block_width,
    uint x, uint y
) const {
    uint32_t buf_it = 0;

    for ( uint32_t i=y; i<y+block_width; i++ ) {
        for ( uint32_t j=x; j<x+block_width; j++ ) {
            getValue( j, i, block_buf[buf_it] );
            buf_it++;
        }
    }
} /* getBlock() */
#endif
/*---------------------------------------------------------------*/

std::string GridTile::getTileName () const {
    return tile_name;
} /* getTileName() */

/*---------------------------------------------------------------*/

float* GridTile::getData () const {
    return tile;
} /* getData() */

/*---------------------------------------------------------------*/

float GridTile::block_accumulate (
    int x_start, int y_start,
    int x_end, int y_end,
    int downsampling_method
) {
    float sum = 0.0, cnt = 0, min, max;
    switch ( downsampling_method ) {
        case AVG:
            for ( int y = y_start; y < y_end; y++ ) {
                for ( int x = x_start; x < x_end; x++ ) {
                    sum += tile[y*width+x];
                    cnt++;
                }
            }

            return sum / cnt;


        case MIN:
            min = tile[y_start*width+x_start];
            for ( int y = y_start; y < y_end; y++ ) {
                for ( int x = x_start; x < x_end; x++ ) {
                    if ( tile[y*width+x] < min ) {
                        min = tile[y*width+x];
                    }
                }
            }

            return min;


        case MAX:
            max = tile[y_start*width+x_start];
            for ( int y = y_start; y < y_end; y++ ) {
                for ( int x = x_start; x < x_end; x++ ) {
                    if ( tile[y*width+x] > max ) {
                        max = tile[y*width+x];
                    }
                }
            }

            return max;

        default:
            return tile[y_start*width+x_start];
    }
} /* block_accumulate() */


int GridTile::resampleTile ( float factor, int downsampling_method ) {
    int new_width = (int)( width * factor );
    float* new_tile = new float[new_width * new_width];

    if ( factor == 1.0 ) {
        return SUCCESS;
    }
    if ( factor <= 0.0 ) {
        return INVALID_RESAMPLING_FACTOR;
    }

    // Downsampling
    if ( factor < 1.0 ) {

        float step = 1.0 / factor;
        float x_idx = 0.0, y_idx = 0.0;

        for ( int y = 0; y < new_width; y++ ) {
            for ( int x = 0; x < new_width; x++ ) {
                float new_value = block_accumulate(
                    (int)x_idx, (int)y_idx,
                    (int)(x_idx+step), (int)(y_idx+step),
                    downsampling_method
                );
                x_idx += step;

                new_tile[y*new_width+x] = new_value;
            }
            x_idx = 0.0;
            y_idx += step;
        }
    } /* Downsampling */

    // Upsampling
    else {

        float step = factor;

        float x_it = 0.0, y_it = 0.0;
        uint x_block_start, x_block_end, y_block_start, y_block_end;
        uint block_width, block_height;

        float height_x, height_y, height_avg;

        for ( uint y_outer = 0; y_outer < width; y_outer++ ) {
            for ( uint x_outer = 0; x_outer < width; x_outer++ ) {
                x_block_start = (int)x_it;
                x_block_end = (int)( x_it + step );
                y_block_start = (int)y_it;
                y_block_end = (int)( y_it + step);

                block_width = x_block_end - x_block_start;
                block_height = y_block_end - y_block_start;

                if ( y_outer < width - 1 && x_outer < width - 1 ) {
                    for ( uint y_inner = 0; y_inner < block_height; y_inner++ ) {
                        height_y =
                            ( tile[y_outer*width+x_outer+1] - tile[y_outer*width+x_outer] ) /
                            block_height * y_inner + tile[y_outer*width+x_outer];

                        for ( uint x_inner = 0; x_inner < block_width; x_inner++ ) {
                            height_x =
                                ( tile[(y_outer+1)*width+x_outer] - tile[y_outer*width+x_outer] ) /
                                block_width * x_inner + tile[y_outer*width+x_outer];

                            height_avg = ( height_x + height_y ) / 2.0;

                            new_tile[(y_block_start+y_inner)*new_width+x_block_start+x_inner] = height_avg;
                        }
                    }
                }
                else {
                    for ( uint y_inner = 0; y_inner < block_height; y_inner++ ) {
                        for ( uint x_inner = 0; x_inner < block_width; x_inner++ ) {
                            new_tile[(y_block_start+y_inner)*new_width+x_block_start+x_inner] =
                                tile[y_outer*width+x_outer];
                        }
                    }
                }

                x_it += step;
            }
            x_it = 0.0;
            y_it += step;
        }
    } /* Upsampling */


    width = new_width;
    delete[] tile;
    tile = new_tile;

    return SUCCESS;
} /* resampleTile() */




#if 0
void GridTile::resampleTile (
    float factor,
    float (*method)(float*,uint)
) {
    if ( factor == 1.0 ) {
        return;
    }

    uint factor_int;
    uint new_width;

    float* new_tile;

    // Downsampling
    if ( factor < 1.0 ) {
        factor_int = (uint) round( 1.0 / factor );
        new_width = width / factor_int;

        int len = new_width * new_width;

        float* subblock = new float [factor_int * factor_int];
        new_tile = new float [len];

        int new_tile_it = 0;

        float new_value;

        for ( uint i = 0; i < width; i += factor_int ) {
            for ( uint j = 0; j < width; j += factor_int ) {
                // Get a block of pixels from the current tile
                // which should be turned into one pixel
                getBlock( subblock, factor_int, j, i );

                // Apply the specified accumulation method to the pixel block
                new_value = method( subblock, factor_int );
                new_tile[new_tile_it] = new_value;
                new_tile_it++;
            }
        }

        delete[] subblock;

    } /* Downsampling */

    // Upsampling
    else {
        factor_int = (uint) factor;

        new_width = width * factor_int;
        new_tile = new float [new_width * new_width];

        // Altitude of the current and the next tile
        // in horizontal and vertial direction
        float
            x_alt_current, y_alt_current,
            x_alt_next, y_alt_next;

        // Slope (m) and y-axis intercept (t) of the interpolation functions
        float
            m_x, t_x,
            m_y, t_y;

        // Altitude interpolation values for the horizontal and vertical
        // direction and the average of both
        float
            x_alt_interpolate,
            y_alt_interpolate,
            avg_interpolate;

        // Index in the new tile array
        uint new_tile_index;

        for ( uint y_outer = 0; y_outer < width-1; y_outer++ ) {
            for ( uint x_outer = 0; x_outer < width-1; x_outer++ ) {
                x_alt_current   = tile[y_outer*width+x_outer];
                x_alt_next      = tile[y_outer*width+x_outer+1];
                y_alt_current   = tile[y_outer*width+x_outer];
                y_alt_next      = tile[(y_outer+1)*width+x_outer];


                m_x = ( x_alt_next - x_alt_current ) / factor_int;
                t_x = x_alt_current;

                m_y = ( y_alt_next - y_alt_current ) / factor_int;
                t_y = y_alt_current;

                for ( uint y_inner = 0; y_inner < factor_int; y_inner++ ) {
                    for ( uint x_inner = 0; x_inner < factor_int; x_inner++ ) {
                        // Interpolate the new pixels
                        x_alt_interpolate = m_x * x_inner + t_x;
                        y_alt_interpolate = m_y * y_inner + t_y;

                        // Average of the x and y direction interpolation
                        avg_interpolate = ( x_alt_interpolate + y_alt_interpolate ) / 2.0;

                        new_tile_index =
                            y_outer*factor_int*new_width + // Outer row
                            y_inner*new_width +        // Inner row
                            x_outer*factor_int +           // Outer column
                            x_inner;                   // Inner column

                        new_tile[new_tile_index] = avg_interpolate;
                    } /* x_inner */
                } /* y_inner */
            } /* x_outer */
        } /* y_outer */


        float x_alt, y_alt;

        // Fill the right and upper edge with the corresponding pixels from
        // the original tile (flat without interpolation)
        for ( uint i = 0; i < width; i++ ) {
            x_alt = tile[(width-1)*width+i];
            y_alt = tile[i*width+width-1];

            for ( uint y_inner = 0; y_inner < factor_int; y_inner++ ) {
                for ( uint x_inner = 0; x_inner < factor_int; x_inner++ ) {
                    new_tile_index =
                        (width-1) * factor_int * new_width + // Outer row
                        y_inner * new_width +                // Inner row
                        i * factor_int +                     // Outer column
                        x_inner;                             // Inner column

                    new_tile[new_tile_index] = x_alt;

                    new_tile_index =
                        i * factor_int * new_width + // Outer row
                        y_inner * new_width +        // Inner row
                        (width-1) * factor_int +     // Outer column
                        x_inner;                     // Inner column

                    new_tile[new_tile_index] = y_alt;
                }
            }
        }
    } /* Upsampling */

    delete [] tile;
    tile = new_tile;
    width = new_width;
} /* resampleTile() */
#endif

/*---------------------------------------------------------------*/

int GridTile::createTifFile ( std::string file_path ) {
    TIFF* tif = TIFFOpen( file_path.data(), "w" );
    if ( !tif ) {
        return FILE_NOT_CREATABLE;
    }

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, width);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 32);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);

    for ( uint row = 0; row < width; row++ ) {
        float* row_data = &tile[(width-row-1) * width];
        if ( TIFFWriteScanline(tif, row_data, row, 0) < 0 ) {
            return FILE_NOT_CREATABLE;
        }
    }

    TIFFClose(tif);
    return SUCCESS;
} /* createTifFile() */
