#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include "vector2f.h"

using namespace std;

namespace parser
{

    struct Vec3f
    {
        Vec3f();
        Vec3f(double x, double y, double z) {
            this->x = x;
            this->y = y;
            this->z= z;
        }
        double x, y, z;
        Vec3f operator+(Vec3f &rhs);
        Vec3f operator*(double scalar);
        double operator*(Vec3f &rhs);
        Vec3f operator-(Vec3f &rhs);
    };

    struct Vec3i
    {
        Vec3i() {}
        Vec3i operator+(Vec3i &rhs);
        Vec3i(int,int,int);
        int x, y, z;
    };

    struct Vec4f
    {
        double x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        double near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        PointLight() {}
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        double phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
        int old_v0;
        int old_v1;
        int old_v2;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
        int texture_id = -1;
        std::vector<string> transformations;
        vector<vector<double>> all_transform;
        vector<vector<double>> all_transform_reverse;
        Mesh() {
            for(int i = 0; i < 4;i++) {
                vector<double> temp(4);
                all_transform_reverse.push_back(temp);
            }
        };
    };

    struct Triangle
    {
        int texture_id = -1;
        int material_id;
        Face indices;
        std::vector<string> transformations;
        vector<vector<double>> all_transform;
        vector<vector<double>> all_transform_reverse;
        Triangle() {
            for(int i = 0; i < 4;i++) {
                vector<double> temp(4);
                all_transform_reverse.push_back(temp);
            }
        };
    };

    struct Sphere
    {
        Sphere() {
            for(int i = 0; i < 4;i++) {
                vector<double> temp(4);
                all_transform_reverse.push_back(temp);
            }
        };
        vector<vector<double>> all_transform;
        vector<vector<double>> all_transform_reverse;
        vector<vector<double>> all_rotate = vector<vector<double>>(0);
        vector<vector<double>> combinedScalings;
        vector<vector<double>> combinedTranslations;
        int material_id;
        int center_vertex_id;
        int old_vertex;
        double radius;
        int texture_id = -1;
        std::vector<string> transformations;
        void openVector(vector<vector<double>> &v) {
            for(int i = 0; i < 4;i++) {
                vector<double> temp(4);
                v.push_back(temp);
            }
        }
    };

    class Transformation {
    public:
        int transform_id;
        vector<vector<double>> transform_matrix;
        Transformation() {
            for(int i = 0; i < 4;i++) {
                vector<double> temp(4);
                transform_matrix.push_back(temp);
            }

        }
        void setTransformationId(int id) {
            this->transform_id = id;
        }
        void setTransformationMatrix(double matrix[4][4]) {
            for(int i = 0; i < 4;i++) {
                for(int j = 0; j < 4; j++) {
                    this->transform_matrix[i][j] = matrix[i][j];
                }
            }
        }
    };


    class Translation : public Transformation {
    public:
        Translation() {
            Transformation();
        }
    };


    struct Scaling : public Transformation {
    public:
        Scaling() {
            Transformation();
        }
    };


    struct Rotation : public Transformation {
    public:
        Rotation() {
            Transformation();
        }
    };


    struct Texture {
        string ImageName;
        string Interpolation;
        string DecalMode;
        string Appearance;
    };


    struct Scene
    {
        Vec3i background_color;
        double shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Transformation> translations;
        std::vector<Transformation> scalings;
        std::vector<Transformation> rotations;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;
        vector<Texture> textures;
        std::vector<fst::math::Vector2f> tex_coord_data;

        void loadFromXml(const std::string& filepath);
        Scene() {
        }
    };
}




using namespace parser;
parser::Vec3f crossProduct(parser::Vec3f &vector_1, parser::Vec3f &vector_2);
vector< vector<double> > computeTransformationMatrix(std::vector<string> &transformations,std::vector<Transformation> &translation,std::vector<Transformation> &scaling,std::vector<Transformation> &rotation);
void matrixMultiplication(double firstMatrix[4][4], double secondMatrix[4][4],double resultMatrix[4][4]);
vector< vector<double> > matrixMultiplication(vector< vector<double> > &first,vector< vector<double> > &second);
void matrixMultiplication(vector<vector<double>> &firstMatrix, double secondMatrix[4][4],double resultMatrix[4][4]);
parser::Vec3f start_transform(std::vector<std::vector<double>> &vect_trans,parser::Vec3f v1);
parser::Vec3f normalizeVector(parser::Vec3f &ray);
void copyMatrixVector(vector<vector<double>> &first,double second[4][4]);
void copyMatrix(double firstMatrix[4][4], double secondMatrix[4][4]);
void copyMatrixVector(double first[4][4],vector<vector<double>> &second);
void twoDimentionToOne(vector<vector<double>> &first, double second[]);
void oneDimentionToTwo(double first[],vector<vector<double>> &second);

#endif
