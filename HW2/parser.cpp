#include "parser.h"
#include "tinyxml2.h"
#include "helper.h"
#include <iostream>
#include <algorithm>
#include "matrixInverse.h"
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <cmath>

using namespace parser;
using namespace std;

#define PI 3.14159265

void parser::Scene::loadFromXml(const std::string& filepath)
{
    tinyxml2::XMLDocument file;
    std::stringstream stream;

    auto res = file.LoadFile(filepath.c_str());
    if (res)
    {
        throw std::runtime_error("Error: The xml file cannot be loaded.");
    }

    auto root = file.FirstChild();
    if (!root)
    {
        throw std::runtime_error("Error: Root is not found.");
    }

    //Get BackgroundColor
    auto element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> background_color.x >> background_color.y >> background_color.z;

    //Get ShadowRayEpsilon
    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> shadow_ray_epsilon;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> max_recursion_depth;

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        stream >> camera.near_plane.x >> camera.near_plane.y >> camera.near_plane.z >> camera.near_plane.w;
        stream >> camera.near_distance;
        stream >> camera.image_width >> camera.image_height;
        stream >> camera.image_name;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }

    //Get Lights
    stream.clear();
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambient_light.x >> ambient_light.y >> ambient_light.z;
    element = element->FirstChildElement("PointLight");
    PointLight point_light;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> point_light.position.x >> point_light.position.y >> point_light.position.z;
        stream >> point_light.intensity.x >> point_light.intensity.y >> point_light.intensity.z;

        point_lights.push_back(point_light);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;
        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.phong_exponent;
        child = element->FirstChildElement("MirrorReflectance");
        if(child) {
            stream << child->GetText() << std::endl;
            stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
        }
        else {
            material.mirror.x = 0;
            material.mirror.y = 0;
            material.mirror.z = 0;
        }


        materials.push_back(material);
        element = element->NextSiblingElement("Material");
        stream.clear();
    }

    //Get Textures
    element = root->FirstChildElement("Textures");
    if(element) {

        element = element->FirstChildElement("Texture");
        Texture texture;
        while(element) {
            child = element->FirstChildElement("ImageName");
            stream << child->GetText() << endl;
            child = element->FirstChildElement("Interpolation");
            stream << child->GetText() << endl;
            child = element->FirstChildElement("DecalMode");
            stream << child->GetText() << endl;
            child = element->FirstChildElement("Appearance");
            stream << child->GetText() << endl;


            stream >> texture.ImageName >> texture.Interpolation >> texture.DecalMode >> texture.Appearance;
            this->textures.push_back(texture);
            element = element->NextSiblingElement("Texture");
            stream.clear();

        }
    }


    //Get Transformations
    element = root->FirstChildElement("Transformations");
    if(element)
    {
        element = element->FirstChildElement("Translation");
        Translation trans = Translation();
        while(element)
        {
            stream << element->GetText() << std::endl;
            stream >> trans.transform_matrix[0][3] >> trans.transform_matrix[1][3] >> trans.transform_matrix[2][3];
            trans.transform_matrix[0][0] = 1;
            trans.transform_matrix[1][1] = 1;
            trans.transform_matrix[2][2] = 1;
            trans.transform_matrix[3][3] = 1;
            trans.transform_id = translations.size()+1;
            translations.push_back(trans);
            stream.clear();
            element = element->NextSiblingElement("Translation");
        }
        element = root->FirstChildElement("Transformations");
        element = element->FirstChildElement("Scaling");
        Scaling scale = Scaling();
        while(element)
        {
            stream << element->GetText() << std::endl;
            stream >> scale.transform_matrix[0][0] >> scale.transform_matrix[1][1] >> scale.transform_matrix[2][2];
            scale.transform_matrix[3][3] = 1;
            scale.transform_id = scalings.size()+1;
            scalings.push_back(scale);
            stream.clear();
            element = element->NextSiblingElement("Scaling");
        }

        element = root->FirstChildElement("Transformations");
        element = element->FirstChildElement("Rotation");
        Rotation rotation = Rotation();
        while(element)
        {
            stream << element->GetText() << std::endl;
            double rotationAngle;
            stream >> rotationAngle;
            Vec3f rotationAxisVector;
            stream >> rotationAxisVector.x >> rotationAxisVector.y >> rotationAxisVector.z;
            Vec3f normalizedUAxis = normalizeVector(rotationAxisVector);
            if(normalizedUAxis.y == 0.0 and normalizedUAxis.z == 0 ) {
                double rotationMatrix[4][4] = {
                        {
                                1, 0, 0, 0
                        },
                        {
                                0, cos(rotationAngle*PI/180), -1*sin(rotationAngle*PI/180), 0
                        },
                        {
                                0, sin(rotationAngle*PI/180), cos(rotationAngle*PI/180), 0
                        },
                        {
                                0, 0, 0, 1
                        }
                };
                rotation.setTransformationMatrix(rotationMatrix);
            }
            else if(normalizedUAxis.x == 0.0 and normalizedUAxis.z == 0.0) {
                double rotationMatrix[4][4] = {
                        {
                                cos(rotationAngle*PI/180), 0, sin(rotationAngle*PI/180), 0
                        },
                        {
                                0, 1, 0, 0
                        },
                        {
                                -1*sin(rotationAngle*PI/180), 0, cos(rotationAngle*PI/180), 0
                        },
                        {
                                0, 0, 0, 1
                        }
                };
                rotation.setTransformationMatrix(rotationMatrix);
            }
            else if(normalizedUAxis.x == 0.0 and normalizedUAxis.y == 0.0) {
                double rotationMatrix[4][4] = {
                        {
                                cos(rotationAngle*PI/180), -1*sin(rotationAngle*PI/180), 0, 0
                        },
                        {
                                sin(rotationAngle*PI/180), cos(rotationAngle*PI/180), 0, 0
                        },
                        {
                                0, 0, 1, 0
                        },
                        {
                                0, 0, 0, 1
                        }
                };
                rotation.setTransformationMatrix(rotationMatrix);
            }
            else {
                Vec3f normalizedVAxis = Vec3f(-1 * normalizedUAxis.y, normalizedUAxis.x, 0);
                Vec3f normalizedWAxis = crossProduct(normalizedUAxis, normalizedVAxis);

                double matrix_M[4][4] = {
                        {
                                normalizedUAxis.x, normalizedUAxis.y, normalizedUAxis.z, 0
                        },
                        {
                                normalizedVAxis.x, normalizedVAxis.y, normalizedVAxis.z, 0
                        },
                        {
                                normalizedWAxis.x, normalizedWAxis.y, normalizedWAxis.z, 0
                        },
                        {
                                0,                 0,                 0,                 1
                        }
                };

                double rotationMatrix[4][4] = {
                        {
                                1, 0,                             0,                                  0
                        },
                        {
                                0, cos(rotationAngle * PI / 180), -1 * sin(rotationAngle * PI / 180), 0
                        },
                        {
                                0, sin(rotationAngle * PI / 180), cos(rotationAngle * PI / 180),      0
                        },
                        {
                                0, 0,                             0,                                  1
                        }
                };

                double inverse_M[4][4] = {
                        {
                                normalizedUAxis.x, normalizedVAxis.x, normalizedWAxis.x, 0
                        },
                        {
                                normalizedUAxis.y, normalizedVAxis.y, normalizedWAxis.y, 0
                        },
                        {
                                normalizedUAxis.z, normalizedVAxis.z, normalizedWAxis.z, 0
                        },
                        {
                                0,                 0,                 0,                 1
                        }
                };
                double middleResultMatrix[4][4] = {
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 1
                        }
                };
                double finalResult[4][4] = {
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 0
                        },
                        {
                                0, 0, 0, 1
                        }
                };

                matrixMultiplication(inverse_M, rotationMatrix, middleResultMatrix);
                matrixMultiplication(middleResultMatrix, matrix_M, finalResult);
                rotation.setTransformationMatrix(finalResult);


            }


            rotation.transform_id = rotations.size()+1;
            rotations.push_back(rotation);
            stream.clear();
            element = element->NextSiblingElement("Rotation");
        }
    }

    //Get TextureCoordData
    element = root->FirstChildElement("TexCoordData");
    if(element) {
        stream << element->GetText() << endl;
        fst::math::Vector2f data;
        while (!(stream >> data.x).eof()) {
            stream >> data.y;
            this->tex_coord_data.push_back(data);
        }
        stream.clear();
    }

    //Get VertexData
    stream.clear();
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vec3f vertex;
    while (!(stream >> vertex.x).eof()) {
        stream >> vertex.y >> vertex.z;
        vertex_data.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> mesh.material_id;
        child = element->FirstChildElement("Texture");
        if(child) {
            stream << child->GetText() << endl;
            stream >> mesh.texture_id;

        }
        child = element->FirstChildElement("Transformations");
        if(child) {
            stream << child->GetText();
            while (!(stream.eof())) {
                string nextTransformation;
                stream >> nextTransformation;
                mesh.transformations.push_back(nextTransformation);
            }
        }
        stream.clear();

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0_id).eof()) {
            stream >> face.v1_id >> face.v2_id;
            Vec3f vertex_A = this->vertex_data[face.v0_id-1];
            Vec3f vertex_B = this->vertex_data[face.v1_id-1];
            Vec3f vertex_C = this->vertex_data[face.v2_id-1];

            face.old_v0 = face.v0_id;
            face.old_v1 = face.v1_id;
            face.old_v2 = face.v2_id;

            if(mesh.transformations.size() > 0) {
                mesh.all_transform = computeTransformationMatrix(mesh.transformations, this->translations, this->scalings, this->rotations);

                double transform1D[16];
                double inverseTransform1D[16];
                twoDimentionToOne(mesh.all_transform,transform1D);
                __matrixInverse_h__::invert(transform1D,inverseTransform1D);
                oneDimentionToTwo(inverseTransform1D,mesh.all_transform_reverse);

                Vec3f rotatedVertex_A = start_transform(mesh.all_transform,vertex_A);
                this->vertex_data.push_back(rotatedVertex_A);
                face.v0_id = (int)(this->vertex_data.size());
                Vec3f rotatedVertex_B = start_transform(mesh.all_transform,vertex_B);
                this->vertex_data.push_back(rotatedVertex_B);
                face.v1_id = (int)(this->vertex_data.size());
                Vec3f rotatedVertex_C = start_transform(mesh.all_transform,vertex_C);
                this->vertex_data.push_back(rotatedVertex_C);
                face.v2_id = (int)(this->vertex_data.size());
            }
            mesh.faces.push_back(face);
        }
        stream.clear();

        meshes.push_back(mesh);
        mesh.faces.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Triangles
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Triangle");
    Triangle triangle;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> triangle.material_id;
        child = element->FirstChildElement("Texture");
        stream.clear();
        if(child) {
            stream << child->GetText() << endl;
            stream >> triangle.texture_id;

        }
        stream.clear();
        child = element->FirstChildElement("Transformations");
        if(child) {
            stream << child->GetText();
            while (!(stream.eof())) {
                string nextTransformation;
                stream >> nextTransformation;
                triangle.transformations.push_back(nextTransformation);
            }
        }
        stream.clear();

        child = element->FirstChildElement("Indices");
        stream << child->GetText() << std::endl;
        stream >> triangle.indices.v0_id >> triangle.indices.v1_id >> triangle.indices.v2_id;

        triangle.indices.old_v0 = triangle.indices.v0_id;
        triangle.indices.old_v1 = triangle.indices.v1_id;
        triangle.indices.old_v2 = triangle.indices.v2_id;

        if(triangle.transformations.size() > 0) {

            Vec3f &vertex_A = this->vertex_data[triangle.indices.v0_id - 1];
            Vec3f &vertex_B = this->vertex_data[triangle.indices.v1_id - 1];
            Vec3f &vertex_C = this->vertex_data[triangle.indices.v2_id - 1];

            triangle.all_transform = computeTransformationMatrix(triangle.transformations,
                                                                                        this->translations,
                                                                                        this->scalings,
                                                                                        this->rotations);

            double transform1D[16];
            double inverseTransform1D[16];
            twoDimentionToOne(triangle.all_transform,transform1D);
            __matrixInverse_h__::invert(transform1D,inverseTransform1D);
            oneDimentionToTwo(inverseTransform1D,triangle.all_transform_reverse);

            Vec3f rotatedVertex_A = start_transform(triangle.all_transform,vertex_A);
            this->vertex_data.push_back(rotatedVertex_A);
            triangle.indices.v0_id = (int)(this->vertex_data.size());
            Vec3f rotatedVertex_B = start_transform(triangle.all_transform,vertex_B);
            this->vertex_data.push_back(rotatedVertex_B);
            triangle.indices.v1_id = (int)(this->vertex_data.size());
            Vec3f rotatedVertex_C = start_transform(triangle.all_transform,vertex_C);
            this->vertex_data.push_back(rotatedVertex_C);
            triangle.indices.v2_id = (int)(this->vertex_data.size());
        }


        triangles.push_back(triangle);
        element = element->NextSiblingElement("Triangle");
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");

    while (element)
    {
        Sphere sphere;
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> sphere.material_id;

        stream.clear();
        child = element->FirstChildElement("Texture");
        if(child) {
            stream << child->GetText() << endl;
            stream >> sphere.texture_id;

        }
        stream.clear();
        child = element->FirstChildElement("Transformations");
        if(child) {
            stream << child->GetText();
            while (!(stream.eof())) {
                string nextTransformation;
                stream >> nextTransformation;
                sphere.transformations.push_back(nextTransformation);
            }
        }
        stream.clear();

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        stream >> sphere.center_vertex_id;
        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;
        sphere.old_vertex = sphere.center_vertex_id;



        if(sphere.transformations.size() > 0) {
            Vec3f &centerPoint = this->vertex_data[sphere.center_vertex_id-1];
            sphere.all_transform = computeTransformationMatrix(sphere.transformations, this->translations, this->scalings, this->rotations);

            double transform1D[16];
            double inverseTransform1D[16];
            twoDimentionToOne(sphere.all_transform,transform1D);
            __matrixInverse_h__::invert(transform1D,inverseTransform1D);
            oneDimentionToTwo(inverseTransform1D,sphere.all_transform_reverse);

            vector<string> translations = vector<string>(0,"");
            vector<string> rotations = vector<string>(0,"");
            vector<string> scalings = vector<string>(0,"");
            Transformation currentTransform;
            double cumulativeScalingFactor = 1.0;
            for(string &transformation : sphere.transformations) {
                if(transformation[0] == 's') {
                    scalings.push_back(transformation);
                    int tranformationIndex = stoi(transformation.substr(1));
                    currentTransform = this->scalings[tranformationIndex-1];
                    if( (currentTransform.transform_matrix[0][0] == currentTransform.transform_matrix[1][1]) and (currentTransform.transform_matrix[1][1] == currentTransform.transform_matrix[2][2])) {
                        double scalingFactor = currentTransform.transform_matrix[0][0];
                        Vec3f &currentCenter = this->vertex_data[sphere.center_vertex_id-1];
                        Vec3f newCenter = start_transform(currentTransform.transform_matrix,currentCenter);
                        this->vertex_data.push_back(newCenter);
                        sphere.center_vertex_id = (int)(this->vertex_data.size());
                        sphere.radius *= scalingFactor;

                    }
                }
                else if(transformation[0] == 't') {
                    int tranformationIndex = stoi(transformation.substr(1));
                    currentTransform = this->translations[tranformationIndex-1];
                    translations.push_back(transformation);
                    Vec3f newCenter = start_transform(currentTransform.transform_matrix,this->vertex_data[sphere.center_vertex_id-1]);
                    this->vertex_data.push_back(newCenter);
                    sphere.center_vertex_id = (int)(this->vertex_data.size());
                }
                else {
                    sphere.openVector(sphere.all_rotate);
                    rotations.push_back(transformation);
                }

            }
            if(rotations.size() > 0) {
                double transform1D[16];
                double inverseTransform1D[16];
                sphere.all_rotate = computeTransformationMatrix(rotations,this->translations,this->scalings,this->rotations);
                twoDimentionToOne(sphere.all_rotate,transform1D);
                __matrixInverse_h__::invert(transform1D,inverseTransform1D);
                oneDimentionToTwo(inverseTransform1D,sphere.all_rotate);
            }
            translations.clear();
            rotations.clear();
            scalings.clear();



        }

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");


        //Get MeshInstances


    }
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("MeshInstance");
    Mesh meshInstance;
    while(element) {
        int baseMeshId = atoi(element->Attribute("baseMeshId",NULL));
        meshInstance = this->meshes[baseMeshId-1];
        child = element->FirstChildElement("Material");
        if(child) {
            stream << child->GetText() << endl;
            stream >> meshInstance.material_id;
        }
        child = element->FirstChildElement("Transformations");
        if(child) {
            stream << child->GetText();
            vector<string> tempTransformations;
            while (!(stream.eof())) {
                string nextTransformation;
                stream >> nextTransformation;
                tempTransformations.push_back(nextTransformation);
            }
            meshInstance.transformations.insert(meshInstance.transformations.end(),tempTransformations.begin(),tempTransformations.end());
            vector< vector<double> > tempTransformationMatrix = computeTransformationMatrix(tempTransformations,this->translations,this->scalings,this->rotations);
            for(Face &face: meshInstance.faces) {
                Vec3f vertex_A = this->vertex_data[face.v0_id-1];
                Vec3f vertex_B = this->vertex_data[face.v1_id-1];
                Vec3f vertex_C = this->vertex_data[face.v2_id-1];

                double transform1D[16];
                double inverseTransform1D[16];
                vector< vector<double> > finalTransform = matrixMultiplication(meshInstance.all_transform,tempTransformationMatrix);
                twoDimentionToOne(finalTransform,transform1D);
                __matrixInverse_h__::invert(transform1D,inverseTransform1D);
                oneDimentionToTwo(inverseTransform1D,meshInstance.all_transform_reverse);
                meshInstance.all_transform = finalTransform;

                Vec3f rotatedVertex_A = start_transform(tempTransformationMatrix,vertex_A);
                this->vertex_data.push_back(rotatedVertex_A);
                face.v0_id = (int)(this->vertex_data.size());
                Vec3f rotatedVertex_B = start_transform(tempTransformationMatrix,vertex_B);
                this->vertex_data.push_back(rotatedVertex_B);
                face.v1_id = (int)(this->vertex_data.size());
                Vec3f rotatedVertex_C = start_transform(tempTransformationMatrix,vertex_C);
                this->vertex_data.push_back(rotatedVertex_C);
                face.v2_id = (int)(this->vertex_data.size());
            }

        }
        child = element->FirstChildElement("Texture");
        if(child) {
            stream << child->GetText() << endl;
            stream >> meshInstance.texture_id;
        }
        element = element->NextSiblingElement("MeshInstance");
        this->meshes.push_back(meshInstance);
        stream.clear();
    }
}
