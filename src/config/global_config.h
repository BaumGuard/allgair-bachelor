#ifndef GLOBAL_CONFIG
#define GLOBAL_CONFIG

#include <string>

// Directory to save the tile files in and read from
extern std::string DATA_DIR;

// URLs for DGM1, DOM20 and LOD2 given by the user
extern std::string
    CHOSEN_URL_DGM1,
    CHOSEN_URL_DOM20,
    CHOSEN_URL_LOD2;

// Maximum number of threads
extern int MAX_THREADS;

// Resolution of the grid tiles in meters
extern double GRID_RESOLUTION;

// Maximum distance from a point to the polygon's ground plane
// until which the point should be pulled into the plane
// When the distance is larger, the polygon will be discarded
extern double PLANE_DISTANCE_THRESHOLD;

// K value to correct the altitudes
extern double K_VALUE;

#define EARTH_RADIUS 6366473.0
extern double EARTH_RADIUS_EFFECTIVE;

extern bool CANCEL_ON_GROUND;

#endif
