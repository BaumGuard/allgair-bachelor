#include "raw_data/gmlfile.h"
#include "tile/vector_tile.h"

#include <stdio.h>
#include <time.h>

int main () {
    clock_t before = clock();

    GmlFile gmlfile;
    gmlfile.readGmlFile( "data/690_5336.gml" );
    VectorTile tile;
    tile.fromGmlFile( gmlfile );
    //tile.createBinaryFile( "data/670_5318.data" );


/*
    VectorTile tile;
    int err = tile.readBinaryFile( "data/690_5336.data" );
    printf("Error: %d\n", err);
*/
    clock_t after = clock();
    printf( "Time : %f s\n", (double)(after-before) / (double)CLOCKS_PER_SEC );

    return 0;
}
