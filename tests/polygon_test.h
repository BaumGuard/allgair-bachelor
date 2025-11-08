#include <gtest/gtest.h>

#include "../src/geometry/vector.h"
#include "../src/geometry/line.h"
#include "../src/geometry/plane.h"
#include "../src/geometry/polygon.h"
#include "../src/status_codes.h"

#include <stdio.h>

TEST( PolygonTest, PointAlreadyAdded ) {
    Plane base;
    base.createPlaneFromCoordinates( 11.0, 10.0, -9.25, -15.75 );

    Vector
        p1 ( -18.0, 7.5, -15.0 ),
        p2 ( -18.0, 7.5, -15.0 );

    Polygon polygon;
    polygon.initPolygonWithPlane( base );

    polygon.addPoint( p1 );

    EXPECT_TRUE( polygon.addPoint(p2) == POINT_DUPLICATE );
}

TEST( PolygonTest, PointInPolygon ) {
    Vector
        p ( 0.0, 0.0, 0.0 ),
        v1 ( 1.0, 0.0, 0.0 ),
        v2 ( 0.0, 1.0, 0.0 );

    Polygon polygon;

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    polygon.initPolygonWithPlane( plane );

    polygon.addPoint( Vector(1.0, 3.0, 0.0) );
    polygon.addPoint( Vector(3.5, 2.5, 0.0) );
    polygon.addPoint( Vector(2.5, 1.5, 0.0) );
    polygon.addPoint( Vector(5.0, 0.5, 0.0) );
    polygon.addPoint( Vector(1.5, -2.0, 0.0) );
    polygon.addPoint( Vector(-2.5, -0.5, 0.0) );
    polygon.addPoint( Vector(-1.0, 2.0, 0.0) );

    Vector point_in_polygon ( Vector(-1.0, 0.0, 0.0) );

    EXPECT_TRUE( polygon.isPointInPolygon(point_in_polygon) );
}

TEST( PolygonTest, PointOutsidePolygon ) {
    Vector
        p ( 0.0, 0.0, 0.0 ),
        v1 ( 1.0, 0.0, 0.0 ),
        v2 ( 0.0, 1.0, 0.0 );

    Polygon polygon;

    Plane plane;
    plane.createPlaneFromBaseAndVectors( p, v1, v2 );

    polygon.initPolygonWithPlane( plane );

    polygon.addPoint( Vector(1.0, 3.0, 0.0) );
    polygon.addPoint( Vector(3.5, 2.5, 0.0) );
    polygon.addPoint( Vector(2.5, 1.5, 0.0) );
    polygon.addPoint( Vector(5.0, 0.5, 0.0) );
    polygon.addPoint( Vector(1.5, -2.0, 0.0) );
    polygon.addPoint( Vector(-2.5, -0.5, 0.0) );
    polygon.addPoint( Vector(-1.0, 2.0, 0.0) );

    Vector point_in_polygon ( Vector(4.5, -1.0, 0.0) );


    EXPECT_FALSE( polygon.isPointInPolygon(point_in_polygon) );
}

TEST( PolygonTest, LineIntersection ) {
    Plane base;
    base.createPlaneFromCoordinates( 11.0, 10.0, -9.25, -15.75 );

    Vector
        p1 ( -18.0, 7.5, -15.0 ),
        p2 ( -6.5, 2.25, -7.0 ),
        p3 ( -1.0, -7.5, -11.0 ),
        p4 ( 0.0, -16.0, -19.0 ),
        p5 ( -28.25, -2.5, -38.0 );

    Polygon polygon;
    polygon.initPolygonWithPlane( base );

    polygon.addPoint( p1 );
    polygon.addPoint( p2 );
    polygon.addPoint( p3 );
    polygon.addPoint( p4 );
    polygon.addPoint( p5 );

    Vector
        line_base_point ( -7.0, -8.25, -10.0 ),
        line_vector ( -1.0, 2.0, -3.5 );

    Line ray;
    ray.createLineFromBaseAndVector(
        line_base_point, line_vector
    );

    Vector intersect;
    int status = polygon.lineIntersection( ray, intersect );

    Vector p ( -9.0, -4.25, -17.0 );

    EXPECT_TRUE( status == LINE_INTERSECTS_POLYGON && intersect == p );
}

TEST( PolygonTest, LineNoIntersection ) {
    Plane base;
    base.createPlaneFromCoordinates( 11.0, 10.0, -9.25, -15.75 );

    Vector
        p1 ( -18.0, 7.5, -15.0 ),
        p2 ( -6.5, 2.25, -7.0 ),
        p3 ( -1.0, -7.5, -11.0 ),
        p4 ( 0.0, -16.0, -19.0 ),
        p5 ( -28.25, -2.5, -38.0 );

    Polygon polygon;
    polygon.initPolygonWithPlane( base );

    polygon.addPoint( p1 );
    polygon.addPoint( p2 );
    polygon.addPoint( p3 );
    polygon.addPoint( p4 );
    polygon.addPoint( p5 );

    Vector
        line_base_point ( -6.0, 16.5, 9.0 ),
        line_vector ( -1.0, 2.0, -3.5 );

    Line ray;
    ray.createLineFromBaseAndVector(
        line_base_point, line_vector
    );

    Vector intersect;
    int status = polygon.lineIntersection( ray, intersect );

    Vector p ( -9.0, -4.25, -17.0 );

    EXPECT_TRUE( status == POLYGON_LINE_NO_INTERSECT );
}
