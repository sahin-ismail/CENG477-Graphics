#pragma once

#include "camera.h"
#include "material.h"
#include "mesh.h"
#include "parser.h"
#include "point_light.h"
#include "ray.h"
#include "sphere.h"
#include "vector2f.h"
#include "vector3f.h"
#include "texture.h"
#include <vector>

namespace fst
{
    struct Scene
    {
        std::vector<Camera> cameras;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<math::Vector3f> vertex_data;
        std::vector<math::Vector2f> tex_coord_data;
        std::vector<Mesh> meshes;
        std::vector<Sphere> spheres;
        math::Vector3f ambient_light;
        math::Vector3f background_color;
        int max_recursion_depth;
        float shadow_ray_epsilon;
        std::vector<Texture> textures;
        void loadFromParser(const parser::Scene& parser);
        bool intersect(const Ray& ray, HitRecord& hit_record, float max_distance) const;
        bool intersectShadowRay(const Ray& ray, float max_distance) const;
    };
}
