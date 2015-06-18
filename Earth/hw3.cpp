#include <string>
#include <iostream>
#include <math.h>
#include <GL/gl.h>   
#include <GL/glut.h>
#include "SOIL.h"
#include "glinit.h"
#include "utils.h"
#include "glentry.h"

// In order to handle the error mentioned for HW2.
#include <pthread.h>
void junk() {
  int i;
  i=pthread_getconcurrency();
};

using namespace std;

int toggleTexture = 1;
int toggleColor = 1;

class Point;
Point **surfacePoints;
int latCount, lonCount, rad;

GLuint gProgramCubeMap;					// program handler of the cube map with two attached shaders: CubeMap.vert, CubeMap.frag
GLuint gTextureCubeMap;					// texture handler for the cube map. 
GLuint gVerticesVBO, gIndicesVBO;		// Vertex Buffer Objects for the cubemap's indices and vertices.
GLfloat gDispLevel = 0;					// displacement level for the height map. Can be passed to the shader source


GLuint gProgramEarthShader;              // program handler for our shader
GLuint gEarthTextureMap;				// texture handler for the texture map.
GLuint gEarthTextureColorMap;			// texture handler for the height map.


// Camera's properties
float gCamAngle = 0.0f;
float gCamDirX  = 0.0f;
float gCamDirZ  = 0.0f;
float gCamDist  = 5.0f;

// Earth's properties
float gEarthAngle	 = 0.0f;			// rotation angle of the Earth around its own axis
float gEarthAngleSun = 0.0f;			// rotation angle of the Earth around Sun
float gEarthPosX     = 0.0f;
float gEarthPosZ     = -1.0f;

class Point{
public:
    float x, y, z;

    Point(){
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Point(float X, float Y, float Z){
        this->x = X;
        this->y = Y;
        this->z = Z;
    }

    Point operator*(const float& scaler){
        float cX = x * scaler;
        float cY = y * scaler;
        float cZ = z * scaler;

        return Point(cX, cY, cZ);
    }

    Point operator+(const Point& p){
        float cX = x + p.x;
        float cY = y + p.y;
        float cZ = z + p.z;

        return Point(cX, cY, cZ);
    }


    void print(){
        cout << this->x << " " << this->y << " " << this->z;
    }
};


void reshape(int w, int h) 
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(45, double(w)/h, 0.1f, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
		
void mouse(int button, int state, int x, int y)
{
    if (button == 3 && state == GLUT_UP)		 	// mouse wheel down
   		gCamDist -= 0.1f;
	else if (button == 4 && state == GLUT_UP)    	// mouse wheel up
    	gCamDist += 0.1f;
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 27:	// Escape
            exit(0); 
            break; 
        case 'a' :	// rotate camera around the Earth clockwise
			gCamAngle -= 0.01f;
			break;
		case 'd' :	// rotate camera around the Earth counterclockwise
			gCamAngle += 0.01f;
			break;
		case '-':	// decrease displacement level of the height map
			gDispLevel -= 0.01f;
			break;
		case '+':	// increase displacement level of the height map
			gDispLevel += 0.01f;
			break;
        case '1': // toggle colortmap
            toggleColor = 1-toggleColor; break;
        case '2': // toggle texture
            toggleTexture = 1-toggleTexture; break;
        default:  
            break;
    }
        switch(key) 
    {
        case 27:	// Escape
            exit(0); 
            break; 
        case '1': // toggle colortmap
            toggleColor = 1-toggleColor; break;
        case '2': // toggle texture
            toggleTexture = 1-toggleTexture; break;
        default:
            break;
    }
}

void init(int *argc, char** argv) 
{
    glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glShadeModel(GL_SMOOTH);
	
	initTextures();
    initShaders();      
    
	if (*argc < 3) {
		cout << "Missing argument(s)." << endl 
			 << "Usage: ./hw3 <# of Latitudes> <# of Longitudes> <radius of the Earth>" << endl;
			 
		exit(0);	
	}
	
	// initialize Cubemap
	initCubeMap(&gVerticesVBO, &gIndicesVBO);
    
	// read command line arguments
	float radius  = atof(argv[--*argc]);
	int   num_lon = atoi(argv[--*argc]);
	int   num_lat = atoi(argv[--*argc]);
	
	latCount = num_lat;
	lonCount = num_lon;
	rad = radius;
		
    // TODO: initialize the Earth.
    
    surfacePoints = new Point*[num_lat];
    for (int i = 0; i < num_lat; i++) {
        surfacePoints[i] = new Point[num_lon+1];
    }
    
    float decAmountX = 180.f / (float)(num_lat-1);
    float incAmountY = 360.f / (float)num_lon;
    
    for(int i = 0 ; i<num_lat ; i++){
		for(int j = 0; j<num_lon+1 ; j++){
			float aroundX = 90 - (i*decAmountX);
			float aroundY = 0  + (j*incAmountY);
			float angle1 = M_PI * aroundX / 180.f;
			float angle2 = M_PI * aroundY / 180.f;
			float x = radius * cos(angle1) * cos(angle2);
			float y = radius * sin(angle1);
			float z = -1 * radius * cos(angle1) * sin (angle2);
			
			Point p(x, y, z);
			surfacePoints[i][j] = p;
			//p.print();
			//cout << " | ";
		}
		//cout << endl;
	}
	
}

void display()
{	
    long startTime = getCurrentTime();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	
	glLoadIdentity();
	
	// calculations for the position of the Earth
	gEarthAngleSun += 0.002739f;
	gEarthPosX = sin(gEarthAngleSun) * 50;
	gEarthPosZ = cos(gEarthAngleSun) * 50;

	// calculations for the position and view direction of the camera
	gCamDirX = sin(gCamAngle);
	gCamDirZ = cos(gCamAngle);

	gluLookAt(gEarthPosX + gCamDirX*gCamDist, 0, gEarthPosZ + gCamDirZ*gCamDist, 
			  gEarthPosX                    , 0, gEarthPosZ, 
			  0.0f						    , 1, 0.0f);        
	
	// Stars -- Cubemap
	glPushMatrix();
		glScalef(300,100,300);
		glUseProgram(gProgramCubeMap);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVerticesVBO);
		glVertexPointer(3, GL_FLOAT, sizeof(float) * 3, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndicesVBO);
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT,  0);
	glPopMatrix();
		
	// Earth
	glPushMatrix();
		gEarthAngle += 1;
		glTranslatef(gEarthPosX, 0, gEarthPosZ);
		glRotatef(23.5f, 0, 0, 1);
		glRotatef(gEarthAngle, 0, 1, 0);
		
		// TODO: Display the Earth	
		glUseProgram(gProgramEarthShader);


		glUniform1i(glGetUniformLocation(gProgramEarthShader, "texturemap"), 1);
		glBindTexture(GL_TEXTURE_2D,gEarthTextureMap);
		glActiveTexture(GL_TEXTURE0);
		
		glUniform1i(glGetUniformLocation(gProgramEarthShader, "heightmap"), 1);
		glBindTexture(GL_TEXTURE_2D,gEarthTextureColorMap);
		glActiveTexture(GL_TEXTURE1);
	
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "heightConstant"), gDispLevel);
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "lightPositionX"), -gEarthPosX);
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "lightPositionZ"), -gEarthPosZ);
		
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "camPosX"), (gEarthPosX + gCamDirX*gCamDist));
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "camPosZ"), (gEarthPosZ + gCamDirZ*gCamDist));
		
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "earthAngle"), gEarthAngle);
		
		
		/*
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "lightPosition"), 0.0, 0.0, 0.0);
		
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "materialAmbient"), 0.25, 0.25, 0.25);
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "materialDiffuse"), 1.0, 1.0, 1.0);
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "materialSpecular"), 1.0, .0, 1.0);
		glUniform1f(glGetUniformLocation(gProgramEarthShader, "materialShininess"), 100.0);
		
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "lightAmbient"), 0.3, 0.3, 0.3);
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "lightDiffuse"), 1.0, 1.0, 1.0);
		glUniform3f(glGetUniformLocation(gProgramEarthShader, "lightSpecular"), 1.0, 1.0, 1.0);
		*/
				
		glBegin(GL_TRIANGLES);

		for(int i = 0; i < latCount-1; i++)
			for(int j = 0; j < lonCount; j++){
				float fi = (float)i;
				float fj = (float)j;
				glNormal3f(surfacePoints[i][j].x/rad, surfacePoints[i][j].y/rad, surfacePoints[i][j].z/rad);
				glTexCoord2f(fj/(float)lonCount,fi/(float)(latCount-1));
				glVertex3f( surfacePoints[i][j].x, surfacePoints[i][j].y, surfacePoints[i][j].z);
				
				glNormal3f(surfacePoints[i+1][j].x/rad, surfacePoints[i+1][j].y/rad, surfacePoints[i+1][j].z/rad);
				glTexCoord2f(fj/(float)lonCount,(fi+1)/(float)(latCount-1));
				glVertex3f( surfacePoints[i+1][j].x, surfacePoints[i+1][j].y, surfacePoints[i+1][j].z);
				
				glNormal3f(surfacePoints[i][j+1].x/rad, surfacePoints[i][j+1].y/rad, surfacePoints[i][j+1].z/rad);
				glTexCoord2f((fj+1)/(float)lonCount,fi/(float)(latCount-1));
				glVertex3f( surfacePoints[i][j+1].x, surfacePoints[i][j+1].y, surfacePoints[i][j+1].z);
				
				glNormal3f(surfacePoints[i+1][j].x/rad, surfacePoints[i+1][j].y/rad, surfacePoints[i+1][j].z/rad);
				glTexCoord2f(fj/(float)lonCount,(fi+1)/(float)(latCount-1));
				glVertex3f( surfacePoints[i+1][j].x, surfacePoints[i+1][j].y, surfacePoints[i+1][j].z);
				
				glNormal3f(surfacePoints[i+1][j+1].x/rad, surfacePoints[i+1][j+1].y/rad, surfacePoints[i+1][j+1].z/rad);
				glTexCoord2f((fj+1)/(float)lonCount,(fi+1)/(float)(latCount-1));
				glVertex3f( surfacePoints[i+1][j+1].x, surfacePoints[i+1][j+1].y, surfacePoints[i+1][j+1].z);
				
				glNormal3f(surfacePoints[i][j+1].x/rad, surfacePoints[i][j+1].y/rad, surfacePoints[i][j+1].z/rad);
				glTexCoord2f((fj+1)/(float)lonCount,fi/(float)(latCount-1));
				glVertex3f( surfacePoints[i][j+1].x, surfacePoints[i][j+1].y, surfacePoints[i][j+1].z);

			}
		glEnd();	
		
	glPopMatrix();
	
	// Sun
	glPushMatrix();
		glUseProgram(0); // 0 means using no program		
		glColor3f(1,1,0);
		glutSolidSphere(1, 20, 20);
		
		// BONUS: Add some nice shader effects, textures etc. for the Sun
	
	glPopMatrix();
	
	glutSwapBuffers();
	
	// Set FPS to 100
	long diff = getCurrentTime() - startTime;
	if (diff < 10000)
        usleep(10000 - diff);
}

int main(int argc, char** argv)   
{	
    // Hack. Handle the error OpenGL - NVidia consistency error.
	junk();
	
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("20141 - CEng477 - HW3");

    init_glentry();
    init(&argc, argv);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(glutPostRedisplay);

    glutMainLoop();

    return 0;
}

