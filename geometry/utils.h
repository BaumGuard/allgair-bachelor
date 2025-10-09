#ifndef UTILS_H
#define UTILS_H

#include <cmath>

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

#endif
