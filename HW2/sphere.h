#pragma once

#include <vector>
#include "vector3f.h"

namespace fst
{
    class Ray;
    struct HitRecord;

    class Sphere
    {
    public:
        Sphere(const math::Vector3f& center, float radius, int material_id);


        int id_of_text = -1;
        std::vector<std::vector<double>> all_transform;
        std::vector<std::vector<double>> all_transform_reverse;
        std::vector<std::vector<double>> all_rotate;
        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const;
        bool intersectShadowRay(const Ray& ray, float max_distance) const;
        math::Vector3f m_center;
        float m_radius;

    private:


        int m_material_id;

    };
}
