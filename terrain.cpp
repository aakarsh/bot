
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


void Camera::setup_perspective(int w,int h ){
    int aspect = w/h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect,zNear, zFar); 
    glMatrixMode(GL_MODELVIEW);
}

void Camera::setup_lookat(double width,double length){
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


void Light::enable(){  
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

double Terrain::getHeight(int x,int z) {
  pixel_t px =this->heights[(x)+ this->width*(z)];
  int r = RED(px);
  int g = GREEN(px);
  int b = BLUE(px);
  return r;
}
/*
VertexMatrix* Terrain::vertexGrid() {
  if(grid.size() > 0){
    return &grid;
  }  
  for(int z = 0; z < this->length; z+= 1) {
    vector<Vertex*> triangle_strip;
    // questionable try x+
    for(int x = 0; x < this->width; x+=1) {      
      
      Vertex* v1 = new Vertex(x,this->getHeight(x,z),z);
      Vertex* v2 = new Vertex(x,this->getHeight(x,z+1),z+1);      
      triangle_strip.push_back(v1);
      triangle_strip.push_back(v2);
      cout<<"v1: "<<"["<<v1->to_s()<<"]"<<endl;
      cout<<"v2: "<<v2->to_s()<<"]"<<endl;
    }
    grid.push_back(triangle_strip);
  }
  return &grid;
}
*/

Vertex Terrain::make_cell(int x, int z){
  return Vertex(x,this->getHeight(x,z),z);
}

Grid* Terrain::vertexGrid() {
  if(grid.size() > 0){
    return &grid;
  }
  
  for(int z=0; z+1 < this->length; z+= 1) {
    vector<GridCell*> cells;
    for(int x=0; x+1 < this->width; x+= 1) {
      // Each grid scell consists of two triangles.      
      //Triangle 1: v0,v1,v2 [(x,z),(x,z+1),(x+1,z+1)]
      //Triangle 2:v0,v1,v3 [(x,z),(x+1,z+1),(x+1,z)]
      cells.push_back(new GridCell(make_cell(x,z), 
                                   make_cell(x,z+1),
                                   make_cell(x+1,z+1),
                                   make_cell(x+1,z)));



    }
    grid.push_back(cells);
  }
  return &grid;
}

NormalGrid* Terrain::vertexNormals(){  
  if(grid.size() > 0 && normals.size() > 0 ){
    return &normals;
  }
  
  for(int z = 0; z+1 < this->length; z+=1) {
    bool reverse_winding = true; 
    vector<NormalCell*> row;
    for(int x = 0; x+1 < this->width; x+=1) {        
      // Vertex* v[3];
      // v[0] = grid[z][x];
      // v[1] = grid[z][x+1];
      // v[2] = grid[z][x+2];
      GridCell* gc = grid[z][x];
      Vec3d  n1 = normal((gc->v[0]),
                         (gc->v[1]),
                         (gc->v[2]));
      
      Vec3d  n2 = normal((gc->v[0]),
                          (gc->v[2]),
                          (gc->v[3]));

      NormalCell* nc = new NormalCell(n1,n1,n2,n2);
      row.push_back(nc);      

    }
    normals.push_back(row);
  }  
  return &normals;
}


NormalGrid* Terrain::surfaceNormals() {
  
  if(grid.size() > 0 && surface_normals.size() > 0 ){
    return &surface_normals;
  }

    
  for (int z = 0; z+1 < this->length ; z+=1){
    //    cout<<"z = "<<z<<endl;
    vector<NormalCell*> surface_normal_row;        
    for(int x = 0; x+1 < this->width; x+=1) {    
      //    cout<<"x = "<<x<<endl;
      Vec3d sum ;
      
      for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < 2 ; j++) {
          if(z-i > 0  && x-j > 0)
            sum+=normals[z-i][x-j]->average();;
        }
      } 
      NormalCell* cur = new NormalCell(sum,sum,sum,sum);      
      surface_normal_row.push_back(cur);      
    }
    surface_normals.push_back(surface_normal_row);
  }

  /*
  for(int z =0 ; z < this->length ; z+=1){
    vector<NormalCell*> surface_normal_row;    
    for(int x = 0; x+3 < this->width; x+=1) {
      Vec3d sum(0,0,0);
      
      for(int i=-1; i<2 ; i+=1){
        for(int j=-1;j<2; j+=1){
          if(z+i>0 && z+i< this->length-1 && x+j>0 && (x+j)<this->length-4){
            sum = sum + normals[z+i][x+j]->normal[0]; // TODO 
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
  */

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
  Grid grid = *(this->vertexGrid());
  NormalGrid normals =*(this->vertexNormals());
        
  
  if(surfaceNormalsEnabled)
    normals =*(this->surfaceNormals());  
  
   cout<<"length "<<this->length<<" width "<<this->width<<endl;
  
  // i was z 
  for(int i = 0; i+1 < this->length; i+= 1) {
    glBegin(GL_TRIANGLES);                
    for(int x = 0; x+1 < this->width ; x+=1) {     
      GridCell* gc = grid[i][x];
      
      Vertex v1 = gc->v[0];                  
      Vertex v2 = gc->v[1];                  
      Vertex v3 = gc->v[2];                  
      Vertex v4 = gc->v[3];                  


      NormalCell* nc = normals[i][x];
      
      Vec3d n1 = nc->normal[0];                  
      Vec3d n2 = nc->normal[1];                  
      Vec3d n3 = nc->normal[2];                  
      Vec3d n4 = nc->normal[3];                  


      if(textureEnabled)
        glTexCoord2f(v1.x/this->width,v1.z/this->length);
      n1.glNormal();
      v1.gl();
                  
      if(textureEnabled)
        glTexCoord2f(v2.x/this->width,v2.z/this->length);                  
      n2.glNormal();
      v2.gl();

      if(textureEnabled)
        glTexCoord2f(v3.x/this->width,v3.z/this->length);                  
      n3.glNormal();
      v3.gl();
  




  
      if(textureEnabled)
        glTexCoord2f(v1.x/this->width,v1.z/this->length);
      n1.glNormal();
      v1.gl();
                  
      if(textureEnabled)
        glTexCoord2f(v3.x/this->width,v3.z/this->length);                  
      n3.glNormal();
      v3.gl();

      if(textureEnabled)
        glTexCoord2f(v4.x/this->width,v4.z/this->length);                  
      n4.glNormal();
      v4.gl();

      
    }
      glEnd();
  }          
  //  exit(0);
}




void Terrain::drawTerrain() {  
      this->camera->setup_perspective(windowWidth,windowHeight);
      glLoadIdentity();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode(GL_MODELVIEW);
      glBindTexture(GL_TEXTURE_2D, textureId);
        
      glLoadIdentity();

      Grid grid = *(this->vertexGrid());
      NormalGrid normals = *(this->vertexNormals());
        
      if(surfaceNormalsEnabled)
        normals =*(this->surfaceNormals());

      camera->setup_lookat(this->width,this->length);

      float scale = 1*this->scale_factor;
      glScalef(scale, scale, scale);        
        
       // if(displayListEnabled){
       //   glCallList(dl_id);         
       // } else{
         specifyGeometry();
         //       }
       
      glBindTexture(GL_TEXTURE_2D, 0);      
      glutSwapBuffers();  

      for(int i  = 0 ; i <  droids.size() ; i++)  {        
        glPushMatrix();
        glScalef(.5,.5,.5);
        android* droid = droids[i];        
        droid->mode = WALKING;  
        Vertex pos = droid->animate(key_frame++);
        
        if(pos.x+3 < this->length and  pos.z  < this->width) { 
          // height
          pos.y =  grid[pos.x][pos.z]->v[0].y;
          // up
          Vec3d up = normals[pos.x][pos.z]->normal[0];
          droid->draw(pos,&up);                  
        } else{
          // draw as stationary object
          droid->draw();
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
