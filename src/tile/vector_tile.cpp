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

std::vector<uint>& VectorTile::getSectionStarts () {
    return section_starts;
} /* getSectionStarts () */

/*---------------------------------------------------------------*/

union data_block {
    uint8_t u8;
    uint32_t u32;

    float f32;
    double f64;

    char bytes [8];
};

int VectorTile::createBinaryFile ( std::string file_path ) {
    uint32_t byte_count = 0;
    int section_index = 0;

    union data_block data_sect;

    FILE* file = fopen( file_path.data(), "wb" );

    if ( !file ) {
        return FILE_NOT_CREATABLE;
    }

    fprintf( file, "DATA" );

    data_sect.u32 = 0;
    fwrite( data_sect.bytes, 1, 4, file );

    data_sect.f64 = lower_corner.getX();
    fwrite( data_sect.bytes, 1, 8, file );
    data_sect.f64 = lower_corner.getY();
    fwrite( data_sect.bytes, 1, 8, file );
    data_sect.f64 = lower_corner.getZ();
    fwrite( data_sect.bytes, 1, 8, file );

    data_sect.f64 = upper_corner.getX();
    fwrite( data_sect.bytes, 1, 8, file );
    data_sect.f64 = upper_corner.getY();
    fwrite( data_sect.bytes, 1, 8, file );
    data_sect.f64 = upper_corner.getZ();
    fwrite( data_sect.bytes, 1, 8, file );


    uint32_t len_polygons = (uint32_t) polygons.size();
    uint32_t len_point_list;
    data_sect.u32 = len_polygons;

    fwrite( data_sect.bytes, 1, 4, file );

    data_sect.f32 = error_rate;
    fwrite( data_sect.bytes, 1, 4, file );

    byte_count += 12;

    Plane p;
    Vector v;
    std::vector<Vector> points;
    Vector point;

    for ( uint32_t i = 0; i < len_polygons; i++ ) {
        if ( section_index < section_starts.size() && i == section_starts[section_index] ) {
            while ( section_index < section_starts.size() && i == section_starts[section_index++] ) {
                fprintf( file, "STRP" );
            }
        }
        fprintf( file, "PLGN" );

        data_sect.u32 = i;
        fwrite( data_sect.bytes, 1, 4, file );

        data_sect.u8 = (uint8_t) polygons[i].getSurfaceType();
        fwrite( data_sect.bytes, 1, 1, file );

        data_sect.u8 = (uint8_t) polygons[i].getID().length();
        fwrite( data_sect.bytes, 1, 1, file );

        fwrite( polygons[i].getID().data(), 1, data_sect.u8, file );

        data_sect.f64 = polygons[i].getArea();
        fwrite( data_sect.bytes, 1, 8, file );

        v = polygons[i].getCentroid();
        data_sect.f64 = v.getX();
        fwrite( data_sect.bytes, 1, 8, file );
        data_sect.f64 = v.getY();
        fwrite( data_sect.bytes, 1, 8, file );
        data_sect.f64 = v.getZ();
        fwrite( data_sect.bytes, 1, 8, file );


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

    fread( data.bytes, 1, 8, file );
    lower_corner.setX( data.f64 );
    fread( data.bytes, 1, 8, file );
    lower_corner.setY( data.f64 );
    fread( data.bytes, 1, 8, file );
    lower_corner.setZ( data.f64 );

    fread( data.bytes, 1, 8, file );
    upper_corner.setX( data.f64 );
    fread( data.bytes, 1, 8, file );
    upper_corner.setY( data.f64 );
    fread( data.bytes, 1, 8, file );
    upper_corner.setZ( data.f64 );


    fread( data.bytes, 1, 4, file );
    uint32_t n_polygons = data.u32;

    fread( data.bytes, 1, 4, file );
    error_rate = data.f32;

    double x, y, z, n;
    Plane base;
    Vector point;

    uint32_t n_points;
    int add_count = 0;

    for ( uint32_t i = 0; i < n_polygons; i++ ) {
        Polygon polygon;

        while ( true ) {
            fread( data.bytes, 1, 4, file );
            if ( STRNEQUAL(data.bytes, "STRP", 4) ) {
                section_starts.push_back( i );
            }
            else {
                break;
            }
        }

        //fread( data.bytes, 1, 4, file );
        if ( !STRNEQUAL(data.bytes, "PLGN", 4) ) {
            return FILE_CORRUPT;
        }

        fread( data.bytes, 1, 4, file );
        if ( data.u32 != i )
            return FILE_CORRUPT;

        fread( data.bytes, 1, 1, file );
        polygon.setSurfaceType( data.u8 );

        fread( data.bytes, 1, 1, file );
        char id [64];
        fread( id, 1, data.u8, file );
        polygon.setID( std::string(id) );

        fread( data.bytes, 1, 8, file );
        polygon.setArea( data.f64 );

        Vector centroid;
        fread( data.bytes, 1, 8, file );
        centroid.setX( data.f64 );
        fread( data.bytes, 1, 8, file );
        centroid.setY( data.f64 );
        fread( data.bytes, 1, 8, file );
        centroid.setZ( data.f64 );

        polygon.setCentroid( centroid );

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

bool isPolygonAlreadyInList (
    std::vector<Polygon>& polygon_list, Polygon& compare_polygon ) {

    uint len = polygon_list.size();
    if ( len == 0 ) {
        return false;
    }

    for ( uint i = 0; i < len; i++ ) {
        if ( polygon_list[i] != compare_polygon ) {
            return false;
        }
    }
    return true;
} /* isPolygonAlreadyInList() */

int VectorTile::fromGmlFile ( GmlFile& gmlfile ) {
    Vector p1, p2, p3;
    Vector dv1, dv2;

    Plane base_plane;

    std::vector<Surface>& surfaces = gmlfile.getSurfaces();

    lower_corner = gmlfile.getLowerCorner();
    upper_corner = gmlfile.getUpperCorner();

    uint len = surfaces.size();
    uint len_pos_list;

    Vector
        pos,
        corr_vec,
        corr_point;

    Line corr_line;

    int yes = 0, no = 0;

    double dist;

    int add_count = 0;

    for ( uint i = 0; i < len; i++ ) {
        if ( surfaces[i].area < 1.0 ) {
            no++;
            continue;
        }

        bool point_too_far_away = false;

        // Select the first point of the point list as the
        // base point of the polygon
        p1 = surfaces[i].pos_list[0];

        uint
            p2_index,
            p3_index;


        len_pos_list = surfaces[i].pos_list.size();

        double length;
        double max_value = 0.0;


        // For the second point choose the point from the list
        // which is furthest away from the base point
        for ( uint j = 1; j < len_pos_list; j++ ) {
            p2 = surfaces[i].pos_list[j];
            length = (p2 - p1).length();

            if ( length > max_value ) {
                max_value = length;
                p2_index = j;
            }
        }
        p2 = surfaces[i].pos_list[p2_index];

        max_value = 0.0;

        // For the third point choose the point from the list
        // for which the cross product P1P2 x P1P3 is the biggest
        // while skipping the already chosen second point in the list
        // By that we find two direction vectors that span the largest
        // possible area
        for ( uint j = 1; j < len_pos_list; j++ ) {
            if ( j != p2_index ) {
                p3 = surfaces[i].pos_list[j];
                length = (p2 - p1).crossProduct( p3 - p1 ).length();

                if ( length > max_value ) {
                    max_value = length;
                    p3_index = j;
                }
            }
        }
        p3 = surfaces[i].pos_list[p3_index];


        base_plane.createPlaneFromPoints( p1, p2, p3 );

        Polygon polygon;
        polygon.setSurfaceType( surfaces[i].surface_type );
        polygon.setID( surfaces[i].id );
        polygon.setArea( surfaces[i].area );
        polygon.initPolygonWithPlane( base_plane );

        len_pos_list = surfaces[i].pos_list.size();

        // Add all points to the Polygon object
        for ( uint j = 0; j < len_pos_list; j++ ) {
            pos = surfaces[i].pos_list[j];
            dist = base_plane.distanceOfPointToPlane(pos);

            // If the point's distance to the base plane is within
            // a given tolerance, pull it into the base plane along
            // the base plane's normal vector
            if ( !base_plane.isPointOnPlane(pos) ) {

                if ( dist < PLANE_DISTANCE_THRESHOLD ) {
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


        // If all the distances of the points to the base plane were
        // within the tolerance and if the Polygon object contains at
        // least three points, add it to the tile's list of polygons
        if ( !point_too_far_away && polygon.getPoints().size() >= 3 ) {
            polygon.getCentroid();
            polygons.push_back( polygon );

            add_count++;
            yes++;
        }
        else {
            no++;
        }
    } /* for ( int i=0; i<len; i++ ) */

    // Calculate how many Polygon objects could not be created
    // successfully for statistical and debugging purposes
    error_rate = (double)no / (double)( yes + no );

    orderPolygonsInStripes( 20 );

    return SUCCESS;
} /* fromGmlFile() */

/*---------------------------------------------------------------*/

int VectorTile::partition ( int start, int end, bool by_x ) {
    Polygon& pivot = polygons[end];
    int i = start - 1;

    for ( int j = start; j < end; j++ ) {
        if ( by_x ) {
            if ( polygons[j].getCentroid().getX() < pivot.getCentroid().getX() ) {
                i++;

                Polygon tmp = polygons[j];
                polygons[j] = polygons[i];
                polygons[i] = tmp;
            }
        }
        else {
            if ( polygons[j].getCentroid().getY() < pivot.getCentroid().getY() ) {
                i++;

                Polygon tmp = polygons[j];
                polygons[j] = polygons[i];
                polygons[i] = tmp;
            }
        }
    }

    Polygon tmp = polygons[end];
    polygons[end] = polygons[i+1];
    polygons[i+1] = tmp;

    return i + 1;
} /* partition () */


void VectorTile::sortPolygons ( int start, int end, bool by_x ) {
    if ( start < end ) {
        int pivot = partition( start, end, by_x );

        sortPolygons( start, pivot-1, by_x );
        sortPolygons( pivot+1, end, by_x );
    }
} /* sortPolygons () */


void VectorTile::orderPolygonsInStripes ( int n_stripes ) {
    // Sort the polygons by the y coordinate of their centroid
    sortPolygons( 0, polygons.size()-1, false );

    double stripe_width =
        ( upper_corner.getY() - lower_corner.getY() ) / (double)n_stripes;

    section_starts.push_back( 0 );

    int* stripe_counts = new int [n_stripes];
    for ( int i = 0; i < n_stripes; i++ ) {
        stripe_counts[i] = 0;
    }

    uint len_polygons = polygons.size();
    for ( uint i = 0; i < len_polygons; i++ ) {
        int idx = (int)( ( polygons[i].getCentroid().getY() - lower_corner.getY() ) / stripe_width );
        stripe_counts[idx]++;
    }

    for ( uint i = 1; i < n_stripes; i++ ) {
        section_starts.push_back( stripe_counts[i-1] + section_starts[i-1] );
    }

    delete[] stripe_counts;

    uint len_sections = section_starts.size();
    for ( uint i = 0; i < len_sections-1; i++ ) {
        sortPolygons( section_starts[i], section_starts[i+1]-1, true );
    }
    sortPolygons( section_starts[len_sections-1], polygons.size()-1, true );
} /* orderPolygonsInStripes () */
