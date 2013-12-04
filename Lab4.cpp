#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <vector>

#include "ppm_canvas.h"
#include "util.h"
#include "terrain.h"
#include "android.h"

using namespace std;

#define PI 3.14159265

float xrot, yrot = 0;
bool keyState[256] = { false };


int windowWidth = 1000, windowHeight = 900;


canvas_t skin;
GLuint textureId;


int   mouseX =  0 ;
int   mouseY = 0;
bool mouseLeftDown = false;
bool mouseRightDown = false;

double cameraDistance = 0;
double cameraAngleX = 30;
double cameraAngleY = 90;

bool adjustEye;

float diffuseColour[3];
float ambientColour[3];

bool firstPersonMode = true;

class Environment{
public:
  Light* light;  
  canvas_t height;
  canvas_t texture;  
  Terrain* terrain;
  
  vector<android*> droids;
  
  bool animate;
  int key_frame;
  animation_mode  current_animation;
  int camera_angle;
  int bot_count;
  static const int MAX_KEY_FRAME = 10000000;

  Environment(char* height_file, char* texture_file):
    animate(false),
    key_frame(0),
    current_animation(WALKING),
    camera_angle(0),
    bot_count(20){
    terrain_setup(height_file,texture_file);    
  }

  void terrain_setup(char* height_file, char* texture_file);
  void drawAndroids();
  
};


Environment* env;
//BOT Glboal variables

int pmouse_x;
int pmouse_y;


bool env_first = false;

void Environment::terrain_setup(char* height_file, char* texture_file){
  printf("Loading file '%s'... ", height_file);
  ppmLoadCanvas(height_file, &height);  
  printf("Done.\n");
  printf("Loading file '%s'... ", texture_file);
  ppmLoadCanvas(texture_file, &texture);          
  terrain = new Terrain(windowWidth,windowHeight,texture,height);
  for(int j = 0 ; j < bot_count ; j++) {
    android* droid = new android(bot_count%10 +j*10 ,0,0);
    droid->walk_direction.x = rand();
    droid->walk_direction.z = rand();
    droid->walk_direction.normalize();
    droids.push_back(droid);
    
  }
  
  for(int i = 0 ; i < droids.size(); i ++){
    terrain->add_stuff(droids[i]);
  }  
}

void Environment::drawAndroids(){
    if( droids.size() <= bot_count ){
      for(int i  = droids.size(); i < bot_count ; i++) {       
        android* a = new android((i*6)% 16,0,i/3*8);
        droids.push_back(a);
      }
    }  

    if(animate)
      key_frame++;
  
    for(int j = 0; j < droids.size(); j++) {       
      droids[j]->mode = current_animation;  
      droids[j]->animate(key_frame);
      droids[j]->draw();
    }
  }
      



void cb_idle() {	  
  env->terrain->drawTerrain();
  glutPostRedisplay();  
}

void cb_display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  env->terrain->drawTerrain();    
  glFlush();
  glutSwapBuffers(); // for smoother animation
}


void cb_reshape(int w, int h) {
  env->terrain->camera->setup_perspective(w,h);
}

void cb_mouseclick(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;
    //here we record the button click and state

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)          
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
} 

void cb_mousemove(int x, int y)
{
  double dx = (x - mouseX);
  double dy = (y - mouseY);
  
  if(mouseLeftDown)
    {
      cameraAngleY += dx;
      cameraAngleX += dy;
      mouseX = x;
      mouseY = y;
    }
  if(mouseRightDown)
    {
      cameraDistance -= dy * 0.2f;
      mouseY = y;
    }
  env->terrain->mouseChange(mouseLeftDown,x,y,dx,dy);
}

void display_help() {
  cout<<"Use any of the following commands while painting"<<endl;
  cout<<"q - Quit the pane "<<endl;
  cout<<"h - display this message "<<endl;
  cout<<"l - change move lighting "<<endl;
  cout<<"w - move camera forward "<<endl;
  cout<<"s - move camera backward "<<endl;
  cout<<"p - toggle camera panning "<<endl;
  cout<<"0 - reset near and far plane and field of view "<<endl;
  cout<<"g - increase near plane "<<endl;
  cout<<"G - decrease near plane "<<endl;
  cout<<"f - increase far plane "<<endl;
  cout<<"F - decrease far plane "<<endl;
  cout<<"v - increase field of view "<<endl;
  cout<<"V - Decrease field of view "<<endl;
  cout<<"n - toggle between vertex and face normals "<<endl;
  cout<<"+ - scale in  "<<endl;
  cout<<"- - scale out  "<<endl;
  cout<<"x,y,z,X,Y,Z - adjust eye location"<<endl;
}

void cb_keyboard(unsigned char key, int x, int y) {
 float inc[] = {10.0,0.0,0.0};
 double eyeDirection[3] = {0,0,0};
 double v [3] ;
 Vec3d* vec;
 Camera * camera = env->terrain->camera;
 
 switch(key) {
 case 'h':
   display_help();
 case 'd':
   env->terrain->toggleDisplayList();
 case 'l':    
   env->terrain->toggleMouseLighting();
   break;
 case 'w':  
   env->terrain->camera->forward(20);
   break;
 case 's': 
   env->terrain->camera->backward(20); 
   break;
 case 'p':
   env->terrain->toggleMousePanning();
   break;
 case '0':
   env->terrain->camera->zNear=.1; 
   env->terrain->camera->zFar=1000; 
   env->terrain->camera->fov = 90;
   break;
 case 'g':
   env->terrain->camera->zNear+=50;
   break;
 case 'G':
   env->terrain->camera->zNear-=10;
   break;
 case '1':
   env->terrain->toggleMouseMode(FIRST_PERSON);
   break;
 case 'f':
   env->terrain->camera->zFar+=200;
   cout<<"Far clipping plane set to "<<env->terrain->camera->zFar<<endl;
   break;
 case 'F':
   env->terrain->camera->zFar-=200;
   cout<<"Far clipping plane set to "<<env->terrain->camera->zFar<<endl;
   break;
 case 'v':
   env->terrain->camera->fov +=20;
   cout<<"Field of View set to  "<<env->terrain->camera->fov<<endl;
   break;
 case 'V':
   env->terrain->camera->fov -=20;
   cout<<"Field of View set to  "<<env->terrain->camera->fov<<endl;
   break;
 case 'e':
   adjustEye = !adjustEye;
   break;
 case 't':
   env->terrain->toggleTexture();
   break;
 case 'n':
   env->terrain->toggleSurfaceNormals();
   break;
  case '+': 
    env->terrain->scale_factor*=1.1;
    break;
  case '-': 
    env->terrain->scale_factor/=1.1;
    break;
 case 'x':
   camera->eyeLocation[0]+=10;
   break;
 case 'X':
   camera->eyeLocation[0]-=10;
   break;
 case 'y':
   camera->eyeLocation[1]+=10;
   break;
 case 'Y':
   camera->eyeLocation[1]-=10;
   break;
 case 'z':
   camera->eyeLocation[2]+=10;
   break;
 case 'Z':
   camera->eyeLocation[2]-=10;
   break;
 case 'q':
   exit(0);
   break;
 }
}


int main(int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Lab 3: Terrain Mapping");
	glutIgnoreKeyRepeat(true);
	glEnable(GL_TEXTURE_2D);
        
	if (argc == 3) {
          env = new Environment(argv[1] , // height file
                                argv[2]);  // texture file          
	} else {
          printf("Usage: %s terrain.ppm texture.ppm\n", argv[0]);
          return 1;
	}
        
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable (GL_DEPTH_TEST);


      //      glEnable(GL_TEXTURE_2D);

        // an commented
        glEnable(GL_LIGHTING);
        
        glEnable(GL_COLOR_MATERIAL);
        
        glutMouseFunc(cb_mouseclick);
        glutMotionFunc(cb_mousemove);

	glutDisplayFunc(cb_display);
        glutIdleFunc(cb_idle);
        
	glutReshapeFunc(cb_reshape);
        glutKeyboardFunc(cb_keyboard);	

	glClearColor(0,0,1,0); // set background to black

	glutMainLoop();
	return 0;
}
