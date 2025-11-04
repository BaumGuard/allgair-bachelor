#include "tile/vector_tile.h"
#include "tile/field.h"
#include "raw_data/gmlfile.h"

#include <iostream>
#include <vector>
#include <string>

int main () {
    Field field;

    std::vector<std::string> tile_names = field.tilesOnRay( 47.708326, 10.468767, 47.766369, 10.614620 );

    for ( int i=0; i<tile_names.size(); i++ ) {
        std::cout << tile_names[i] << '\n';
    }

    return 0;
}
