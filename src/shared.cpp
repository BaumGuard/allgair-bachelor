#include "shared.h"

#include "web/urls.h"

std::string DATA_DIR = "data";

std::string
    CHOSEN_URL_DGM1  = URL_DGM1_BAVARIA,
    CHOSEN_URL_DOM20 = URL_DOM20_BAVARIA,
    CHOSEN_URL_LOD2  = URL_LOD2_BAVARIA;

int MAX_THREADS;

double GRID_RESOLUTION;

double PLANE_DISTANCE_THRESHOLD;

double MIN_AREA;

double K_VALUE;

double EARTH_RADIUS_EFFECTIVE;

bool CANCEL_ON_GROUND;


pthread_t* bresenham_threads;
struct Bresenham_Thread_Data* bresenham_data;
std::vector<bool>* decision_arrays;

pthread_mutex_t selected_polygons_mutex;
struct Precalculate_Thread_Data* precalc_data;
pthread_t* precalc_threads;

pthread_mutex_t polygon_list_mutex;
pthread_t* ground_area_threads;
struct PolygonsInGroundArea_Thread_Data* ground_area_data;
std::vector<Polygon>* ground_area_polygons;

double
    fresnel_time, ground_area_time, precalc_time, bresenham_time, output_time;

FILE* time_file;
