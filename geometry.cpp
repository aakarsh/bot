
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>
#include <list>

#include <GL/glut.h>
#include <math.h>

#include "util.h"

#include "geometry.h"

using namespace std;

double dot(Vec3d e1 , Vec3d e2){
  return e1.x*e2.x
         +e1.y*e2.y
         +e1.z*e2.z;
}

double to_degrees(double radians) {
  return (180.0/3.14)*radians;
}

double angle(Vec3d e1 , Vec3d e2) {
  double dt  = dot(e1,e2);
  double mag  = e1.len()* e2.len();
  if(mag  == 0 )
    mag = 1;  
  return to_degrees(acos(dt/mag));
}

Vec3d cross(Vec3d e1 , Vec3d e2){
  return Vec3d(e1.y*e2.z - e1.z*e2.y,
               e1.z*e2.x - e1.x*e2.z,
               e1.x*e2.y - e1.y*e2.x);  
}

Vec3d normal(Vertex v1 , Vertex v2,Vertex v3) {
  Vec3d e1(v1.x- v2.x,v1.y- v2.y,v1.z - v2.z);
  Vec3d e2(v2.x- v3.x,v2.y- v3.y,v2.z- v3.z);
  Vec3d n = cross(e1,e2);
  n.normalize();
  return n;
}





