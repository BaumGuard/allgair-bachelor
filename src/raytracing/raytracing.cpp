#include "raytracing.h"

#include "../geometry/polygon.h"
#include "../precalc/precalc.h"
#include "../tile/tile_types.h"
#include "result_output.h"
#include "../config/global_config.h"

#include <unistd.h>
#include <pthread.h>

#define NUM_CORES sysconf( _SC_NPROCESSORS_ONLN )


#if 0
Raytracer::Raytracer (
    Vector& start_point,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,
    double grid_resolution,
    int max_threads,

    std::string url_dgm1,
    std::string url_dom20,
    std::string url_lod2
) {
    this->start_point = start_point;
    this->select_method = select_method;
    this->max_point_to_plane_distance = max_point_to_plane_distance;
    this->fresnel_zone = fresnel_zone;
    this->freq = freq;

    GRID_RESOLUTION = grid_resolution;

    grid_field = new Field( grid_resolution );

    if ( max_threads >= 1 ) {
        MAX_THREADS = max_threads;
    }
    else {
        MAX_THREADS = NUM_CORES;
    }

    chosen_url_dgm1 = url_dgm1;
    chosen_url_dom20 = url_dom20;
    chosen_url_lod2 = url_lod2;
}

int Raytracer::raytracingWithReflection ( Vector& end_point ) {
    int status;

    std::vector<Polygon> selected_polygons;
    status = precalculate( selected_polygons, start_point, end_point, select_method, fresnel_zone, freq );
    if ( status != SUCCESS ) {
        return status;
    }

    uint n_polygons = selected_polygons.size();
    for ( uint i = 0; i < n_polygons; i++ ) {
        Vector reflect_point = selected_polygons[i].getCentroid();

        uint
            count_dgm_1, count_dgm_2,
            count_dom_1, count_dom_2,
            ground_count,
            veg_count;

        Vector intersection;
#if 0
        pthread_t
            bresenham1_thread,
            bresenham2_thread,
            bresenham3_thread,
            bresenham4_thread;

        Bresenham_Data bresenham1_data = {
            .field = *grid_field,
            .start = start_point,
            .end = reflect_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dgm_1,
            .tile_type = DGM
        };
        Bresenham_Data bresenham2_data = {
            .field = *grid_field,
            .start = start_point,
            .end = reflect_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_1,
            .tile_type = DOM
        };
        Bresenham_Data bresenham3_data = {
            .field = *grid_field,
            .start = reflect_point,
            .end = end_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dgm_2,
            .tile_type = DGM
        };
        Bresenham_Data bresenham4_data = {
            .field = *grid_field,
            .start = reflect_point,
            .end = end_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_2,
            .tile_type = DOM
        };

        pthread_create( &bresenham1_thread, NULL, Thread_bresenhamPseudo3D, &bresenham1_data );
        pthread_create( &bresenham2_thread, NULL, Thread_bresenhamPseudo3D, &bresenham2_data );
        pthread_create( &bresenham3_thread, NULL, Thread_bresenhamPseudo3D, &bresenham3_data );
        pthread_create( &bresenham4_thread, NULL, Thread_bresenhamPseudo3D, &bresenham4_data );

        pthread_join( bresenham1_thread, NULL );
        pthread_join( bresenham2_thread, NULL );
        pthread_join( bresenham3_thread, NULL );
        pthread_join( bresenham4_thread, NULL );

        /*
        status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dgm_1, DGM );
        status = grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_1, DOM );
        status = grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dgm_2, DGM );
        status = grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_2, DOM );
        */
        ground_count = count_dgm_1 + count_dgm_2;
        veg_count    = count_dom_1 + count_dom_2 - ground_count;

        status = createResultFile_WithReflection(
            start_point, end_point, reflect_point,
            selected_polygons[i],
            ( reflect_point - start_point ).length(),
            veg_count, ground_count,
            select_method,
            fresnel_zone
        );
#endif
    }

    return status;
}

int Raytracer::raytracingDirect ( Vector& end_point, int tile_type ) {
    uint ground_count;

    int status = grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &ground_count, tile_type );

    createResultFile_Direct(
        start_point, end_point,
        ( end_point - start_point ).length(),
        0, ground_count
    );

    return status;
}
#endif


/*
void initRaytracing () {
    Vector& start_point,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,
    double grid_resolution,
    int max_threads,

    std::string url_dgm1,
    std::string url_dom20,
    std::string url_lod2
} {
    chosen_url_dgm1  = url_dgm1;
    chosen_url_dom20 = url_dom20;
    chosen_url_lod2  = url_lod2;

    GRID_RESOLUTION = grid_resolution;

    if ( max_threads >= 1 ) {
        MAX_THREADS = max_threads;
    }
    else {
        MAX_THREADS = NUM_CORES;
    }
}
*/

int raytracingWithReflection (
    Vector& start_point, Vector& end_point,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,
    double grid_resolution,
    int max_threads,

    std::string url_dgm1,
    std::string url_dom20,
    std::string url_lod2
) {

    chosen_url_dgm1  = url_dgm1;
    chosen_url_dom20 = url_dom20;
    chosen_url_lod2  = url_lod2;

    GRID_RESOLUTION = grid_resolution;

    if ( max_threads >= 1 ) {
        MAX_THREADS = max_threads;
    }
    else {
        MAX_THREADS = NUM_CORES;
    }

    int status;

    std::vector<Polygon> selected_polygons;
    status = precalculate( selected_polygons, start_point, end_point, select_method, fresnel_zone, freq );
    if ( status != SUCCESS ) {
        return status;
    }


    uint n_polygons = selected_polygons.size();
    for ( uint i = 0; i < n_polygons; i++ ) {
        Vector reflect_point = selected_polygons[i].getCentroid();

        uint
            count_dgm_1, count_dgm_2,
            count_dom_1, count_dom_2,
            count_dom_masked_1, count_dom_masked_2,
            ground_count,
            vegetation_count,
            infrastructure_count;

        Field grid_field( grid_resolution );
        Vector intersection;
#if 0
        pthread_t
            bresenham1_thread,
            bresenham2_thread,
            bresenham3_thread,
            bresenham4_thread,
            bresenham5_thread,
            bresenham6_thread;

        Bresenham_Data bresenham1_data = {
            .field = grid_field,
            .start = start_point,
            .end = reflect_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dgm_1,
            .tile_type = DGM
        };
        Bresenham_Data bresenham2_data = {
            .field = grid_field,
            .start = start_point,
            .end = reflect_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_1,
            .tile_type = DOM
        };
        Bresenham_Data bresenham3_data = {
            .field = grid_field,
            .start = start_point,
            .end = reflect_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_masked_1,
            .tile_type = DOM_MASKED
        };
        Bresenham_Data bresenham4_data = {
            .field = grid_field,
            .start = reflect_point,
            .end = end_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dgm_2,
            .tile_type = DGM
        };
        Bresenham_Data bresenham5_data = {
            .field = grid_field,
            .start = reflect_point,
            .end = end_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_2,
            .tile_type = DOM
        };
        Bresenham_Data bresenham6_data = {
            .field = grid_field,
            .start = reflect_point,
            .end = end_point,
            .ground_level_threshold = 1.0,
            .ground_count = &count_dom_masked_2,
            .tile_type = DOM_MASKED
        };
#endif

        grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dgm_1, DGM );
        grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_1, DOM );
        grid_field.bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_masked_1, DOM_MASKED );
        grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dgm_2, DGM );
        grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_2, DOM );
        grid_field.bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_masked_2, DOM_MASKED );

        ground_count         = count_dgm_1 + count_dgm_2;
        vegetation_count     = count_dom_masked_1 + count_dom_masked_2 - ground_count;
        infrastructure_count = count_dom_1 + count_dom_2 - vegetation_count - ground_count;


        status = createResultFile_WithReflection(
            start_point, end_point, reflect_point,
            selected_polygons[i],
            ( reflect_point - start_point ).length(),
            ground_count, vegetation_count, infrastructure_count,
            select_method,
            fresnel_zone
        );

    }

    return status;
} /* raytracingWithReflection() */


int raytracingDirect (
    Vector& start_point, Vector& end_point,
    double grid_resolution,
    int max_threads,

    std::string dgm1_url,
    std::string dom20_url,
    int n_threads
) {
    chosen_url_dgm1 = dgm1_url;
    chosen_url_dom20 = dom20_url;

    GRID_RESOLUTION = grid_resolution;

    if ( max_threads >= 1 ) {
        MAX_THREADS = max_threads;
    }
    else {
        MAX_THREADS = NUM_CORES;
    }

    uint
        dgm_count, dom_count, dom_masked_count,
        ground_count, veg_count, infrastructure_count;

    Field grid_field( grid_resolution );
    grid_field.bresenhamPseudo3D( start_point, end_point, 1.0, &dgm_count, DGM );
    grid_field.bresenhamPseudo3D( start_point, end_point, 1.0, &dom_count, DOM );
    grid_field.bresenhamPseudo3D( start_point, end_point, 1.0, &dom_masked_count, DOM_MASKED );

    ground_count = dgm_count;
    veg_count = dom_masked_count - ground_count;
    infrastructure_count = dom_count - veg_count - ground_count;

    createResultFile_Direct(
        start_point, end_point,
        ( end_point - start_point ).length(),
        ground_count, veg_count, infrastructure_count
    );

    return SUCCESS;
} /* raytracingDirect() */
