#include "sphere.h"
#include "hit_record.h"
#include "ray.h"

namespace fst
{
    Sphere::Sphere(const math::Vector3f& center, float radius, int material_id)
        : m_center(center)
        , m_radius(radius)
        , m_material_id(material_id)
    {}

    bool Sphere::intersect(const Ray& ray, HitRecord& ht, float maxD) const
    {
        auto e = m_center - ray.get_origin();
        auto a = math::dot(e, ray.get_direction());
        auto x = m_radius * m_radius + a * a - math::dot(e, e);
        if (x < 0.0f)
        {
            return false;
        }
        bool in = false;
        auto dst = a - sqrtf(x);
        auto d_2 = a + sqrtf(x);
        auto r_d = -1.0f;

        if(d_2 > dst) {
            if(dst < 0.0) {
                r_d = d_2;
                in = true;
            }
            if(dst > 0.0) {
                r_d = dst;
            }
        }
         if(dst > d_2) {
            if(d_2 < 0.0) {
                in = true;
                r_d = dst;
            }
            if(d_2 > 0.0) {
                r_d = d_2;
            }
        }
        if(dst == d_2)
            r_d = dst;



        if (r_d > 0.0f && r_d < maxD)
        {

            ht.id_of_text = this->id_of_text;
            ht.distance = r_d;
            ht.type_of_intersection = "sphere";
            ht.triangle_intersection = NULL;
            ht.point_intersection = ray.get_origin() + ray.get_direction()*r_d;
            ht.sphere_intersection = this;
            if(!in) {
                ht.normal = math::normalize(ray.getPoint(ht.distance) - m_center);
            }
            else {
                ht.normal = math::normalize(m_center - ray.getPoint(ht.distance));
            }
            ht.material_id = m_material_id;

            return true;
        }
        else
        return false;
    }

    bool Sphere::intersectShadowRay(const Ray& ray, float maxD) const
    {
        auto e = m_center - ray.get_origin();
        auto a = math::dot(e, ray.get_direction());
        auto x = m_radius * m_radius + a * a - math::dot(e, e);

        if (x < 0.0f)
        {
            return false;
        }

        auto dst = a - sqrtf(x);
        auto d_2 = a + sqrtf(x);
        auto r_d = -1.0f;

        if(d_2 > dst) {
            if(dst < 0.0) {
                r_d = d_2;
            }
            if(dst > 0.0) {
                r_d = dst;
            }
        }
        if(dst > d_2) {
            if(d_2 < 0.0) {
                r_d = dst;
            }
            if(d_2 > 0.0) {
                r_d = d_2;
            }
        }
        if(dst == d_2)
            r_d = dst;

        bool rt =r_d > 0.0f && r_d < maxD;
        return rt;
    }
}
