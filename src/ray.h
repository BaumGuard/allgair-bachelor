#ifndef RAY_H
#define RAY_H

#include "../geometry/line.h"

typedef struct {
    Line ray_line;
    double signal_level;
} Ray;

#endif
