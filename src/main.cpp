#include "tile/field.h"
#include "coord.h"
#include "utils.h"
#include "config/config.h"

#include <iostream>
#include <cstdio>
#include <string>

int main () {

    Field field;
    Coord intersect;
    //field.bresenhamPseudo3D( intersect, 48.17959, 11.52543, 745.2, 48.11685, 11.68722, 24.5 );
    int status = field.surfaceIntersection( intersect, 47.623289, 10.633178, 1000.0, 47.621091, 10.635238, 100.0 );
    //int status = field.bresenhamPseudo3D( intersect, 47.623289, 10.633178, 1000.0, 47.621091, 10.635238, 100.0, DOM20 );

    printf("%d\n", status);
    printf("%.10f %.10f %f\n", intersect.lat, intersect.lon, intersect.altitude);

    /*
    Config config;
    int status = config.readConfigFile( "config.cfg" );
    ConfigObject obj;
    obj.uint_value = 42;
    obj.datatype = U_INT;
    status = config.saveConfigOption( "option3", obj );
    printMessage( NORMAL, "%d\n", status );
    */
    return 0;
}
