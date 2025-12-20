#ifndef UTILS_H
#define UTILS_H

#include <cstring>
#include <string>

#define STRNEQUAL(str1, str2, n)   !strncmp(str1, str2, n)
#define FILE_EXISTS(file_path)     !access(file_path, F_OK)

typedef unsigned int uint;

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
 - n         : Double variable to clamp
 - precision : Decimal places

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
Build the tile name string from the UTM easting and northing

Args:
 - x : Easting in km
 - y : Northing in km

Returns:
 - Tile name string "easting_northing"
*/
std::string buildTileName ( uint x, uint y );

/*
Extract the file name from a given file path or URL

Args:
 - path : File path or URL

Returns:
 - Extracted file name
*/
std::string extractFilename ( std::string path );

/*
Create a file path from a directory path and a file name

Args:
 - dir       : Directory path
 - file_name : File name

Returns:
 - Constructed file path
*/
std::string buildFilepath ( std::string dir, std::string file_name );

/*
Remove the file ending from a file name including the .

Args:
 - file_name : Pure file name without a preceding path

Returns:
 - File name without the file ending
*/
std::string removeFileEnding ( std::string file_name );

/*
Remove preceding and trailing spaces from a string

Args:
 - str : Reference to the string to trim
*/
void trimString ( std::string& str );

/*
Split string into two parts at the delimiter

Args:
 - str          : String to split into two parts
 - delimiter    : Symbol at which the string should be split

Returns:
 - string array of size 2
    - first element: Trimmed string before the delimiter
    - second element : Trimmed string after the delimiter
*/
std::string* splitString ( std::string str, char delimiter );


bool stringIsSint ( std::string str );
bool stringIsUint ( std::string str );
bool stringIsFloat ( std::string str );

#endif
