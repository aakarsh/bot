
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>
#include <list>

#include <GL/glut.h>
#include <math.h>
#include "ppm_canvas.h"
#include "util.h"
#include "geometry.h"

#include "android.h"
#include "terrain.h"

using namespace std;


double Terrain::getHeight(int x,int z) {
  pixel_t px =this->heights[(x)+ this->width*(z)];
  int r = RED(px);
  int g = GREEN(px);
  int b = BLUE(px);
  return r;
}

VertexMatrix* Terrain::vertexGrid() {
  if(grid.size() > 0){
    return &grid;
  }  
  for(int z = 0; z < this->length; z+=1) {
    vector<Vertex*> triangle_strip;
    // questionable try x+
    for(int x = 0; x < this->width; x+=1) {      
      Vertex* v1 = new Vertex(x,this->getHeight(x,z),z);
      Vertex* v2 = new Vertex(x,this->getHeight(x,z+1),z+1);      
      triangle_strip.push_back(v1);
      triangle_strip.push_back(v2);
    }
    grid.push_back(triangle_strip);
  }
  return &grid;
}



VecMatrix* Terrain::vertexNormals(){  
  if(grid.size() > 0 && normals.size() > 0 ){
    return &normals;
  }
  for(int z = 0; z < this->length; z+=1) {
    bool reverse_winding = true; 
    vector<Vec3d*> normal_row;
    for(int x = 0; x+3 < this->width; x+=1) {        
      Vertex* v[3];
      v[0] = grid[z][x];
      v[1] = grid[z][x+1];
      v[2] = grid[z][x+2];
      Vec3d nrm = normal(*v[0],*v[1],*v[2]);
      if(reverse_winding){
        nrm.reverse();
        reverse_winding = false;
      }else{
        reverse_winding = true;
      }
      normal_row.push_back(new Vec3d(nrm));
    }
    normals.push_back(normal_row);
  }  
  return &normals;
}


VecMatrix* Terrain::surfaceNormals(){  
  if(grid.size() > 0 && surface_normals.size() > 0 ){
    return &surface_normals;
  }
  for(int z =0 ; z < this->length ; z+=1){
    vector<Vec3d*> surface_normal_row;
    for(int x = 0; x+3 < this->width; x+=1) {
      Vec3d sum(0,0,0);
      for(int i=-1; i<2 ; i+=1){
        for(int j=-1;j<2; j+=1){
          if(z+i>0 && z+i<this->length-1 && x+j>0 && (x+j)<this->length-4){
               sum=sum+*(normals[z+i][x+j]);
          }
        }
      }      
      double len = sum.len();
      if(len == 0)
        len = 1;
      surface_normal_row.push_back(new Vec3d(sum.x/len,sum.y/len,sum.z/len));      
    }  
    surface_normals.push_back(surface_normal_row);
  }
  return &surface_normals;
}

void Terrain::load_texture(canvas_t map) {
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map.height, map.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, map.pixels);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
}

void Terrain::specifyGeometry() {
  VertexMatrix grid = *(this->vertexGrid());
  VecMatrix normals =*(this->vertexNormals());
        
  if(surfaceNormalsEnabled)
    normals =*(this->surfaceNormals());  

  // i was z 
  for(int i = 0; i < this->length; i+=1) {
    glBegin(GL_TRIANGLE_STRIP);                
    // 
    for(int x = 0; x+3 < this->width ; x+=1) {
      
      if(x > this->width/2){
        glColor3ub(255,0,0);
      } else {
        glColor3ub(255,255,255);
      }

      Vertex* v1 = grid[i][x];                  
      Vertex* v2 = grid[i][x+1];
      Vertex* v3 = grid[i][x+2];                  
      Vec3d* normal = normals[i][x];

      if(textureEnabled)
        glTexCoord2f(v1->x/this->width,v1->z/this->length);
      normal->glNormal();
      v1->gl();
                  
      if(textureEnabled)
        glTexCoord2f(v2->x/this->width,v2->z/this->length);                  
      normal->glNormal();
      v2->gl();

      if(textureEnabled)
        glTexCoord2f(v3->x/this->width,v3->z/this->length);                  
      normal->glNormal();
      v3->gl();
      
    }
    glEnd();
  }          
}


void Terrain::drawTerrain() {  
      this->camera->setup_perspective(windowWidth,windowHeight);
      glLoadIdentity();
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
      // whe
      //      glColor3ub(255,255,255);
      
      glMatrixMode(GL_MODELVIEW);
      //      glEnable(GL_DEPTH_TEST);
      //      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textureId);
        
      glLoadIdentity();

      VertexMatrix grid = *(this->vertexGrid());
      VecMatrix normals = *(this->vertexNormals());
        
      if(surfaceNormalsEnabled)
        normals =*(this->surfaceNormals());

      camera->setup_lookat(normals,this->width,this->length);

      float scale = 1*this->scale_factor;
      glScalef(scale, scale, scale);        
        
       if(displayListEnabled){
         glCallList(dl_id);         
       } else{
         specifyGeometry();
       }      
      
       //
      light->enable(); // ?? questionable 


      glBindTexture(GL_TEXTURE_2D, 0);
      
      glutSwapBuffers();  

      for(int i  = 0 ; i <  droids.size() ; i++)  {        
        glPushMatrix();
        //        glScalef(.2,.2,.2);
        android* droid = droids[i];        
        droid->mode = WALKING;  
        Vertex pos = droid->animate(key_frame++);
        
        if(pos.x+3 < this->length and  pos.z  < this->width) { 
          // height
          pos.y =  grid[pos.x][pos.z]->y;          
          // up
          Vec3d* up = normals[pos.x][pos.z];
          droid->draw(pos,up);                  
        } else{
          // draw as stationary object
          //          droid->draw();
        }

        glPopMatrix();        
      }      
}


void Terrain::compile_display_list() {
  glNewList(dl_id, GL_COMPILE);
  specifyGeometry();
  glEndList();
  glFlush();
}
