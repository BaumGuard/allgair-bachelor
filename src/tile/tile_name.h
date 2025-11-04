#include <string>

#define UTM_ZONE 32

/*
Positions of tile adjacence
*/
enum Adjacence {
    LEFT_UPPER_CORNER,
    UPPER_EDGE,
    RIGHT_UPPER_CORNER,
    LEFT_EDGE,
    RIGHT_EDGE,
    LEFT_LOWER_CORNER,
    LOWER_EDGE,
    RIGHT_LOWER_CORNER
};


/*
Convert latitude and longitude to the tile name

Args:
 - lat : Latitude
 - lon : Longitude

Returns:
 - Tile name
*/
std::string latLonToTileName ( float lat, float lon );


/*
Determine the name of an adjacent tile based on a given tile
and an edge or corner

Args:
 - tile_name : Current tile name
 - adjacence : Edge or corner
                - LEFT_UPPER_CORNER
                - UPPER_EDGE
                - RIGHT_UPPER_CORNER
                - LEFT_EDGE
                - RIGHT_EDGE
                - LEFT_LOWER_CORNER
                - LOWER_EDGE
                - RIGHT_LOWER_CORNER
 - width_km  : Tile width in km

Returns:
 - Name of the adjacent tile
*/
std::string adjacentTile ( std::string tile_name, int adjacence, int width_km );


/*
Build the tile name string from the UTM easting and northing

Args:
 - x : Easting in km
 - y : Northing in km

Returns:
 - Tile name string "easting_northing"
*/
std::string buildTileName ( uint x, uint y );
