#include "raytracing.h"

#include "../geometry/polygon.h"
#include "../precalc/precalc.h"
#include "../tile/field.h"
#include "../tile/tile_types.h"
#include "result_output.h"

int raytracingWithReflection (
    Vector& start_point, Vector& end_point,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,
    double grid_resolution,

    std::string url_dgm1,
    std::string url_dom20,
    std::string url_lod2
) {
    chosen_url_dgm1  = url_dgm1;
    chosen_url_dom20 = url_dom20;
    chosen_url_lod2  = url_lod2;

    Polygon selected_polygon =
        precalculate( start_point, end_point, select_method, fresnel_zone, freq );

    Vector reflect_point = selected_polygon.getCentroid();

    int
        count_dgm_1, count_dgm_2,
        count_dom_1, count_dom_2,
        ground_count,
        veg_count;

    Field grid_field( grid_resolution );
    Vector intersection;


    int status;
    status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dgm_1, DGM1 );
    status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_1, DOM1 );
    status = grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dgm_2, DGM1 );
    status = grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_2, DOM1 );

    ground_count = count_dgm_1 + count_dgm_2;
    veg_count    = count_dom_1 + count_dom_2 - ground_count;

    // TODO: Calculate distance
    createResultFile_WithReflection(
        start_point, end_point, reflect_point,
        selected_polygon,
        0.0,
        veg_count, ground_count,
        select_method,
        fresnel_zone
    );
} /* raytracingWithReflection() */



int raytracingDirect (
    Vector& start_point, Vector& end_point,
    int tile_type,
    double grid_resolution,

    std::string dgm1_url,
    std::string dom20_url
) {
    if ( tile_type != DGM1 && tile_type != DOM20 ) {
        return INVALID_TILE_TYPE;
    }

    chosen_url_dgm1 = dgm1_url;
    chosen_url_dom20 = dom20_url;

    int ground_count;

    Field grid_field( grid_resolution );
    int status = grid_field.bresenhamPseudo3D( start_point, end_point, 1.0, &ground_count, tile_type );

    // TODO: Calculate veg_count and distance
    createResultFile_Direct(
        start_point, end_point,
        0.0,
        0, ground_count
    );

    return status;
} /* raytracingDirect() */
