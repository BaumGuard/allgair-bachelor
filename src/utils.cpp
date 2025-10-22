#include "utils.h"
#include "status_codes.h"

#include <cmath>
#include "string.h"
#include "stdio.h"

/*---------------------------------------------------------------*/

double clampDouble ( double n, int precision ) {
    long n_long = (long) ( n * pow(10, precision) );

    return (double) ( (double)n_long / pow(10, precision) );
} /* double clampDouble ( double n ) */

/*---------------------------------------------------------------*/

double roundDouble ( double n, int precision ) {
    int shift = pow( 10, precision );
    return round( n * shift ) / shift;
}

bool equalWithThreshold ( double n1, double n2, double threshold ) {
    return fabs( fabs( n1 ) - fabs( n2 ) ) < threshold;
}

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


bool inRange ( double n, double threshold ) {
    return fabs(n) < threshold;
}
