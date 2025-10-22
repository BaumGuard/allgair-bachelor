#include "vector_tile.h"

#include "../geometry/vector.h"
#include "../geometry/line.h"
#include "../geometry/plane.h"
#include "../raw_data/surface.h"

#include <cmath>
#include <cstring>

/*---------------------------------------------------------------*/

VectorTile::VectorTile ( GmlFile& gmlfile ) {
    Vector p1, p2, p3;
    Vector dv1, dv2;

    Plane base_plane;

    std::vector<Surface>& surfaces = gmlfile.getSurfaces();

    int len = surfaces.size();
    int len_pos_list;

    Vector
        pos,
        subtrahend = gmlfile.getLowerCorner(),
        corr_vec ( 0.0, 0.0, 1.0 ),
        corr_point;

    Line corr_line;

    int yes = 0, no = 0;

    subtrahend.setZ( 0.0 );

    for ( int i=0; i<len; i++ ) {
        bool point_to_far_away = false;

        p1 = surfaces[i].pos_list[0];

        int p2_index = 0;


        len_pos_list = surfaces[i].pos_list.size();

        for ( int k = 1; k < len_pos_list; k++ ) {
            p2 = surfaces[i].pos_list[k];
            dv1 = p2 - p1;

            if ( dv1.length() > 1.0 ) {
                p2_index = k;
                break;
            }
        }



        int point_chosen = 0;

        for ( int k=1; k<len_pos_list; k++ ) {
            if ( k == p2_index ) {
                continue;
            }
            p3 = surfaces[i].pos_list[k];

            dv2 = p3 - p1;

            if ( !dv1.linearDependant(dv2) && dv2.length() > 0.05 ) {
                point_chosen = k;
                break;
            }
        }

        base_plane.createPlaneFromPoints( p1, p2, p3 );

        Polygon polygon;
        polygon.initPolygonWithPlane( base_plane );

        len_pos_list = surfaces[i].pos_list.size();
        for ( int j=0; j<len_pos_list; j++ ) {
            pos = surfaces[i].pos_list[j];
            //pos = pos - subtrahend;

            if ( !base_plane.isPointOnPlane(pos) ) {
                double dist = base_plane.distanceOfPointToPlane(pos);

                if ( dist < PLANE_DISTANCE_THRESHOLD ) {
                    corr_line.createLineFromBaseAndVector( pos, corr_vec );

                    base_plane.lineIntersection( corr_line, corr_point );

                    polygon.addPoint( corr_point );
                }
                else {
                    Plane pl;

                    double norm1 = sqrt( dv1.getX()*dv1.getX() + dv1.getY()*dv1.getY() + dv1.getZ()*dv1.getZ() );
                    double norm2 = sqrt( dv2.getX()*dv2.getX() + dv2.getY()*dv2.getY() + dv2.getZ()*dv2.getZ() );

                    dv1 /= norm1;
                    dv2 /= norm2;

                    printf("Distance %f\n", dist);
                    printf("Point chosen 1 %d\n", p2_index);
                    printf("Point chosen 2 %d\n", point_chosen);
                    printf("dv1 %f %f %f\n", dv1.getX(), dv1.getY(), dv1.getZ());
                    printf("dv2 %f %f %f\n", dv2.getX(), dv2.getY(), dv2.getZ());
                    base_plane.printPlane();

                    uint32_t len = surfaces[i].pos_list.size();
                    for ( uint32_t k=0; k<len; k++ ) {
                        Vector v = surfaces[i].pos_list[k];
                        v -= subtrahend;
                        printf( "%f %f %f\n", v.getX(), v.getY(), v.getZ() );
                    }
                    printf( "---------\n" );

                    point_to_far_away = true;
                    break;
                }
            }
            else {
                polygon.addPoint( pos );
            }
        }

        if ( !point_to_far_away ) {
            polygons.push_back( polygon );
            yes++;
        }
        else {
            no++;
        }
    }

    printf( "yes=%d no=%d -> %f %%\n", yes, no, (double)yes*100.0 / (double)(yes+no) );
} /* VectorTile::VectorTile ( GmlFile& gmlfile ) */

/*---------------------------------------------------------------*/

VectorTile::VectorTile () {}

/*---------------------------------------------------------------*/

Vector VectorTile::getLowerCorner () {
    return lower_corner;
} /* Vector VectorTile::getLowerCorner () */

/*---------------------------------------------------------------*/

Vector VectorTile::getUpperCorner () {
    return upper_corner;
} /* Vector VectorTile::getUpperCorner () */

/*---------------------------------------------------------------*/

std::vector<Polygon>& VectorTile::getPolygons () {
    return polygons;
} /* std::vector<Polygon>& VectorTile::getPolygons () */

/*---------------------------------------------------------------*/

union data_block {
    uint32_t u32;
    float f32;

    char bytes [4];
};

int VectorTile::createBinaryFile ( char* file_path ) {
    uint32_t byte_count = 0;

    union data_block data_sect;

    FILE* file = fopen( file_path, "wb" );

    if ( !file ) {
        return BINARY_FILE_NOT_CREATABLE;
    }

    fprintf( file, "DATA" );

    data_sect.u32 = 0;
    fwrite( data_sect.bytes, 1, 4, file );

    uint32_t len_polygons = (uint32_t) polygons.size();
    uint32_t len_point_list;
    data_sect.u32 = len_polygons;

    fwrite( data_sect.bytes, 1, 4, file );

    byte_count += 12;

    Plane p;
    Vector v;
    std::vector<Vector> points;
    Vector point;

    for ( uint32_t i = 0; i < len_polygons; i++ ) {
        fprintf( file, "PLGN" );

        data_sect.u32 = i;
        fwrite( data_sect.bytes, 1, 4, file );

        fprintf( file, "PLAN" );

        p = polygons[i].getBasePlane();

        data_sect.f32 = p.getX();
        fwrite( data_sect.bytes, 1, 4, file );

        data_sect.f32 = p.getY();
        fwrite( data_sect.bytes, 1, 4, file );

        data_sect.f32 = p.getZ();
        fwrite( data_sect.bytes, 1, 4, file );

        data_sect.f32 = p.getN();
        fwrite( data_sect.bytes, 1, 4, file );

        fprintf( file, "PNTS" );

        byte_count += 32;

        points = polygons[i].getPoints();
        len_point_list = points.size();

        for ( uint32_t j = 0; j < len_point_list; j++ ) {
            point = points[j];

            data_sect.f32 = point.getX();
            fwrite( data_sect.bytes, 1, 4, file );

            data_sect.f32 = point.getY();
            fwrite( data_sect.bytes, 1, 4, file );

            data_sect.f32 = point.getZ();
            fwrite( data_sect.bytes, 1, 4, file );

            byte_count += 12;
        }
    }


    fseek( file, 4, SEEK_SET );

    data_sect.u32 = byte_count;
    fwrite( data_sect.bytes, 1, 4, file );

    fclose( file );

    return BINARY_FILE_CREATED;
} /* int VectorTile::createBinaryFile ( char* file_path ) */

/*---------------------------------------------------------------*/

int VectorTile::readBinaryFile ( char* file_path ) {
    FILE* file = fopen( file_path, "rb" );

    union data_block data;

    fread( data.bytes, 1, 4, file );

    if ( strcmp(data.bytes, "DATA") != 0 ) {
        return BINARY_FILE_NOT_FOUND;
    }

    fread( data.bytes, 1, 4, file );
    uint32_t file_size = data.u32;

    fread( data.bytes, 1, 4, file );
    uint32_t n_polygons = data.u32;

    double x, y, z, n;
    Plane base;
    Polygon polygon;
    Vector point;

    uint32_t n_points;

    for ( uint32_t i = 0; i < n_polygons; i++ ) {
        fread( data.bytes, 1, 4, file );
        if ( data.u32 != i )
            return CORRUPT_BINARY_FILE;

        fread( data.bytes, 1, 4, file );
        if ( strcmp(data.bytes, "PLAN") != 0 )
            return CORRUPT_BINARY_FILE;

        fread( data.bytes, 1, 4, file );
        x = data.f32;

        fread( data.bytes, 1, 4, file );
        y = data.f32;

        fread( data.bytes, 1, 4, file );
        z = data.f32;

        fread( data.bytes, 1, 4, file );
        n = data.f32;

        base.createPlaneFromCoordinates( x, y, z, n );

        polygon.initPolygonWithPlane( base );

        fread( data.bytes, 1, 4, file );
        if ( strcmp(data.bytes, "PNTS") != 0 )
            return CORRUPT_BINARY_FILE;

        fread( data.bytes, 1, 4, file );
        n_points = data.u32;

        for ( uint32_t j = 0; j < n_points; j++ ) {
            fread( data.bytes, 1, 4, file );
            x = data.f32;

            fread( data.bytes, 1, 4, file );
            y = data.f32;

            fread( data.bytes, 1, 4, file );
            z = data.f32;

            point = Vector( x, y, z );

            polygon.addPoint( point );
        }

        polygons.push_back( polygon );
    }

    fclose( file );

    return READ_BINARY_FILE_SUCCESS;
} /* int readBinaryFile ( char* file_path ) */
