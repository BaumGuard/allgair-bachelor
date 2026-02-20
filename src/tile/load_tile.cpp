#include "load_tile.h"

#include "../raw_data/geotiff.h"
#include "../web/download.h"
#include "../config/global_config.h"
#include "../utils.h"
#include "../tile/tile_types.h"

#include <unistd.h>
#include <cstdlib>

/*---------------------------------------------------------------*/

int getGridTile ( GridTile& grid_tile, std::string tile_name, int tile_type ) {
    // Build the file name/path of the tif file
    std::string raw_file_name, raw_file_name_masked;

    std::string data_dir = "data";
    std::string data_dir_masked = "data";

    switch ( tile_type ) {
        case DGM1:
            data_dir = DATA_DIR + "/DGM1";
            raw_file_name = tile_name + ".tif";
            break;

        case DOM20:
            data_dir = DATA_DIR + "/DOM20";
            raw_file_name = "32" + tile_name + "_20_DOM.tif";
            break;

        case DOM20_MASKED:
            data_dir = DATA_DIR + "/DOM20";
            data_dir_masked = DATA_DIR + "/DOM20_MASKED";
            raw_file_name = "32" + tile_name + "_20_DOM.tif";
            raw_file_name_masked = "32" + tile_name + "_20_DOM_MASKED.tif";
            break;
    }

    std::string raw_file_path = data_dir + "/" + raw_file_name;
    std::string raw_file_path_masked = data_dir_masked + "/" + raw_file_name_masked;

    GeoTiffFile geotiff;

    if ( (tile_type == DGM1 || tile_type == DOM20) && FILE_EXISTS(raw_file_path.data()) ) {

        // Read the raw tiff file
        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;
        }

        return SUCCESS;
    }
    else if ( tile_type == DOM20_MASKED && FILE_EXISTS(raw_file_path_masked.data()) ) {
        geotiff.readGeoTiffFile( raw_file_path_masked, DOM20_MASKED );
        grid_tile.fromGeoTiffFile( geotiff );

        return SUCCESS;
    }

    if ( tile_type == DOM20_MASKED ) {
        getGridTile( grid_tile, tile_name, DOM20 );

        VectorTile vector_tile;
        getVectorTile( vector_tile, tile_name );

        std::string dom20_masked_filepath =
            "data/DOM20_MASKED/32" + tile_name + "_20_DOM_MASKED.tif";

        grid_tile.maskTile( vector_tile );
        grid_tile.createTifFile( dom20_masked_filepath );

        return SUCCESS;
    }


    // Download the raw file
    std::string url;
    switch ( tile_type ) {
        case DGM1:
            url = CHOSEN_URL_DGM1 + raw_file_name;
            break;

        case DOM20:
            url = CHOSEN_URL_DOM20 + raw_file_name;
            break;

        case DOM20_MASKED:
            url = CHOSEN_URL_DOM20 + raw_file_name;
            break;
    }

    if ( downloadFile(url, data_dir) == SUCCESS ) {
        GeoTiffFile geotiff;

        // Read the tif file
        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DOM20_MASKED:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );

                VectorTile vector_tile;
                getVectorTile( vector_tile, tile_name );

                grid_tile.maskTile( vector_tile );
                grid_tile.createTifFile( raw_file_path_masked );

                break;
        }

        return SUCCESS;
    }

    return TILE_NOT_AVAILABLE;
}

/*---------------------------------------------------------------*/

int getVectorTile ( VectorTile& vector_tile, std::string tile_name ) {

    std::string tile_name_parts [2];
    splitString( tile_name, tile_name_parts, '_' );
    uint
        easting  = std::stoi( tile_name_parts[0] ),
        northing = std::stoi( tile_name_parts[1] );

    easting  = easting  - (easting  % 2);
    northing = northing - (northing % 2);

    tile_name = buildTileName( easting, northing );

    // Build the file name/path of the binary file and the raw file (.gml)

    std::string
        data_dir = DATA_DIR + "/LOD2",
        binary_file_name = tile_name + "_LOD2.data",
        raw_file_name    = tile_name + ".gml";

    std::string
        binary_file_path = data_dir + "/" + binary_file_name,
        raw_file_path    = data_dir + "/" + raw_file_name;

    // Check for the existence of the binary file
    bool binary_file_exists = FILE_EXISTS( binary_file_path.data() );
    if ( binary_file_exists ) {
        vector_tile.fromBinaryFile( binary_file_path );
        return SUCCESS;
    }

    if ( FILE_EXISTS(raw_file_path.data()) ) {
        // Read the gml file and generate a binary file
        GmlFile gml_file;
        gml_file.readGmlFile( raw_file_path );

        vector_tile.fromGmlFile( gml_file );
        vector_tile.createBinaryFile( binary_file_path );

        return SUCCESS;
    }

    // Download the raw file
    std::string url = CHOSEN_URL_LOD2 + raw_file_name;

    if ( downloadFile(url, data_dir) == SUCCESS ) {

        // Read the gml file and generate a binary file
        GmlFile gml_file;
        gml_file.readGmlFile( raw_file_path );

        vector_tile.fromGmlFile( gml_file );
        vector_tile.createBinaryFile( binary_file_path );

        return SUCCESS;
    }

    return TILE_NOT_AVAILABLE;
}
