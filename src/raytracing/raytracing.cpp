#include "raytracing.h"

#include "../geometry/polygon.h"
#include "../precalc/precalc.h"
#include "../tile/field.h"
#include "../tile/tile_types.h"

int raytracingWithReflection (
    Vector& start_point, std::vector<Vector> end_points,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,

    std::string dgm1_url,
    std::string dom20_url,
    std::string lod2_url
) {
    Polygon selected_polygon = precalculate( start_point, end_points, select_method, fresnel_zone, freq );

    Vector reflect_point = selected_polygon.getCentroid();

    int
        count_dgm_1, count_dgm_2,
        count_dom_1, count_dom_2,
        ground_count,
        veg_count;

    Field grid_field;
    Vector intersection;


    int status;
    status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dgm_1, DGM1 );
    status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_1, DOM1 );
    status = grid_field.bresenhamPseudo3D( reflect_point, end_points[0], 1.0, &count_dgm_2, DGM1 );
    status = grid_field.bresenhamPseudo3D( reflect_point, end_points[0], 1.0, &count_dom_2, DOM1 );

    ground_count = count_dgm_1 + count_dgm_2;
    veg_count = count_dom_1 + count_dom_2 - ground_count;
} /* raytracingWithReflection() */



int raytracingDirect (
    Vector& start_point, std::vector<Vector> end_points,
    int tile_type,

    std::string dgm1_url,
    std::string dom20_url
) {
    if ( tile_type != DGM1 && tile_type != DOM20 ) {
        return INVALID_TILE_TYPE;
    }

    int ground_count;

    Field grid_field;
    int status = grid_field.bresenhamPseudo3D( start_point, end_points[0], 1.0, &ground_count, tile_type );

    return status;
} /* raytracingDirect() */
