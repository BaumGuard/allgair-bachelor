#include "grid_tile.h"

#include "../status_codes.h"
#include "../utils.h"

#include <cstdint>

/*---------------------------------------------------------------*/

GridTile::GridTile ( uint width ) {
    this->width = width;

    tile = new float [width*width];
} /* GridTile () */

GridTile::GridTile ( GeoTiffFile& geotiff ) {
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
} /* GridTile ( float* values, uint width ) */

GridTile::GridTile ( VectorTile& vector_tile, uint width ) {
    this->width = width;
    tile = new float [width*width];

    Vector
        lower_corner = vector_tile.getLowerCorner(),
        upper_corner = vector_tile.getUpperCorner();

    float
        x_step = ( upper_corner.getX() - lower_corner.getX() ) / (double)width,
        y_step = ( upper_corner.getY() - lower_corner.getY() ) / (double)width;

    float
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

    for ( float y=0.0f; y<y_end; y+=y_step ) {
        for ( float x=0.0f; x<x_end; x+=x_step ) {
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
} /* GridTile ( VectorTile& vector_tile, uint width ) */

GridTile::GridTile () {}

/*---------------------------------------------------------------*/

GridTile::~GridTile () {
    //delete[] tile;
} /* ~GridTile() */

/*---------------------------------------------------------------*/

union output_data {
    float f32;
    int16_t i16;

    char bytes [4];
};

int GridTile::writeBinaryFile ( std::string file_path, int output_type ) {

    union output_data data;

    FILE* file = fopen( file_path.data(), "wb" );
    if ( !file ) {
        return FILE_NOT_CREATABLE;
    }

    fprintf( file, "GRID" );

    data.i16 = output_type;
    fwrite( data.bytes, 1, 2, file );

    data.i16 = (int16_t) width;
    fwrite( data.bytes, 1, 2, file );

    int len = width * width;

    for ( int i=0; i<len; i++ ) {
        switch ( output_type ) {
            case INT_PIXEL:
                data.i16 = (int16_t) tile[i];
                fwrite( data.bytes, 1, 2, file );
                break;

            case FLOAT_PIXEL:
                data.f32 = tile[i];
                fwrite( data.bytes, 1, 4, file );
                break;
        }
    }

    fclose( file );

    return SUCCESS;
} /* writeBinaryFile() */

/*---------------------------------------------------------------*/

int GridTile::fromBinaryFile ( std::string file_path ) {
    FILE* file = fopen( file_path.data(), "rb" );
    if ( !file ) {
        return FILE_NOT_FOUND;
    }

    union output_data data;

    fread( data.bytes, 1, 4, file );
    if ( !STREQUAL(data.bytes, "GRID") ) {
        return FILE_CORRUPT;
    }

    fread( data.bytes, 1, 2, file );
    int16_t pixel_type = data.i16;
    if ( pixel_type != INT_PIXEL && pixel_type != FLOAT_PIXEL ) {
        return FILE_CORRUPT;
    }

    fread( data.bytes, 1, 2, file );
    uint16_t w = (uint16_t) data.i16;
    width = w;

    tile = new float [width*width];

    int len = width * width;

    for ( int i=0; i<len; i++ ) {
        switch ( pixel_type ) {
            case INT_PIXEL:
                fread( data.bytes, 1, 2, file );
                tile[i] = data.i16;
                break;

            case FLOAT_PIXEL:
                fread( data.bytes, 1, 4, file );
                tile[i] = data.f32;
                break;
        }
    }

    fclose( file );

    return SUCCESS;
} /* fromBinaryFile() */

/*---------------------------------------------------------------*/

int GridTile::getValue ( uint x, uint y, float& value ) const {
    if ( x >= width || y >= width ) {
        return COORDINATES_OUTSIDE_TILE;
    }

    value = tile[width*(width-y-1)+x];

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

int GridTile::downsampleTile (
    uint factor,
    float (*method)(float*,uint)
) {
    if ( width % factor != 0 ) {
        return 1;
    }

    uint32_t step = width / factor;

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

    return 0;
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
