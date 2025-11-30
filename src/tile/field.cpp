#include "field.h"

#include "../geometry/vector.h"
#include "../geometry/line.h"
#include "../geometry/polygon.h"
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

int Field::loadTile ( std::string tile_name, int tile_type ) {
    // Build the file name/path of the binary file and the
    // raw file (.gml/.tif)
    std::string binary_file_name;
    std::string raw_file_name;
    switch ( tile_type ) {
        case DGM1:
            binary_file_name = tile_name + "_DGM1.grid";
            raw_file_name = tile_name + ".tif";
            break;

        case DOM20:
            binary_file_name = tile_name + "_DOM20.grid";
            raw_file_name = "32" + tile_name + "_20_DOM.tif";
            break;

        case LOD2:
            binary_file_name = tile_name + "_LOD2.data";
            raw_file_name = tile_name + ".gml";
            break;
    }

    std::string binary_file_path = DATA_DIR + "/" + binary_file_name;
    std::string raw_file_path = DATA_DIR + "/" + raw_file_name;


    // Check for the existence of the binary file
    printMessage(
        NORMAL,
        "Checking for binary file '%s' in '%s'... ",
        binary_file_name.data(), DATA_DIR.data()
    );
    if ( FILE_EXISTS(binary_file_path.data()) ) {
        GridTile grid_tile;
        VectorTile vector_tile;

        switch ( tile_type ) {
            case DGM1:
                grid_tile.fromBinaryFile( binary_file_path );
                grid_tiles_dgm1[tile_name] = grid_tile;
                break;

            case DOM20:
                grid_tile.fromBinaryFile( binary_file_path );
                grid_tiles_dom20[tile_name] = grid_tile;
                break;

            case LOD2:
                vector_tile.fromBinaryFile( binary_file_path );
                vector_tiles_lod2[tile_name] = vector_tile;
                break;
        }


        printMessage( NORMAL, "Found\n" );
        return SUCCESS;
    }
    printMessage( NORMAL, "Not found\n" );


    // Check for the existence of the raw file (.gml/.tif)
    printMessage(
        NORMAL,
        "Checking for raw file '%s' in '%s'... ",
        raw_file_name.data(),
        DATA_DIR.data()
    );

    if ( FILE_EXISTS(raw_file_path.data()) ) {
        GeoTiffFile geotiff;
        GridTile grid_tile;

        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );

                grid_tile.writeBinaryFile( binary_file_path, FLOAT_PIXEL );

                grid_tiles_dgm1[tile_name] = grid_tile;
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );

                grid_tile.writeBinaryFile( binary_file_path, FLOAT_PIXEL );

                grid_tiles_dom20[tile_name] = grid_tile;
                break;

            case LOD2:
                GmlFile gml_file;
                gml_file.readGmlFile( raw_file_path );

                VectorTile vector_tile;
                vector_tile.fromGmlFile( gml_file );

                vector_tile.createBinaryFile( binary_file_path );

                vector_tiles_lod2[tile_name] = vector_tile;
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

        case DOM20:
            url = URL_DOM20 + raw_file_name;
            break;

        case LOD2:
            url = URL_LOD2 + raw_file_name;
            break;
    }
    printMessage(
        NORMAL,
        "Downloading the raw file '%s' into '%s'... ",
        raw_file_name.data(),
        DATA_DIR.data()
    );
    if ( downloadFile( url, DATA_DIR ) == SUCCESS ) {
        GeoTiffFile geotiff;
        GridTile grid_tile;

        switch ( tile_type ) {
            case DGM1:
                geotiff.readGeoTiffFile( raw_file_path, DGM1 );
                grid_tile.fromGeoTiffFile( geotiff );

                grid_tile.writeBinaryFile( binary_file_path, FLOAT_PIXEL );

                grid_tiles_dgm1.insert( {tile_name, grid_tile} );
                break;

            case DOM20:
                geotiff.readGeoTiffFile( raw_file_path, DOM20 );
                grid_tile.fromGeoTiffFile( geotiff );

                grid_tile.writeBinaryFile( binary_file_path, FLOAT_PIXEL );

                grid_tiles_dom20.insert( {tile_name, grid_tile} );
                break;

            case LOD2:
                GmlFile gml_file;
                gml_file.readGmlFile( raw_file_path );


                VectorTile vector_tile;
                vector_tile.fromGmlFile( gml_file );

                vector_tile.createBinaryFile( binary_file_path );

                vector_tiles_lod2.insert( {tile_name, vector_tile} );
                break;
        }


        printMessage( NORMAL, "Done\n" );
        return SUCCESS;
    }

    printMessage( NORMAL, "Error\n" );
    return TILE_NOT_AVAILABLE;
} /* loadTile() */

/*---------------------------------------------------------------*/

bool Field::tileAlreadyLoaded ( std::string tile_name, int tile_type ) {
    switch ( tile_type ) {
        case DGM1:
            return grid_tiles_dgm1.contains( tile_name );
        case DOM20:
            return grid_tiles_dom20.contains( tile_name );
        case LOD2:
            return vector_tiles_lod2.contains( tile_name );
        default:
            return false;
    }
} /* tileAlreadyLoaded() */

/*---------------------------------------------------------------*/

bool Field::tileAlreadyLoaded ( double lat, double lon, int tile_type ) {
    double xf, yf;
    LatLonToUTMXY( lat, lon, 32, xf, yf );

    uint
        x = (uint) ( xf / 1000.0 ),
        y = (uint) ( yf / 1000.0 );

    if ( tile_type == LOD2 ) {
        x -= x % 2;
        y -= y % 2;
    }

    std::string tile_name = buildTileName( x, y );

    return tileAlreadyLoaded( tile_name, tile_type );
} /* tileAlreadyLoaded() */

/*---------------------------------------------------------------*/

double Field::getAltitudeAtLatLon ( double lat, double lon, int tile_type ) {
    double x, y;
    LatLonToUTMXY( lat, lon, 32, x, y );

    uint
        tile_x = (uint) ( x / 1000.0 ),
        tile_y = (uint) ( y / 1000.0 );

    std::string tile_name = buildTileName( tile_x, tile_y );

    if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
        loadTile( tile_name, tile_type );
    }

    uint
        easting =  (uint) fmod( x, 1000.0 ),
        northing = (uint) fmod( y, 1000.0 );

    GridTile* tile;
    switch ( tile_type ) {
        case DGM1:
            tile = &grid_tiles_dgm1[tile_name];
            break;

        case DOM20:
            tile = &grid_tiles_dom20[tile_name];
            break;
    }


    float value = 0.0;
    tile->getValue( easting, northing, value );

    return value;
} /* getAltitudeAtLatLon() */

/*---------------------------------------------------------------*/

double Field::getAltitudeAtXY ( uint x, uint y, int tile_type ) {
    uint
        tile_x = x / 1000,
        tile_y = y / 1000;

    std::string tile_name = buildTileName( tile_x, tile_y );

    if ( !tileAlreadyLoaded(tile_name, tile_type) ) {
        loadTile( tile_name, tile_type );
    }

    uint
        easting = x % 1000,
        northing = y % 1000;

    GridTile* tile;
    switch ( tile_type ) {
        case DGM1:
            tile = &grid_tiles_dgm1[tile_name];
            break;

        case DOM20:
            tile = &grid_tiles_dom20[tile_name];
            break;
    }

    float value;
    tile->getValue( easting, northing, value );

    return value;
} /* getAltitudeAtXY () */

/*---------------------------------------------------------------*/

std::vector<std::string> Field::tilesOnRay (
    double lat_start, double lon_start,
    double lat_end, double lon_end,
    uint tile_width_km
) {
    double x1, y1, x2, y2;
    LatLonToUTMXY( lat_start, lon_start, 32, x1, y1 );
    LatLonToUTMXY( lat_end, lon_end, 32, x2, y2 );

    x1 /= 1000.0;
    y1 /= 1000.0;
    x2 /= 1000.0;
    y2 /= 1000.0;

    double m = (double)( y2 - y1 ) / (double)( x2 - x1 );
    double t = y1 - m * x1;

    double
        x1_f = x1,
        x2_f = x2;

    double tmp;

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

        for ( int j = y_bound1; j <= y_bound2; j += tile_width_km ) {
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
} /* tilesOnRay() */

/*---------------------------------------------------------------*/

int Field::bresenhamPseudo3D (
    Coord& intersection,
    double lat_start, double lon_start, double alt_start,
    double lat_end, double lon_end, double alt_end,
    int tile_type
)
{
    if ( tile_type != DGM1 && tile_type != DOM20 ) {
        return INVALID_TILE_TYPE;
    }

    // Converting latitude/longitude to UTM coordinates
    double
        x_start_f, y_start_f,
        x_end_f, y_end_f;

    LatLonToUTMXY(
        (double) lat_start, (double) lon_start,
        32,
        x_start_f, y_start_f
    );
    LatLonToUTMXY(
        lat_end, lon_end,
        32,
        x_end_f, y_end_f
    );

    // Cast start and end values to integers
    int
        x_start = (int) x_start_f,
        y_start = (int) y_start_f,
        z_start = (int) round( alt_start ),
        x_end   = (int) x_end_f,
        y_end   = (int) y_end_f,
        z_end   = (int) round( alt_end );

    // Distances between the start and end coordinate
    int
        dx = abs( x_end - x_start ),
        dy = abs( y_end - y_start ),
        dz = abs( z_end - z_start );

    // Axes
    enum { X, Y, Z };

    // Find the largest distance
    // The corresponding axis will become the iteration axis
    int axis;
    if ( dx > dy && dx > dz ) axis = X;
    else if ( dy > dz ) axis = Y;
    else axis = Z;

    int
        e1, e2,                // Bresenham error (if 0 increment/decrement dep1/dep2)
        it1, it2,              // Iterators to subtract from e1/e2
        corr,                  // Correction value to add to e1/e2 when they become negative
        start_it, end_it,      // Start/End value on the iteration axis
        dep1, dep2,            // Values dependant on the iteration axis (the other two axes)
        start_dep1, end_dep1,  // Start/End value on the first dependant axis
        start_dep2, end_dep2;  // Start/End value on the second dependant axis


    int x, y, z;

    // Map the x, y and z values to iteration and dependant coordinates
    switch ( axis ) {
        case X:
            e1          = dx;
            e2          = dx;
            start_it    = x_start;
            end_it      = x_end;
            start_dep1  = y_start;
            end_dep1    = y_end;
            start_dep2  = z_start;
            end_dep2    = z_end;
            it1         = 2 * dy;
            it2         = 2 * dz;
            corr        = 2 * dx;
            dep1        = y_start;
            dep2        = z_start;
            break;

        case Y:
            e1          = dy;
            e2          = dy;
            start_it    = y_start;
            end_it      = y_end;
            start_dep1  = x_start;
            end_dep1    = x_end;
            start_dep2  = z_start;
            end_dep2    = z_end;
            it1         = 2 * dx;
            it2         = 2 * dz;
            corr        = 2 * dy;
            dep1        = x_start;
            dep2        = z_start;
            break;

        case Z:
            e1          = dz;
            e2          = dz;
            start_it    = z_start;
            end_it      = z_end;
            start_dep1  = x_start;
            end_dep1    = x_end;
            start_dep2  = y_start;
            end_dep2    = y_end;
            it1         = 2 * dx;
            it2         = 2 * dy;
            corr        = 2 * dz;
            dep1        = x_start;
            dep2        = y_start;
            break;
    }


    int it = start_it;

    // Find an intersection between the ray and the ground
    // using Bresenham's algorithm modified for 3D
    while ( it != end_it ) {
        e1 -= it1;
        e2 -= it2;

        if ( e1 < 0 ) {
            if ( start_dep1 < end_dep1 ) {
                dep1++;
            }
            else {
                dep1--;
            }
            e1 += corr;
        }

        if ( e2 < 0 ) {
            if ( start_dep2 < end_dep2 ) {
                dep2++;
            }
            else {
                dep2--;
            }
            e2 += corr;
        }


        if ( start_it < end_it ) {
            it++;
        }
        else {
            it--;
        }


        // Map back to x, y and z values
        switch ( axis ) {
            case X:
                x = it;
                y = dep1;
                z = dep2;
                break;

            case Y:
                x = dep1;
                y = it;
                z = dep2;
                break;

            case Z:
                x = dep1;
                y = dep2;
                z = it;
                break;
        }

        // Get the altitude at the current x/y position
        int altitude_at_xy = (int) round( getAltitudeAtXY(x, y, tile_type) );

        // If the value of z is equal or smaller than the altitude
        // at x/y they ray has hit the ground
        if ( z <= altitude_at_xy ) {
            double lat_final, lon_final;

            UTMXYToLatLon(
                (double)x, (double)y,
                32, false,
                lat_final, lon_final
            );

            // Convert the intersection point back to latitude/longitude
            intersection.lat = (double) RAD_TO_DEG( lat_final );
            intersection.lon = (double) RAD_TO_DEG( lon_final );
            intersection.altitude = z;

            return INTERSECTION_FOUND;
        }
    }

    return NO_INTERSECTION_FOUND;
} /* bresenhamPseudo3D() */

/*---------------------------------------------------------------*/

int Field::surfaceIntersection (
    Coord& intersection,
    double lat_start, double lon_start, double alt_start,
    double lat_end, double lon_end, double alt_end
)
{
    // Find all tiles in the path of the ray and load them
    // if not already done
    std::vector<std::string> tiles_on_ray =
        tilesOnRay( lat_start, lon_start, lat_end, lon_end, 2 );

    std::vector<VectorTile> tiles;

    uint len = tiles_on_ray.size();
    for ( uint i = 0; i < len; i++ ) {
        if ( !tileAlreadyLoaded(tiles_on_ray[i], LOD2) ) {
            loadTile( tiles_on_ray[i], LOD2 );
        }
        tiles.push_back( vector_tiles_lod2[tiles_on_ray[i]] );
    }

    // Transform lat/lon coordinates to UTM32 coordinates
    double start_x, start_y, end_x, end_y;
    LatLonToUTMXY( lat_start, lon_start, 32, start_x, start_y );
    LatLonToUTMXY( lat_end, lon_end, 32, end_x, end_y );

    Vector
        start_point ( start_x, start_y, alt_start ),
        end_point ( end_x, end_y, alt_end ),
        intersect;


    // Create a ray between the start and the end point
    Line ray;
    ray.createLineFromTwoPoints( start_point, end_point );

    int status;
    bool found_intersection = false;

    // List for storing all intersections of the ray with surfaces
    std::vector<Vector> intersections;
    std::vector<Polygon> polygons;

    for ( uint i = 0; i < len; i++ ) {
        std::vector<Polygon>& surfaces = tiles[i].getPolygons();

        uint surfaces_len = surfaces.size();
        for ( uint j = 0; j < surfaces_len; j++ ) {
            status = surfaces[j].lineIntersection( ray, intersect );

            if ( status == INTERSECTION_FOUND ) {
                intersections.push_back( intersect );
                polygons.push_back( surfaces[j] );
                found_intersection = true;
            }
        }
    }

    // Find the intersection closest to the starting point
    if ( found_intersection ) {
        double min_distance = ( intersections[0] - start_point ).length();
        double current_distance;
        uint min_index = 0;

        uint intersections_len = intersections.size();
        for ( uint i = 1; i < intersections_len; i++ ) {
            current_distance = ( intersections[i] - start_point ).length();

            if ( current_distance < min_distance ) {
                min_distance = current_distance;
                min_index = i;
            }
        }

        // Transform back to lat/lon coordinates
        double intersection_lat, intersection_lon;
        UTMXYToLatLon(
            intersections[min_index].getX(), intersections[min_index].getY(),
            32, false,
            intersection_lat, intersection_lon
        );

        intersection.lat = RAD_TO_DEG( intersection_lat );
        intersection.lon = RAD_TO_DEG( intersection_lon );
        intersection.altitude = intersections[min_index].getZ();

        for ( size_t k = 0; k < polygons.size(); k++ ) {
            polygons[k].printPolygon();
            printf("------------\n");
            intersections[k].printVector();
            printf("------------\n");
            polygons[k].getBasePlane().getVector1().printVector();
        }

        return INTERSECTION_FOUND;
    }

    return NO_INTERSECTION_FOUND;
} /* surfaceIntersection() */
