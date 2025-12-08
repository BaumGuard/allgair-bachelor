#include "signal_cone.h"

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../utils.h"
#include "../status_codes.h"
#include "../raw_data/gmlfile.h"
#include "../tile/vector_tile.h"
#include "../tile/tile_name.h"

#include <cmath>

/*---------------------------------------------------------------*/

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

/*---------------------------------------------------------------*/

std::vector<std::string> tilesInGroundArea ( Polygon& ellipse ) {
    std::vector<Vector> ellipse_points = ellipse.getPoints();
    double
        min_utmx = ellipse_points[0].getX(),
        max_utmx = ellipse_points[0].getX(),
        min_utmy = ellipse_points[0].getY(),
        max_utmy = ellipse_points[0].getY();

    uint len = ellipse_points.size();
    for ( uint i = 1; i < len; i++ ) {
        if ( ellipse_points[i].getX() < min_utmx ) {
            min_utmx = ellipse_points[i].getX();
        }
        if ( ellipse_points[i].getX() > max_utmx ) {
            max_utmx = ellipse_points[i].getX();
        }
        if ( ellipse_points[i].getY() < min_utmy ) {
            min_utmx = ellipse_points[i].getY();
        }
        if ( ellipse_points[i].getY() > max_utmy ) {
            min_utmx = ellipse_points[i].getY();
        }
    }

    min_utmx -= fmod( min_utmx, 2000.0 );
    max_utmx -= fmod( max_utmx, 2000.0 );
    min_utmy -= fmod( min_utmy, 2000.0 );
    max_utmy -= fmod( min_utmy, 2000.0 );

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
                ellipse.isPointInPolygon( coord_lower_left )  ||
                ellipse.isPointInPolygon( coord_lower_right ) ||
                ellipse.isPointInPolygon( coord_upper_left )  ||
                ellipse.isPointInPolygon( coord_upper_right )
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

std::vector<Polygon> getPolygonsInGroundArea ( std::vector<std::string>& tile_names ) {
    VectorTile vector_tile;

    uint len = tile_names.size();
    for ( uint i = 0; i < len; i++ ) {

    }
}
