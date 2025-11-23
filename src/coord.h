#ifndef COORD_H
#define COORD_H

typedef struct {
    double
        lat,
        lon;

    double altitude;
} Coord;

typedef struct {
    int
        x,
        y,
        z;
} TileCoord;

#endif
