#include <gtest/gtest.h>

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../geometry/polygon.h"

TEST( PolygonTest, PointInPolygon ) {
    Vector
        p ( 0.0, 0.0, 0.0 ),
        v1 ( 1.0, 0.0, 0.0 ),
        v2 ( 0.0, 1.0, 0.0 );

    Polygon polygon;

    Plane plane;
    plane.buildPlaneFromBaseAndVectors( p, v1, v2 );

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
    plane.buildPlaneFromBaseAndVectors( p, v1, v2 );

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
