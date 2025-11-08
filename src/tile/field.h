#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"
#include "../coord.h"

#include <unordered_map>
#include <vector>
#include <string>

enum TileTypes {
    GRID,
    VECTOR
};

class Field {
public:
    int surfaceIntersection (
        Coord& intersection,
        float lat_start, float lon_start, float alt_start,
        float lat_end, float lon_end, float alt_end
    );

private:
    std::unordered_map<std::string, GridTile> grid_tiles;
    std::unordered_map<std::string, VectorTile> vector_tiles;

    /*
    Check if a tile has already been loaded into grid_tiles or
    vector_tiles using a tile name

    Args:
     - tile_name : Name of the tile (easting_northing)
     - tile_type : Tile type (GRID, VECTOR)

    Returns:
     - Tile with name tile_name already loaded?
    */
    bool tileAlreadyLoaded ( std::string tile_name, int tile_type );

    /*
    Check if a tile has already been loaded into grid_tiles or
    vector_tiles using coordinates (latitude, longitude)

    Args:
     - lat       : Latitude on the tile in degrees
     - lon       : Longitude on the tile in degrees
     - tile_type : Tile type (GRID, VECTOR)

    Returns:
     - Tile with name tile_name already loaded?
    */
    bool tileAlreadyLoaded ( float lat, float lon, int tile_type );

    /*
    Find all tiles through which a ray given by starting and end
    coordinates passes to load the tiles and perform raytracing on

    Args:
     - lat_start     : Latitude of the starting point in degrees
     - lon_start     : Longitude of the starting point in degrees
     - lat_end       : Latitude of the end point in degrees
     - lon_end       : Longitudde of the end point in degrees
     - tile_width_km : Width of the tile in km

    Returns:
     - All tile names of the tiles through which the ray passes
    */
    std::vector<std::string> tilesOnRay (
        float lat_start, float lon_start,
        float lat_end, float lon_end,
        uint tile_width_km
    );


    /*
    Load a grid tile

    3 step check:
     1. Binary file (.grid) in data folder?
     2. TIFF file available in data folder?
     3. Download the TIFF file, create a binary file and load
        into grid_tiles

    Args:
     - tile_name : Name of the tile (easting_northing)

    Returns:
     - Status code
        - TILE_NOT_AVAILABLE
        - TILE_LOADED_SUCCESSFULLY (Success)
    */
    int loadGridTile ( std::string tile_name );

    /*
    Load a vector tile

    3 step check:
     1. Binary file (.data) in data folder?
     2. Gml file available in data folder?
     3. Download the Gml file, create a binary file and load
        into vector_tiles

    Args:
     - tile_name : Name of the tile (easting_northing)

    Returns:
     - Status code
        - TILE_NOT_AVAILABLE
        - TILE_LOADED_SUCCESSFULLY (Success)
    */
    int loadVectorTile ( std::string tile_name );

    /*
    Get the altitude at the given coordinates (grid)

    Args:
     - lat : Latitude in degrees
     - lon : Lontitude in degrees

    Returns:
     - Altitude in meters
    */
    float getHeightAtLatLon ( float lat, float lon );


    /*
    Perform the Bresenham algorithm in pseudo 3D space

    Args:
     - intersection : Reference to the Coord object to store the
                      intersection of the ray with the terrain in
     - lat_start    : Latitude of the starting point in degrees
     - lon_start    : Longitude of the starting point in degrees
     - alt_start    : Altitude at the starting point in meters
     - lat_end      : Latitude of the end point in degrees
     - lon_end      : Longitudde of the end point in degrees
     - alt_end      : Altitude at the end point in meters

    Returns:
     - Status code
    */
    int bresenhamPseudo3D (
        Coord& intersection,
        float lat_start, float lon_start, float alt_start,
        float lat_end, float lon_end, float alt_end
    );

    /*
    Find the nearest intersection of a ray with a surface

    Args:
     - intersection : Reference to the Coord object to store the
                      nearest intersection of the ray and a surface in
     - lat_start    : Latitude of the starting point in degrees
     - lon_start    : Longitude of the starting point in degrees
     - alt_start    : Altitude at the starting point in meters
     - lat_end      : Latitude of the end point in degrees
     - lon_end      : Longitudde of the end point in degrees
     - alt_end      : Altitude at the end point in meters

    Returns:
     - Status code
    */

};

#endif
