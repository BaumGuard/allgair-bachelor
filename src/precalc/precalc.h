#ifndef PRECALC_H
#define PRECALC_H

#include "../geometry/vector.h"
#include "../geometry/polygon.h"

#include <vector>

Polygon getPolygonWithMinDistance ( Vector& start_point, std::vector<Polygon>& polygons );

Polygon getPolygonWithMaxArea ( std::vector<Polygon>& polygons );

#endif
