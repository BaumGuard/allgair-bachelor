#include "field.h"
#include "../web/download.h"
#include "../web/urls.h"
#include "../raw_data/geotiff.h"
#include "../lib/UTM.h"
#include "../utils.h"

#include <unistd.h>
#include <iostream>

const std::string DATA_DIR = "data";

/*---------------------------------------------------------------*/

int Field::loadGridTile ( std::string tile_name ) {
    std::string binary_file_name = DATA_DIR + "/" + tile_name + ".grid";
    if ( FILE_EXISTS(binary_file_name.data()) ) {
        GridTile grid_tile;
        grid_tile.readBinaryFile( binary_file_name.c_str() );

        grid_tiles[tile_name] = grid_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }

    std::string geotiff_file_name = DATA_DIR + "/" + tile_name + ".tif";
    if ( FILE_EXISTS(geotiff_file_name.data()) ) {

        std::cout << "Path: " << geotiff_file_name.c_str() << '\n';
        GeoTiffFile geotiff ( geotiff_file_name.c_str() );
        GridTile grid_tile ( geotiff );

        grid_tile.writeBinaryFile( binary_file_name.data(), FLOAT_PIXEL );

        grid_tiles[tile_name] = grid_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }


    std::string geotiff_url = URL_DGM1 + tile_name + ".tif";
    if ( downloadFile( geotiff_url.data(), DATA_DIR.data() ) == DOWNLOAD_SUCCESSFUL ) {
        GeoTiffFile geotiff ( geotiff_file_name.c_str() );
        GridTile grid_tile ( geotiff );

        grid_tile.writeBinaryFile( binary_file_name.data(), FLOAT_PIXEL );

        grid_tiles[tile_name] = grid_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }

    return TILE_NOT_AVAILABLE;
} /* int Field::loadGridTile ( std::string tile_name ) */

/*---------------------------------------------------------------*/

int Field::loadVectorTile ( std::string tile_name ) {
    std::string binary_file_name = DATA_DIR + "/" + tile_name + ".data";
    if ( FILE_EXISTS(binary_file_name.data()) ) {
        VectorTile vector_tile;
        vector_tile.readBinaryFile( binary_file_name.data() );

        vector_tiles[tile_name] = vector_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }

    std::string gml_file_name = DATA_DIR + "/" + tile_name + ".gml";
    if ( FILE_EXISTS(gml_file_name.data()) ) {
        GmlFile gml_file;
        gml_file.readGmlFile( gml_file_name );

        VectorTile vector_tile;
        vector_tile.fromGmlFile( gml_file );

        vector_tile.createBinaryFile( binary_file_name.c_str() );

        vector_tiles[tile_name] = vector_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }

    std::string gml_url = URL_LOD2 + tile_name + ".gml";
    if ( downloadFile( gml_url.data(), DATA_DIR.data() ) == DOWNLOAD_SUCCESSFUL ) {
        GmlFile gml_file;
        gml_file.readGmlFile( gml_file_name );

        VectorTile vector_tile;
        vector_tile.fromGmlFile( gml_file );

        vector_tile.createBinaryFile( binary_file_name.c_str() );

        vector_tiles[tile_name] = vector_tile;

        return TILE_LOADED_SUCCESSFULLY;
    }

    return TILE_NOT_AVAILABLE;
} /* int Field::loadVectorTile ( std::string tile_name ) */

/*---------------------------------------------------------------*/

bool Field::tileAlreadyLoaded ( std::string tile_name, int tile_type ) {
    switch ( tile_type ) {
        case GRID:
            return grid_tiles.contains( tile_name );
        case VECTOR:
            return vector_tiles.contains( tile_name );
        default:
            return false;
    }
} /* bool Field::tileAlreadyLoaded ( std::string tile_name, int tile_type ) */

/*---------------------------------------------------------------*/

std::string Field::buildTileName ( unsigned int x, unsigned int y ) {
    return std::to_string(x) + "_" + std::to_string(y);
} /* std::string Field::buildTileName ( unsigned int x, unsigned int y ) */

/*---------------------------------------------------------------*/

float Field::getHeightAtLatLon ( float lat, float lon ) {
    float x, y;
    LatLonToUTMXY( lat, lon, 32, x, y );

    unsigned int
        tile_x = (unsigned int) ( x / 1000.0 ),
        tile_y = (unsigned int) ( y / 1000.0 );

    std::string tile_name = buildTileName( tile_x, tile_y );

    if ( !tileAlreadyLoaded(tile_name, GRID) ) {
        loadGridTile( tile_name );
    }

    unsigned int
        easting =  (unsigned int) fmod( x, 1000.0 ),
        northing = (unsigned int) fmod( y, 1000.0 );

    GridTile& tile = grid_tiles[tile_name];

    return tile.getValue( easting, northing );
} /* float Field::getHeightAtLatLon ( float lat, float lon ) */
