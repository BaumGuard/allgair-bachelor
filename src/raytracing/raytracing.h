#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "selection_methods.h"
#include "../geometry/vector.h"
#include "../tile/field.h"
#include "../utils.h"
#include "../web/urls.h"

#include <vector>

/*
class Raytracer {
public:
    Raytracer (
        Vector& start_point,
        int select_method,
        double max_point_to_plane_distance = 0.1,
        uint fresnel_zone = 2, double freq = 868.0e6,
        double grid_resolution = 1.0,
        int max_threads = 0,

        std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
        std::string url_dom20 = std::string( URL_DOM20_BAVARIA ),
        std::string url_lod2  = std::string( URL_LOD2_BAVARIA )
    );

    int raytracingWithReflection( Vector& end_point );

    int raytracingDirect( Vector& end_point, int tile_type );


private:
    Vector start_point;
    int select_method;
    double max_point_to_plane_distance;
    uint fresnel_zone;
    double freq;

    Field* grid_field;
};
*/


int raytracingWithReflection (
    Vector& start_point, Vector& end_point,
    int select_method,
    double max_point_to_plane_distance = 0.1,
    uint fresnel_zone = 2, double freq = 868.0e6,
    double grid_resolution = 1.0,
    int max_threads = 0,

    std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
    std::string url_dom20 = std::string( URL_DOM20_BAVARIA ),
    std::string url_lod2  = std::string( URL_LOD2_BAVARIA )
);


int raytracingDirect (
    Vector& start_point, Vector& end_point,
    int tile_type,
    double grid_resolution = 1.0,
    int max_threads = 0,

    std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
    std::string url_dom20 = std::string( URL_DOM20_BAVARIA )
);


#endif
