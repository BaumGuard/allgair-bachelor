#include "load_tile.h"

#include "../raw_data/geotiff.h"
#include "../web/download.h"
#include "../web/urls.h"
#include "../utils.h"
#include "../tile/tile_types.h"

#include <unistd.h>

std::string DATA_DIR = "data";

/*---------------------------------------------------------------*/

int getGridTile ( GridTile& grid_tile, std::string tile_name, int tile_type ) {
    // Build the file name/path of the tif file
    std::string raw_file_name;

    std::string data_dir = "data";

    switch ( tile_type ) {
        case DGM1:
            data_dir = DATA_DIR + "/DGM1";
            raw_file_name = tile_name + ".tif";
            break;

        case DGM20:
            data_dir = DATA_DIR + "/DGM1";
            raw_file_name = tile_name + ".tif";
            break;

        case DOM20:
            data_dir = DATA_DIR + "/DOM20";
            raw_file_name = "32" + tile_name + "_20_DOM.tif";
            break;

        case DOM1:
            data_dir = DATA_DIR + "/DOM20";
            raw_file_name = "32" + tile_name + "_20_DOM.tif";
            break;
    }

    std::string raw_file_path = data_dir + "/" + raw_file_name;


    // Check for the existence of the tif file
    printMessage(
        NORMAL,
        "Checking for raw file '%s' in '%s'... ",
        raw_file_name.data(),
        data_dir.data()
    );

    if ( FILE_EXISTS(raw_file_path.data()) ) {
        GeoTiffFile geotiff;

        // Read the raw tiff file
        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DGM20:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );

                // Resample the DGM1 tile from 1 m resolution to
                // 20 cm resolution
                grid_tile.resampleTile( 5.0 );
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DOM1:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );

                // Resample the DOM20 tile from 20 cm resultion to
                // 1 m resolution
                grid_tile.resampleTile( 0.2 );
                break;
        }

        printMessage( NORMAL, "Found\n" );
        return SUCCESS;
    }
    printMessage( NORMAL, "Not found\n" );


    // Download the raw file
    std::string url;
    switch ( tile_type ) {
        case DGM1:
            url = URL_DGM1 + raw_file_name;
            break;

        case DGM20:
            url = URL_DGM1 + raw_file_name;
            break;

        case DOM20:
            url = URL_DOM20 + raw_file_name;
            break;

        case DOM1:
            url = URL_DOM20 + raw_file_name;
            break;
    }


    printMessage(
        NORMAL,
        "Downloading the raw file '%s' into '%s'... ",
        raw_file_name.data(),
        data_dir.data()
    );
    if ( downloadFile(url, data_dir) == SUCCESS ) {
        GeoTiffFile geotiff;

        // Read the tif file
        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DGM20:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );

                // Resample the DGM1 tile from 1 m resolution to
                // 20 cm resolution
                grid_tile.resampleTile( 5.0 );
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );
                break;

            case DOM1:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );

                // Resample the DOM20 tile from 20 cm resultion to
                // 1 m resolution
                grid_tile.resampleTile( 0.2 );
                break;
        }


        printMessage( NORMAL, "Done\n" );
        return SUCCESS;
    }

    printMessage( NORMAL, "Error\n" );
    return TILE_NOT_AVAILABLE;
}

/*---------------------------------------------------------------*/

int getVectorTile ( VectorTile& vector_tile, std::string tile_name ) {
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
        printMessage(
            NORMAL,
            "Checking for binary file '%s' in '%s'... ",
            binary_file_name.data(), data_dir.data()
        );

        vector_tile.fromBinaryFile( binary_file_path );

        printMessage( NORMAL, "Found\n" );
        return SUCCESS;
    }
    else if ( !binary_file_exists ) {
        printMessage( NORMAL, "Not found\n" );
    }


    // Check for the existence of the gml file
    printMessage(
        NORMAL,
        "Checking for raw file '%s' in '%s'... ",
        raw_file_name.data(),
        data_dir.data()
    );

    if ( FILE_EXISTS(raw_file_path.data()) ) {
        // Read the gml file and generate a binary file
        GmlFile gml_file;
        gml_file.readGmlFile( raw_file_path );

        vector_tile.fromGmlFile( gml_file );
        vector_tile.createBinaryFile( binary_file_path );

        printMessage( NORMAL, "Found\n" );
        return SUCCESS;
    }
    printMessage( NORMAL, "Not found\n" );


    // Download the raw file
    std::string url = URL_LOD2 + raw_file_name;

    printMessage(
        NORMAL,
        "Downloading the raw file '%s' into '%s'... ",
        raw_file_name.data(),
        data_dir.data()
    );
    if ( downloadFile(url, data_dir) == SUCCESS ) {

        // Read the gml file and generate a binary file
        GmlFile gml_file;
        gml_file.readGmlFile( raw_file_path );

        vector_tile.fromGmlFile( gml_file );
        vector_tile.createBinaryFile( binary_file_path );

        printMessage( NORMAL, "Done\n" );
        return SUCCESS;
    }

    printMessage( NORMAL, "Error\n" );
    return TILE_NOT_AVAILABLE;
}
