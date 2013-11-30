//#pragma once

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

#include "ppm_canvas.h"
#include "util.h"

using namespace std;

class Vec3d{
  
public:
  double x;
  double y;
  double z;  

  Vec3d(double i,double j, double k): x(i),y(j),z(k){};
  Vec3d(double* v){
    x=v[0];
    y=v[1];
    z=v[2];
  }
  Vec3d(const Vec3d & v){
    x = v.x;
    y = v.y;
    z = v.z;
  }
  friend Vec3d cross(Vec3d e1 , Vec3d e2);
  
  double* to_array(){
    double* v = (double*) malloc(sizeof(double)*3);
    v[0] = x;
    v[1] = y; 
    v[2] = z;
    return v;
  }

  Vec3d operator+(Vec3d o){
    return Vec3d(x+o.x,y+o.y,z+o.z);
  }
  
  void glNormal(){
    glNormal3f(x,y,z);   
  }
  
  double len(){
    return sqrt(x*x+ y*y+ z*z);
  }
  void scale(double factor){
    x *= factor;
    y *= factor;
    z *= factor;
  }

  void reverse(){
    scale(-1);
  }
  
  void normalize(){
    double dist = len();
    if (dist == 0.0f) // avoid div by 0
      return ;    
    x = x/dist;
    y = y/dist;
    z = z/dist;    
  }  
};

class Vertex{
public:
  double x;
  double y;
  double z;
  
  Vertex(double i,double j, double k): x(i),y(j),z(k){};
  Vertex(double* v): x(v[0]),y(v[1]),z(v[2]){};
  
  double* to_array(){
    double* v = (double*) malloc(sizeof(double)*3);
    v[0] = x;
    v[1] = y; 
    v[2] = z;
    return v;
  }
  void gl(){
    glVertex3d(x,y,z);
  }
  friend Vec3d normal(Vertex v1 , Vertex v2,Vertex v3);  
};

typedef vector<Vertex*> row;
typedef vector<row> VertexMatrix;

typedef vector<Vec3d*> row_vec;
typedef vector<row_vec> VecMatrix;

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

  void setup_perspective(int w,int h ){
    int aspect = w/h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect,zNear, zFar); 
    glMatrixMode(GL_MODELVIEW);
  }

  void setup_lookat(VecMatrix & normals,double width,double length){
    Vertex eye(this->eyeLocation);
    Vertex lookat(this->eyeCenter);        
    Vec3d up(0.0,1.0,0.0);
        
    gluLookAt(eye.x,eye.y,eye.z,
              lookat.x,lookat.y,lookat.z,
              up.x,up.y,up.z); //up        


    glRotatef(this->cameraAngleX, 0.1f, 0.0f, 0.0f);
    glRotatef(this->cameraAngleY, 0.0f, 0.1f, 0.0f);
    
    glTranslatef(-(float)(width)/2.0 ,0.0f, -(float)(length)/2.0 );
    
  }
  
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
  
  void enable(){  
    GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 0.0f};
    // Ambient white light 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);	
    // Diffuse white light
    GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f};
    //    GLfloat lightPos0[] = {-0.5f, 0.5f, 0.1f, 0.0f};
    glLightfv(light_id, GL_DIFFUSE, lightColor0);        
    glLightfv(light_id, GL_POSITION, light_position);
    glEnable(light_id);
  }
  
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


class Terrain{
  
private:
  int windowWidth; 
  int windowHeight;
  GLuint textureId;
  
  int width;
  int length;
  pixel_t *heights;
  pixel_t *texture;
  VertexMatrix grid;
  VecMatrix normals;
  VecMatrix surface_normals;
  vector<Vec3d*> textures;

  GLuint dl_id;

  bool textureEnabled;  
  bool surfaceNormalsEnabled;
  bool mousePanning ;
  bool firstPersonMode;
  bool displayListEnabled;

  VertexMatrix* vertexGrid();
  VecMatrix* vertexNormals();
  VecMatrix* surfaceNormals();

public:
  Light* light;
  Camera* camera; 
  int angle;
  double scale_factor;  
  MouseControlMode mouseMode;// = NONE;
  
 Terrain(int ww,int wh,
         canvas_t& textureMap,
         canvas_t& heightMap):

        
        windowWidth(ww),
        windowHeight(wh),
        length(heightMap.height),
        width(heightMap.width),
        angle(0),
        texture(textureMap.pixels),
        heights(heightMap.pixels),
        scale_factor(1),
        mousePanning(false),
        firstPersonMode(true),
        displayListEnabled(true),  
        textureEnabled(true),
        surfaceNormalsEnabled(true),
        mouseMode(NONE)
 {
    
    load_texture(textureMap);
    cout<<"Terrain of length "<<length<<" width "<<width<<endl;
    light = new Light(GL_LIGHT0);
    camera = new Camera();
    dl_id = glGenLists(1);
    compile_display_list();
  }
  
  void drawTerrain();

  void compile_display_list();
  void specifyGeometry() ;
  void load_texture(canvas_t map);
  
  void toggleTexture(){    
    textureEnabled = !textureEnabled;
    compile_display_list();
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
