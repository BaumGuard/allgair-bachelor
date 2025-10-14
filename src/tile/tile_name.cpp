#include "tile_name.h"

#include <cmath>
#include <cstdio>

#include "../lib/UTM.h"


std::string latLonToTileName ( float lat, float lon ) {
    float
        utm_easting,
        utm_northing;

    LatLonToUTMXY( lat, lon, UTM_ZONE, utm_easting, utm_northing );

    int
        easting_part  = floor( utm_easting / 1000.0 ),
        northing_part = floor( utm_northing / 1000.0 );

    char tile_name [10];

    snprintf( tile_name, 10, "%d_%d", easting_part, northing_part );

    return std::string( tile_name );
}
