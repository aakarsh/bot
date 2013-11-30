#include "util.h"

#include <GL/glut.h>

float rotationX = 0;
float rotationY = 0;

// Some camera functions are provided for viewing convinience


void RotateView(float deltaX, float deltaY) {
	
}

// Draw string at specified coords x, y, z
void draw_string( float x, float y, float z, char *txt ) {
  //       glColor3f(1.0,1.0,1.0);
	glRasterPos3f(x, y, z);
	while(*txt != '\0') {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *txt);
		txt++;
	}
}

// Draw axis, w/specified length
void draw_axis(float len) {
	// Axis colors
	float c[4][3] = {
	{len, 0, 0},
	{0, len, 0},
	{0, 0, len},
	{len, len, len}};

	// Direction labels
	char* txt[3] = {"+X",
                        "+Y",
                        "+Z"};

	// Vertex positions  
	float v[4][3] = {
	{len, 0, 0},
	{0, len, 0},
	{0, 0, len},
	{0, 0, 0}};

	glPushMatrix();

		// Draw axis
		for( int i = 0; i < 3; i++ ) {		
			glBegin( GL_LINES );
				glColor3fv(c[i]);
				glVertex3fv(v[i]);
				glColor3fv(c[3]);
				glVertex3fv(v[3]);
			glEnd();
		}

		glColor3f(1, 1, 1);

		draw_string(v[0][0], v[0][1], v[0][2], txt[0]);
		draw_string(v[1][0], v[1][1], v[1][2], txt[1]);
		draw_string(v[2][0], v[2][1], v[2][2], txt[2]);
  
	glPopMatrix();
}
