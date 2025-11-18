#ifndef STATUS_CODES_H
#define STATUS_CODES_H

enum Statuscodes {
    // Line
    LINES_INTERSECT,            /*  0 */
    LINES_NO_INTERSECT,         /*  1 */
    LINES_IDENTICAL,            /*  2 */
    LINES_PARALLEL,             /*  3 */
    POINTS_IDENTICAL,           /*  4 */

    // Line / Plane
    VECTOR_LENGTH_0,            /*  5 */
    ALL_COORDINATES_ZERO,       /*  6 */
    VECTORS_LINEAR_DEPENDANT,   /*  7 */
    LINE_INTERSECTS_PLANE,      /*  8 */
    LINE_PARALLEL_TO_PLANE,     /*  9 */
    LINE_ON_PLANE,              /* 10 */
    CREATION_SUCCEEDED,         /* 11 */

    // Polygon
    POINT_DUPLICATE,            /* 12 */
    POINT_ADDED,                /* 13 */
    POINT_NOT_ON_BASE_PLANE,    /* 14 */
    POLYGON_NOT_INITIALIZED,    /* 15 */
    TOO_FEW_POLYGON_POINTS,     /* 16 */
    POLYGON_LINE_NO_INTERSECT,  /* 17 */
    LINE_INTERSECTS_POLYGON,    /* 18 */

    // Downloader
    INVALID_URL,                /* 19 */
    DOWNLOAD_SUCCESSFUL,        /* 20 */
    FILE_NOT_FOUND,             /* 21 */
    FILE_NOT_CREATABLE,         /* 22 */
    FILE_ALREADY_EXISTS,        /* 23 */
    FILE_CREATED,               /* 24 */

    // Tiles
    TILE_SIZES_UNEQUAL,         /* 25 */
    TILES_MASKED,               /* 26 */
    BINARY_FILE_NOT_FOUND,      /* 27 */
    CORRUPT_BINARY_FILE,        /* 28 */
    TILE_NOT_AVAILABLE,         /* 29 */
    TILE_LOADED_SUCCESSFULLY,   /* 30 */
    READ_BINARY_FILE_SUCCESS,   /* 31 */
    BINARY_FILE_NOT_CREATABLE,  /* 32 */
    BINARY_FILE_CREATED,        /* 33 */
    COORDINATES_OUTSIDE_TILE,   /* 34 */

    // Raw data
    NO_SURFACES,                /* 35 */

    // Raytracing
    INTERSECTION_FOUND,         /* 36 */
    NO_INTERSECTION_FOUND,      /* 37 */

    // Config
    UNKNOWN_CONFIG_DATATYPE,    /* 38 */
    UNKNOWN_CONFIG_OPTION,      /* 39 */
    CONFIG_FILE_NOT_FOUND,      /* 40 */
    CONFIG_FILE_NOT_LOADED_YET, /* 41 */
    CONFIG_TYPES_FILE_NOT_FOUND,/* 42 */
    INVALID_TYPE_FOR_OPTION,    /* 43 */
    CONFIG_PARSED_SUCCESS,      /* 44 */
    CONFIG_WRITE_SUCCESS,       /* 45 */
    CONFIG_WRITE_FAILURE,       /* 46 */

    SUCCESS                     /* 47 */
};

#endif
