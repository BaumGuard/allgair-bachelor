#include "raw_data/gmlfile.h"
#include "tile/vector_tile.h"

int main () {


    GmlFile gmlfile;
    gmlfile.readGmlFile( "data/540_5552.gml" );
    VectorTile tile;
    tile.fromGmlFile( gmlfile );
    //tile.createBinaryFile( "data/670_5318.data" );


    /*
    VectorTile tile;
    tile.readBinaryFile( "data/670_5318.data" );
    */

    return 0;
}
