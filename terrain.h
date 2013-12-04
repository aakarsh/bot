#ifndef TERRAIN_H
#define TERRAIIN_H

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glut.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <math.h>

#include "android.h"
#include "ppm_canvas.h"
#include "util.h"
#include "geometry.h"

using namespace std;


class Camera{
public:
  float fov;
  float zNear;
  float zFar;

  double cameraAngleX  ;
  double cameraAngleY  ;
  
  double eyeLocation[3];
  double eyeCenter[3];

 Camera():
          fov(90),
          zNear(.1),
          zFar(800),
          cameraAngleX(30),
          cameraAngleY(90)  
  {
    eyeLocation[0] = 0;
    eyeLocation[1] = 100;
    eyeLocation[2] = 500;

    eyeCenter[0] = 0;
    eyeCenter[1] = 0;
    eyeCenter[2] = 0;
  }

  void setup_perspective(int w,int h );

  void lookat_android(android* droid);
  void setup_lookat(double width,double length);

  
  Vec3d direction(){
    Vec3d result(eyeCenter[0] - eyeLocation[0],
                 eyeCenter[1] - eyeLocation[1],
                 eyeCenter[2] - eyeLocation[2]);
    result.normalize();
    return result;    
  }

  void forward(int steps){
    Vec3d dir = direction();
    dir.scale(steps);
    eyeLocation[0]+=dir.x;
    eyeLocation[1]+=dir.y;
    eyeLocation[2]+=dir.z;    
  }

  void backward(int steps){
    forward(-1*steps);
  }
  
};


class Light{
public:
  GLfloat* mat_specular;
  GLfloat* mat_shininess;
  GLfloat* light_position;

  GLenum light_id;
  
  Light(GLenum id):light_id(id){

    this->mat_specular = new GLfloat[4];
    this->mat_specular[0] = 1.0 ; 
    this->mat_specular[1] = 0.0 ; 
    this->mat_specular[2] = 0.0 ; 
    this->mat_specular[3] = 0.0 ;
    
    this->mat_shininess = new GLfloat[1];
    this->mat_shininess[0] = 50;

    this->light_position = new GLfloat[4];
    this->light_position[0] = 0.0 ; 
    this->light_position[1] = 1.0 ; 
    this->light_position[2] = 1.0 ; 
    this->light_position[3] = 1.0 ;
  }
  
  void enable();
  
  void disable(){
    glDisable(light_id);
  }
  
  void move(float pos[]){
    light_position[0]  += pos[0];
    light_position[1]  += pos[1];
    light_position[2]  += pos[2];
  }
  
  void move_to(float* pos){
    light_position[0] = pos[0];
    light_position[1]  = pos[1];
    light_position[2]  = pos[2];
  }
  
};  

enum MouseControlMode {NONE, CAMERA_PANNING, VIEW_POINT,LOOKAT_CENTER, LIGHT_MOTION,FIRST_PERSON};

class GridCell {
public:  
  Vertex v[4];
  
  GridCell(Vertex v1, Vertex v2,
           Vertex v3, Vertex v4){    
    v[0] = v1;
    v[1] = v2;
    v[2] = v3;
    v[3] = v4;
  }  
};

class NormalCell {
public:  
  Vec3d normal[4];
  
  NormalCell(Vec3d v1, Vec3d v2,
           Vec3d v3, Vec3d v4){    
    normal[0] = v1;
    normal[1] = v2;
    normal[2] = v3;
    normal[3] = v4;
  }  

  Vec3d average() { 
    Vec3d sum;
    for(int i =0 ; i < 3 ;  i++){
      sum += normal[i];
    }    
    return sum;
  }
};


typedef vector<GridCell*> cell_row;
typedef vector<cell_row> Grid;

typedef vector<NormalCell*> normal_row;
typedef vector<normal_row> NormalGrid;

class Terrain{
  
private:
  int windowWidth; 
  int windowHeight;
  GLuint textureId;
  
  int width;
  int length;
  
  pixel_t *heights;
  pixel_t *texture;
  
  Grid grid;

  NormalGrid normals;
  NormalGrid surface_normals;
  vector<Vec3d*> textures;
  
  GLuint dl_id;

  int key_frame;

  bool textureEnabled;  
  bool surfaceNormalsEnabled;
  bool mousePanning ;
  bool firstPersonMode;
  bool displayListEnabled;

  bool droidView;
  int currentDroid;

  //  VertexMatrix* vertexGrid();
  Grid* vertexGrid() ;

  NormalGrid* vertexNormals();
  NormalGrid* surfaceNormals();

public:
  Light* light;
  Camera* camera; 
  int angle;
  double scale_factor;  
  MouseControlMode mouseMode;
  vector<android*>  droids;

  
  
 Terrain(int ww,int wh,
         canvas_t& textureMap,
         canvas_t& heightMap):
        
        key_frame(0),
        windowWidth(ww),
        windowHeight(wh),
        length(heightMap.height),
        width(heightMap.width),        
        angle(0),
        texture(textureMap.pixels),
        heights(heightMap.pixels),
        scale_factor(1),
        droidView(false),
        currentDroid(0),
        mousePanning(false),
        firstPersonMode(true),
        displayListEnabled(true),  
        textureEnabled(true),
        surfaceNormalsEnabled(false),
        mouseMode(NONE)
 {
    
    load_texture(textureMap);
    light = new Light(GL_LIGHT0);
    camera = new Camera();
    dl_id = glGenLists(1);
    compile_display_list();
    // Moving the light
    light->enable(); // ?? questionable 
  }
  
  void drawTerrain();
  Vertex make_cell(int x, int z);  
  void addDroid(android* d){
    this->droids.push_back(d);
  }

  void compile_display_list();
  void specifyGeometry() ;
  void load_texture(canvas_t map);
  
  void toggleTexture(){    
    textureEnabled = !textureEnabled;
    compile_display_list();
  }
  
  void toggleDroidView(){
    droidView = !droidView;
  }
  
  void nextDroid(){
    currentDroid = (currentDroid + 1) % droids.size();
    droidView = true;
  }
  
  void toggleSurfaceNormals(){
    surfaceNormalsEnabled = !surfaceNormalsEnabled;
    compile_display_list();
  }

  void toggleMouseMode(MouseControlMode mode){
    if(mouseMode == mode){
      mouseMode = NONE;
    }
    mouseMode = mode;
  }
  
  void toggleMousePanning(){
    toggleMouseMode(CAMERA_PANNING);  
  }
  void toggleDisplayList(){
    displayListEnabled= !displayListEnabled;
  }
  void toggleMouseLighting(){
    toggleMouseMode(LIGHT_MOTION);
  }
  
  double getHeight(int x,int z);
  ~Terrain(){
    //    free(vertices);
  }  
  
  void mouseChange(bool left,int x, int y, int dx , int dy){
    if(left) {  
      if(mouseMode == CAMERA_PANNING) { 
        camera->cameraAngleY += dx;
        camera->cameraAngleX += dy;  
      }else if(mouseMode == LIGHT_MOTION){
        float increment[] = {dx , 0 , dy};
        light->move(increment);
        // float pos [] ={x,y,0};
        // light->move_to(pos);
      } else if (mouseMode == VIEW_POINT) {
        camera->eyeLocation[0]+=dx;
        camera->eyeLocation[2]+=dy;
      } else if (mouseMode == LOOKAT_CENTER) {  
        camera->eyeCenter[0]+=dx;
        camera->eyeCenter[1]-=dy;
      } else if (mouseMode == FIRST_PERSON) {
        camera->eyeCenter[0]+=dx;
        camera->eyeCenter[1]-=dy;
      } 
    }
  }

};

#endif
