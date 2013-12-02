
#ifndef AND_H
#define AND_H
#include <math.h>
#include "geometry.h"

enum animation_mode {WALKING,JUMP};

class arm{
public:
  int shoulder_angle;
  int elbow_angle;
  int position;
  arm():shoulder_angle(90),elbow_angle(10),position(0){}
    
  void swing(double t, double period, double offset) ;
  void raise(double t, double period, double offset);
  void draw() ;
  void draw_elbow();
  void draw_shoulder();
  void draw_hand();
};

class leg{
public:
  int leg_angle ;
  int knee_angle;
  int position;

  leg():leg_angle(0),knee_angle(0),position(0){
  }    
  void walk(double key_frame, double period, double offset);
  void jump(double key_frame, double period, double offset);
  void draw();
};


class android {
public:
  double posx;
  double posy;
  double posz;
  double tilt_angle;

  arm * left_arm;
  arm * right_arm;
  
  leg * left_leg;
  leg * right_leg;

  animation_mode mode;
  int last_keyframe;
  int height;

  Vertex* pos;
  double facing_direction;
  Vec3d walk_direction;
  double step_size;
  Vec3d up;

  android(int x,int y,int z) :
    posx(x),
    posy(y),
    posz(z),
    step_size(.06),
    walk_direction(1,0,0),
    up(0,1,0),
    tilt_angle(0),
    height(3),
    last_keyframe(0),
    left_leg(new leg()),
    right_leg(new leg()),
    mode(WALKING),
    left_arm(new arm()),
    right_arm(new arm()) {
  }    
  double position(int key_frame,double period , double offset){
    double position = sin((2*3.14/period)*(key_frame+offset));
    return position;
  }    
  Vertex animate(double key_frame);    
  Vertex animate_walk(double key_frame);
  Vertex animate_jump(double key_frame) ;


  void draw();
  void draw(Vertex pos , Vec3d* up);

  void drawHead();
  void drawLowerTorso();
  void drawUpperTorso();
  void drawSpinalCord();   
};
#endif
