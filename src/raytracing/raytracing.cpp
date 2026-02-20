#include "raytracing.h"

#include "../geometry/vector.h"
#include "../geometry/polygon.h"
#include "../tile/tile_types.h"
#include "../config/global_config.h"
#include "../utils.h"

#include <unistd.h>

void createResultFileName ( char* dst_string ) {
    time_t rawtime;
    time( &rawtime );

    struct tm *info;
    info = localtime( &rawtime );

    char filename_timestamp[32];
    strftime( filename_timestamp, 32, "%Y%m%d_%H%M%S", info );

    snprintf( dst_string, 256, "results/Result_%s.json", filename_timestamp );

    printf( "Writing to the result file: %s\n", dst_string );
} /* createResultFileName() */

/*---------------------------------------------------------------*/

Raytracer::Raytracer (
    Vector& start_point,
    int select_method,
    double max_point_to_plane_distance,
    uint fresnel_zone, double freq,
    double grid_resolution,
    double k_value,
    bool cancel_on_ground,
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

    // Global settings in global_config.h
    GRID_RESOLUTION = grid_resolution;

    PLANE_DISTANCE_THRESHOLD = max_point_to_plane_distance;

    grid_field = new Field( grid_resolution );

    if ( max_threads >= 1 ) {
        MAX_THREADS = max_threads;
    }
    else {
        MAX_THREADS = NUM_CORES;
    }

    K_VALUE = k_value;
    CANCEL_ON_GROUND = cancel_on_ground;
    EARTH_RADIUS_EFFECTIVE = EARTH_RADIUS * k_value;


    createEnvironment();

    createResultFileName( result_file_name );
    result_file = fopen( result_file_name, "w" );

    fprintf( result_file, "[\n" );

    CHOSEN_URL_DGM1 = url_dgm1;
    CHOSEN_URL_DOM20 = url_dom20;
    CHOSEN_URL_LOD2 = url_lod2;
} /* Raytracer() */

/*---------------------------------------------------------------*/

Raytracer::~Raytracer () {
    fclose( result_file );

    // Correct the end of the file
    // Remove the comma after the last JSON object and close the list with ]
    FILE *result_file = fopen( result_file_name, "r+" );
    fseek( result_file, 0, SEEK_END );

    long pos = ftell( result_file );

    int ch;
    while ( pos > 0 ) {
        pos--;
        fseek( result_file, pos, SEEK_SET );

        ch = fgetc( result_file );
        if ( ch == ',' || ch == '[' ) {
            break;
        }
    }

    if ( ch == ',' ) {
        ftruncate(fileno( result_file), pos );
        fseek( result_file, 0, SEEK_END );
        fputs( "\n]", result_file );
    }
    else if ( ch == '[' ) {
        fseek( result_file, 0, SEEK_END );
        fputc( ']', result_file );
    }

    fclose( result_file );

    delete grid_field;
} /* ~Raytracer() */

/*---------------------------------------------------------------*/

void Raytracer::calculateCounterValues (
    std::vector<bool>& dgm_decision_array,
    std::vector<bool>& dom_decision_array,
    std::vector<bool>& dom_masked_decision_array,

    int& ground_count,
    int& vegetation_count,
    int& infrastructure_count
) {
    uint len_decision_array = dgm_decision_array.size();

    int
        dgm_count = 0,
        dom_count = 0,
        dom_masked_count = 0;

    for ( uint i = 0; i < len_decision_array; i++ ) {
        if ( dgm_decision_array[i] ) {
            dgm_count++;
        }
        if ( dom_decision_array[i] && !dgm_decision_array[i] ) {
            dom_count++;
        }
        if ( dom_masked_decision_array[i] && !dgm_decision_array[i] ) {
            dom_masked_count++;
        }
    }

    ground_count += dgm_count;
    vegetation_count += dom_masked_count;
    infrastructure_count += dom_count - dom_masked_count;
} /* calculateCounterValues() */


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

        std::vector<bool>
            dgm_decision_array_1, dgm_decision_array_2,
            dom_decision_array_1, dom_decision_array_2,
            dom_masked_decision_array_1, dom_masked_decision_array_2;

        int
            ground_count = 0,
            vegetation_count = 0,
            infrastructure_count = 0;

        Vector intersection;

        status = grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &dgm_decision_array_1, DGM, CANCEL_ON_GROUND );
        if ( !(CANCEL_ON_GROUND && status == INTERSECTION_FOUND) ) {
            grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &dom_decision_array_1, DOM );
            grid_field->bresenhamPseudo3D( start_point, reflect_point, 1.0, &dom_masked_decision_array_1, DOM_MASKED );
        }
        else {
            return status;
        }

        grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &dgm_decision_array_2, DGM, CANCEL_ON_GROUND );
        if ( !(CANCEL_ON_GROUND && status == INTERSECTION_FOUND) ) {
            grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &dom_decision_array_2, DOM );
            grid_field->bresenhamPseudo3D( reflect_point, end_point, 1.0, &dom_masked_decision_array_2, DOM_MASKED );
        }
        else {
            return status;
        }

        calculateCounterValues(
            dgm_decision_array_1, dom_decision_array_1, dom_masked_decision_array_1,
            ground_count, vegetation_count, infrastructure_count
        );
        calculateCounterValues(
            dgm_decision_array_2, dom_decision_array_2, dom_masked_decision_array_2,
            ground_count, vegetation_count, infrastructure_count
        );

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

    std::vector<bool>
        dgm_decision_array,
        dom_decision_array,
        dom_masked_decision_array;

    int status = grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &dgm_decision_array, DGM, CANCEL_ON_GROUND );

    if ( !(CANCEL_ON_GROUND && status == INTERSECTION_FOUND) ) {
        grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &dom_decision_array, DOM );
        grid_field->bresenhamPseudo3D( start_point, end_point, 1.0, &dom_masked_decision_array, DOM_MASKED );

        int
            ground_count = 0,
            vegetation_count = 0,
            infrastructure_count = 0;

        calculateCounterValues(
            dgm_decision_array, dom_decision_array, dom_masked_decision_array,
            ground_count, vegetation_count, infrastructure_count
        );

        //printf("DGM %d - DOM %d  - DOM_MASKED %d\n", count_dgm, count_dom, count_dom_masked);

        writeResultObject_Direct(
            end_point,
            ( end_point - start_point ).length(),
            ground_count, vegetation_count, infrastructure_count
        );
    }

    return 1;
} /* raytracingDirect() */

/*---------------------------------------------------------------*/

int Raytracer::writeResultObject_WithReflection (
    Vector& end_point,
    Vector& reflection_point,
    Polygon& reflecting_polygon,
    float distance,
    int ground_count, int vegetation_count, int infrastructure_count
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
    int ground_count, int vegetation_count, int infrastructure_count
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
