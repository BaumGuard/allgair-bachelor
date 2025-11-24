#include "vector_tile.h"

#include "../geometry/vector.h"
#include "../geometry/line.h"
#include "../geometry/plane.h"
#include "../raw_data/surface.h"
#include "../utils.h"

#include <cstdint>
#include <iostream>

/*---------------------------------------------------------------*/

Vector VectorTile::getLowerCorner () const {
    return lower_corner;
} /* getLowerCorner() */

/*---------------------------------------------------------------*/

Vector VectorTile::getUpperCorner () const {
    return upper_corner;
} /* getUpperCorner() */

/*---------------------------------------------------------------*/

std::vector<Polygon>& VectorTile::getPolygons () {
    return polygons;
} /* getPolygons() */

/*---------------------------------------------------------------*/

union data_block {
    uint32_t u32;
    double f64;

    char bytes [8];
};

int VectorTile::createBinaryFile ( std::string file_path ) {
    uint32_t byte_count = 0;

    union data_block data_sect;

    FILE* file = fopen( file_path.data(), "wb" );

    if ( !file ) {
        return FILE_NOT_CREATABLE;
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

        data_sect.u32 = polygons[i].getSurfaceType();
        fwrite( data_sect.bytes, 1, 4, file );

        fprintf( file, "PLAN" );

        p = polygons[i].getBasePlane();

        data_sect.f64 = p.getX();
        fwrite( data_sect.bytes, 1, 8, file );

        data_sect.f64 = p.getY();
        fwrite( data_sect.bytes, 1, 8, file );

        data_sect.f64 = p.getZ();
        fwrite( data_sect.bytes, 1, 8, file );

        data_sect.f64 = p.getN();
        fwrite( data_sect.bytes, 1, 8, file );

        fprintf( file, "PNTS" );

        points = polygons[i].getPoints();
        len_point_list = points.size();

        data_sect.u32 = len_point_list;
        fwrite( data_sect.bytes, 1, 4, file );

        byte_count += 36;

        for ( uint32_t j = 0; j < len_point_list; j++ ) {
            point = points[j];

            data_sect.f64 = point.getX();
            fwrite( data_sect.bytes, 1, 8, file );
            //printf("%f\n", data_sect.f64);

            data_sect.f64 = point.getY();
            fwrite( data_sect.bytes, 1, 8, file );

            data_sect.f64 = point.getZ();
            fwrite( data_sect.bytes, 1, 8, file );

            byte_count += 12;
        }
    }


    fseek( file, 4, SEEK_SET );

    data_sect.u32 = byte_count;
    fwrite( data_sect.bytes, 1, 4, file );

    fclose( file );

    return SUCCESS;
} /* createBinaryFile() */

/*---------------------------------------------------------------*/

int VectorTile::fromBinaryFile ( std::string file_path ) {
    FILE* file = fopen( file_path.data(), "rb" );
    if ( !file ) {
        return FILE_NOT_FOUND;
    }

    union data_block data;

    fread( data.bytes, 1, 4, file );

    if ( !STRNEQUAL(data.bytes, "DATA", 4) ) {
        return FILE_CORRUPT;
    }

    fread( data.bytes, 1, 4, file );
    //uint32_t file_size = data.u32;

    fread( data.bytes, 1, 4, file );
    uint32_t n_polygons = data.u32;

    double x, y, z, n;
    Plane base;
    Vector point;

    uint32_t n_points;
    int add_count = 0;

    for ( uint32_t i = 0; i < n_polygons; i++ ) {
        Polygon polygon;


        fread( data.bytes, 1, 4, file );
        if ( !STRNEQUAL(data.bytes, "PLGN", 4) ) {
            return FILE_CORRUPT;
        }

        fread( data.bytes, 1, 4, file );
        if ( data.u32 != i )
            return FILE_CORRUPT;

        fread( data.bytes, 1, 4, file );
        polygon.setSurfaceType( data.u32 );

        fread( data.bytes, 1, 4, file );
        if ( !STRNEQUAL(data.bytes, "PLAN", 4) )
            return FILE_CORRUPT;


        fread( data.bytes, 1, 8, file );
        x = data.f64;

        fread( data.bytes, 1, 8, file );
        y = data.f64;

        fread( data.bytes, 1, 8, file );
        z = data.f64;

        fread( data.bytes, 1, 8, file );
        n = data.f64;


        base.createPlaneFromCoordinates( x, y, z, n );

        polygon.initPolygonWithPlane( base );

        fread( data.bytes, 1, 4, file );
        if ( !STRNEQUAL(data.bytes, "PNTS", 4) )
            return FILE_CORRUPT;

        fread( data.bytes, 1, 4, file );
        n_points = data.u32;


        for ( uint32_t j = 0; j < n_points; j++ ) {
            fread( data.bytes, 1, 8, file );
            x = data.f64;

            fread( data.bytes, 1, 8, file );
            y = data.f64;

            fread( data.bytes, 1, 8, file );
            z = data.f64;


            point = Vector( x, y, z );

            polygon.addPoint( point );
        }


        add_count++;
        polygons.push_back( polygon );
    }

    fclose( file );

    return SUCCESS;
} /* fromBinaryFile() */

/*---------------------------------------------------------------*/

int VectorTile::fromGmlFile ( GmlFile& gmlfile, double* success_rate ) {
    Vector p1, p2, p3;
    Vector dv1, dv2;

    Plane base_plane;

    std::vector<Surface>& surfaces = gmlfile.getSurfaces();

    int len = surfaces.size();
    int len_pos_list;

    Vector
        pos,
        subtrahend = gmlfile.getLowerCorner(),
        corr_vec,
        corr_point;

    Line corr_line;

    int yes = 0, no = 0;

    subtrahend.setZ( 0.0 );

    double dist;

    int add_count = 0;

    for ( int i=0; i<len; i++ ) {

        bool point_too_far_away = false;

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


        for ( int k=1; k<len_pos_list; k++ ) {
            if ( k == p2_index ) {
                continue;
            }
            p3 = surfaces[i].pos_list[k];

            dv2 = p3 - p1;

            if ( !dv1.linearDependant(dv2) && dv2.length() > 0.05 ) {
                break;
            }
        }


        base_plane.createPlaneFromPoints( p1, p2, p3 );

        Polygon polygon;
        polygon.setSurfaceType( surfaces[i].surface_type );
        polygon.initPolygonWithPlane( base_plane );

        len_pos_list = surfaces[i].pos_list.size();

        for ( int j=0; j<len_pos_list; j++ ) {
            pos = surfaces[i].pos_list[j];

            if ( !base_plane.isPointOnPlane(pos) ) {

                dist = base_plane.distanceOfPointToPlane(pos);

                if ( dist != 0.0 /*< PLANE_DISTANCE_THRESHOLD*/ ) {
                    corr_vec = base_plane.normalVector();
                    corr_line.createLineFromBaseAndVector( pos, corr_vec );

                    base_plane.lineIntersection( corr_line, corr_point );

                    polygon.addPoint( corr_point );
                }
                else {
                    point_too_far_away = true;
                    break;
                }
            } /* if ( !base_plane.isPointOnPlane(pos) ) */
            else {
               polygon.addPoint( pos );
            }

        } /* for ( int j=0; j<len_pos_list; j++ ) */


        if ( !point_too_far_away ) {
            add_count++;
            polygons.push_back( polygon );

            yes++;
        }
        else {
            no++;
        }
    } /* for ( int i=0; i<len; i++ ) */
    printf("Success rate : %f\n", (double)yes / (double)(yes+no));
    if ( success_rate != nullptr ) {
        *success_rate = (double)yes / (double)( yes + no );
    }
    return SUCCESS;
} /* fromGmlFile() */
