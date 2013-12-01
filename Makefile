default: Lab4.cpp ppm_canvas.cpp ppm_canvas.h util.cpp util.h terrain.h terrain.cpp android.h android.cpp geometry.h geometry.cpp
	g++  Lab4.cpp geometry.h geometry.cpp android.cpp android.h terrain.cpp terrain.h ppm_canvas.cpp ppm_canvas.h util.cpp util.h -lGLEW -lglut -lX11 -lGL -lGLU   -o Lab4
	global -u	
	./Lab4 map.ppm skin.ppm 
