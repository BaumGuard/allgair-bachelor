#include "result_output.h"

#include "selection_methods.h"
#include "../utils.h"
#include "../status_codes.h"

int createResultFile_WithReflection (
    Vector& start_point, Vector& end_point,
    Vector& reflection_point,
    Polygon& reflecting_polygon,
    float distance,
    uint vegetation_count, uint ground_count,
    uint selection_method,
    uint fresnel_zone
) {
    time_t rawtime;
    time( &rawtime );

    struct tm *info;
    info = localtime( &rawtime );

    char timestamp [32];
    strftime( timestamp, 32, "%Y-%m-%d %H:%M:%S", info );
    char filename_timestamp[32];
    strftime( filename_timestamp, 32, "%Y%m%d_%H%M%S", info );

    char file_name [64];
    snprintf( file_name, 64, "results/Result_%s.json", filename_timestamp );

    FILE* result_file = fopen( file_name, "w" );
    if ( !result_file ) {
        return FILE_NOT_CREATABLE;
    }

    fprintf( result_file, "{\n" );
    fprintf( result_file, "\t\"timestamp\": \"%s\",\n", timestamp );

    fprintf( result_file, "\t\"raytracing_method\": \"REFLECTION\",\n" );

    fprintf( result_file, "\t\"points\": {\n" );
    fprintf( result_file, "\t\t\"start_point\": [%.3f, %.3f, %.3f],\n",
             start_point.getX(), start_point.getY(), start_point.getZ() );
    fprintf( result_file, "\t\t\"end_point\": [%.3f, %.3f, %.3f],\n",
             end_point.getX(), end_point.getY(), end_point.getZ() );
    fprintf( result_file, "\t\t\"reflect_point\": [%.3f, %.3f, %.3f]\n",
             reflection_point.getX(), reflection_point.getY(), reflection_point.getZ() );
    fprintf( result_file, "\t},\n" );

    fprintf( result_file, "\t\"reflecting_polygon\": \"%s\",\n",
             reflecting_polygon.getID().data() );
    fprintf( result_file, "\t\"distance\": %.3f,\n", distance );
    fprintf( result_file, "\t\"area\": %.3f,\n", reflecting_polygon.getArea() );

    switch ( selection_method ) {
        case BY_MAX_AREA:
            fprintf( result_file, "\t\"selection_method\": \"MAX_AREA\",\n" );
            break;

        case BY_MIN_DISTANCE:
            fprintf( result_file, "\t\"selection_method\": \"MIN_DISTANCE\",\n" );
            break;

        case ALL:
            fprintf( result_file, "\t\"selection_method\": \"ALL\",\n" );
            break;
    }

    fprintf( result_file, "\t\"counters\": {\n" );
    fprintf( result_file, "\t\t\"vegetation\": %d,\n", vegetation_count );
    fprintf( result_file, "\t\t\"ground\": %d\n", ground_count );
    fprintf( result_file, "\t}\n" );

    fprintf( result_file, "}" );


    fclose( result_file );

    return SUCCESS;
} /* createResultFile_WithReflection() */


int createResultFile_Direct (
    Vector& start_point, Vector& end_point,
    float distance,
    uint vegetation_count, uint ground_count
) {
    time_t rawtime;
    time( &rawtime );

    struct tm *info;
    info = localtime( &rawtime );

    char timestamp [32];
    strftime( timestamp, 32, "%Y-%m-%d %H:%M:%S", info );
    char filename_timestamp[32];
    strftime( filename_timestamp, 32, "%Y%m%d_%H%M%S", info );

    char file_name [64];
    snprintf( file_name, 64, "results/Result_%s.json", filename_timestamp );

    FILE* result_file = fopen( file_name, "w" );
    if ( !result_file ) {
        return FILE_NOT_CREATABLE;
    }

    fprintf( result_file, "{\n" );
    fprintf( result_file, "\t\"timestamp\": \"%s\",\n", timestamp );

    fprintf( result_file, "\t\"raytracing_method\": \"DIRECT\",\n" );

    fprintf( result_file, "\t\"points\": {\n" );
    fprintf( result_file, "\t\t\"start_point\": [%.3f, %.3f, %.3f],\n",
             start_point.getX(), start_point.getY(), start_point.getZ() );
    fprintf( result_file, "\t\t\"end_point\": [%.3f, %.3f, %.3f]\n",
             end_point.getX(), end_point.getY(), end_point.getZ() );
    fprintf( result_file, "\t},\n" );

    fprintf( result_file, "\t\"distance\": %.3f,\n", distance );


    fprintf( result_file, "\t\"counters\": {\n" );
    fprintf( result_file, "\t\t\"vegetation\": %d,\n", vegetation_count );
    fprintf( result_file, "\t\t\"ground\": %d\n", ground_count );
    fprintf( result_file, "\t}\n" );

    fprintf( result_file, "}" );


    fclose( result_file );

    return SUCCESS;
} /* createResultFile_Direct() */
