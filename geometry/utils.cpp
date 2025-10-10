#include "utils.h"

#include <cmath>

double clampDouble ( double n ) {
    long n_long = (long) ( n * pow(10, NUM_DECIMAL_PLACES) );

    return (double) ( (double)n_long / pow(10, NUM_DECIMAL_PLACES) );
} /* double clampDouble ( double n ) */
