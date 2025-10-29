#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#define STREQUAL(str1, str2) (!strcmp(str1, str2))

enum MessageTypes {
    NORMAL,
    DEBUG,
    ERROR
};

/*
Print a message to a specified output sink

Args:
 - type     : Output type (NORMAL, DEBUG, ERROR)
 - format   : printf format string
 - ...      : Variable argument list for printf
*/
void printMessage ( int type, const char* format, ... );

/*
Clamp a double variable to a number of decimal places
to prevent false negative comparisons

Args:
 - n : Double variable to clamp

Returns:
 - Clamped double value
*/
double clampDouble ( double n, int precision = 2 );

/*
Check if to variables are equal with a certain tolerance threshold

Args:
 - n1           : First number
 - n2           : Second number
 - threshold    : Tolerance threshold (must be positive)
*/
bool equalWithThreshold ( double n1, double n2, double threshold );

/*
Check if the absolute value of a variable n is less than a threshold

Args:
 - n            : Number to check
 - threshold:   : Threshold (must be positive)
*/
bool inRange ( double n, double threshold );

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

/*
Remove the file ending from a file name including the .

Args:
 - dest      : Destination to write the file name without the ending to
 - file_name : Pure file name without a preceding path
 - max_len   : Maximum length of the destination string (to prevent segfaults)
*/
void removeFileEnding ( char* dest, char* file_name, int max_len );

#endif
