#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>



#include <limits.h>
#include <unistd.h>


using namespace std;
using parser::Vec3f;

//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;
static bool lights_enabled = true;
static bool light_disabled[10];

char hostname[HOST_NAME_MAX];

double lastTime;
int nbFrames;
void showFPS(GLFWwindow *win)
{
    // Measure speed
    int result;
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
	   char ss[500] = {};
     nbFrames++;

     result = gethostname(hostname, HOST_NAME_MAX);

     if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago
         //cout << 1000.0/double(nbFrames) << endl;

         double fps = ((double)(nbFrames)) / delta;
        std::string title = "CENG477 - HW3[";
        title += std::to_string(fps);
        title += " FPS]@";
        title +=hostname;
        glfwSetWindowTitle(win, title.c_str());
         //sprintf(ss,"CENG477 - HW3 [%lf FPS]@",fps);

         //glfwSetWindowTitle(win, (ss+hostname));

         nbFrames = 0;
         lastTime = currentTime;
     }
}

Vec3f faceNormal(parser::Face f){
    Vec3f v0 = scene.vertex_data[(f.v0_id-1)];
    Vec3f v1 = scene.vertex_data[(f.v1_id-1)];
    Vec3f v2 = scene.vertex_data[(f.v2_id-1)];
    Vec3f a = v0-v1;
    Vec3f b = v0-v2;
    return a.CrossProduct(b).Normalized();
}

void start(){
    glEnable(GL_DEPTH_TEST);

    if(scene.culling_enabled == 1){
       glEnable(GL_CULL_FACE);
       if(scene.culling_face == 1){
          glCullFace(GL_FRONT); 
       }else{
           glCullFace(GL_BACK);
       }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glEnable(GL_LIGHTING);  
    
    glShadeModel(GL_SMOOTH); 
}



void startCamera() {
  const parser::Camera camera = scene.camera;

  const Vec3f eye = camera.position;
  const Vec3f center = eye + (camera.gaze * camera.near_distance);
  const Vec3f v = camera.up;

  const parser::Vec4f near_plane = camera.near_plane;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glFrustum(near_plane.x, near_plane.y, near_plane.z, near_plane.w, camera.near_distance, camera.far_distance);

  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();

  gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, v.x, v.y, v.z);
}

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void render() {

  glClearColor(scene.background_color.x, scene.background_color.y, scene.background_color.z, 1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const parser::Mesh& mesh : scene.meshes) {

    glPushMatrix();
    
    GLfloat ambient[] = {scene.materials[(mesh.material_id-1)].ambient.x, scene.materials[(mesh.material_id-1)].ambient.y, scene.materials[(mesh.material_id-1)].ambient.z, 1.};
    GLfloat diffuse[] = {scene.materials[(mesh.material_id-1)].diffuse.x, scene.materials[(mesh.material_id-1)].diffuse.y, scene.materials[(mesh.material_id-1)].diffuse.z, 1.};
    GLfloat specular[] = {scene.materials[(mesh.material_id-1)].specular.x, scene.materials[(mesh.material_id-1)].specular.y,scene.materials[(mesh.material_id-1)].specular.z, 1.};
    GLfloat phong_exponent[] = {scene.materials[(mesh.material_id-1)].phong_exponent};
    
    for (int index = mesh.transformations.size() - 1; index >= 0; index--) {

      if (mesh.transformations[index].transformation_type == "Translation") {
        const Vec3f& translation = scene.translations[(mesh.transformations[index].id-1)];

        glTranslatef(translation.x, translation.y, translation.z);

      } else if (mesh.transformations[index].transformation_type == "Scaling") {
        const Vec3f& scaling = scene.scalings[(mesh.transformations[index].id-1)];

        glScalef(scaling.x, scaling.y, scaling.z);

      } else {
        const parser::Vec4f& rotation = scene.rotations[(mesh.transformations[index].id-1)];

        glRotatef(rotation.x, rotation.y, rotation.z, rotation.w);

      }
    }
  
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, phong_exponent);

    if (mesh.mesh_type == "Solid") {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    const int vertex_count = scene.vertex_data.size();
    std::vector<Vec3f> normal_data(vertex_count); 
    for (const parser::Face& face : mesh.faces) {
      normal_data[(face.v0_id)-1] += faceNormal(face);
      normal_data[(face.v1_id)-1] += faceNormal(face);
      normal_data[(face.v2_id)-1] += faceNormal(face);
    }

    for(int i = 0; i < normal_data.size();i++){
      normal_data[i].x /= normal_data.size();
      normal_data[i].y /= normal_data.size();
      normal_data[i].z /= normal_data.size();

      parser::Vec3f normal;
      normal.x = normal_data[i].x;
      normal.y = normal_data[i].y;
      normal.z = normal_data[i].z;

      float magn = 0;
      magn += normal.x * normal.x;
      magn += normal.y * normal.y;
      magn += normal.z * normal.z;

      magn = sqrtf(magn);

      Vec3f temp;

      temp.x = normal.x / magn;
      temp.y = normal.y / magn;
      temp.z = normal.z / magn;
      
      normal = temp;

      normal_data[i].x = normal.x;
      normal_data[i].y = normal.y;
      normal_data[i].z = normal.z;
    }

    for (int j = 0; j < mesh.faces.size(); j++) {

      glBegin(GL_TRIANGLES);

      parser::Face face = mesh.faces[j];
      parser::Vec3f vertex;

      vertex = scene.vertex_data[face.v0_id-1];

      glNormal3f(normal_data[face.v0_id - 1].x, normal_data[face.v0_id - 1].y, normal_data[face.v0_id - 1].z);
      glVertex3f(vertex.x, vertex.y, vertex.z);

      vertex = scene.vertex_data[face.v1_id-1];

      glNormal3f(normal_data[face.v1_id - 1].x, normal_data[face.v1_id - 1].y, normal_data[face.v1_id - 1].z);
      glVertex3f(vertex.x, vertex.y, vertex.z);

      vertex = scene.vertex_data[face.v2_id-1];

      glNormal3f(normal_data[face.v2_id - 1].x, normal_data[face.v2_id - 1].y, normal_data[face.v2_id - 1].z);
      glVertex3f(vertex.x, vertex.y, vertex.z);

      glEnd();

    }

    glPopMatrix();
  }
}

void startLights() {
  const GLfloat ambient[4] = {scene.ambient_light.x, scene.ambient_light.y, scene.ambient_light.z, 1.};
  const std::vector<parser::PointLight> lights = scene.point_lights;
  
  for (int i = 0; i < lights.size(); i++) {
    const parser::PointLight light = lights[i];
    const GLfloat intensity[4] = {light.intensity.x, light.intensity.y, light.intensity.z, 1.};

    glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, intensity);
    glLightfv(GL_LIGHT0 + i, GL_SPECULAR, intensity);

    glEnable(GL_LIGHT0 + i);
  }
}





int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);


    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        ::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        glfwTerminate();
        ::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        ::exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    start();
    startLights();
    startCamera();
    while(!glfwWindowShouldClose(win)) {
        render();

        showFPS(win);

        glfwPollEvents();

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    ::exit(EXIT_SUCCESS);

    return 0;
}
