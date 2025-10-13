#include "vector_tile.h"

#include "../geometry/vector.h"
#include "../geometry/plane.h"
#include "../raw_data/surface.h"

VectorTile::VectorTile ( GmlFile& gmlfile ) {
    Vector p1, p2, p3;
    Plane base_plane;

    std::vector<Surface>& surfaces = gmlfile.getSurfaces();

    int len = surfaces.size();
    int len_pos_list;

    int err;

    for ( int i=0; i<len; i++ ) {
        p1 = surfaces[i].pos_list[0];
        p2 = surfaces[i].pos_list[1];
        p3 = surfaces[i].pos_list[2];

        base_plane.buildPlaneFromPoints( p1, p2, p3 );

        Polygon polygon;
        polygon.initPolygonWithPlane( base_plane );

        len_pos_list = surfaces[i].pos_list.size();
        for ( int j=0; j<len_pos_list; j++ ) {
            err = polygon.addPoint( surfaces[i].pos_list[j] );

            if ( err == 13 ) {
                double dist = polygon.getBasePlane().distanceOfPointToPlane( surfaces[i].pos_list[j] );
                //printf("Distance: %.30f\n", dist);
            }
        }

        polygons.push_back( polygon );
    }
}
