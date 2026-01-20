#ifndef STATUS_CODES_H
#define STATUS_CODES_H

enum Statuscodes {
    SUCCESS                     =  1,

    // Intersections
    INTERSECTION_FOUND          =  2,
    NO_INTERSECTION_FOUND       =  3,

    // Line
    LINES_IDENTICAL             =  4,
    LINES_PARALLEL              =  5,
    POINTS_IDENTICAL            =  6,

    // Line / Plane
    VECTOR_LENGTH_0             =  7,
    ALL_COORDINATES_ZERO        =  8,
    VECTORS_LINEAR_DEPENDANT    =  9,
    LINE_PARALLEL_TO_PLANE      = 10,
    LINE_ON_PLANE               = 11,

    // Polygon
    POINT_DUPLICATE             = 12,
    POINT_NOT_ON_BASE_PLANE     = 13,
    POLYGON_NOT_INITIALIZED     = 14,
    TOO_FEW_POLYGON_POINTS      = 15,

    // Tiles
    TILE_SIZES_UNEQUAL          = 16,
    TILES_MASKED                = 17,
    TILE_NOT_AVAILABLE          = 18,
    INVALID_TILE_TYPE           = 19,
    INVALID_RESAMPLING_FACTOR   = 20,
    COORDINATES_OUTSIDE_TILE    = 21,
    VECTOR_INCOMPLETE           = 22,

    // Raw data
    NO_SURFACES                 = 23,

    // Downloader
    INVALID_URL                 = 24,

    // Files
    FILE_NOT_FOUND              = 25,
    FILE_NOT_CREATABLE          = 26,
    FILE_ALREADY_EXISTS         = 27,
    FILE_CORRUPT                = 28,

    // Config
    UNKNOWN_CONFIG_DATATYPE     = 29,
    UNKNOWN_CONFIG_OPTION       = 30,
    CONFIG_FILE_NOT_LOADED_YET  = 31,
    CONFIG_TYPES_FILE_NOT_FOUND = 32,
    INVALID_TYPE_FOR_OPTION     = 33,
    CONFIG_WRITE_FAILURE        = 34,

    // Precalculator
    INVALID_SIGNAL_CONE_ANGLE   = 35
};

#endif
