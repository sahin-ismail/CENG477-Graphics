#pragma once


#include "triangular.h"
#include "vector3f.h"
#include <vector>
#include "vector2f.h"

namespace fst
{
    class Ray;
    struct HitRecord;

    class Triangle : public Triangular
    {
    public:
        Triangle(const math::Vector3f& v0, const math::Vector3f& edge1, const math::Vector3f& edge2);

        int id = -1;
        std::vector<std::vector<double>> all_transform;
        std::vector<std::vector<double>> all_transform_reverse;
        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const override;
        bool intersectShadowRay(const Ray& ray, float max_distance) const override;

        std::vector<math::Vector2f> triangle_Texture;

    private:
        math::Vector3f m_v0, m_edge1, m_edge2;
        math::Vector3f m_normal;
    };
}
