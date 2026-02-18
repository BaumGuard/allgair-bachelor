#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "selection_methods.h"
#include "../geometry/vector.h"
#include "../tile/field.h"
#include "../web/urls.h"

#include <vector>
#include <cstdio>


class Raytracer {
public:
    /*
    CONSTRUCTOR

    Args:
     - start_point                  : Start point of the raytracing
     - select_method                : Selection method by which to select
                                      the polygons (See selection_methods.h)
                                       - MAX_AREA
                                       - MIN_DISTANCE
                                       - ALL
     - max_point_to_plane_distance  : Maximum distance of a point to a polygon's plane
                                      in meters when creating polygon objects from LOD2 data
     - fresnel_zone                 : Number of the Fresnel zone around the start and the end
                                      point to use for filtering surfaces
     - freq                         : Signal frequency of the drone in Hz for calculating the
                                      Fresnel zone
     - grid_resolution              : Resolution of the DGM and DOM grid in meters
                                      Both the DGM and the DOM tiles will be resampled to the
                                      resolution given
     - max_threads                  : Maximum number of parallel threads (if 0 the max_threads
                                      will be equal to the number of CPU cores)
     - url_dgm1                     : URL from which the DGM1 tiles should be downloaded
     - url_dom20                    : URL from which the DOM20 tiles should be downloaded
     - url_lod2                     : URL from which the LOD2 tiles should be downloaded
    */
    Raytracer (
        Vector& start_point,
        int select_method,
        double max_point_to_plane_distance = 0.1,
        uint fresnel_zone = 2, double freq = 868.0e6,
        double grid_resolution = 1.0,
        double k_value = 4.0 / 3.0,
        bool cancel_on_ground = false,
        int max_threads = 0,

        std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
        std::string url_dom20 = std::string( URL_DOM20_BAVARIA ),
        std::string url_lod2  = std::string( URL_LOD2_BAVARIA )
    );

    ~Raytracer();

    /*
    Perform raytracing with reflection
     1. Get all surfaces in the Fresnel zone around the start and end point
     2. Select all surfaces within the Fresnel zone that reflect the ray in way
        that it hits the end point or passes it closely
     3. Perform raytracing on the selected surfaces with their corresponding
        reflecting points (Start point to reflection point and reflecting point to end point)

    Args:
     - end_point : End point of the raytracing
    */
    int raytracingWithReflection( Vector& end_point );

    /*
    Perform raytracing on the direct line between the start and end point

    Args:
     - end_point : End point of the raytracing
    */
    int raytracingDirect( Vector& end_point );


    /*
    Write a JSON object in the result file with the current result

    Args:
     - end_point            : End point of the raytracing
     - reflection_point     : Reflecting point on a polygon
     - reflecting_polygon   : Reflecting polygon with the poing reflection_point on it
     - distance             : Distance (Start to reflection + Reflection to end) in m
     - ground_count         : Counter of how often the ray was below ground level
     - vegetation_count     : Counter of how often the ray passes through vegetation
     - infrastructure_count : Counter of how often the ray passes through infrastructure
    */
    int writeResultObject_WithReflection (
        Vector& end_point,
        Vector& reflection_point,
        Polygon& reflecting_polygon,
        float distance,
        uint ground_count, uint vegetation_count, uint infrastructure_count
    );

    /*
    Write a JSON object in the result file with the current result

    Args:
     - end_point            : End point of the raytracing
     - distance             : Distance (Start to end point) in m
     - ground_count         : Counter of how often the ray was below ground level
     - vegetation_count     : Counter of how often the ray passes through vegetation
     - infrastructure_count : Counter of how often the ray passes through infrastructure
    */
    int writeResultObject_Direct (
        Vector& end_point,
        float distance,
        uint ground_count, uint vegetation_count, uint infrastructure_count
    );


private:
    Vector start_point;
    int select_method;
    double max_point_to_plane_distance;
    uint fresnel_zone;
    double freq;

    Field* grid_field;

    char result_file_name [256];
    FILE* result_file;


    void calculateCounterValues (
        std::vector<bool>& dgm_decision_array,
        std::vector<bool>& dom_decision_array,
        std::vector<bool>& dom_masked_decision_array,

        int& ground_count,
        int& vegetation_count,
        int& infrastructure_count
    );
};

#endif
