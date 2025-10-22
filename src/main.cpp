#include "raw_data/gmlfile.h"
#include "tile/vector_tile.h"

#include <iostream>

int main () {
    //GmlFile gmlfile ( "data/690_5336.gml" );
    VectorTile tile;
    //tile.createBinaryFile( "data/690_5336.data" );
    tile.readBinaryFile( "data/690_5336.data" );
    return 0;
}
