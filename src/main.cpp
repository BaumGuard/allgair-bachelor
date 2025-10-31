#include "tile/field.h"

#include <iostream>

int main () {
    Field field;
    float altitude = field.getHeightAtLatLon( 47.622189, 10.635297 );
    std::cout << altitude << '\n';
    return 0;
}
