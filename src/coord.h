#ifndef COORD_H
#define COORD_H

typedef struct {
    float
        lat,
        lon;

    float altitude;
} Coord;

typedef struct {
    int
        x,
        y,
        z;
} TileCoord;

#endif
