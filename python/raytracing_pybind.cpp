#include "../src/geometry/vector.h"
#include "../src/raytracing/raytracing.h"

#include <tuple>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <time.h>
#include <stdio.h>

namespace py = pybind11;

PYBIND11_MODULE( raytracing, m ) {
    m.doc() = "Raytracing with reflection";

    m.def(
        "raytracing_with_reflection",
        [](
            const std::tuple<double, double, double>& start_point,
            const std::vector<std::tuple<double, double, double>>& end_points,
            std::string select_method,
            double max_point_to_plane_distance,
            uint fresnel_zone,
            double freq,
            double grid_resolution,
            int max_threads,

            std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
            std::string url_dom20 = std::string( URL_DOM20_BAVARIA ),
            std::string url_lod2  = std::string( URL_LOD2_BAVARIA )
        ) {
            Vector _start_point(
                std::get<0>(start_point),
                std::get<1>(start_point),
                std::get<2>(start_point)
            );

            int _select_method;
            if ( select_method == "max_area" ) {
                _select_method = BY_MAX_AREA;
            }
            else if ( select_method == "min_distance" ) {
                _select_method = BY_MIN_DISTANCE;
            }
            else if ( select_method == "all" ) {
                _select_method = ALL;
            }
            else {
                printf( "ERROR: Wrong selection method '%s'\n", select_method.data() );
                return 1;
            }

            Raytracer raytracer (
                _start_point,
                _select_method,
                max_point_to_plane_distance,
                fresnel_zone, freq,
                grid_resolution,
                max_threads,

                url_dgm1,
                url_dom20,
                url_lod2
            );

            uint len_end_points = end_points.size();
            for ( uint i = 0; i < len_end_points; i++ ) {
                Vector end_point(
                    std::get<0>(end_points[i]),
                    std::get<1>(end_points[i]),
                    std::get<2>(end_points[i])
                );
                printf("%d\n", i);
                raytracer.raytracingWithReflection( end_point );

                if ( PyErr_CheckSignals() != 0 ) {
                    throw pybind11::error_already_set();
                }
            }

            return 0;
        },
        py::arg( "start_point" ),
        py::arg( "end_points" ),
        py::arg( "select_method" ),
        py::arg( "max_point_to_plane_distance" ) = 0.1,
        py::arg( "fresnel_zone" ) = 2,
        py::arg( "freq" ) = 868.0e6,
        py::arg( "grid_resolution" ) = 1.0,
        py::arg( "max_threads" ) = 0,
        py::arg( "url_dgm1" ) = std::string( URL_DGM1_BAVARIA ),
        py::arg( "url_dom20" ) = std::string( URL_DOM20_BAVARIA ),
        py::arg( "url_lod2" ) = std::string( URL_LOD2_BAVARIA )
    );


    m.def(
        "raytracing_direct",
        [](
            const std::tuple<double, double, double>& start_point,
            const std::vector<std::tuple<double, double, double>>& end_points,
            double grid_resolution,
            int max_threads,

            std::string url_dgm1  = std::string( URL_DGM1_BAVARIA ),
            std::string url_dom20 = std::string( URL_DOM20_BAVARIA )
        ) {
            Vector _start_point(
                std::get<0>(start_point),
                std::get<1>(start_point),
                std::get<2>(start_point)
            );

            Raytracer raytracer (
                _start_point,
                BY_MAX_AREA,
                0.1,
                2, 868.0e6,
                grid_resolution,
                max_threads
            );

            clock_t start, end;
            uint len_end_points = end_points.size();
            for ( uint i = 0; i < len_end_points; i++ ) {
                Vector _end_point(
                    std::get<0>(end_points[i]),
                    std::get<1>(end_points[i]),
                    std::get<2>(end_points[i])
                );

                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);

                raytracer.raytracingDirect( _end_point );

                clock_gettime(CLOCK_MONOTONIC, &end);

                double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                printf("%d\n", i);
                printf("Duration: %f s\n", elapsed);


                if ( PyErr_CheckSignals() != 0 ) {
                    throw pybind11::error_already_set();
                }
            }

            return 0;
        },
        py::arg( "start_point" ),
        py::arg( "end_points" ),
        py::arg( "grid_resolution" ) = 1.0,
        py::arg( "max_threads" ) = 0,
        py::arg( "url_dgm1" ) = std::string( URL_DGM1_BAVARIA ),
        py::arg( "url_dom20" ) = std::string( URL_DOM20_BAVARIA )
    );

}
