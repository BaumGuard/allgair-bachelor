#ifndef TILE_TYPES_H
#define TILE_TYPES_H

enum TileTypes {
    DGM,    // Terrain model (General)
    DOM,    // Surface model (General)
    DGM1,   // Terrain model (GeoTIFF) (Width: 1 km, Resolution: 1 m)
    DOM20,  // Surface model (GeoTIFF) (Width: 1 km, Resolution: 20 cm)
    LOD2    // Buildings     (Gml)     (Width: 2 km)
};

#endif
