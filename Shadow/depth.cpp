#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <GL/gl.h>   
#include <GL/glut.h>
#include "glentry.h"
#include <sstream>
#include <pthread.h>
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

struct ONLight{
	float lightPos[3];
	float ambientCoef[3];
	float diffuseCoef[3];
	float specularCoef[3];
};

struct ONCamera{
	float camPosition[3];
	float camUpVector[3];
};

struct ONMaterial{
	float ambientCoef[4];
	float diffuseCoef[4];
	float specularCoef[4];
	float specularExp;
};

struct ONTransformation{
	float translation[3];
	float rotation[3];
	float scale[3];
};

int triangleCount;
string stageFileName;
string objectFileName;

ONLight light;
ONCamera cam;
ONMaterial objMat, stageMat, boxMat;
ONTransformation stageTrans, objTrans;

GLuint vboBoxVertex;
GLuint vboBoxNormal;
GLuint vboBoxIndex;

GLuint objVertices;
GLuint objNormals;
GLuint objIndices;

GLuint stgVertices;
GLuint stgNormals;
GLuint stgIndices;

vector<glm::vec4> objVerticesVec;
vector<glm::vec3> objNormalsVec;
vector<GLushort> objElementsVec;

vector<glm::vec4> stgVerticesVec;
vector<glm::vec3> stgNormalsVec;
vector<GLushort> stgElementsVec;


vector<glm::vec3> BoxVertCoords;
vector<glm::vec3> BoxNormCoords;
vector<GLushort> VertIndices;

GLuint gObjectsProgram;

GLuint FBO; // frame buffer object
unsigned int depthTexture;

int _w = 640, _h = 480;

float modelview[16];
float modelview0[16];
float modelview1[16];
float modelview2[16];
float projection[16];

void junk() {
  int i;
  i=pthread_getconcurrency();
};

void reshape(int w, int h) 
{
	_h = h;
	_w = w;
	
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    glViewport(0, 0, w, h);
}

void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements) {
	ifstream in(filename, ios::in);
	if(!in) {
		cerr << "Cannot open "<< filename<<endl; 
		exit(1);
	}
	
	string line;
	while(getline(in, line)) {
		if(line.substr(0,2) == "v ") {
			istringstream s(line.substr(2));
			glm::vec4 v;
			s>>v.x; s>>v.y; s>>v.z; v.w=1.0f;
			vertices.push_back(v);			
		} else if(line.substr(0,2) == "f ") {
			istringstream s(line.substr(2));
			GLushort a, b, c;
			s>>a; s>>b; s>>c;
			a--; b--; c--;
			elements.push_back(a);
			elements.push_back(b);
			elements.push_back(c);
		}
		else {/* ignoring this line */}
	}
	
	normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	for(int i = 0 ; i < elements.size() ; i += 3) {
		GLushort ia = elements[i];
		GLushort ib = elements[i+1];
		GLushort ic = elements[i+2];
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
		normals[ia] = normals[ib] = normals[ic] = normal;		        
	}
}

void readFile(char* fileName){
	ifstream inpFile(fileName);
	string line;
	vector<glm::vec3> tempBoxVertCoords;
	
	while (getline(inpFile, line))
	{
		if (line == "#Light"){
			inpFile >> light.lightPos[0] >> light.lightPos[1] >> light.lightPos[2];
			inpFile >> light.ambientCoef[0] >> light.ambientCoef[1] >> light.ambientCoef[2];
			inpFile >> light.diffuseCoef[0] >> light.diffuseCoef[1] >> light.diffuseCoef[2];
			inpFile >> light.specularCoef[0] >> light.specularCoef[1] >> light.specularCoef[2];
		}

		else if (line == "#Camera"){
			inpFile >> cam.camPosition[0] >> cam.camPosition[1] >> cam.camPosition[2];
			inpFile >> cam.camUpVector[0] >> cam.camUpVector[1] >> cam.camUpVector[2];
		}
		
		else if (line == "#MaterialStage"){
			inpFile >> stageMat.ambientCoef[0] >> stageMat.ambientCoef[1] >> stageMat.ambientCoef[2] >> stageMat.ambientCoef[3];
			inpFile >> stageMat.diffuseCoef[0] >> stageMat.diffuseCoef[1] >> stageMat.diffuseCoef[2] >> stageMat.diffuseCoef[3];
			inpFile >> stageMat.specularCoef[0] >> stageMat.specularCoef[1] >> stageMat.specularCoef[2] >> stageMat.specularCoef[3];
			inpFile >> stageMat.specularExp;
		}

		else if (line == "#MaterialObject"){
			inpFile >> objMat.ambientCoef[0] >> objMat.ambientCoef[1] >> objMat.ambientCoef[2] >> objMat.ambientCoef[3];
			inpFile >> objMat.diffuseCoef[0] >> objMat.diffuseCoef[1] >> objMat.diffuseCoef[2] >> objMat.diffuseCoef[3];
			inpFile >> objMat.specularCoef[0] >> objMat.specularCoef[1] >> objMat.specularCoef[2] >> objMat.specularCoef[3];
			inpFile >> objMat.specularExp;
		}

		else if (line == "#MaterialBox"){
			inpFile >> boxMat.ambientCoef[0] >> boxMat.ambientCoef[1] >> boxMat.ambientCoef[2] >> boxMat.ambientCoef[3];
			inpFile >> boxMat.diffuseCoef[0] >> boxMat.diffuseCoef[1] >> boxMat.diffuseCoef[2] >> boxMat.diffuseCoef[3];
			inpFile >> boxMat.specularCoef[0] >> boxMat.specularCoef[1] >> boxMat.specularCoef[2] >> boxMat.specularCoef[3];
			inpFile >> boxMat.specularExp;
		}

		else if (line == "#Box"){
			for (int i = 0; i < 8; i++)
			{
				float vertCoordsX, vertCoordsY, vertCoordsZ;
				inpFile >> vertCoordsX >> vertCoordsY >> vertCoordsZ;
				tempBoxVertCoords.push_back(glm::vec3(vertCoordsX, vertCoordsY, vertCoordsZ));
			}

			inpFile >> triangleCount;

			for (int i = 0; i < triangleCount; i++)
			{
				int a, b, c;
				inpFile >> a >> b >> c;
				BoxVertCoords.push_back(tempBoxVertCoords[a]);
				BoxVertCoords.push_back(tempBoxVertCoords[b]);
				BoxVertCoords.push_back(tempBoxVertCoords[c]);
				
				// normal calculation
				glm::vec3 atoc = tempBoxVertCoords[c] - tempBoxVertCoords[a];
				glm::vec3 atob = tempBoxVertCoords[b] - tempBoxVertCoords[a];
				glm::vec3 norm = glm::normalize(glm::cross(atob, atoc));
				
				BoxNormCoords.push_back(norm);
				BoxNormCoords.push_back(norm);
				BoxNormCoords.push_back(norm);
			}
			
			for(GLushort i = 0; i<triangleCount*3; i++)
					VertIndices.push_back(i);
		}

		else if (line == "#StageFile"){
			inpFile >> stageFileName;
		}

		else if (line == "#ObjectFile"){
			inpFile >> objectFileName;
		}

		else if (line == "#StageTransformation"){
			inpFile >> stageTrans.translation[0] >> stageTrans.translation[1] >> stageTrans.translation[2];
			inpFile >> stageTrans.rotation[0] >> stageTrans.rotation[1] >> stageTrans.rotation[2];
			inpFile >> stageTrans.scale[0] >> stageTrans.scale[1] >> stageTrans.scale[2];
		}

		else if (line == "#ObjectTransformation"){
			inpFile >> objTrans.translation[0] >> objTrans.translation[1] >> objTrans.translation[2];
			inpFile >> objTrans.rotation[0] >> objTrans.rotation[1] >> objTrans.rotation[2];
			inpFile >> objTrans.scale[0] >> objTrans.scale[1] >> objTrans.scale[2];
		}

	}
	inpFile.close();
}


unsigned int createTexture(int w,int h,bool isDepth=false)
{
	unsigned int textureId;
	glGenTextures(1,&textureId);
	glBindTexture(GL_TEXTURE_2D,textureId);
	glTexImage2D(GL_TEXTURE_2D,0,(!isDepth ? GL_RGBA8 : GL_DEPTH_COMPONENT),w,h,0,isDepth ? GL_DEPTH_COMPONENT : GL_RGBA,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
	
	int i;
	i=glGetError();
	if(i!=0)
	{
		std::cout << "Error happened while loading the texture: " << i << std::endl;
	}
    glBindTexture(GL_TEXTURE_2D,0);
	return textureId;
}

/// Print out the information log for a shader object 
/// @arg obj handle for a program object
static void printProgramInfoLog(GLuint obj)
{
	GLchar infoLog[500];
	GLint charsWritten = 0;
	GLint infologLength = 0;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
	printf("%s\n",infoLog);
}

void readShader(const char *fname, char *source)
{
     FILE *fp;
     fp = fopen(fname,"r");
     if (fp==NULL)
     {
         fprintf(stderr,"The shader file %s cannot be opened!\n",fname);
         exit(1);
     }
     char tmp[300];
     while (fgets(tmp,300,fp)!=NULL)
     {
           strcat(source,tmp);
           strcat(source,"\n");
     }
}

unsigned int loadShader(const char *source, unsigned int mode)
{
         unsigned int id;
         id = glCreateShader(mode);
                  
         glShaderSource(id,1,&source,NULL);
         
         glCompileShader(id);
         
         char error[1000];
         
         glGetShaderInfoLog(id,1000,NULL,error);
         printf("Compile status: \n %s\n",error);
         
         return id;         
}

void initObjectsShader()
{
	GLenum err = glewInit(); 
	char *vsSource,*fsSource;
    GLuint vs,fs;

    vsSource = (char *)malloc(sizeof(char)*20000);
    fsSource = (char *)malloc(sizeof(char)*20000);

    vsSource[0]='\0';
    fsSource[0]='\0';
    
    gObjectsProgram = glCreateProgram();

    readShader("objects.vs",vsSource);
    readShader("objects.frag",fsSource);

    vs = loadShader(vsSource,GL_VERTEX_SHADER);
    fs = loadShader(fsSource,GL_FRAGMENT_SHADER);
    printProgramInfoLog(gObjectsProgram);   // verifies if all this is ok so far

    glAttachShader(gObjectsProgram, vs);
    glAttachShader(gObjectsProgram, fs);
    printProgramInfoLog(gObjectsProgram);   // verifies if all this is ok so far

    glLinkProgram(gObjectsProgram);
    printProgramInfoLog(gObjectsProgram);   // verifies if all this is ok so far

}

void getDepthImage()
{
	glViewport(0,0,2048,2048);
		
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);    
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // switch to modelview matrix for viewing and transformation process.
    glLoadIdentity();


	gluLookAt(light.lightPos[0], light.lightPos[1], light.lightPos[2], 0, 0, 0, cam.camUpVector[0], cam.camUpVector[1], cam.camUpVector[2]);
	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	
	glUseProgram(0);
    
    // box
    glPushMatrix();

		glGetFloatv(GL_MODELVIEW_MATRIX, modelview0);    
		glBindBuffer(GL_ARRAY_BUFFER, vboBoxVertex);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(3, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vboBoxNormal);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBoxIndex);
		glDrawElements(GL_TRIANGLES, VertIndices.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();
   

    // object    	
	glPushMatrix();
	
		glTranslatef(objTrans.translation[0], objTrans.translation[1], objTrans.translation[2]);
		glRotatef(objTrans.rotation[0], 1, 0, 0);
		glRotatef(objTrans.rotation[1], 0, 1, 0);
		glRotatef(objTrans.rotation[2], 0, 0, 1);
		glScalef(objTrans.scale[0], objTrans.scale[1], objTrans.scale[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview1);
		glBindBuffer(GL_ARRAY_BUFFER, objVertices);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(4, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, objNormals);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objIndices);
		glDrawElements(GL_TRIANGLES, objElementsVec.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();
	
	// stage
	glPushMatrix();
		
		glTranslatef(stageTrans.translation[0], stageTrans.translation[1], stageTrans.translation[2]);
		glRotatef(stageTrans.rotation[0], 1, 0, 0);
		glRotatef(stageTrans.rotation[1], 0, 1, 0);
		glRotatef(stageTrans.rotation[2], 0, 0, 1);
		glScalef(stageTrans.scale[0], stageTrans.scale[1], stageTrans.scale[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, modelview2);
		glBindBuffer(GL_ARRAY_BUFFER, stgVertices);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(4, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, stgNormals);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stgIndices);
		glDrawElements(GL_TRIANGLES, stgElementsVec.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();
	
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
 	
    glFlush();
    glFinish();

    glBindFramebuffer(GL_FRAMEBUFFER,0);

	glUseProgram(0);

}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
 	glMatrixMode(GL_MODELVIEW); // switch to modelview matrix for viewing and transformation process.
	glLoadIdentity(); // clear current matrix by setting it to the identity matrix.
	
	glMatrixMode(GL_PROJECTION);	// switch to projection matrix for adjusting the camera projection. 
	glLoadIdentity();	// clear current matrix by setting it to the identity matrix.
    glOrtho(-10,10, -10,10, 1, 25);	// set projection of camera. you should set the camera parameters with respect to the camera parameters in scene file (Hint: glOrtho, glFrustum).
    
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	getDepthImage();
	
	glMatrixMode(GL_PROJECTION);	// switch to projection matrix for adjusting the camera projection. 
	glLoadIdentity();	// clear current matrix by setting it to the identity matrix.
    gluPerspective(50,640.0/480.0,1,1000);	// set projection of camera. you should set the camera parameters with respect to the camera parameters in scene file (Hint: glOrtho, glFrustum).
    
    glViewport(0, 0, _w, _h);
	glMatrixMode(GL_MODELVIEW); // switch to modelview matrix for viewing and transformation process.
    glLoadIdentity();
        
    gluLookAt(cam.camPosition[0], cam.camPosition[1], cam.camPosition[2], 0, 0, 0, cam.camUpVector[0], cam.camUpVector[1], cam.camUpVector[2]);

	// display all object with color
	glUseProgram(gObjectsProgram);
    glUniform1f(glGetUniformLocation(gObjectsProgram, "isLight"), 1.0);
	glUniform3fv(glGetUniformLocation(gObjectsProgram, "lightPosition"), 1, light.lightPos);
	glUniform3fv(glGetUniformLocation(gObjectsProgram, "camPosition"), 1, cam.camPosition);
	
	glUniform3fv(glGetUniformLocation(gObjectsProgram, "Id"), 1, light.diffuseCoef);
	glUniform3fv(glGetUniformLocation(gObjectsProgram, "Is"), 1, light.specularCoef);
	glUniform3fv(glGetUniformLocation(gObjectsProgram, "Iamb"), 1, light.ambientCoef);
    
	glUniformMatrix4fv(glGetUniformLocation(gObjectsProgram, "projection"), 1, GL_FALSE, projection);
//	glUniformMatrix4fv(glGetUniformLocation(gObjectsProgram, "modelview"), 1, GL_FALSE, modelview);
	
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,depthTexture);
    glUniform1i(glGetUniformLocation(gObjectsProgram,"depth"),0);

	// light
    glPushMatrix();
		glTranslatef(light.lightPos[0], light.lightPos[1], light.lightPos[2]);
		glBegin(GL_QUADS);
                glVertex3f(-0.05,0.05,0);    		
                glVertex3f(-0.05,-0.05,0);
                glVertex3f(0.05,-0.05,0);
                glVertex3f(0.05,0.05,0);
        glEnd();      
	glPopMatrix();
	
	glUniform1f(glGetUniformLocation(gObjectsProgram, "isLight"), 0.0);
 
    // box
    glPushMatrix();
 
 	glUniformMatrix4fv(glGetUniformLocation(gObjectsProgram, "modelview"), 1, GL_FALSE, modelview0);   
    glUniform4fv(glGetUniformLocation(gObjectsProgram, "kd"), 1, boxMat.diffuseCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ka"), 1, boxMat.ambientCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ks"), 1, boxMat.specularCoef);
	glUniform1f(glGetUniformLocation(gObjectsProgram, "specExp"), boxMat.specularExp);
	
		glBindBuffer(GL_ARRAY_BUFFER, vboBoxVertex);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(3, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, vboBoxNormal);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBoxIndex);
		glDrawElements(GL_TRIANGLES, VertIndices.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();
    // object    	
	glPushMatrix();
 	glUniformMatrix4fv(glGetUniformLocation(gObjectsProgram, "modelview"), 1, GL_FALSE, modelview1); 	
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "kd"), 1, objMat.diffuseCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ka"), 1, objMat.ambientCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ks"), 1, objMat.specularCoef);
	glUniform1f(glGetUniformLocation(gObjectsProgram, "specExp"), objMat.specularExp);
	
		glTranslatef(objTrans.translation[0], objTrans.translation[1], objTrans.translation[2]);
		glRotatef(objTrans.rotation[0], 1, 0, 0);
		glRotatef(objTrans.rotation[1], 0, 1, 0);
		glRotatef(objTrans.rotation[2], 0, 0, 1);
		glScalef(objTrans.scale[0], objTrans.scale[1], objTrans.scale[2]);

		glBindBuffer(GL_ARRAY_BUFFER, objVertices);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(4, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, objNormals);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objIndices);
		glDrawElements(GL_TRIANGLES, objElementsVec.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();
	
	// stage
	glPushMatrix();
 	glUniformMatrix4fv(glGetUniformLocation(gObjectsProgram, "modelview"), 1, GL_FALSE, modelview2); 	
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "kd"), 1, stageMat.diffuseCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ka"), 1, stageMat.ambientCoef);
	glUniform4fv(glGetUniformLocation(gObjectsProgram, "ks"), 1, stageMat.specularCoef);
	glUniform1f(glGetUniformLocation(gObjectsProgram, "specExp"), stageMat.specularExp);
	
		glTranslatef(stageTrans.translation[0], stageTrans.translation[1], stageTrans.translation[2]);
		glRotatef(stageTrans.rotation[0], 1, 0, 0);
		glRotatef(stageTrans.rotation[1], 0, 1, 0);
		glRotatef(stageTrans.rotation[2], 0, 0, 1);
		glScalef(stageTrans.scale[0], stageTrans.scale[1], stageTrans.scale[2]);

		glBindBuffer(GL_ARRAY_BUFFER, stgVertices);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glVertexPointer(4, GL_FLOAT, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, stgNormals);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stgIndices);
		glDrawElements(GL_TRIANGLES, stgElementsVec.size(), GL_UNSIGNED_SHORT, 0);

	glPopMatrix();

	glutSwapBuffers(); // swap buffers in a double buffered scene.
 	
    glFlush();
    glFinish();
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);       

	glutSwapBuffers(); // swap buffers in a double buffered scene.
}

/**************************************** myInit() ******************/

void myInit()
{
	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glShadeModel(GL_SMOOTH);
	depthTexture=createTexture(2048,2048,true);
	initObjectsShader();
    
    glGenBuffers(1, &objVertices);
    glGenBuffers(1, &objNormals);
    glGenBuffers(1, &objIndices);    

    glGenBuffers(1, &stgVertices);
    glGenBuffers(1, &stgNormals);
    glGenBuffers(1, &stgIndices);    
    
    glGenBuffers(1, &vboBoxVertex);
    glGenBuffers(1, &vboBoxNormal);
    glGenBuffers(1, &vboBoxIndex);
    
    // -----------object buffer arrays----------
	glBindBuffer(GL_ARRAY_BUFFER, objVertices);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec4)*objVerticesVec.size(), &objVerticesVec[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, objNormals);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*objNormalsVec.size(), &objNormalsVec[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*objElementsVec.size(), &objElementsVec[0], GL_STATIC_DRAW);    
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // -----------stage buffer arrays----------
    glBindBuffer(GL_ARRAY_BUFFER, stgVertices);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec4)*stgVerticesVec.size(), &stgVerticesVec[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, stgNormals);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*stgNormalsVec.size(), &stgNormalsVec[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stgIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*stgElementsVec.size(), &stgElementsVec[0], GL_STATIC_DRAW);    
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // ----------box buffer arrays------------
    glBindBuffer(GL_ARRAY_BUFFER, vboBoxVertex);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*BoxVertCoords.size(), &BoxVertCoords[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboBoxNormal);
    glBufferData(GL_ARRAY_BUFFER,sizeof(glm::vec3)*BoxNormCoords.size(), &BoxNormCoords[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBoxIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLushort)*VertIndices.size(), &VertIndices[0], GL_STATIC_DRAW);    
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
  	glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST); // enables depth buffering.
	
	
	glGenFramebuffers(1,&FBO);
	glBindFramebuffer(GL_FRAMEBUFFER,FBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTexture,0);

	int i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(i!=GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is not OK, status=" << i << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	glClearColor( 0.0, 0.0, 0.0, 1.0f ); // set initial value of color buffer (you can set background color here).
	
	glMatrixMode(GL_PROJECTION);	// switch to projection matrix for adjusting the camera projection. 
	glLoadIdentity();	// clear current matrix by setting it to the identity matrix.
    gluPerspective(50,640.0/480.0,1,1000);	// set projection of camera. you should set the camera parameters with respect to the camera parameters in scene file (Hint: glOrtho, glFrustum).
    
 	glMatrixMode(GL_MODELVIEW); // switch to modelview matrix for viewing and transformation process.

}

void specialKeys(int key, int x, int y){
	if (key == GLUT_KEY_UP) cam.camPosition[1] += 0.1;
	else if (key == GLUT_KEY_DOWN) cam.camPosition[1] -= 0.1;
	else if (key == GLUT_KEY_LEFT) cam.camPosition[0] -= 0.1;
	else if (key == GLUT_KEY_RIGHT) cam.camPosition[0] += 0.1;
	glutPostRedisplay();
}

void processNormalKeys(unsigned char key, int x, int y) {
	
	if (key == 27) 
	{
		exit(0);
	}
	if (key == 'w' || key == 'W' )
	{
		light.lightPos[1] += 0.2;
	}
	else if(key == 's' || key == 'S')
	{
		light.lightPos[1] -= 0.2;
	}
	else if(key == 'a' || key == 'A')
	{
		light.lightPos[0] -= 0.2;		
	}
	else if(key == 'd' || key == 'D')
	{
		light.lightPos[0] += 0.2;
	}
	else if(key == 't' || key == 'T')
	{
		light.lightPos[2] += 0.2;		
	}
	else if(key == 'y' || key == 'Y')
	{
		light.lightPos[2] -= 0.2;
	}
	
	else if(key == 'n' || key == 'N')
	{
		cam.camPosition[2] += 0.5;
	}
	
	else if(key == 'm' || key == 'M')
	{
		cam.camPosition[2] -= 0.5;
	}
	else if(key == 'o' || key == 'O')
	{
		stageTrans.rotation[1] -= 0.5;
		objTrans.rotation[1] -= 0.5;
	}
	
	else if(key == 'p' || key == 'P')
	{
		stageTrans.rotation[1] += 0.5;
		objTrans.rotation[1] += 0.5;
	}
     
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{	
	// Initialize GLUT and create window.	
	junk();
	if(argc != 2){
		cout << "File name missed!"<<endl;
		exit(0);
	}
	
	readFile(argv[1]);
	load_obj(objectFileName.c_str(), objVerticesVec, objNormalsVec, objElementsVec);
    load_obj(stageFileName.c_str(), stgVerticesVec, stgNormalsVec, stgElementsVec);

	glutInit(&argc,argv);	// glut initialization function
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);	// sets display mode.
	glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    init_glentry();

	glutInitWindowSize( 640, 480 );	// set window size. you should use the input file parameters for setting.

	glutCreateWindow( "ceng477 - HW4 (Onur Can UNER)" );
	glutDisplayFunc( myGlutDisplay );	// set display function.
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(specialKeys);
	glutReshapeFunc(reshape);

	myInit();
	
	glutMainLoop();	// start main loop after initialization.?

}

