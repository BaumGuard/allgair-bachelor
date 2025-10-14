#include "UTM.h"
#include <stdio.h>

int main () {
    float
        lat = 47.6222,
        lon = 10.6315;

    float x, y;

    LatLonToUTMXY ( lat, lon, 32, x, y );

    printf( "%f %f\n", x, y );

    return 0;
}
