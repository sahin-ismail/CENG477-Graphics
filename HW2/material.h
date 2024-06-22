#pragma once
#include "vector2f.h"
#include "hit_record.h"
#include "vector3f.h"
#include "texture.h"
#include <vector>


namespace fst
{
    class Material
    {
    public:
        Material(const math::Vector3f& ambient, const math::Vector3f& diffuse, const math::Vector3f& specular, const math::Vector3f& mirror, float phong_exponent);

        math::Vector3f computeBrdf(const math::Vector3f& wi, const math::Vector3f& wo, const HitRecord &hR,const std::vector<Texture> &text_vector,const std::vector<math::Vector2f>  &coord_of_text) const;
        const math::Vector3f& get_ambient() const { return m_ambient; }
        const math::Vector3f& get_mirror() const { return m_mirror; }

    private:
        math::Vector3f m_ambient;
        math::Vector3f m_diffuse;
        math::Vector3f m_specular;
        math::Vector3f m_mirror;
        float m_phong_exponent;
    };
    fst::math::Vector3f start_transform(const std::vector<std::vector<double>> &vect_trans,fst::math::Vector3f v1);
}
