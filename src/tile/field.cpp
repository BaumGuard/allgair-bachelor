#include "field.h"
#include "../web/download.h"
#include "../web/urls.h"
#include "../raw_data/geotiff.h"
#include "tile_name.h"
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

bool Field::tileAlreadyLoaded ( float lat, float lon, int tile_type ) {
    double xf, yf;
    LatLonToUTMXY( lat, lon, 32, xf, yf );

    uint
        x = (uint) ( xf / 1000.0 ),
        y = (uint) ( yf / 1000.0 );

    if ( tile_type == VECTOR ) {
        x -= x % 2;
        y -= y % 2;
    }

    std::string tile_name = buildTileName( x, y );

    return tileAlreadyLoaded( tile_name, tile_type );
} /* bool Field::tileAlreadyLoaded ( float lat, float lon, int tile_type ) */

/*---------------------------------------------------------------*/

float Field::getHeightAtLatLon ( float lat, float lon ) {
    double x, y;
    LatLonToUTMXY( lat, lon, 32, x, y );

    uint
        tile_x = (uint) ( x / 1000.0 ),
        tile_y = (uint) ( y / 1000.0 );

    std::string tile_name = buildTileName( tile_x, tile_y );

    if ( !tileAlreadyLoaded(tile_name, GRID) ) {
        loadGridTile( tile_name );
    }

    uint
        easting =  (uint) fmod( x, 1000.0 ),
        northing = (uint) fmod( y, 1000.0 );

    GridTile& tile = grid_tiles[tile_name];

    return tile.getValue( easting, northing );
} /* float Field::getHeightAtLatLon ( float lat, float lon ) */

/*---------------------------------------------------------------*/

std::vector<std::string> Field::tilesOnRay (
    float lat_start, float lon_start,
    float lat_end, float lon_end,
    uint tile_width_km
) {
    double x1, y1, x2, y2;
    LatLonToUTMXY( lat_start, lon_start, 32, x1, y1 );
    LatLonToUTMXY( lat_end, lon_end, 32, x2, y2 );

    x1 /= 1000.0;
    y1 /= 1000.0;
    x2 /= 1000.0;
    y2 /= 1000.0;

    float m = (float)( y2 - y1 ) / (float)( x2 - x1 );
    float t = y1 - m * x1;

    float
        x1_f = x1,
        x2_f = x2;

    float tmp;

    if ( x2 < x1 ) {
        x1_f = x2;
        x2_f = x1;
    }

    int
        x1_i = (int) floor( x1_f ),
        x2_i = (int) floor( x2_f );

    x1_i -= x1_i % tile_width_km;
    x2_i -= x2_i % tile_width_km;


    std::string current_tile;
    std::vector<std::string> tile_names;

    int
        y_bound1,
        y_bound2;

    for ( int i = x1_i; i < x2_i; i += tile_width_km ) {
        y_bound1 = (int) floor( m * i + t );
        y_bound2 = (int) floor( m * (i+1) + t );

        y_bound1 -= y_bound1 % tile_width_km;
        y_bound2 -= y_bound2 % tile_width_km;

        if ( y_bound2 < y_bound1 ) {
            tmp = y_bound1;
            y_bound1 = y_bound2;
            y_bound2 = tmp;
        }

        for ( int j = y_bound1; j <= y_bound2; j++ ) {
            current_tile = buildTileName( i, j );
            tile_names.push_back( current_tile );
        }
    }

    if ( x1 < x2 ) {
        current_tile = buildTileName(
            (int)floor(x2) - (int)floor(x2) % tile_width_km,
            (int)floor(y2) - (int)floor(y2) % tile_width_km
        );
        tile_names.push_back( current_tile );
    }
    else {
        current_tile = buildTileName(
            (int)floor(x1) - (int)floor(x1) % tile_width_km,
            (int)floor(y1) - (int)floor(y1) % tile_width_km
        );
        tile_names.push_back( current_tile );
    }

    return tile_names;
} /* std::vector<std::string> tilesOnRay ( float lat1, float lon1, float lat2, float lon2 ) */

/*---------------------------------------------------------------*/

int Field::bresenhamPseudo3D (
    Coord& intersection,
    float lat_start, float lon_start,
    float lat_end, float lon_end
)
{
    std::vector<std::string> tiles_on_ray =
        tilesOnRay( lat_start, lon_start, lat_end, lon_end, 1 );

    std::vector<GridTile> tiles;

    uint len = tiles_on_ray.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( !tileAlreadyLoaded(tiles_on_ray[i], GRID) ) {
            loadGridTile( tiles_on_ray[i] );
        }
        tiles.push_back( grid_tiles[tiles_on_ray[i]] );
    }

    // TODO : Bresenham-Algorithmus
} /* bresenhamPseudo3D() */

/*---------------------------------------------------------------*/

int Field::surfaceIntersection (
    Coord& intersection,
    float lat_start, float lon_start,
    float lat_end, float lon_end
)
{
    std::vector<std::string> tiles_on_ray =
        tilesOnRay( lat_start, lon_start, lat_end, lon_end, 2 );

    std::vector<VectorTile> tiles;

    uint len = tiles_on_ray.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( !tileAlreadyLoaded(tiles_on_ray[i], VECTOR) ) {
            loadVectorTile( tiles_on_ray[i] );
        }
        tiles.push_back( vector_tiles[tiles_on_ray[i]] );
    }

    // TODO
} /* surfaceIntersection() */
