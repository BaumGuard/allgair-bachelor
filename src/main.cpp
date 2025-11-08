#include "tile/field.h"
#include "coord.h"

#include <iostream>
#include <cstdio>

int main () {
    Field field;
    Coord intersect;
    field.surfaceIntersection( intersect, 48.17959, 11.52543, 745.2, 48.11685, 11.68722, 24.5 );

    printf("%f %f %f\n", intersect.lat, intersect.lon, intersect.altitude);

    return 0;
}
