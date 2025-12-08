#ifndef TILE_TYPES_H
#define TILE_TYPES_H

enum TileTypes {
    DGM1,   // Terrain model (GeoTIFF) (Width: 1 km, Resolution: 1 m)
    DGM20,  // Upsampled DGM1          (Width: 1 km, Resolution: 20 cm)
    DOM20,  // Surface model (GeoTIFF) (Width: 1 km, Resolution: 20 cm)
    DOM1,   // Downsampled DOM20       (Width: 1 km, Resolution: 1 m)
    LOD2    // Buildings     (Gml)     (Width: 2 km)
};

#endif
