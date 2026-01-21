#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "selection_methods.h"
#include "../geometry/vector.h"
#include "../utils.h"
#include "../web/urls.h"

#include <vector>

int raytracingWithReflection (
    Vector& start_point, std::vector<Vector> end_points,
    int select_method,
    double max_point_to_plane_distance = 0.1,
    uint fresnel_zone = 2, double freq = 868.0e6,

    std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
    std::string url_dom20 = std::string( URL_DOM20_BAVARIA ),
    std::string url_lod2  = std::string( URL_LOD2_BAVARIA )
);


int raytracingDirect (
    Vector& start_point, std::vector<Vector> end_points,
    int tile_type,
    std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
    std::string url_dom20 = std::string( URL_DOM20_BAVARIA )
);

#endif
