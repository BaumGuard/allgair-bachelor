cmake_minimum_required(VERSION 3.5...3.18)
set(CMAKE_BUILD_TYPE Debug)

project(Raytracing)

add_compile_options(-g -O0)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

execute_process(
    COMMAND $(Python3_EXECUTABLE) -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(pybind11_DIR $(PYBIND11_CMAKE_DIR))

find_package(pybind11 REQUIRED)
find_package(CURL REQUIRED)
find_package(TIFF REQUIRED)
find_package(GDAL REQUIRED)

pybind11_add_module(raytracing SHARED
raytracing_pybind.cpp
../src/geometry/vector.cpp
../src/geometry/line.cpp
../src/geometry/plane.cpp
../src/geometry/polygon.cpp
../src/precalc/fresnel_zone.cpp
../src/precalc/precalc.cpp
../src/raw_data/geotiff.cpp
../src/raw_data/gmlfile.cpp
../src/raw_data/surface.cpp
../src/raytracing/raytracing.cpp
../src/raytracing/result_output.cpp
../src/tile/grid_tile.cpp
../src/tile/vector_tile.cpp
../src/tile/field.cpp
../src/tile/load_tile.cpp
../src/web/download.cpp
../src/config/global_config.cpp
../src/utils.cpp
)

target_link_libraries(raytracing PRIVATE
    CURL::libcurl
)

if(TARGET TIFF::TIFF)
    target_link_libraries(raytracing PRIVATE TIFF::TIFF)
else()
    target_include_directories(raytracing PRIVATE ${TIFF_INCLUDE_DIRS})
    target_link_libraries(raytracing PRIVATE ${TIFF_LIBRARIES})
endif()

if(TARGET GDAL::GDAL)
    target_link_libraries(raytracing PRIVATE GDAL::GDAL)
else()
    target_include_directories(raytracing PRIVATE ${GDAL_INCLUDE_DIRS})
    target_link_libraries(raytracing PRIVATE ${GDAL_LIBRARIES})
endif()

set_target_properties(raytracing PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)


set(INIT_PY "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
if(NOT EXISTS ${INIT_PY})
file(WRITE ${INIT_PY} "")
endif()
