#include "tile/field.h"
#include "coord.h"
#include "utils.h"
#include "config/config.h"
#include "web/download.h"

#include <cstdio>
#include <string>
#include <cmath>

#include <time.h>

int main () {

    Field field;
    Coord intersect;
    //field.bresenhamPseudo3D( intersect, 48.17959, 11.52543, 745.2, 48.11685, 11.68722, 24.5 );
    clock_t start = clock();
    int status = field.surfaceIntersection( intersect, 48.137841,11.570411, 700.0, 48.132831,11.575546, 300.0 );
    clock_t end = clock();

    double exec_time = (double)( end - start ) / CLOCKS_PER_SEC;

    printf("Execution time : %f\n", exec_time);

    //int status = field.bresenhamPseudo3D( intersect, 47.623289, 10.633178, 1000.0, 47.621091, 10.635238, 100.0, DGM1 );

    printf("%d\n", status);
    printf("%.10f %.10f %f\n", intersect.lat, intersect.lon, intersect.altitude);

    return 0;
}
