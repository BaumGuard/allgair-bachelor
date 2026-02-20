#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"
#include "tile_types.h"
#include "../geometry/vector.h"
#include "../config/global_config.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <pthread.h>


/*
Class for managing grid tiles and vector tiles and performing
raytracing on the data

The class hides the process of loading the tiles from the user
letting him concentrate on the raytracing
*/
class Field {

private:
    // Hashmaps for the tiles
    std::unordered_map<std::string, GridTile> grid_tiles_dgm;
    std::unordered_map<std::string, GridTile> grid_tiles_dom;
    std::unordered_map<std::string, GridTile> grid_tiles_dom_masked;
    std::unordered_map<std::string, VectorTile> vector_tiles_lod2;

    pthread_mutex_t dgm_mutex, dom_mutex, dom_masked_mutex;

    double grid_resolution;

    /*
    Check if a tile has already been loaded into the hashmaps using a tile name

    Args:
     - tile_name : Name of the tile (easting_northing)
     - tile_type : Tile type (DGM, DOM, DOM_MASKED, LOD2)

    Returns:
     - Tile with name tile_name already loaded?
    */
    bool tileAlreadyLoaded ( std::string tile_name, int tile_type );

    /*
    Load a tile of a certain type into the corresponding map

    Args:
     - tile_name : Name of the tile (easting_northing)
     - tile_type : Tile type (DGM, DOM, DOM_MASKED, LOD2)

    Returns:
     - Status code
        - SUCCESS

        - TILE_NOT_AVAILABLE
    */
    int loadTile ( std::string tile_name, int tile_type );

    /*
    Get the altitude at the UTM x, y coordinates (grid)

    Args:
     - x         : UTM x coordinate (easting)
     - y         : UTM y coordinate (northing)
     - tile_type : Tile type (DGM, DOM, DOM_MASKED)

    Returns:
     - Altitude in meters
    */
    double getAltitudeAtXY ( double x, double y, int tile_type );


    /*
    Find all polygons in the vector tiles that are located within the
    ground area

    Args:
    - polygons :    Reference to the list of polygons where the polygons
                    inside the ground area should be stored in
    - ground_area : Ground area as a Polygon object

    Returns:
    - Status code:
        - SUCCESS

        - TILE_NOT_AVAILABLE
    */
    int getPolygonsInGroundArea (
        std::vector<Polygon>& polygons,
        Polygon& ground_area
    );



    friend void* Thread_bresenhamPseudo3D ( void* arg );
    friend void* Thread_precalculate ( void* arg );
    friend void* Thread_getPolygonsInGroundArea ( void* arg );


public:
    Field ( double grid_resolution );

    /*
    Perform the Bresenham algorithm in pseudo 3D space

    Args:
     - start                  : Starting coordinates in degrees and altitude in meters
     - end                    : End coordinates in degrees and altitude in meters
     - ground_level_threshold : Maximum ground level below the ground level as given by
                                the GeoTIFF file to which a pixel should be classified
                                as ground
     - decision_arrays_united : Pointer to an array of std::vector<bool> to store the information
                                in whether the ray was below the terrain at a point
     - tile_type              : Tile type (DGM, DOM)
     - cancel_on_ground       : Stop the algorithm when the ray has hit the ground
     - n_threads              : Number of threads for parallel Bresenham algorithm

    Returns:
     - Status code
        - INTERSECTION_FOUND

        - NO_INTERSECTION_FOUND
        - INVALID_TILE_TYPE
    */
    int bresenhamPseudo3D (
        Vector& start,
        Vector& end,
        float ground_level_threshold,
        std::vector<bool>* decision_arrays_united,
        int tile_type,
        bool cancel_on_ground = false,
        int n_threads = MAX_THREADS
    );


    /*
    Find all polygons in the Fresnel zone between the start and the end point
    that satisfy the following condition:
    For every polygon in the Fresnel zone the algorithm first creates a line
    between the polygon's centroid and the end point and uses the direction
    vector of this line and the end point to create a plane perpendicular to
    the line.
    The algorithm proceeds by reflecting every corner point of the polygon
    and the centroid onto the newly created plane.
    If the reflected centroid is inside the reflected polygon, the algorithm
    adds the polygon to the list of selected polygons.

    Args:
    - selected_polygons : Reference to the list to add the selected polygons
    - start_point       : Start point as UTM coordinates and altitude
    - end_points        : List of end points as UTM coordinates and altitude
    - select_method     : Choose which of the polygons should be returned:
                            - BY_MIN_DISTANCE : The polygon that is closest to
                                                the starting point
                            - BY_MAX_AREA     : The polygon with the largest area
                            - ALL             : All selected polygons
    - fresnel_zone      : Number of the Fresnel zone (Default: 2)
    - freq              : Frequency of the signal in Hz

    Returns:
    - List of polygons that satisfy the condition above
    */
    int precalculate (
        std::vector<Polygon>& selected_polygons,
        Vector& start_point,
        Vector& end_point,
        int select_method,
        int fresnel_zone = 2,
        double freq = 868.0e6
    );
};

void* Thread_bresenhamPseudo3D ( void* arg );
void* Thread_precalculate ( void* arg );
void* Thread_getPolygonsInGroundArea ( void* arg );

typedef struct {
    Field& field;
    Vector& start;
    Vector& end;
    float ground_level_threshold;
    int* ground_count;
    int tile_type;
    bool cancel_on_ground;
} Bresenham_Data;

#endif
