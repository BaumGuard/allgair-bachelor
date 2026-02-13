#include "raytracing.h"

#include "../geometry/polygon.h"
#include "../precalc/precalc.h"
#include "../tile/tile_types.h"
#include "../config/global_config.h"

#include <unistd.h>
#include <pthread.h>

#define NUM_CORES sysconf( _SC_NPROCESSORS_ONLN )


void createResultFileName ( char* dst_string ) {
    time_t rawtime;
    time( &rawtime );

    struct tm *info;
    info = localtime( &rawtime );

    char filename_timestamp[32];
    strftime( filename_timestamp, 32, "%Y%m%d_%H%M%S", info );

    snprintf( dst_string, 64, "results/Result_%s.json", filename_timestamp );
} /* createResultFileName() */

/*---------------------------------------------------------------*/

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

    createResultFileName( result_file_name );
    result_file = fopen( result_file_name, "w" );

    fprintf( result_file, "[\n" );

    chosen_url_dgm1 = url_dgm1;
    chosen_url_dom20 = url_dom20;
    chosen_url_lod2 = url_lod2;
} /* Raytracer() */

/*---------------------------------------------------------------*/

Raytracer::~Raytracer () {
    fclose( result_file );

    fopen( result_file_name, "r+" );
    fseek( result_file, 0, SEEK_END );
    long pos = ftell( result_file );
    fseek( result_file, pos-2, SEEK_SET);
    fprintf( result_file, "\n]\n" );
    fclose( result_file );

    delete grid_field;
} /* ~Raytracer() */

/*---------------------------------------------------------------*/

int Raytracer::raytracingWithReflection ( Vector& end_point ) {
    int status;

    std::vector<Polygon> selected_polygons;
    status = grid_field->precalculate(
        selected_polygons, start_point, end_point, select_method, fresnel_zone, freq );

    if ( status != SUCCESS ) {
        return status;
    }

    uint n_polygons = selected_polygons.size();
    for ( uint i = 0; i < n_polygons; i++ ) {
        Vector reflect_point = selected_polygons[i].getCentroid();

        int
            count_dgm_1, count_dgm_2,
            count_dom_1, count_dom_2,
            count_dom_masked_1, count_dom_masked_2,
            ground_count,
            vegetation_count,
            infrastructure_count;

        Vector intersection;

        grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dgm_1, DGM );
        grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_1, DOM );
        grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &count_dom_masked_1, DOM_MASKED );
        grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dgm_2, DGM );
        grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_2, DOM );
        grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &count_dom_masked_2, DOM_MASKED );

        ground_count = count_dgm_1 + count_dgm_2;
        vegetation_count = count_dom_masked_1 + count_dom_masked_2 - ground_count;
        infrastructure_count = count_dom_1 + count_dom_2 - vegetation_count - ground_count;

        status = writeResultObject_WithReflection(
            end_point, reflect_point,
            selected_polygons[i],
            ( reflect_point - start_point ).length(),
            ground_count, vegetation_count, infrastructure_count
        );

    }

    return status;
} /* raytracingWithReflection() */

/*---------------------------------------------------------------*/

int Raytracer::raytracingDirect ( Vector& end_point ) {
    int count_dgm, count_dom, count_dom_masked;

    grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &count_dgm, DGM );
    grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &count_dom, DOM );
    grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &count_dom_masked, DOM_MASKED );

    int
        ground_count = count_dgm,
        vegetation_count = count_dom_masked - count_dgm,
        infrastructure_count = count_dom - vegetation_count - ground_count;

    //printf("DGM %d - DOM %d  - DOM_MASKED %d\n", count_dgm, count_dom, count_dom_masked);

    writeResultObject_Direct(
        end_point,
        ( end_point - start_point ).length(),
        ground_count, vegetation_count, infrastructure_count
    );

    return 1;
} /* raytracingDirect() */

/*---------------------------------------------------------------*/

int Raytracer::writeResultObject_WithReflection (
    Vector& end_point,
    Vector& reflection_point,
    Polygon& reflecting_polygon,
    float distance,
    uint ground_count, uint vegetation_count, uint infrastructure_count
) {
    fprintf( result_file, "\t{\n" );
    //fprintf( result_file, "\t\"timestamp\": \"%s\",\n", timestamp );

    fprintf( result_file, "\t\t\"raytracing_method\": \"REFLECTION\",\n" );

    fprintf( result_file, "\t\t\"points\": {\n" );
    fprintf( result_file, "\t\t\t\"start_point\": [%.3f, %.3f, %.3f],\n",
             start_point.getX(), start_point.getY(), start_point.getZ() );
    fprintf( result_file, "\t\t\t\"end_point\": [%.3f, %.3f, %.3f],\n",
             end_point.getX(), end_point.getY(), end_point.getZ() );
    fprintf( result_file, "\t\t\t\"reflect_point\": [%.3f, %.3f, %.3f]\n",
             reflection_point.getX(), reflection_point.getY(), reflection_point.getZ() );
    fprintf( result_file, "\t\t},\n" );

    fprintf( result_file, "\t\t\"reflecting_polygon\": \"%s\",\n",
             reflecting_polygon.getID().data() );
    fprintf( result_file, "\t\t\"distance\": %.3f,\n", distance );
    fprintf( result_file, "\t\t\"area\": %.3f,\n", reflecting_polygon.getArea() );

    switch ( select_method ) {
        case BY_MAX_AREA:
            fprintf( result_file, "\t\t\"selection_method\": \"MAX_AREA\",\n" );
            break;

        case BY_MIN_DISTANCE:
            fprintf( result_file, "\t\t\"selection_method\": \"MIN_DISTANCE\",\n" );
            break;

        case ALL:
            fprintf( result_file, "\t\t\"selection_method\": \"ALL\",\n" );
            break;
    }

    fprintf( result_file, "\t\t\"counters\": {\n" );
    fprintf( result_file, "\t\t\t\"ground\": %d,\n", ground_count );
    fprintf( result_file, "\t\t\t\"vegetation\": %d,\n", vegetation_count );
    fprintf( result_file, "\t\t\t\"infrastructure\" : %d\n", infrastructure_count );
    fprintf( result_file, "\t\t}\n" );

    fprintf( result_file, "\t},\n" );

    return SUCCESS;
} /* writeResultObject_WithReflection() */

/*---------------------------------------------------------------*/

int Raytracer::writeResultObject_Direct (
    Vector& end_point,
    float distance,
    uint ground_count, uint vegetation_count, uint infrastructure_count
) {
    fprintf( result_file, "\t{\n" );
    //fprintf( result_file, "\t\"timestamp\": \"%s\",\n", timestamp );

    fprintf( result_file, "\t\t\"raytracing_method\": \"DIRECT\",\n" );

    fprintf( result_file, "\t\t\"points\": {\n" );
    fprintf( result_file, "\t\t\t\"start_point\": [%.3f, %.3f, %.3f],\n",
             start_point.getX(), start_point.getY(), start_point.getZ() );
    fprintf( result_file, "\t\t\t\"end_point\": [%.3f, %.3f, %.3f]\n",
             end_point.getX(), end_point.getY(), end_point.getZ() );
    fprintf( result_file, "\t\t},\n" );

    fprintf( result_file, "\t\t\"distance\": %.3f,\n", distance );


    fprintf( result_file, "\t\t\"counters\": {\n" );
    fprintf( result_file, "\t\t\t\"ground\": %d,\n", ground_count );
    fprintf( result_file, "\t\t\t\"vegetation\": %d,\n", vegetation_count );
    fprintf( result_file, "\t\t\t\"infrastructure\": %d\n", infrastructure_count );
    fprintf( result_file, "\t\t}\n" );

    fprintf( result_file, "\t},\n" );

    return SUCCESS;
} /* writeResultObject_Direct() */
