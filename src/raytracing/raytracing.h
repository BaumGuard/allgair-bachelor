#ifndef RAYTRACING_H
#define RAYTRACING_H

struct RaytracingResult {
    uint ground_count,
    uint vegetation_count
};

int performRaytracing ( Vector& start_point, std::vector<Vector> end_points, int select_method, double max_point_to_plane_distance = 0.1) {
    Polygon selected_polygon = precalculate( start_point, end_points, select_method );

    Vector reflect_point = selected_polygon.getCentroid();

    int
        count1, count2,
        ground_count1, ground_count2,
        veg_count1, veg_count2;

    Field grid_field;
    Vector intersection;


    int status;
    status = field.bresenhamPseudo3D( intersection, start_point, reflect_point, 1.0, &count1, DGM1 )
}

#endif
