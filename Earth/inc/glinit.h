#ifndef _GL_INIT_H_
#define _GL_INIT_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <GL/gl.h>   
#include <GL/glut.h> 
#include "SOIL.h"
#include "utils.h"
#include "glentry.h"

using namespace std;

extern char * result_string_pointer; 	// used to print logs of image loading -- Externed from SOIL.h

extern GLuint gProgramCubeMap;			// used in program generation -- Externed from main.cpp
extern GLuint gTextureCubeMap;			// used in texture generation -- Externed from main.cpp

extern GLuint gProgramEarthShader;
extern GLuint gEarthTextureMap;
extern GLuint gEarthTextureColorMap;

void initShaders();

void initTextures();

void initCubeMap(GLuint *, GLuint *);

#endif
