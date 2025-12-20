#include "utils.h"
#include "status_codes.h"

#include <cmath>
#include <cstdio>
#include <cstdarg>

/*---------------------------------------------------------------*/

void printMessage ( int type, const char* format, ... ) {
    va_list argptr;

    switch ( type ) {
        case NORMAL:
            va_start( argptr, format );
            vfprintf( stdout, format, argptr );
            va_end( argptr );
            break;

        case DEBUG:
            va_start( argptr, format );
            vfprintf( stdout, format, argptr );
            va_end( argptr );
            break;

        case ERROR:
            va_start( argptr, format );
            vfprintf( stderr, format, argptr );
            va_end( argptr );
            break;
    }
} /* printMessage() */

/*---------------------------------------------------------------*/

double clampDouble ( double n, int precision ) {
    long n_long = (long) ( n * pow(10, precision) );

    return (double) ( (double)n_long / pow(10, precision) );
} /* clampDouble() */

/*---------------------------------------------------------------*/

double roundDouble ( double n, int precision ) {
    int shift = pow( 10, precision );
    return round( n * shift ) / shift;
} /* roundDouble() */

/*---------------------------------------------------------------*/

bool equalWithThreshold ( double n1, double n2, double threshold ) {
    if ( (n1 < 0.0 && n2 < 0.0) || (n1 > 0.0 && n2 > 0.0) ) {
        return fabs( fabs( n1 ) - fabs( n2 ) ) < threshold;
    }
    return false;

} /* equalWithThreshold() */

/*---------------------------------------------------------------*/

std::string buildTileName ( uint x, uint y ) {
    return std::to_string(x) + "_" + std::to_string(y);
} /* buildTileName() */

/*---------------------------------------------------------------*/

std::string extractFilename ( std::string path ) {
    int len = path.length();

    int i = len - 1;
    for ( ; i>=0; i-- ) {
        if ( path[i] == '/' ) {
            break;
        }
    }
    i++;

    return path.substr( i );
} /* extractFilename() */

/*---------------------------------------------------------------*/

std::string buildFilepath ( std::string dir, std::string file_name ) {
    if ( dir.back() != '/' ) {
        dir += '/';
    }

    return dir + file_name;
} /* buildFilepath() */

/*---------------------------------------------------------------*/

bool inRange ( double n, double threshold ) {
    return fabs(n) < threshold;
} /* inRange() */

/*---------------------------------------------------------------*/

std::string removeFileEnding ( std::string file_name ) {
    int len = file_name.length();
    int i=0;
    for ( ; i<len; i++ ) {
        if ( file_name[i] == '.' ) {
            break;
        }
    }

    return file_name.erase( i );
} /* removeFileEnding() */

/*---------------------------------------------------------------*/

void trimString ( std::string& str ) {
    size_t last_space = 0;
    size_t len = str.length();
    for ( size_t i = 0; i < len; i++ ) {
        if ( str[i] == ' ' || str[i] == '\n' ) {
            last_space++;
        }
        else {
            break;
        }
    }
    str.erase( 0, last_space );

    len -= last_space;

    last_space = 0;
    for ( size_t i = len-1; i >= 0; i-- ) {
        if ( str[i] == ' ' || str[i] == '\n' ) {

            last_space++;
        }
        else {
            break;
        }
    }

    str.erase( len-last_space );
} /* trimString () */

/*---------------------------------------------------------------*/

std::string* splitString ( std::string str, char delimiter ) {
    static std::string strings [2];
    strings[0].clear();
    strings[1].clear();

    size_t len = str.length();
    size_t i = 0;
    for ( ; i < len; i++ ) {
        if ( str[i] != delimiter ) {
            strings[0] += str[i];
        }
        else {
            break;
        }
    }

    i++;

    for ( ; i < len; i++ ) {
        strings[1] += str[i];
    }

    trimString( strings[0] );
    trimString( strings[1] );

    return strings;
} /* splitString () */
