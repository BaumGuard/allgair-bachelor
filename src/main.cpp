#include "raw_data/gmlfile.h"
#include "tile/vector_tile.h"

int main () {
    GmlFile gmlfile ( "data/670_5318.gml" );
    VectorTile vt ( gmlfile );
    return 0;
}
