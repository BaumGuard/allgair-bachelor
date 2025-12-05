#include "grid_tile.h"

#include "../status_codes.h"
#include "../utils.h"

#include <cstdint>
#include <iostream>
#include <cstring>

/*---------------------------------------------------------------*/

GridTile::GridTile () {}

GridTile::GridTile( const GridTile& old_gridtile ) {
    width = old_gridtile.getTileWidth();
    tile_name = old_gridtile.getTileName();

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
    }

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

uint GridTile::getTileWidth () const {
    return width;
} /* getGridTileWidth() */

/*---------------------------------------------------------------*/

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

/*---------------------------------------------------------------*/

std::string GridTile::getTileName () const {
    return tile_name;
} /* getTileName() */

/*---------------------------------------------------------------*/

float* GridTile::getData () const {
    return tile;
} /* getData() */

/*---------------------------------------------------------------*/

void GridTile::resampleTile (
    float factor,
    float (*method)(float*,uint)
) {
    if ( factor == 1.0 ) {
        return;
    }


    uint factor_int;

    // Downsampling
    if ( factor < 1.0 ) {
        factor_int = (uint)( 1.0 / factor );
        uint32_t step = width / factor_int;

        int len = step * step;

        float* subblock = new float [len];
        float* new_tile = new float [len];

        int new_tile_it = 0;

        float new_value;

        for ( uint i=0; i<width; i+=step ) {
            for ( uint j=0; j<width; j+=step ) {
                getBlock( subblock, step, j, i );

                new_value = method( subblock, step );
                new_tile[new_tile_it] = new_value;
                new_tile_it++;
            }
        }

        for ( int i = 0; i<len; i++ ) {
            tile[i] = new_tile[i];
        }
        width = step;

        delete[] new_tile;
        delete[] subblock;
    } /* Downsampling */

    // Upsampling
    else {
        factor_int = (uint) factor;

        uint new_width = width * factor;
        float* new_tile = new float [new_width * new_width];

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

                m_x = ( x_alt_next - x_alt_current ) / factor;
                t_x = x_alt_current;

                m_y = ( y_alt_next - y_alt_current ) / factor;
                t_y = y_alt_current;

                for ( uint y_inner = 0; y_inner < factor; y_inner++ ) {
                    for ( uint x_inner = 0; x_inner < factor; x_inner++ ) {
                        // Interpolate the new pixels
                        x_alt_interpolate = m_x * x_inner + t_x;
                        y_alt_interpolate = m_y * y_inner + t_y;

                        // Average of the x and y direction interpolation
                        avg_interpolate = ( x_alt_interpolate + y_alt_interpolate ) / 2.0;

                        new_tile_index =
                            y_outer*factor*new_width + // Outer row
                            y_inner*new_width +        // Inner row
                            x_outer*factor +           // Outer column
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

            for ( uint y_inner = 0; y_inner < factor; y_inner++ ) {
                for ( uint x_inner = 0; x_inner < factor; x_inner++ ) {
                    new_tile_index =
                        i*factor*new_width + // Outer row
                        y_inner*new_width +  // Inner row
                        (width-1)*factor +   // Outer column
                        x_inner;             // Inner column

                    new_tile[new_tile_index] = x_alt;


                    new_tile_index =
                        (width-1)*factor*new_width + // Outer row
                        y_inner*new_width +          // Inner row
                        i*factor +                   // Outer column
                        x_inner;                     // Inner column

                    new_tile[new_tile_index] = y_alt;
                }
            }
        }


        delete [] tile;
        tile = new_tile;
        width = new_width;
    } /* Upsampling */
} /* downsampleGridTile() */

/*---------------------------------------------------------------*/

float max ( float* block, uint width ) {
    int len = width * width;

    float max_value = block[0];

    for ( int i=1; i<len; i++ ) {
        if ( block[i] > max_value ) {
            max_value = block[i];
        }
    }

    return max_value;
} /* max() */

float min ( float* block, uint width ) {
    int len = width * width;

    float min_value = block[0];

    for ( int i=1; i<len; i++ ) {
        if ( block[i] < min_value ) {
            min_value = block[i];
        }
    }

    return min_value;
} /* min() */

float avg ( float* block, uint width ) {
    int len = width * width;

    float sum = 0.0;

    for ( int i=0; i<len; i++ ) {
        sum += block[i];
    }

    return sum / (float)len;
} /* avg() */
