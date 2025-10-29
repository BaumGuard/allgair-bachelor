#include "raw_data/geotiff.h"
#include "tile/grid_tile.h"

#include <stdio.h>
#include <time.h>

int main () {
    clock_t start = clock();
    GeoTiffFile geotiff( "data/646_5319.tif" );
    GridTile tile ( geotiff.getData(), 1000 );
    //tile.writeBinaryFile( "data/646_5319.grid", FLOAT );
    /*
    GridTile tile;
    tile.readBinaryFile( "data/646_5319.grid" );
    */
    clock_t end = clock();

    printf("%f s\n", ((double)end-(double)start)/CLOCKS_PER_SEC);
    //GridTile tile;
    //tile.writeBinaryFile( "data/646_5319.grid" );
/*
    clock_t before = clock();

    GmlFile gmlfile;
    gmlfile.readGmlFile( "data/690_5336.gml" );
    VectorTile tile;
    tile.fromGmlFile( gmlfile );
    //tile.createBinaryFile( "data/670_5318.data" );



    VectorTile tile;
    int err = tile.readBinaryFile( "data/690_5336.data" );
    printf("Error: %d\n", err);

    clock_t after = clock();
    printf( "Time : %f s\n", (double)(after-before) / (double)CLOCKS_PER_SEC );
*/
    return 0;
}
