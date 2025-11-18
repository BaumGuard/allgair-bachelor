#ifndef VECTOR_TILE_H
#define VECTOR_TILE_H

#include "../geometry/polygon.h"
#include "../raw_data/gmlfile.h"
#include "../status_codes.h"

#include <vector>


#define PLANE_DISTANCE_THRESHOLD 0.1

/*
Class to save vector data (surfaces) read from a GML file
*/
class VectorTile {
public:

    /* GETTERS */

    /*
    Return the lower left corner of the tile as a Vector
    */
    Vector getLowerCorner () const;

    /*
    Return the upper right corner of the tile as a Vector
    */
    Vector getUpperCorner () const;

    /*
    Return a reference to the std::vector with the Polygon objects
    */
    std::vector<Polygon>& getPolygons ();



    /* INPUT / OUTPUT */

    /*
    Parse a GML file and initialize the tile with the data in the file

    Args:
     - file_path : Path to the binary file that should be read and parsed

    Returns:
     - Status code
        - CREATION_SUCCEEDED
    */
    int fromGmlFile ( GmlFile& gmlfile, float* success_rate = nullptr );

    /*
    Deserialize a binary file and initialize the tile object with
    the data in the file

    Args:
     - file_path : Path to the binary file to be read and
                   deserialized

    Returns:
     - Status code
        - BINARY_FILE_NOT_FOUND
        - CORRUPT_BINARY_FILE
        - READ_BINARY_FILE_SUCCESS (Success)
    */
    int readBinaryFile ( const char* file_path );

    /*
    Serialize the tile object and write it to a file in a binary
    format

    Args:
     - file_path : Path to the binary file that should be created

    Returns:
     - Status code
        - BINARY_FILE_NOT_CREATABLE
        - BINARY_FILE_CREATED (Success)
    */
    int createBinaryFile ( const char* file_path );



private:
    std::vector<Polygon> polygons;

    Vector
        lower_corner,
        upper_corner;
};

#endif
