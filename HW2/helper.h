#include "parser.h"
#include <cmath>
#include <sstream>



parser::Vec3f::Vec3f() {}

parser::Vec3f parser::Vec3f::operator+(parser::Vec3f &rhs) {
    Vec3f result; 
    result =  Vec3f(this->x+rhs.x,this->y+rhs.y,this->z+rhs.z);
    return result;
}

double parser::Vec3f::operator*(parser::Vec3f &rhs) {
    double result;
    result = this->x*rhs.x+this->y*rhs.y+this->z*rhs.z;
    return result;
}

parser::Vec3f parser::Vec3f::operator*(double scalar) {
    return Vec3f(this->x*scalar,this->y*scalar,this->z*scalar);
}

parser::Vec3f parser::Vec3f::operator-(parser::Vec3f &rhs) {
    return Vec3f(this->x-rhs.x,this->y-rhs.y,this->z-rhs.z);
}

parser::Vec3i parser::Vec3i::operator+(parser::Vec3i &rhs) {
    return Vec3i(this->x + rhs.x,this->y+rhs.y,this->z+rhs.z);
}

parser::Vec3i::Vec3i(int x, int y, int z) {
    this->x = x;
    this->y = y;
    this->z = z;
}





void matrixMultiplication(vector<vector<double>> &first, double second[4][4],double result[4][4]) {
    double temp[4][4] = {
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
                    0, 0, 0, 0
            }
    };
    for(int i = 0; i <4; i++) {
        for(int j = 0; j < 4; j++) {
            for (int z = 0; z < 4; z++) {
                temp[i][j] += first[i][z] * second[z][j];

            }
        }
    }
    copyMatrix(result,temp);
}

void matrixMultiplication(double first[4][4], double second[4][4],double result[4][4]) {
    double temp[4][4] = {
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
    copyMatrix(temp,result);
    for(int i = 0; i <4; i++) {
        for(int j = 0; j < 4; j++) {
            temp[i][j] = 0;
            for (int z = 0; z < 4; z++) {
                temp[i][j] += first[i][z] * second[z][j];

            }
        }
    }
    copyMatrix(result,temp);
}

vector< vector<double> > matrixMultiplication(vector< vector<double> > &first,vector< vector<double> > &second) {
    std::vector<std::vector<double> > result(4, std::vector<double>(4));
    for(int i = 0; i <4; i++) {
        for(int j = 0; j < 4; j++) {
            for (int z = 0; z < 4; z++) {
                result[i][j] += first[i][z] * second[z][j];

            }
        }
    }
    return result;
}




parser::Vec3f crossProduct(parser::Vec3f &v1, parser::Vec3f &v2) {
    parser::Vec3f c_v;
    c_v.x = (v1.y*v2.z) - (v1.z*v2.y);
    c_v.y = (v1.z*v2.x) - (v1.x*v2.z);
    c_v.z = (v1.x*v2.y) - (v1.y*v2.x);
    return c_v;
}




parser::Vec3f normalizeVector(parser::Vec3f &ray){
    parser::Vec3f n_v = parser::Vec3f(ray.x, ray.y, ray.z);
    double n_f = sqrt( (n_v.x*n_v.x)+ (n_v.y*n_v.y) + (n_v.z)*(n_v.z) );
    n_v.x /= n_f;
    n_v.y /= n_f;
    n_v.z /= n_f;
    return n_v;
}



parser::Vec3f start_transform(std::vector<std::vector<double>> &transformations,parser::Vec3f vector) {
    parser::Vec3f result = Vec3f(vector);
    double tempX = result.x * transformations[0][0] + result.y * transformations[0][1] + result.z * transformations[0][2] + transformations[0][3];
    double tempY = result.x * transformations[1][0] + result.y * transformations[1][1] + result.z * transformations[1][2] + transformations[1][3];
    double tempZ = result.x * transformations[2][0] + result.y * transformations[2][1] + result.z * transformations[2][2] + transformations[2][3];
    result.x = tempX;
    result.y = tempY;
    result.z = tempZ;
    return result;
}



void copyMatrix(double first[4][4], double second[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j =0; j < 4;j++) {
            first[i][j] = second[i][j];
        }
    }
}

void copyMatrixVector(double first[4][4],vector<vector<double>> &second) {
    for(int i = 0; i < 4; i++) {
        for(int j =0; j < 4;j++) {
            first[i][j] = second[i][j];
        }
    }
}

void copyMatrixVector(vector<vector<double>> &first,double second[4][4]) {
    for(int i = 0; i < 4; i++) {
        for(int j =0; j < 4;j++) {
            first[i][j] = second[i][j];
        }
    }
}


void twoDimentionToOne(vector<vector<double>> &first, double second[]) {
    for(int i = 0; i < 4;i++) {
        for(int j = 0; j < 4;j++) {
            second[i*4+j] = first[i][j];
        }
    }
}

void oneDimentionToTwo(double first[], vector<vector<double>> &second) {
    for(int i = 0; i < 16;i++) {
        second[i/4][i%4] = first[i];
    }
}



vector< vector<double> > computeTransformationMatrix(std::vector<string> &transformations,std::vector<Transformation> &translation,std::vector<Transformation> &scaling,std::vector<Transformation> &rotation) {
    double result_matrix[4][4] = {
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
                    0, 0, 0, 0
            }
    };
    string current_transform_string = transformations[0];
    Transformation current_transform;
    if(current_transform_string[0] == 't') {
        int tranformation_index = stoi(current_transform_string.substr(1));
        current_transform = translation[tranformation_index-1];

    }
    else if(current_transform_string[0] == 's') {
        int tranformation_index = stoi(current_transform_string.substr(1));
        current_transform = scaling[tranformation_index-1];
    }
    else {
        int tranformation_index = stoi(current_transform_string.substr(1));
        current_transform = rotation[tranformation_index-1];

    }
    copyMatrixVector(result_matrix,current_transform.transform_matrix);
    for(std::vector<string>::iterator begin = transformations.begin()+1; begin != transformations.end(); ++begin) {
        stringstream stream;
        stream << (*begin);
        current_transform_string = stream.str();
        if(current_transform_string[0] == 't') {
            int tranformation_index = stoi(current_transform_string.substr(1));
            current_transform = translation[tranformation_index-1];

        }
        else if(current_transform_string[0] == 's') {
            int tranformation_index = stoi(current_transform_string.substr(1));
            current_transform = scaling[tranformation_index-1];
        }
        else {
            int tranformation_index = stoi(current_transform_string.substr(1));
            current_transform = rotation[tranformation_index-1];

        }
        matrixMultiplication(current_transform.transform_matrix,result_matrix,result_matrix);
    }
    std::vector<std::vector<double> > result_vector(4, std::vector<double>(4));
    copyMatrixVector(result_vector,result_matrix);
    return result_vector;
}