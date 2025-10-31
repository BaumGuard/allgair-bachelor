#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"

#include <unordered_map>
#include <string>

enum TileTypes {
    GRID,
    VECTOR
};

class Field {
public:
    float getHeightAtLatLon ( float lat, float lon );

private:
    std::unordered_map<std::string, GridTile> grid_tiles;
    std::unordered_map<std::string, VectorTile> vector_tiles;

    int loadGridTile ( std::string tile_name );
    int loadVectorTile ( std::string tile_name );

    bool tileAlreadyLoaded ( std::string tile_name, int tile_type );

    std::string buildTileName ( unsigned int x, unsigned int y );
};

#endif
