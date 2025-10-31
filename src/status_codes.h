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

    // Downloader
    INVALID_URL,                /* 17 */
    DOWNLOAD_SUCCESSFUL,        /* 18 */
    FILE_NOT_FOUND,             /* 19 */
    FILE_NOT_CREATABLE,         /* 20 */
    FILE_ALREADY_EXISTS,        /* 21 */
    FILE_CREATED,               /* 22 */

    // Tiles
    TILE_SIZES_UNEQUAL,         /* 23 */
    TILES_MASKED,               /* 24 */
    BINARY_FILE_NOT_FOUND,      /* 25 */
    CORRUPT_BINARY_FILE,        /* 26 */
    TILE_NOT_AVAILABLE,         /* 27 */
    TILE_LOADED_SUCCESSFULLY,   /* 28 */
    READ_BINARY_FILE_SUCCESS,   /* 29 */
    BINARY_FILE_NOT_CREATABLE,  /* 30 */
    BINARY_FILE_CREATED,        /* 31 */

    // Raw data
    NO_SURFACES                 /* 32 */
};

#endif
