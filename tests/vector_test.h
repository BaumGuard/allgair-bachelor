#include <gtest/gtest.h>
#include "../src/geometry/vector.h"

TEST( VectorTest, VectorAddUnary ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -2.0, 2.0, 6.25 ),
        v_exp ( -1.0, -1.0, 8.75 );

    v1 += v2;

    EXPECT_TRUE( v1 == v_exp );
}

TEST( VectorTest, VectorAddBinary ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -2.0, 2.0, 6.25 ),
        v_res = v1 + v2,
        v_exp ( -1.0, -1.0, 8.75 );

    EXPECT_TRUE( v_res == v_exp );
}

TEST( VectorTest, VectorSubtractUnary ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -2.0, 2.0, 6.25 ),
        v_exp ( 3.0, -5.0, -3.75 );

    v1 -= v2;

    EXPECT_TRUE( v1 == v_exp );
}

TEST( VectorTest, VectorSubtractBinary ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -2.0, 2.0, 6.25 ),
        v_res = v1 - v2,
        v_exp ( 3.0, -5.0, -3.75 );

    EXPECT_TRUE( v_res == v_exp );
}

TEST( VectorTest, VectorMultiplyUnary ) {
    Vector
        v ( 1.0, -3.0, 2.5 ),
        v_exp ( 1.5, -4.5, 3.75 );

    v *= 1.5;

    EXPECT_TRUE( v == v_exp );
}

TEST( VectorTest, VectorMultiplyBinary ) {
    Vector
        v ( 1.0, -3.0, 2.5 ),
        v_res = v * 1.5,
        v_exp ( 1.5, -4.5, 3.75 );

    EXPECT_TRUE( v_res == v_exp );
}

TEST( VectorTest, VectorDivideUnary ) {
    Vector
        v ( 1.0, -3.0, 2.5 ),
        v_exp ( 0.5, -1.5, 1.25 );

    v /= 2.0;

    EXPECT_TRUE( v == v_exp );
}

TEST( VectorTest, VectorDivideBinary ) {
    Vector
        v ( 1.0, -3.0, 2.5 ),
        v_res = v / 2.0,
        v_exp ( 0.5, -1.5, 1.25 );

    EXPECT_TRUE( v_res == v_exp );
}

TEST( VectorTest, VectorsEqual1 ) {
    Vector
        v1 ( 2.5, -2.0, -3.0 ),
        v2 ( 2.5, -2.0, -3.0 );

    EXPECT_TRUE( v1 == v2 );
}

TEST( VectorTest, VectorsUnequal1 ) {
    Vector
        v1 ( 2.5, -2.0, -3.0 ),
        v2 ( 2.5, -2.1, -3.0 );

    EXPECT_FALSE( v1 == v2 );
}

TEST( VectorTest, VectorsEqual2 ) {
    Vector
        v1 ( 2.5, -2.0, -3.0 ),
        v2 ( 2.5, -2.1, -3.0 );

    EXPECT_TRUE( v1 != v2 );
}

TEST( VectorTest, VectorsUnequal2 ) {
    Vector
        v1 ( 2.5, -2.0, -3.0 ),
        v2 ( 2.5, -2.0, -3.0 );

    EXPECT_FALSE( v1 != v2 );
}

TEST( VectorTest, VectorLength ) {
    Vector v ( 2.0, -6.0, -3.0 );

    EXPECT_EQ( v.length(), 7.0 );
}

TEST( VectorTest, LinearDependant ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -3.0, 9.0, -7.5 );

    EXPECT_TRUE( v1.linearDependant(v2) );
}

TEST( VectorTest, LinearIndependant ) {
    Vector
        v1 ( 1.0, -3.0, 2.5 ),
        v2 ( -3.0, 8.5, -7.5 );

    EXPECT_FALSE( v1.linearDependant(v2) );
}
