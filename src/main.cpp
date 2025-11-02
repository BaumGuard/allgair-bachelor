#include "tile/vector_tile.h"
#include "tile/field.h"
#include "raw_data/gmlfile.h"

#include <iostream>

int main () {
    Field field;
    field.loadVectorTile( "622_5274" );

    GmlFile gml;
    gml.readGmlFile( "data/622_5274.gml" );

    VectorTile tile;
    tile.fromGmlFile( gml );
    tile.createBinaryFile( "data/622_5274.data" );

    return 0;
}
