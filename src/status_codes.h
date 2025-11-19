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
    COORDINATES_OUTSIDE_TILE    = 19,
    VECTOR_INCOMPLETE           = 20,

    // Raw data
    NO_SURFACES                 = 21,

    // Downloader
    INVALID_URL                 = 22,

    // Files
    FILE_NOT_FOUND              = 23,
    FILE_NOT_CREATABLE          = 24,
    FILE_ALREADY_EXISTS         = 25,
    FILE_CORRUPT                = 26,

    // Config
    UNKNOWN_CONFIG_DATATYPE     = 27,
    UNKNOWN_CONFIG_OPTION       = 28,
    CONFIG_FILE_NOT_LOADED_YET  = 29,
    CONFIG_TYPES_FILE_NOT_FOUND = 30,
    INVALID_TYPE_FOR_OPTION     = 31,
    CONFIG_WRITE_FAILURE        = 32
};

#endif
