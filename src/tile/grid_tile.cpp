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

GridTile::GridTile ( float* values, unsigned int width ) {
    this->width = width;

    tile = new float [width*width];

    int len = width * width;
    for ( int i=0; i<len; i++ ) {
        tile[i] = values[i];
    }
} /* GridTile::GridTile ( float* values, unsigned int width ) */

GridTile::GridTile ( VectorTile& vector_tile, unsigned int width ) {
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

                if ( status == LINE_INTERSECTS_PLANE ) {
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
} /* GridTile::GridTile ( VectorTile& vector_tile, unsigned int width ) */

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
