#include <gtest/gtest.h>
#include "../src/geometry/plane.h"

TEST( PlaneTest, NormalVector ) {
    Vector
        p  ( 5.5, -1.0, 3.5 ),
        v1 ( -2.0, 1.0, 7.0 ),
        v2 ( 3.0, -4.0, 0.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector nv_exp ( 28.0, 21.0, 5.0 );
    double nv_exp_len = nv_exp.length();
    nv_exp /= nv_exp_len;

    Vector nv_res = plane.normalVector();

    EXPECT_TRUE( nv_exp == nv_res );
}

TEST( PlaneTest, PointOnPlane ) {
    Vector
        p  ( 5.5, -1.0, 3.5 ),
        v1 ( -2.0, 1.0, 7.0 ),
        v2 ( 3.0, -4.0, 0.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector point ( 1.5, 1.0, 17.5 );

    EXPECT_TRUE( plane.isPointOnPlane(point) );
}

TEST( PlaneTest, PointNotOnPlane ) {
    Vector
        p  ( 5.5, -1.0, 3.5 ),
        v1 ( -2.0, 1.0, 7.0 ),
        v2 ( 3.0, -4.0, 0.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector point ( 1.5, 0.0, 17.5 );

    EXPECT_TRUE( !plane.isPointOnPlane(point) );
}

TEST( PlaneTest, LineParallelToPlane ) {
    Vector
        p ( -1.0, 2.0, -2.0 ),
        v1( -3.0, -1.0, 4.0 ),
        v2( 0.0, 1.0, 3.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector
        p_plane( 5.0, -1.0, 12.0 ),
        v_plane( -3.0, -3.0, -2.0 );

    Line line;
    line.createLineFromBaseAndVector( p_plane, v_plane );

    Vector intersect;

    int status = plane.lineIntersection(line, intersect);

    EXPECT_EQ( status, LINE_PARALLEL_TO_PLANE );
}

TEST( PlaneTest, LineOnPlane ) {
    Vector
        p ( -1.0, 2.0, -2.0 ),
        v1( -3.0, -1.0, 4.0 ),
        v2( 0.0, 1.0, 3.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector
        p_line( 8.0, 7.0, -8.0 ),
        v_line( 3.0, 5.0, 8.0 );

    Line line;
    line.createLineFromBaseAndVector( p_line, v_line );

    Vector intersect;

    int status = plane.lineIntersection(line, intersect);

    EXPECT_EQ( status, LINE_ON_PLANE );
}

TEST( PlaneTest, LineIntersectsPlane ) {
    Vector
        p ( -1.0, 2.0, -2.0 ),
        v1( -3.0, -1.0, 4.0 ),
        v2( 0.0, 1.0, 3.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector
        p_line( -10.0, 8.0, 9.0 ),
        v_line( -1.0, 3.0, 2.0 );

    Line line;
    line.createLineFromBaseAndVector( p_line, v_line );

    Vector
        intersect_res,
        intersect_exp( -7.0, -1.0, 3.0 );

    int status = plane.lineIntersection(line, intersect_res);

    EXPECT_TRUE(
        intersect_res == intersect_exp &&
        status == INTERSECTION_FOUND
    );
}

TEST( PlaneTest, LineReflect ) {
    Vector
        p ( 2.0, 1.0, 3.0 ),
        v1( 1.0, 1.0, 0.0 ),
        v2( -1.0, 0.0, -1.0 );

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    Vector
        p_line( 0.0, 5.0, 0.0 ),
        v_line( 2.0, 1.0, 0.0 );

    Line line;
    line.createLineFromBaseAndVector( p_line, v_line );

    Line l_res;
    plane.reflectLine( line, l_res );

    Vector
        base_res = l_res.getBaseVector(),
        dir_res  = l_res.getDirectionVector(),
        base_exp ( 2.0, 3.0, -2.0 ),
        dir_exp  ( 0.6, 0.75, 0.3 );

    Line l_exp;
    l_exp.createLineFromBaseAndVector( base_exp, dir_exp );

    EXPECT_TRUE( l_res == l_exp );
}
