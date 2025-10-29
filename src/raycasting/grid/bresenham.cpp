#include "../coord.h"

enum Axes {
    X,
    Y,
    Z
};



// Find a line of cubes from a start to an end point in a 3D grid
// using a version of the Bresenham algorithm modified for 3D grids
void bresenham3D ( GridTile& grid, TileCoord* intersect, TileCoord* start, TileCoord* end ) {

    // Calculate deltas of x, y and z
    uint32_t
        dx = abs( end->x - start->x ),
        dy = abs( end->y - start->y ),
        dz = abs( end->z - start->z );

    int
        iterator,   // Delta of the direction used for iteration
        delta1,     // Delta of the first dependent variable
        delta2,     // Delta of the second dependent variable
        value1,     // First dependent variable
        value2,     // Second dependent variable

        iterator_start, // Starting value for the independent iterator
        iterator_end,   // End value for the independent iterator
        value1_start,   // Starting value for the first dependent variable
        value1_end,     // End value for the first dependent variable
        value2_start,   // Starting value for the second dependent variable
        value2_end;     // End value for the second dependent variable

    // Find the fastest direction using the deltas of x, y and z
    int iteration_axis;

    if ( dx > dy && dx > dz ) {
        iterator_axis = X;
    }
    else if ( dy > dx && dy > dz ) {
        iterator_axis = Y;
    }
    else {
        iterator_axis = Z
    }

    // The iterator is assigned the fastest direction and the other
    // axes become dependent variables of the iterator
    //
    // EXAMPLE:
    // Consider the following deltas:
    // - dx = 7
    // - dy = 11
    // - dz = 5
    //
    // Since dy is the largest delta in this case, it is assigned
    // to the iterator
    // The first dependent variable is then x and the second z
    // (in alphabetic order)
    switch ( biggest_delta ) {
        case X:
            iterator = 2 * dx;
            delta1 = dy;
            delta2 = dz;
            value1 = start->y;
            value2 = start->z;
            iterator_start = start->x;
            iterator_end = end->x;
            value1_start = start->y;
            value1_end = end->y;
            value2_start = start->z;
            value2_end = end->z;
            break;

        case Y:
            iterator = 2 * dy;
            delta1 = dx;
            delta2 = dz;
            value1 = start->x;
            value2 = start->z;
            iterator_start = start->y;
            iterator_end = end->y;
            value1_start = start->x;
            value1_end = end->x;
            value2_start = start->z;
            value2_end = end->z;
            break;

        case Z:
            iterator = 2 * dz;
            delta1 = dx;
            delta2 = dy;
            value1 = start->x;
            value2 = start->y;
            iterator_start = start->z;
            iterator_end = end->z;
            value1_start = start->x;
            value1_end = end->x;
            value2_start = start->y;
            value2_end = end->y;
            break;
    }

    // Calculate the initial error for the first and second
    // dependent variable
    int
        e1 = iterator,
        e2 = iterator;

    // Coordinate variables to map the iterator and the dependent
    // variables back to 3D coordinates
    int
        x,
        y,
        z;

    // Count the number of cubes the ray passes through until hitting
    // the grid boundary or an obstacle
    uint32_t cube_count = 1;

    // Iterator
    int i;

    // Check if the Bresenham algorithm will run from left to right
    // or from right to left and set the starting point for the
    // iteration accordingly
    bool left_to_right = iterator_start < iterator_end;
    if ( left_to_right ) {
        i = iterator_start+1;
    }
    else {
        i = iterator_start-1;
    }


    while ( true ) {

        e1 -= delta1;
        e2 -= delta2;

        if ( e1 < 0 ) {
            e1 += iterator;

            if ( value1_start < value1_end )
                value1++;
            else
                value1--;
        }

        if ( e2 < 0.0 ) {
            e2 += iterator;

            if ( value2_start < value2_end )
                value2++;
            else
                value2--;
        }

        // Map the iterator and the dependent variables
        // back to the cartesian coordinates
        switch( biggest_delta ) {
            case X:
                x = i;
                y = value1;
                z = value2;
                break;

            case Y:
                x = value1;
                y = i;
                z = value2;
                break;

            case Z:
                x = value1;
                y = value2;
                z = i;
                break;
        }

        // Check if the ray hit and obstacle
        uint8_t altitude_at_xy = grid.getValue( x, y );
        if ( altitude_at_xy == z ) {
            intersect->x = x;
            intersect->y = y;
            intersect->z = z;
            return;
        }

        // When runnning from left to right, increment the iterator
        // and when running from right to left, decrement the iterator
        if ( left_to_right ) {
            if ( i < iterator_end ) i++;
            else break;
        }
        else {
            if ( i > iterator_end ) i--;
            else break;
        }
    }

    intersect->x = end->x;
    intersect->y = end->y;
    intersect->z = end->z;
    return;
}
