#include "../src/geometry/vector.h"
#include "../src/raytracing/raytracing.h"

#include <tuple>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;

PYBIND11_MODULE( raytracing, m ) {
    m.doc() = "Raytracing with reflection";

    m.def(
        "raytracing_with_reflection",
        [](
            const std::tuple<double, double, double>& start_point,
            const std::tuple<double, double, double>& end_point,
            int select_method,
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

            Vector _end_point(
                std::get<0>(end_point),
                std::get<1>(end_point),
                std::get<2>(end_point)
            );

            return raytracingWithReflection(
                _start_point, _end_point,
                select_method, max_point_to_plane_distance,
                fresnel_zone, freq,
                grid_resolution,
                max_threads,

                url_dgm1,
                url_dom20,
                url_lod2
            );
        },
        py::arg( "start_point" ),
        py::arg( "end_point" ),
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
            const std::tuple<double, double, double>& end_point,
            int tile_type,
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

            Vector _end_point(
                std::get<0>(end_point),
                std::get<1>(end_point),
                std::get<2>(end_point)
            );

            return raytracingDirect(
                _start_point, _end_point,
                tile_type,
                grid_resolution,
                max_threads,

                url_dgm1,
                url_dom20
            );
        },
        py::arg( "start_point" ),
        py::arg( "end_point" ),
        py::arg( "tile_type" ),
        py::arg( "grid_resolution" ) = 1.0,
        py::arg( "max_threads" ) = 0,
        py::arg( "url_dgm1" ) = std::string( URL_DGM1_BAVARIA ),
        py::arg( "url_dom20" ) = std::string( URL_DOM20_BAVARIA )
    );

}
