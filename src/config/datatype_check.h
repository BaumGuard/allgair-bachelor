#ifndef DATATYPE_CHECK_H
#define DATATYPE_CHECK_H

#include <string>

bool stringIsSint ( std::string str ) {
    size_t len = str.length();
    size_t it = 0;

    int state = 0;

    while ( it < len ) {
        switch ( state ) {
            case 0:
                if ( str[it] == '-' ) state = 1;
                else if ( str[it] >= 48 && str[it] <= 57 ) state = 2;
                else return false;
                break;

            case 1:
                if ( str[it] >= 48 && str[it] <= 57 ) state = 2;
                else return false;
                break;

            case 2:
                if ( !(str[it] >= 48 && str[it] <= 57) ) return false;
                break;
        }
        it++;
    }

    return state == 2;
} /* stringIsSint () */

/*---------------------------------------------------------------*/

bool stringIsUint ( std::string str ) {
    size_t len = str.length();
    size_t it = 0;

    int state = 0;

    while ( it < len ) {
        switch ( state ) {
            case 0:
                if ( str[it] >= 48 && str[it] <= 57 ) state = 1;
                else return false;
                break;

            case 1:
                if ( !(str[it] >= 48 && str[it] <= 57) ) return false;
                break;
        }
        it++;
    }

    return state == 1;
} /* stringIsUint () */

/*---------------------------------------------------------------*/

bool stringIsFloat ( std::string str ) {
    size_t len = str.length();
    size_t it = 0;

    int state = 0;

    while ( it < len ) {
        switch ( state ) {
            case 0:
                if ( str[it] >= 48 && str[it] <= 57 ) state = 2;
                else if ( str[it] == '-' ) state = 1;
                else if ( str[it] == '.' ) state = 3;
                else return false;
                break;

            case 1:
                if ( str[it] >= 48 && str[it] <= 57 ) state = 1;
                else if ( str[it] == '.' ) state = 3;
                else return false;
                break;

            case 2:
                if ( str[it] >= 48 && str[it] <= 57 ) state = 2;
                else if ( str[it] == '.' ) state = 3;
                else return false;
                break;

            case 3:
                if ( !(str[it] >= 48 && str[it] <= 57) ) return false;
                break;
        }
        it++;
    }

    return state == 3;
} /* stringIsFloat () */

#endif
