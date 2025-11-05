#include "line.h"

#include "../utils.h"
#include "../status_codes.h"

/*---------------------------------------------------------------*/

int Line::createLineFromTwoPoints ( Vector& p1, Vector& p2 ) {
    if ( p1 == p2 ) {
        return POINTS_IDENTICAL;
    }

    base = p1;
    direction = p2 - p1;

    return CREATION_SUCCEEDED;
} /* createLineFromTwoPoints() */

/*---------------------------------------------------------------*/

int Line::createLineFromBaseAndVector ( Vector& p, Vector& v ) {
    if ( v.length() == 0.0 ) {
        return VECTOR_LENGTH_0;
    }

    base = p;
    direction = v;

    return CREATION_SUCCEEDED;
} /* createLineFromBaseAndVector() */

/*---------------------------------------------------------------*/

Vector Line::getBaseVector () const {
    return base;
} /* getBaseVector() */

Vector Line::getDirectionVector () const {
    return direction;
} /* getDirectionVector() */

/*---------------------------------------------------------------*/

bool Line::pointIsOnLine ( Vector p ) {
    double
        x_a = base.getX(),
        y_a = base.getY(),
        z_a = base.getZ(),

        x_b = direction.getX(),
        y_b = direction.getY(),
        z_b = direction.getZ(),

        x_p = p.getX(),
        y_p = p.getY(),
        z_p = p.getZ(),

        u_x = ( x_p - x_a ) / x_b,
        u_y = ( y_p - y_a ) / y_b,
        u_z = ( z_p - z_a ) / z_b;

    if ( u_x == u_y && u_y == u_z ) {
        return true;
    }
    return false;
} /* pointIsOnLine() */

/*---------------------------------------------------------------*/

int Line::lineIntersect ( Line& l, Vector& intersect, double* factor ) {
    if ( *this == l ) {
        return LINES_IDENTICAL;
    }
    if ( direction.linearDependant(l.getDirectionVector()) ) {
        return LINES_PARALLEL;
    }

    double
        v_num,
        v_denom;

    double
        x_a = base.getX(),
        y_a = base.getY(),
        z_a = base.getZ(),

        x_b = direction.getX(),
        y_b = direction.getY(),
        z_b = direction.getZ(),

        x_c = l.getBaseVector().getX(),
        y_c = l.getBaseVector().getY(),
        z_c = l.getBaseVector().getZ(),

        x_d = l.getDirectionVector().getX(),
        y_d = l.getDirectionVector().getY(),
        z_d = l.getDirectionVector().getZ();



    v_denom = x_d * y_b - x_b * y_d;

    if ( v_denom == 0.0 ) {
        return LINES_NO_INTERSECT;
    }

    v_num = y_b * ( x_a - x_c ) + x_b * ( y_c - y_a );

    double v = v_num / v_denom;

    int avg_count = 0;

    double
        u_x = 0.0,
        u_y = 0.0,
        u_z = 0.0,
        u_comp;

    if ( x_b != 0.0 ) {
        u_x = ( x_c - x_a + v * x_d ) / x_b;
        u_comp = u_x;
        avg_count++;
    }
    if ( y_b != 0.0 ) {
        u_y = ( y_c - y_a + v * y_d ) / y_b;
        u_comp = u_y;
        avg_count++;
    }
    if ( z_b != 0.0 ) {
        u_z = ( z_c - z_a + v * z_d ) / z_b;
        u_comp = u_z;
        avg_count++;
    }

    double avg = clampDouble( (u_x+u_y+u_z) / (double)avg_count );
    u_comp = clampDouble( u_comp );

    if ( avg == u_comp ) {
        intersect = base + u_comp * direction;

        if ( factor != nullptr ) {
            *factor = v;
        }
        return LINES_INTERSECT;
    }

    return LINES_NO_INTERSECT;
} /* lineIntersect() */

/*---------------------------------------------------------------*/

bool Line::operator == ( Line& l ) {
    bool point_is_on_line = pointIsOnLine( l.getBaseVector() );
    bool direction_vectors_linear_dependant =
        direction.linearDependant( l.getDirectionVector() );

    if ( point_is_on_line && direction_vectors_linear_dependant ) {
        return true;
    }
    return false;
} /* operator == */

/*---------------------------------------------------------------*/

void Line::printLine () {
    printMessage(
        NORMAL,
        "x_base=%f y_base=%f z_base=%f\n",
        base.getX(),
        base.getY(),
        base.getZ()
    );
    printMessage(
        NORMAL,
        "x_dir=%f y_dir=%f z_dir=%f\n",
        direction.getX(),
        direction.getY(),
        direction.getZ()
    );
} /* printLine() */
