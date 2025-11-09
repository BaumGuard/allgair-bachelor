#ifndef RAY_H
#define RAY_H

#include "../geometry/line.h"

typedef struct {
    Line ray_line;
    float signal_level;
} Ray;

#endif
