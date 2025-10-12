#ifndef UTILS_H
#define UTILS_H

#define NUM_DECIMAL_PLACES 10

/*
Clamp a double variable to a number of decimal places
to prevent false negative comparisons

Args:
 - n : Double variable to clamp

Returns:
 - Clamped double value
*/
double clampDouble ( double n );

/*
Extract the file name from a given file path or URL

Args:
 - dst_path : Pointer to the destination string where to
              store the file name
 - path     : File path or URL
*/
void extractFilepath ( char* dst_path, char* path );

/*
Create a file path from a directory path and a file name

Args:
 - dst_path  : Pointer to the destination string where to
               store the file path
 - dir       : Directory path
 - file_name : File name
*/
void buildFilepath ( char* dst_path, char* dir, char* file_name );

#endif
