#include "utils.h"
#include "status_codes.h"

#include <cmath>
#include <cstring>
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
    return fabs( fabs( n1 ) - fabs( n2 ) ) < threshold;
} /* equalWithThreshold() */

/*---------------------------------------------------------------*/

void extractFilepath ( char* dst_path, char* path ) {
    int len = strlen( path );

    int i = len - 1;
    for ( ; i>=0; i-- ) {
        if ( path[i] == '/' ) {
            break;
        }
    }
    i++;

    strncpy( dst_path, &path[i], 32 );
} /* extractFilepath() */

/*---------------------------------------------------------------*/

void buildFilepath ( char* dst_path, char* dir, char* file_name ) {
    int dir_len = strlen( dir );

    if ( dir[dir_len-1] != '/' ) {
        dir[dir_len] = '/';
        dir[dir_len+1] = '\0';
    }

    sprintf( dst_path, "%s%s", dir, file_name );
} /* buildFilepath() */

/*---------------------------------------------------------------*/

bool inRange ( double n, double threshold ) {
    return fabs(n) < threshold;
} /* inRange() */

/*---------------------------------------------------------------*/

void removeFileEnding ( char* dest, char* file_name, int max_len ) {
    int i=0;
    for ( ; i<max_len; i++ ) {
        if ( file_name[i] == '.' ) {
            break;
        }
        dest[i] = file_name[i];
    }
    dest[i] = '\0';
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
