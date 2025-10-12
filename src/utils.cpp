#include "utils.h"
#include "status_codes.h"

#include <cmath>
#include "string.h"
#include "stdio.h"

/*---------------------------------------------------------------*/

double clampDouble ( double n ) {
    long n_long = (long) ( n * pow(10, NUM_DECIMAL_PLACES) );

    return (double) ( (double)n_long / pow(10, NUM_DECIMAL_PLACES) );
} /* double clampDouble ( double n ) */

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
} /* void extractFilepath ( char* dst_path, char* path ) */

/*---------------------------------------------------------------*/

void buildFilepath ( char* dst_path, char* dir, char* file_name ) {
    int dir_len = strlen( dir );

    if ( dir[dir_len-1] != '/' ) {
        dir[dir_len] = '/';
        dir[dir_len+1] = '\0';
    }

    sprintf( dst_path, "%s%s", dir, file_name );
} /* void buildFilepath ( char* dst_path, char* dir, char* file_name ) */
