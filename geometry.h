#ifndef GEOM_H
#define GEOM_H
#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glut.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class Vec3d{
  
public:
  double x;
  double y;
  double z;  
  Vec3d(): x(0),y(0),z(0){};
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
  friend double dot(Vec3d e1 , Vec3d e2);
  friend double angle(Vec3d e1 , Vec3d e2);
  
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
  
   Vertex() : x(0),y(0),z(0){};
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

#endif
