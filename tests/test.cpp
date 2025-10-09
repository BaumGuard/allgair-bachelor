#include "vector_test.h"
#include "line_test.h"
#include "plane_test.h"
#include "polygon_test.h"

int main ( int argc, char** argv ) {
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
