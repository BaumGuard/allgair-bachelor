#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"
#include "tile_types.h"
#include "../geometry/vector.h"

#include <unordered_map>
#include <vector>
#include <string>


/*
Class for managing grid tiles and vector tiles and performing
raytracing on the data

The class hides the process of loading the tiles from the user
letting him concentrate on the raytracing
*/
class Field {

private:
    std::unordered_map<std::string, GridTile> grid_tiles_dgm;
    //std::unordered_map<std::string, GridTile> grid_tiles_dgm20;
    std::unordered_map<std::string, GridTile> grid_tiles_dom;
    //std::unordered_map<std::string, GridTile> grid_tiles_dom1;
    //std::unordered_map<std::string, VectorTile> vector_tiles_lod2;

    double grid_resolution;

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
#if 0
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
#endif

    /*
    Load a tile of a certain type into the corresponding map

    Args:
     - tile_name : Name of the tile (easting_northing)
     - tile_type : Tile type (DOM20, DOM1, DGM1, DGM20, LOD2)

    Returns:
     - Status code
        - SUCCESS

        - TILE_NOT_AVAILABLE
    */
    int loadTile ( std::string tile_name, int tile_type );
#if 0
    /*
    Get the altitude at the given coordinates (grid)

    Args:
     - lat : Latitude in degrees
     - lon : Longitude in degrees

    Returns:
     - Altitude in meters
    */
    double getAltitudeAtLatLon ( double lat, double lon, int tile_type );
#endif
    /*
    Get the altitude at the UTM x, y coordinates (grid)

    Args:
     - x : UTM x coordinate (easting)
     - y : UTM y coordinate (northing)

    Returns:
     - Altitude in meters
    */
    double getAltitudeAtXY ( double x, double y, int tile_type );

public:
    Field ( double grid_resolution );
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
        /*Vector& intersection,*/
        Vector& start,
        Vector& end,
        float ground_level_threshold,
        uint* ground_count,
        int tile_type,
        bool cancel_on_ground = false
    );

    friend void* Thread_bresenhamPseudo3D ( void* arg );

#if 0
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
#endif
};

void* Thread_bresenhamPseudo3D ( void* arg );

typedef struct {
    Field& field;
    Vector& start;
    Vector& end;
    float ground_level_threshold;
    uint* ground_count;
    int tile_type;
    bool cancel_on_ground;
} Bresenham_Data;

#endif
