#include <iostream>
#include "material.h"
#include "vector3f.h"
#include "jpeg.h"
#include <cmath>
#include "parser.h"
#include "vector2f.h"

#define PI 3.14159265

namespace fst
{
    Material::Material(const math::Vector3f& ambient, const math::Vector3f& diffuse, const math::Vector3f& specular, const math::Vector3f& mirror, float phong_exponent)
        : m_ambient(ambient)
        , m_diffuse(diffuse)
        , m_specular(specular)
        , m_mirror(mirror)
        , m_phong_exponent(phong_exponent)
    {}

    math::Vector3f Material::computeBrdf(const math::Vector3f& wi, const math::Vector3f& wo, const HitRecord &hR,const std::vector<Texture> &text_vector,const std::vector<math::Vector2f>  &coord_of_text) const
    {
        auto diffuse = math::max(math::dot(hR.normal, wi), 0.0f);
        auto specular = std::pow(math::max(math::dot(math::normalize(wo + wi), hR.normal), 0.0f),
                                 m_phong_exponent);
        if( (hR.triangle_intersection != NULL && hR.triangle_intersection->id > 0) || (hR.sphere_intersection != NULL && hR.sphere_intersection->id_of_text > 0)) {
            double value_of_U;
            double value_of_V;
            math::Vector3f color_of_Texture;
            Texture texture_present;
            if(hR.type_of_intersection == "triangle") {
                const Triangle &triangle_intersection = *(hR.triangle_intersection);
                texture_present  = text_vector[hR.triangle_intersection->id - 1];
                value_of_U = triangle_intersection.triangle_Texture[0].x + (triangle_intersection.triangle_Texture[1].x - triangle_intersection.triangle_Texture[0].x) * hR.B +(triangle_intersection.triangle_Texture[2].x - triangle_intersection.triangle_Texture[0].x) * hR.G;
                value_of_V = triangle_intersection.triangle_Texture[0].y + (triangle_intersection.triangle_Texture[1].y - triangle_intersection.triangle_Texture[0].y) * hR.B +(triangle_intersection.triangle_Texture[2].y- triangle_intersection.triangle_Texture[0].y) * hR.G;
            }
            if(hR.type_of_intersection != "triangle") {
                math::Vector3f norm_Point = hR.point_intersection-hR.sphere_intersection->m_center;
                if(hR.sphere_intersection->all_rotate.capacity() > 0) {
                    norm_Point = start_transform(hR.sphere_intersection->all_rotate,norm_Point);
                }
                texture_present = text_vector[hR.sphere_intersection->id_of_text-1];
                value_of_V = (double)acos(norm_Point.y/hR.sphere_intersection->m_radius) / PI;
                value_of_U = (-1*(double)atan2(norm_Point.z,norm_Point.x) + PI)/(2*PI);

            }
            if(texture_present.m_appearance == "clamp") {
                if(value_of_U > 1.0)
                    value_of_U = 1;
                else if(value_of_U < 0.0)
                    value_of_U = 0;
                if(value_of_V > 1.0)
                    value_of_V =1.0;
                else if(value_of_V < 0.0)
                    value_of_V = 0.0;
            }
            if(texture_present.m_appearance != "clamp") {
                value_of_U -= std::floor(value_of_U);
                value_of_V -= std::floor(value_of_V);
            }
            int U_pixel_of_near = (int)(std::floor(value_of_U * texture_present.m_width));
            int V_pixel_of_near = (int)(std::floor(value_of_V * texture_present.m_height));
            if(texture_present.m_interpolation == "bilinear") {

                int fst_color_pos = 3*(int)(V_pixel_of_near * texture_present.m_width + U_pixel_of_near);
                double d_y      = value_of_V*texture_present.m_height - V_pixel_of_near;
                double d_x      = value_of_U*texture_present.m_width-U_pixel_of_near;
                math::Vector3f coloor1 = math::Vector3f(texture_present.m_image[fst_color_pos],texture_present.m_image[fst_color_pos+1],texture_present.m_image[fst_color_pos+2])*(1-d_x)*(1-d_y);
                U_pixel_of_near = U_pixel_of_near+1;
                fst_color_pos   = 3*(int)(V_pixel_of_near * texture_present.m_width + U_pixel_of_near);
                math::Vector3f coloor2 = math::Vector3f((texture_present.m_image[fst_color_pos]),  (texture_present.m_image[fst_color_pos+1]),  (texture_present.m_image[fst_color_pos+2]))*(d_x)*(1-d_y);
                U_pixel_of_near = U_pixel_of_near-1;
                V_pixel_of_near = V_pixel_of_near+1;
                fst_color_pos   = 3*(int)(V_pixel_of_near * texture_present.m_width + U_pixel_of_near);
                math::Vector3f coloor3 = math::Vector3f(texture_present.m_image[fst_color_pos],texture_present.m_image[fst_color_pos+1],texture_present.m_image[fst_color_pos+2])*(1-d_x)*(d_y);
                U_pixel_of_near = U_pixel_of_near+1;
                fst_color_pos   = 3*(int)(V_pixel_of_near * texture_present.m_width + U_pixel_of_near);
                math::Vector3f coloor4 = math::Vector3f(texture_present.m_image[fst_color_pos],texture_present.m_image[fst_color_pos+1],texture_present.m_image[fst_color_pos+2])*(d_x)*(d_y);
                color_of_Texture = coloor1 + coloor2 + coloor3 + coloor4;
            }
            if(texture_present.m_interpolation != "bilinear") {
                int fst_color_pos =  3*(int)(V_pixel_of_near * texture_present.m_width + U_pixel_of_near);
                color_of_Texture = math::Vector3f(texture_present.m_image[fst_color_pos],texture_present.m_image[fst_color_pos+1],texture_present.m_image[fst_color_pos+2]);
            }
            math::Vector3f last_color;
            if(texture_present.m_decalMode  == "replace_kd") {
                 last_color = (color_of_Texture/255)*diffuse + m_specular * specular;

            }
            if(texture_present.m_decalMode == "replace_all") {
                last_color = color_of_Texture + m_specular * specular ;
            }
            if(texture_present.m_decalMode != "replace_all" && texture_present.m_decalMode  != "replace_kd") {
                last_color = ((color_of_Texture/255+m_diffuse)/2)*diffuse + m_specular * specular ;
            }

            return last_color;
        }

        else {
                return m_diffuse * diffuse +  m_specular * specular ;
        }
    }
    fst::math::Vector3f start_transform(const std::vector<std::vector<double>> &vect_trans,fst::math::Vector3f v1) {
        fst::math::Vector3f final_vect = fst::math::Vector3f(v1.x,v1.y,v1.z);
        double z_value = final_vect.x * vect_trans[2][0] + final_vect.y * vect_trans[2][1] + final_vect.z * vect_trans[2][2] + vect_trans[2][3];
        double y_value = final_vect.x * vect_trans[1][0] + final_vect.y * vect_trans[1][1] + final_vect.z * vect_trans[1][2] + vect_trans[1][3];
        double x_value = final_vect.x * vect_trans[0][0] + final_vect.y * vect_trans[0][1] + final_vect.z * vect_trans[0][2] + vect_trans[0][3];
        final_vect.z = z_value;
        final_vect.y = y_value;
        final_vect.x = x_value;
        return final_vect;
    }
}
