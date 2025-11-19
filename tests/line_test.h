#include <gtest/gtest.h>

#include "../src/geometry/vector.h"
#include "../src/geometry/line.h"
#include "../src/status_codes.h"

TEST( LineTests, IsPointOnLine ) {
    Line l;

    Vector
        p ( 2.5, 3.0, -1.25 ),
        v ( -1.0, 2.5, -4.0 );

    l.createLineFromBaseAndVector( p, v );

    Vector point_on_line ( 0.5, 8.0, -9.25 );

    EXPECT_TRUE( l.pointIsOnLine(point_on_line) );
}

TEST( LineTests, IsPointNotOnLine ) {
    Line l;

    Vector
        p ( 2.5, 3.0, -1.25 ),
        v ( -1.0, 2.5, -4.0 );

    l.createLineFromBaseAndVector( p, v );

    Vector point_on_line ( 0.5, 8.0, -7.25 );

    EXPECT_FALSE( l.pointIsOnLine(point_on_line) );
}

TEST( LineTests, LinesIdentical ) {
    Line l1;

    Vector
        p1 ( 2.5, 3.0, -1.25 ),
        v1 ( -1.0, 2.5, -4.0 );

    l1.createLineFromBaseAndVector( p1, v1 );

    Line l2;

    Vector
        p2 ( 0.5, 8.0, -9.25 ),
        v2 ( 2.0, -5.0, 8.0 );

    l2.createLineFromBaseAndVector( p2, v2 );

    Vector intersect;

    EXPECT_EQ( l1.lineIntersect(l2, intersect), LINES_IDENTICAL );
}

TEST( LineTests, LinesParallel ) {
    Line l1;

    Vector
        p1 ( 2.5, 3.0, -1.25 ),
        v1 ( -1.0, 2.5, -4.0 );

    l1.createLineFromBaseAndVector( p1, v1 );

    Line l2;

    Vector
        p2 ( 2.5, -3.2, -2.5 ),
        v2 ( 2.0, -5.0, 8.0 );

    l2.createLineFromBaseAndVector( p2, v2 );

    Vector intersect;

    EXPECT_EQ( l1.lineIntersect(l2, intersect), LINES_PARALLEL );
}

TEST( LineTests, LinesNoIntersect ) {
    Line l1;

    Vector
        p1 ( 1.0, -2.0, 2.0 ),
        v1 ( 3.0, 4.0, -2.0 );

    l1.createLineFromBaseAndVector( p1, v1 );

    Line l2;

    Vector
        p2 ( 5.0, 0.0, 4.0 ),
        v2 ( -1.0, -1.0, 3.0 );

    l2.createLineFromBaseAndVector( p2, v2 );

    Vector intersect;

    EXPECT_EQ( l1.lineIntersect(l2, intersect), NO_INTERSECTION_FOUND );
}

TEST( LineTests, LinesIntersect ) {
    Line l1;

    Vector
        p1 ( 1.0, -2.0, 2.0 ),
        v1 ( 3.0, 4.0, -2.0 );

    l1.createLineFromBaseAndVector( p1, v1 );

    Line l2;

    Vector
        p2 ( 7.0, 6.0, -2.0 ),
        v2 ( -1.0, -1.0, 3.0 );

    l2.createLineFromBaseAndVector( p2, v2 );

    Vector intersect;

    EXPECT_EQ( l1.lineIntersect(l2, intersect), INTERSECTION_FOUND );
}

