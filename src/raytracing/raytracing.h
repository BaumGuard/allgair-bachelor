#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "../geometry/vector.h"
#include "../utils.h"
#include "../web/urls.h"

#include <vector>

int raytracingWithReflection (
    Vector& start_point, std::vector<Vector> end_points,
    int select_method,
    double max_point_to_plane_distance = 0.1,
    uint fresnel_zone = 2, double freq = 868.0e6,

    std::string dgm1_url = std::string(URL_ROOT) + std::string(DGM1_POSTFIX),
    std::string dom20_url = std::string(URL_ROOT) + std::string(DOM20_POSTFIX),
    std::string lod2_url = std::string(URL_ROOT) + std::string(LOD2_POSTFIX)
);


int raytracingDirect (
    Vector& start_point, std::vector<Vector> end_points,
    int tile_type,
    std::string dgm1_url,
    std::string dom20_url
);

#endif
