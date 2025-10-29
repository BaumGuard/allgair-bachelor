#ifndef GEOTIFF_H
#define GEOTIFF_H

/*
Class to read and save the content of a GeoTIFF file
*/
class GeoTiffFile {
public:
    /* CONSTRUCTOR */

    /*
    Create a GeoTiffFile object from a GeoTIFF file

    Args:
     - file_path : File path to the GeoTIFF file
    */
    GeoTiffFile( const char* file_path );

    /* DESTRUCTOR */
    ~GeoTiffFile();


    /* GETTERS */

    /*
    Return the array with the data from the GeoTIFF file
    */
    float* getData ();

    /*
    Return the width of the tile
    */
    unsigned int getTileWidth ();

private:
    float* data;
    unsigned int tile_width;

};
#endif
