#include "signal_cone.h"

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../utils.h"
#include "../status_codes.h"

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
}
