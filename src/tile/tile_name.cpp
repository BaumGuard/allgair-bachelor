#include "tile_name.h"

#include <cmath>
#include <cstdio>

#include "../lib/UTM.h"

/*---------------------------------------------------------------*/

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
} /* std::string latLonToTileName ( float lat, float lon ) */

/*---------------------------------------------------------------*/

std::string adjacentTile ( std::string tile_name, int adjacence ) {
    std::string
        part1_str,
        part2_str;

    int i = 0;

    int len = tile_name.length();
    for ( ; i<len; i++ ) {
        if ( tile_name[i] == '_' ) {
            break;
        }
        part1_str += tile_name[i];
    }

    i++;

    for ( ; i<len; i++ ) {
        if ( tile_name[i] == '\0' ) {
            break;
        }
        part2_str += tile_name[i];
    }


    int
        part1_int = std::stoi( part1_str ),
        part2_int = std::stoi( part2_str );

    switch ( adjacence ) {
        case LEFT_UPPER_CORNER:
            part1_int--;
            part2_int++;
            break;

        case UPPER_EDGE:
            part2_int++;
            break;

        case RIGHT_UPPER_CORNER:
            part1_int++;
            part2_int++;
            break;

        case LEFT_EDGE:
            part1_int--;
            break;

        case RIGHT_EDGE:
            part1_int++;
            break;

        case LEFT_LOWER_CORNER:
            part1_int--;
            part2_int--;
            break;

        case LOWER_EDGE:
            part1_int--;
            break;

        case RIGHT_LOWER_CORNER:
            part1_int++;
            part2_int--;
            break;
    }

    char new_name [9];
    snprintf( new_name, 9, "%d_%d", part1_int, part2_int );

    return std::string( new_name );
} /* std::string adjacentTile ( std::string tile_name, int adjacence ) */
