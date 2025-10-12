#include <Eigen/Dense>

#include <climits>
#include <cstdlib>
#include <pthread.h>
#include <time.h>

#include "vector.h"
#include "line.h"
#include "plane.h"

int NUM_EXECUTIONS;

using Eigen::Vector3d;
using Eigen::ParametrizedLine;
using Eigen::Hyperplane;

double random ( double max_abs ) {
    return rand() / INT_MAX * max_abs;
}

void* own ( void* arg ) {
    Vector
        p1,
        p2,
        v1,
        v2,
        intersect;

    Line
        l1,
        l2;

    Plane
        plane1,
        plane2;

    for ( int i=0; i<NUM_EXECUTIONS; i++ ) {
         p1 = Vector( random(25.0), random(4.2), random(97.36) );
         p2 = Vector( random(1.5), random(37.8), random(27.0) );

         v1 = Vector( random(78.7), random(3.7), random(15.4) );
         v2 = Vector( random(126.7), random(0.6), random(60.1) );

         l1.createLineFromTwoPoints( p1, p2 );
         l2.createLineFromBaseAndVector( p1, v1 );

         l1.pointIsOnLine( p2 );
/*
         l1.lineIntersect( l2, intersect );
*/
         plane1.buildPlaneFromPoints( p1, p2, v1 );
         plane2.buildPlaneFromBaseAndVectors( p1, v1, v2 );
/*
         plane1.normalVector();

         plane1.lineIntersection( l1, intersect );
         plane2.reflectLine( l1, l2 );
         */
    }

    printf("own finished\n");
    return NULL;
}

void* eigen ( void* arg ) {
    Vector3d
        p1,
        p2,
        v1,
        v2,
        intersect;

    ParametrizedLine<double, 3>
        l1,
        l2;

    Hyperplane<double, 3>
        plane1,
        plane2;

    for ( int i=0; i<NUM_EXECUTIONS; i++ ) {
        p1 = Vector3d( random(25.0), random(4.2), random(97.36) );
        p2 = Vector3d( random(1.5), random(37.8), random(27.0) );

        v1 = Vector3d( random(78.7), random(3.7), random(15.4) );
        v2 = Vector3d( random(126.7), random(0.6), random(60.1) );

        l1 = ParametrizedLine<double, 3>( p1, p2.normalized() );
        l2 = ParametrizedLine<double, 3>( p1, v1.normalized() );

        plane1 = Hyperplane<double, 3>::Through( p1, p2, v1 );
        plane2 = Hyperplane<double, 3>::Through( p1, v1, v2 );

        l1.intersectionPoint( plane1 );
    }

    printf("eigen finished\n");
    return NULL;
}

int main ( int argc, char* argv[] ) {
    NUM_EXECUTIONS = atoi( argv[1] );
    srand( time(0) );

    pthread_t
        own_thread,
        eigen_thread;

    pthread_create( &own_thread, NULL, own, NULL );
    pthread_create( &eigen_thread, NULL, eigen, NULL );

    pthread_join( own_thread, NULL );
    pthread_join( eigen_thread, NULL );

    return 0;
}
