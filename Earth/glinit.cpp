#include "glinit.h"


void initTextures()
{	
	int width, height, channels = 1;
	
	// TODO: Generate texture map and height map for the Earth
	gEarthTextureMap = SOIL_load_OGL_texture("res/texturemap.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glBindTexture(GL_TEXTURE_2D, gEarthTextureMap);
	glActiveTexture(GL_TEXTURE0);

	gEarthTextureColorMap = SOIL_load_OGL_texture("res/heightmap.jpg", SOIL_LOAD_L, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS );
	glBindTexture(GL_TEXTURE_2D, gEarthTextureColorMap);
	glActiveTexture(GL_TEXTURE1);

	glGenerateMipmap(GL_TEXTURE_2D);
		
	// Generate CubeMap
	glGenTextures(1, &gTextureCubeMap);
	glActiveTexture(GL_TEXTURE2);
	
	glBindTexture(GL_TEXTURE_2D, gTextureCubeMap);
	gTextureCubeMap = SOIL_load_OGL_single_cubemap("res/cubemap.jpg", "WNESUD", SOIL_LOAD_AUTO, gTextureCubeMap, SOIL_FLAG_MIPMAPS);
	
	printf("CubeMap Info: %s\n", result_string_pointer);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void initShaders()
{
	// Creation and initialization of CubeMap's shaders
	gProgramCubeMap = glCreateProgram();
	
	glAttachShader(gProgramCubeMap, createVS("res/cubemap.vert"));
	glAttachShader(gProgramCubeMap, createFS("res/cubemap.frag"));

	glLinkProgram(gProgramCubeMap);

	glUniform1i(glGetUniformLocation(gProgramCubeMap, "cubeMap")   , 2);

	// TODO: Create and initialize new program(s) to use new shader(s)
	gProgramEarthShader = glCreateProgram();

	glAttachShader(gProgramEarthShader, createVS("res/texture.vert"));
	glAttachShader(gProgramEarthShader, createFS("res/texture.frag"));
	
	glLinkProgram(gProgramEarthShader);

	GLint uniformHeightMap = glGetUniformLocation(gProgramEarthShader, "heightmap");
    GLint uniformTextMap = glGetUniformLocation(gProgramEarthShader, "texturemap");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gEarthTextureMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,gEarthTextureColorMap);
    	
	glUniform1i(uniformTextMap, 0);
	glUniform1i(uniformHeightMap, 1);
	    
}


void initCubeMap(GLuint *verticesVBO, GLuint *indicesVBO) 
{
	float vertices[] = {
		  1,  1,  1,				
		 -1,  1,  1,
		 -1, -1,  1,
		  1, -1,  1,
		  1, -1, -1,
		 -1, -1, -1,
		 -1,  1, -1,
		  1,  1, -1
	};

	GLuint indices[] = {
		0, 1, 2, 3,                 // Front face of the cube
		7, 4, 5, 6,                 // Back face of the cube
		6, 5, 2, 1,                 // Left face of the cube
		7, 0, 3, 4,                 // Right face of the cube
		7, 6, 1, 0,                 // Top face of the cube
		3, 2, 5, 4,                 // Bottom face of the cube
	};
	
	glGenBuffers(1, verticesVBO);
	glGenBuffers(1, indicesVBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, *verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
}

