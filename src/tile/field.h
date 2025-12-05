#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"
#include "../coord.h"

#include <unordered_map>
#include <vector>
#include <string>


enum TileTypes {
    DGM1,   // Terrain model (GeoTIFF) (Width: 1 km, Resolution: 1 m)
    DGM20,  // Upsampled DGM1          (Width: 1 km, Resolution: 20 cm)
    DOM20,  // Surface model (GeoTIFF) (Width: 1 km, Resolution: 20 cm)
    DOM1,   // Downsampled DOM20       (Width: 1 km, Resolution: 1 m)
    LOD2    // Buildings     (Gml)     (Width: 2 km)
};

/*
Class for managing grid tiles and vector tiles and performing
raytracing on the data

The class hides the process of loading the tiles from the user
letting him concentrate on the raytracing
*/
class Field {

private:
    std::unordered_map<std::string, GridTile> grid_tiles_dgm1;
    std::unordered_map<std::string, GridTile> grid_tiles_dgm20;
    std::unordered_map<std::string, GridTile> grid_tiles_dom20;
    std::unordered_map<std::string, GridTile> grid_tiles_dom1;
    std::unordered_map<std::string, VectorTile> vector_tiles_lod2;

    /*
    Check if a tile has already been loaded into grid_tiles or
    vector_tiles using a tile name

    Args:
     - tile_name : Name of the tile (easting_northing)
     - tile_type : Tile type (DGM1, DGM20, DOM20, DOM1, LOD2)

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
     - tile_type : Tile type (DGM1, DGM20, DOM20, DOM1, LOD2)

    Returns:
     - Tile with name tile_name already loaded?
    */
    bool tileAlreadyLoaded ( double lat, double lon, int tile_type );

    /*
    Find all tiles through which a ray given by starting and end
    coordinates passes to load the tiles and perform raytracing on

    Args:
     - lat_start     : Latitude of the starting point in degrees
     - lon_start     : Longitude of the starting point in degrees
     - lat_end       : Latitude of the end point in degrees
     - lon_end       : Longitude of the end point in degrees
     - tile_width_km : Width of the tile in km

    Returns:
     - All tile names of the tiles through which the ray passes
    */
    std::vector<std::string> tilesOnRay (
        double lat_start, double lon_start,
        double lat_end, double lon_end,
        uint tile_width_km
    );


    /*
    Load a tile

    3 step check:
     1. Binary file (.data) in data folder (only for LOD2)?
     2. TIFF file available in data folder?
     3. Download the TIFF file, create a binary file and load
        into grid_tiles

    Args:
     - tile_name : Name of the tile (easting_northing)

    Returns:
     - Status code
        - SUCCESS

        - TILE_NOT_AVAILABLE
    */
    int loadTile ( std::string tile_name, int tile_type );

    /*
    Get the altitude at the given coordinates (grid)

    Args:
     - lat : Latitude in degrees
     - lon : Longitude in degrees

    Returns:
     - Altitude in meters
    */
    double getAltitudeAtLatLon ( double lat, double lon, int tile_type );

    /*
    Get the altitude at the UTM x, y coordinates (grid)

    Args:
     - x : UTM x coordinate (easting)
     - y : UTM y coordinate (northing)

    Returns:
     - Altitude in meters
    */
    double getAltitudeAtXY ( uint x, uint y, int tile_type );

public:
    /*
    Perform the Bresenham algorithm in pseudo 3D space

    Args:
     - intersection           : Reference to the Coord object to store the
                                intersection of the ray with the terrain in
     - start                  : Starting coordinates in degrees and altitude in meters
     - end                    : End coordinates in degrees and altitude in meters
     - ground_level_threshold : Maximum ground level below the ground level as given by
                                the GeoTIFF file to which a pixel should be classified
                                as ground
     - ground_count           : Pointer to a variable to save the number in how often the
                                ray is below the ground taking ground_level_threshold into
                                account
     - tile_type              : Tile type (DGM1, DGM20, DOM20, DOM1)
     - cancel_on_ground       : Stop the algorithm when the ray has hit the ground

    Returns:
     - Status code
        - INTERSECTION_FOUND

        - NO_INTERSECTION_FOUND
        - INVALID_TILE_TYPE
    */
    int bresenhamPseudo3D (
        Coord& intersection,
        Coord& start,
        Coord& end,
        float ground_level_threshold,
        int* ground_count,
        int tile_type,
        bool cancel_on_ground = false
    );


    /*
    Find the nearest intersection of a ray with a surface

    Args:
     - intersection : Reference to the Coord object to store the
                      nearest intersection of the ray and a surface in
     - start        : Starting coordinates in degrees and altitude in meters
     - end          : End coordinates in degrees and altitude in meters

    Returns:
     - Status code
        - INTERSECTION_FOUND

        - NO_INTERSECTION_FOUND
    */
    int surfaceIntersection (
        Coord& intersection,
        Coord& start,
        Coord& end
    );

};

#endif
