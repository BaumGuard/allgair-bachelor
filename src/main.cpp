#include "geotiff/tile.h"

int main () {
    Tile tile;

    tile.createTileFromGeoTIFF( "data/700_5312.tif" );
    tile.downsampleTile( 5, max );

    return 0;
}
