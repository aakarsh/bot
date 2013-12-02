
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <vector>
#include <list>

#include <GL/glut.h>

#include "util.h"
#include "geometry.h"
#include "android.h"

using namespace std;

double mod(double a , double n){
  return a- n*floor(a/n);
}


/* while walking*/
void arm::swing(double t, double period, double offset) {
    double position = sin((2*3.14/period)*(t+offset));
    shoulder_angle = 90-45*position;
    elbow_angle = -40;
}

/* while jumping */
void arm::raise(double t, double period, double offset){
     double position = sin((2*3.14/period)*(t+offset));
     shoulder_angle = 90-45*position;
     elbow_angle = -40;
}
 
void arm::draw() {
    glPushMatrix();
    glRotatef(-shoulder_angle,0,0,1);
    glTranslatef(1,0,0);
    draw_shoulder();
    draw_elbow();
    glPopMatrix();
}

void arm::draw_shoulder(){
    glPushMatrix(); 
    glRotatef(90,0,1,0);  
    glScalef(.3,.3,2);
    glColor3f(1.,1,1);
    glutSolidCube (1.0);  
    glPopMatrix();  
}

void drawFingers(int n){
  glPushMatrix();
  glTranslatef(1.1,0,.2);
   for(int i = 0;i< n; i++){
      glPushMatrix();
      glRotatef(90,0,1,0); 
      glTranslatef(.1*i,0,0);

      glScalef(.05,.05,.4);   
      glColor3f(0,1,0);
      glutSolidCube(1.0);
      glPopMatrix();  
   }
   glPopMatrix();
}

void arm::draw_elbow(){
   glTranslatef(1,0,0);
   glRotatef(-elbow_angle,0,0,1);
   glTranslatef(1,0,0);

   glPushMatrix();
   glRotatef(90,0,1,0); 
   glScalef(.3,.3,2);   
   glColor3f(1.,0,0);
   glutSolidCube(1.0);
   glPopMatrix();

   drawFingers(5);
}



void leg::walk(double key_frame, double period, double offset){
    double position = sin((2*3.14/period)*(key_frame+offset));
     if(position < 0){ 
       // for half the period we do nothing as the other leg is moving forward       
       double leg_max_angle = 10;
       double knee_max_angle = 10;
       leg_angle = leg_max_angle*position;
       knee_angle = leg_angle -3;
     } else {
       double leg_max_angle = 50;
       double knee_max_angle = -55;
       leg_angle =  leg_max_angle*position;
       knee_angle = knee_max_angle*position;
     }
}

void leg::jump(double key_frame, double period, double offset){
   double position = sin((2*3.14/period)*(key_frame+offset));
   double max_knee_angle = 40;
   leg_angle = 0;
   knee_angle = max_knee_angle*position;
}



void leg::draw(){
   glPushMatrix();      //1
   
   glRotatef(leg_angle-90, 0, 0 ,1);
   glTranslatef(1,0,0);

   glPushMatrix();//1.1

   glTranslatef (-1.0, 0.0, 0.0);
   glRotatef((GLfloat) 0, 0.0, 0.0, 1.0);
   glTranslatef(1.0, 0.0, 0.0);
  
   glPushMatrix(); //1..1
   glColor3f(0, 0, 1);
   glScalef (2.0, 0.4, .4);
   glutSolidCube (1.0);
   glPopMatrix();//1..1

   glPopMatrix(); //1.1


  //set up knee context
   
   glTranslatef (1.0, 0.0, 0.0);
   glRotatef ((GLfloat) knee_angle, 0.0, 0.0, 1.0);
   glTranslatef (1.0, 0.0, 0.0);

   glPushMatrix();  //1..2 // contain the scaling 
   glColor3f(1, .69, .19);
   glScalef(2.0, 0.4, .4);
   glutSolidCube (1.0);  
   glPopMatrix(); //1..2

   
   glPushMatrix();  //1..3
   glColor3f(139/256., 69/256., 19/256.);
   glTranslatef(1,.3,0);
   glScalef (.3, 1, .3);
   glutSolidCube (1.0);  
   glPopMatrix();// 1..3
  
   glPopMatrix();//1
}

Vertex android::animate(double key_frame){    
  if(key_frame == last_keyframe && key_frame!=0){
    return Vertex(0,0,0);
  }
  switch(mode){
  case WALKING:
    return animate_walk(key_frame);
  case JUMP:
    return animate_jump(key_frame);
  }
}
    
Vertex android::animate_walk(double key_frame){
  if(key_frame == last_keyframe){
    return Vertex(0,0,0);
  }
  double duration = 100.0;
  double pi = 3.14;
  double theta = mod((duration*key_frame),(2*pi));
  double position = sin(theta);
  // here we are updating theorem android location.
  posx = mod(posx+.06,300); 
  
  tilt_angle = -5;
  left_arm->swing(key_frame,duration,0);
  right_arm->swing(key_frame,duration,50);

  left_leg->walk(key_frame,duration,0);
  right_leg->walk(key_frame,duration,50);
  last_keyframe = key_frame;
  
  return Vertex(posx,posy,posz);
}
    
Vertex android::animate_jump(double key_frame) {

}
void android::draw(Vertex pos , Vec3d* up){
  glPushMatrix();
  this->posx = pos.x;
  this->posy = pos.y;
  this->posz = pos.z;
  
  this->up.x = up->x;
  this->up.y = up->y;
  this->up.z = up->z;  
  draw();  
  glPopMatrix();
}

void android::draw(){  
  glPushMatrix();  
  glTranslatef(posx,posy,posz);
  glRotatef(tilt_angle,0,0,1);
  glTranslatef(0,height,0);

  glPushMatrix();
  drawHead();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0,4,0);
  drawUpperTorso();
  glPopMatrix();
  
  glPushMatrix();
  drawSpinalCord();  
  drawLowerTorso();
  glPopMatrix();

  glPopMatrix();
}

void android::drawUpperTorso(){
  glPushMatrix();
  glScalef(1,1,2);
  glColor3f(0,.3,1);
  glutSolidCube(1.0);
  glPopMatrix();  

  glPushMatrix();
  glTranslatef(0,0,1.2);  
  left_arm->draw();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0,0,-1.2);
  right_arm->draw();
  glPopMatrix();
}


void android::drawSpinalCord(){
  glPushMatrix();
  glTranslatef(0,+3,0);
  glScalef(.5,2,1.1);
  glColor3f(139/256.0,137/256.0,137/256.0);
  glutSolidCube (1.0);
  glPopMatrix();
}

void android::drawLowerTorso(){
  glTranslatef(0,1,0);  // position relative drawing

  glPushMatrix();
  glColor3f(0,.5,.5);
  glScalef(1,1,1.4);
  glTranslatef(0,.6,0);
  glutSolidCube (1.0);
  glPopMatrix();  
  
  glPushMatrix();
  glTranslatef(0,0,.5);
  
  left_leg->draw();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0,0,-.5);
  glColor3f(1, 0, 1);
  right_leg->draw();
  glColor3f(1, 1, 1);
  glPopMatrix();
}

void drawAntenna() {
  glPushMatrix();
  glPushMatrix();
  glRotatef(-90,1,0,0);
  double antenna_height = 2;
  double antenna_radius = .1;
  glutSolidCone(antenna_radius,antenna_height,30,30);  
  glPopMatrix();
  glTranslatef(0,antenna_height-.1,0);
  glColor3f(255/256.0,215/256.0,0);
  glutSolidSphere(.1,30,30);
  glPopMatrix();
}

void drawEye(double head_radius,double eye_line);

void android::drawHead(){
  double head_radius = .5;

  glPushMatrix();

  glColor3f(139/256.0,137/256.0,137/256.0);
  glTranslatef(0,5,0);// dont translate head while we model it


  glutSolidSphere(head_radius,16,16);
  drawAntenna();
  double eye_line = .2;  

  glPushMatrix();
  glPushMatrix();
  glRotatef(-45,0,1,0);
  drawEye(head_radius,eye_line);
  glPopMatrix();

  glPushMatrix();
  glRotatef(+45,0,1,0);
  drawEye(head_radius,eye_line);
  glPopMatrix();
  glPopMatrix();
  
  double mouth_line = -.3;
  void drawMouth(double head_radius,double eye_line);

  glColor3f(205/256.0,133/256.0,131/256.0);
  drawMouth(head_radius,mouth_line);
  
  glPopMatrix(); 
}

void drawMouth(double head_radius,double mouth_line){
   glPushMatrix(); //1..1   
   glPushMatrix(); // Jaw region
   glTranslatef(0,-head_radius+.2,0);
   glColor3f(139/256.0,137/256.0,137/256.0);
   glScalef (.5, 0.5, .5);
   glutSolidCube (1.0);
   glPopMatrix();
   
   glTranslatef(head_radius-.2,mouth_line,0);

   glPushMatrix();   
   glScalef (1, 0.1, .5);
   glColor3f(139/256.0,137/256.0,137/256.0);
   glutSolidCube (1.0);
   glPopMatrix();
   

   glPushMatrix();
   glTranslatef(0,-.2,0);
   glColor3f(1,0,0);
   glScalef (1, 0.08, .5);
   glutSolidCube (1.0);
   glPopMatrix();
     
   glPopMatrix();
}
void drawEye(double head_radius,double eye_line){  
  glTranslatef(head_radius-.3,eye_line,0);
  glPushMatrix();

  glScalef(.1,.3,.1);
  glColor3f(1,1,1);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glColor3f(1,0,0);
  glScalef(.1,.1,.1);
  glutSolidCube(1);
  glPopMatrix();
}
