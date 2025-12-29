#ifndef LOAD_TILE_H
#define LOAD_TILE_H

#include "grid_tile.h"
#include "vector_tile.h"
#include "../web/urls.h"

/*
Create an instance of GridTile from a tif file

2 step check:
    1. TIFF file available in data folder?
    2. Download the TIFF file

Args:
    - grid_tile : Reference to a GridTile object
    - tile_name : Name of the tile (easting_northing)
    - tile_type : Tile type (DOM20, DOM1, DGM1, DGM20)

Returns:
    - Status code
    - SUCCESS

    - TILE_NOT_AVAILABLE
*/
int getGridTile ( GridTile& grid_tile, std::string tile_name, int tile_type, std::string url = URL_ROOT );


/*
Create an instance of VectorTile from a .gml fle or binary .data file

3 step check:
    1. Binary file (.data) in data folder?
    2. Gml file available in the data folder?
    3. Download the Gml file and create a binary file

Args:
    - vector_tile : Reference to a VectorTile object
    - tile_name   : Name of the tile (easting_northing)

Returns:
    - Status code
       - SUCCESS

       - TILE_NOT_AVAILABLE
*/
int getVectorTile ( VectorTile& vector_tile, std::string tile_name, std::string url = URL_ROOT );

#endif
