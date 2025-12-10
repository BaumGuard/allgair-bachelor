#include "fresnel_zone.h"

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../utils.h"
#include "../status_codes.h"
#include "../raw_data/gmlfile.h"
#include "../tile/vector_tile.h"
#include "../tile/tile_name.h"
#include "../tile/load_tile.h"

#include <cmath>

/*---------------------------------------------------------------*/
#if 0
int signalConeGroundArea (
    Line& center_ray, double cone_angle, int n_samples, Polygon& ground_area
) {
    // Check if the cone angle is between 0° and 90°
    if ( !(cone_angle >= 0.0 && cone_angle < 0.5 * M_PI) ) {
        return INVALID_SIGNAL_CONE_ANGLE;
    }


    // Angle step by which to increase the angle in every iteration
    double angle_step = TWO_PI / (double)n_samples;

    // Auxiliary unit vector in the unit circle on the xy plane
    Vector aux_vector;

    // Vector perpendicular to the center ray of the cone
    Vector perpendicular_vector;

    // Position of the cone tip and the direction vector of
    // the cone's center ray
    Vector cone_tip = center_ray.getBaseVector();
    Vector center_ray_dir = center_ray.getDirectionVector();
    center_ray_dir.toUnitVector();

    // Vector and line on the outer surface of the cone
    Vector cone_vector;
    Line cone_line;

    // Ground plane (xy plane)
    Plane ground_plane;
    ground_plane.createPlaneFromCoordinates( 0.0, 0.0, 1.0, 0.0 );

    // Polygon to represent the intersection area of the cone with
    // the ground plane
    ground_area.initPolygonWithPlane( ground_plane );

    Vector ground_plane_intersection;

    int status;

    // Iterate between 0° and 360° to find intersection points of the cone's
    // outer surface with the ground plane
    for ( double angle = 0.0; angle < TWO_PI; angle += angle_step ) {

        // Use the auxiliary vector (Unit vector ending at the unit circle in
        // the xy plane) to find a vector that is perpendicular to the
        // cone's center ray
        aux_vector = Vector( cos(angle), sin(angle), 0.0 );
        perpendicular_vector = center_ray_dir.crossProduct( aux_vector );

        // Ray on the outer surface of the cone
        cone_vector =
            cos(cone_angle) * center_ray_dir +
            sin(cone_angle) * perpendicular_vector;

        cone_line.createLineFromBaseAndVector( cone_tip, cone_vector );

        // Find the intersection of the ray on the outer surface with the ground plane
        status = ground_plane.lineIntersection(cone_line, ground_plane_intersection);

        if ( status == INTERSECTION_FOUND ) {
            status = ground_area.addPoint( ground_plane_intersection );
            if ( status != SUCCESS ) {
                return status;
            }
        }
        else {
            return status;
        }
    }

    return SUCCESS;
} /* signalConeGroundArea() */
#endif


Polygon fresnelZone (
    Vector& start_point, Vector& end_point,
    double radius,
    uint n_samples
) {
    Vector diff = end_point - start_point;
    Vector ellipse_center = start_point + diff / 2.0;
    double distance = diff.length();

    double e = distance / 2.0;
    double b = radius;
    double a = sqrt( b*b + e*e );

    double step = 4.0 * a / (double)n_samples;

    Polygon ground_area;
    Plane base_plane;
    base_plane.createPlaneFromCoordinates( 0.0, 0.0, 1.0, 0.0 );
    ground_area.initPolygonWithPlane( base_plane );

    for ( double x = -a; x < a; x += step ) {
        double y = b / a * sqrt( a*a - x*x );
        Vector sample( x, y, 0.0 );
        ground_area.addPoint( sample );
    }
    for ( double x = a; x > -a; x -= step ) {
        double y = -b / a * sqrt( a*a - x*x );
        Vector sample( x, y, 0.0 );
        ground_area.addPoint( sample );
    }
    //ground_area.printPolygon();

    double
        delta_x = diff.getX(),
        delta_y = diff.getY();

    double rotation_angle = atan2( delta_y, delta_x );

    std::vector<Vector>& points = ground_area.getPoints();
    uint len = points.size();
    double x, y;
    for ( uint i = 0; i < len; i++ ) {
        x = points[i].getX() * cos(rotation_angle) -
            points[i].getY() * sin(rotation_angle);

        y = points[i].getX() * sin(rotation_angle) +
            points[i].getY() * cos(rotation_angle);

        x += ellipse_center.getX();
        y += ellipse_center.getY();

        points[i].setX( x );
        points[i].setY( y );
    }

    return ground_area;
} /* fresnelZone() */


/*---------------------------------------------------------------*/

std::vector<std::string> tilesInGroundArea ( Polygon& ground_area ) {
    std::vector<Vector> ground_area_points = ground_area.getPoints();
    double
        min_utmx = ground_area_points[0].getX(),
        max_utmx = ground_area_points[0].getX(),
        min_utmy = ground_area_points[0].getY(),
        max_utmy = ground_area_points[0].getY();

    uint len = ground_area_points.size();
    for ( uint i = 1; i < len; i++ ) {
        if ( ground_area_points[i].getX() < min_utmx ) {
            min_utmx = ground_area_points[i].getX();
        }
        if ( ground_area_points[i].getX() > max_utmx ) {
            max_utmx = ground_area_points[i].getX();
        }
        if ( ground_area_points[i].getY() < min_utmy ) {
            min_utmy = ground_area_points[i].getY();
        }
        if ( ground_area_points[i].getY() > max_utmy ) {
            max_utmy = ground_area_points[i].getY();
        }
    }

    min_utmx -= fmod( min_utmx, 2000.0 );
    max_utmx -= fmod( max_utmx, 2000.0 );
    min_utmy -= fmod( min_utmy, 2000.0 );
    max_utmy -= fmod( max_utmy, 2000.0 );

    Vector
        coord_lower_left,
        coord_lower_right,
        coord_upper_left,
        coord_upper_right;

    std::vector<std::string> tiles_in_ground_area;

    for ( double utmy = min_utmy; utmy <= max_utmy; utmy += 2000.0 ) {
        for ( double utmx = min_utmx; utmx <= max_utmx; utmx += 2000.0 ) {
            coord_lower_left  = Vector( utmx,        utmy,        0.0 );
            coord_lower_right = Vector( utmx+2000.0, utmy,        0.0 );
            coord_upper_left  = Vector( utmx,        utmy+2000.0, 0.0 );
            coord_upper_right = Vector( utmx+2000.0, utmy+2000.0, 0.0 );

            if (
                ground_area.isPointInPolygon( coord_lower_left )  ||
                ground_area.isPointInPolygon( coord_lower_right ) ||
                ground_area.isPointInPolygon( coord_upper_left )  ||
                ground_area.isPointInPolygon( coord_upper_right )
            ) {
                uint
                    utmx_km = (uint)( utmx / 1000.0 ),
                    utmy_km = (uint)( utmy / 1000.0 );

                std::string tile_name = buildTileName( utmx_km, utmy_km );
                tiles_in_ground_area.push_back( tile_name );
            }
        }
    }

    return tiles_in_ground_area;
} /* tilesInGroundArea() */

/*---------------------------------------------------------------*/

int getPolygonsInGroundArea (
    std::vector<Polygon>& polygons,
    Polygon& ground_area,
    std::vector<std::string>& tile_names
) {
    VectorTile vector_tile;

    uint len_tiles = tile_names.size();
    uint len_polygons;
    int status;

    Vector test_point;

    for ( uint i = 0; i < len_tiles; i++ ) {
        status = getVectorTile( vector_tile, tile_names[i] );
        if ( status != SUCCESS ) {
            return status;
        }

        std::vector<Polygon>& tile_polygons = vector_tile.getPolygons();
        len_polygons = tile_polygons.size();

        for ( uint j = 0; j < len_polygons; j++ ) {
            test_point = tile_polygons[j].getPoints()[0];
            test_point.setZ( 0.0 );

            if ( ground_area.isPointInPolygon(test_point) ) {
                polygons.push_back( tile_polygons[j] );
            }
        }
    }

    return SUCCESS;
} /* getPolygonsInGroundArea() */
