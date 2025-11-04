#ifndef FIELD_H
#define FIELD_H

#include "grid_tile.h"
#include "vector_tile.h"

#include <unordered_map>
#include <vector>
#include <string>

enum TileTypes {
    GRID,
    VECTOR
};

class Field {
public:
    float getHeightAtLatLon ( float lat, float lon );

    int loadGridTile ( std::string tile_name );
    int loadVectorTile ( std::string tile_name );

    std::vector<std::string> tilesOnRay ( float lat1, float lon1, float lat2, float lon2 );

private:
    std::unordered_map<std::string, GridTile> grid_tiles;
    std::unordered_map<std::string, VectorTile> vector_tiles;

    bool tileAlreadyLoaded ( std::string tile_name, int tile_type );
    bool tileAlreadyLoaded ( float lat, float lon, int tile_type );


};

#endif
