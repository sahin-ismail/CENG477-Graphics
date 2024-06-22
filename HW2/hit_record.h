#pragma once

#include "vector3f.h"
#include "triangle.h"
#include "sphere.h"
#include <string>

namespace fst
{
    struct HitRecord
    {
        math::Vector3f  normal;
        float           distance;
        int             material_id;
        math::Vector3f  point_intersection;
        int             id_of_text;
        const Triangle  *triangle_intersection;
        std::string     type_of_intersection;
        const Sphere    *sphere_intersection;
        double          G = -1.0;
        double          B = -1.0;

    };
}
