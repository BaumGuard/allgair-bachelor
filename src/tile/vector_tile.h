#ifndef VECTOR_TILE_H
#define VECTOR_TILE_H

#include "../geometry/polygon.h"
#include "../raw_data/gmlfile.h"
#include "../config/global_config.h"
#include "../status_codes.h"

#include <vector>

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

    /*
    Return a reference to the std::vector with the start indicies of
    all tile sections
    */
    std::vector<uint>& getSectionStarts ();



    /* INPUT / OUTPUT */

    /*
    Parse a GML file and initialize the tile with the data in the file

    Args:
     - file_path : Path to the binary file that should be read and parsed

    Returns:
     - Status code
        - SUCCESS
    */
    int fromGmlFile ( GmlFile& gmlfile );

    /*
    Deserialize a binary file and initialize the tile object with
    the data in the file

    Args:
     - file_path : Path to the binary file to be read and
                   deserialized

    Returns:
     - Status code
        - SUCCESS

        - FILE_NOT_FOUND
        - FILE_CORRUPT
    */
    int fromBinaryFile ( std::string file_path );

    /*
    Serialize the tile object and write it to a file in a binary
    format

    Args:
     - file_path : Path to the binary file that should be created

    Returns:
     - Status code
        - SUCCESS

        - FILE_NOT_CREATABLE
    */
    int createBinaryFile ( std::string file_path );



private:
    std::vector<Polygon> polygons;
    std::vector<uint> section_starts;

    Vector
        lower_corner,
        upper_corner;

    float error_rate;

    /*
    Sort the list of polygons by the x or y coordinate of
    the polygos's centroid using the Quick Sort algorithm

    Args:
     - start : Start index of the list part
     - end   : End index of the list part
     - by_x  : True -> sort by x coordinate, False -> sort by y coordinate
    */
    int partition ( int start, int end, bool by_x );
    void sortPolygons ( int start, int end, bool by_x );

    /*
    Divide the tile into stripes and find the start of each stripe
    in the polygon list

    Args:
     - n_stripes : Number of stripes to divide the tile into
    */
    void orderPolygonsInStripes ( int n_stripes );
};

#endif
