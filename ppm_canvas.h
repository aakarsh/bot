/********
*
*  ppm_canvas.h
*
*  Author: Adam Smith
*  Date of Version: October 8, 2005
*
*********/

//#pragma once

#ifndef PPM_H
#define PPM_H
typedef unsigned int pixel_t;

typedef struct canvas_t {
	int width;
	int height;
	pixel_t *pixels;
} canvas_t;


#define PIXEL(c,x,y)	((c).pixels[(x)+(c).width*(y)])

#ifdef __BIG_ENDIAN__
#		define RED(x)		(((x)>>24)&0xFF)
#		define GREEN(x)	(((x)>>16)&0xFF)
#		define BLUE(x)	(((x)>> 8)&0xFF)
#else
#		define RED(x)		(((x)>> 0)&0xFF)
#		define GREEN(x)	(((x)>> 8)&0xFF)
#		define BLUE(x)	(((x)>>16)&0xFF)
#endif

/* these functions return 0 if everything is OK */
int ppmLoadCanvas(char *filename, struct canvas_t *canvas);
int ppmSaveCanvas(char *filename, struct canvas_t *canvas);
#endif
