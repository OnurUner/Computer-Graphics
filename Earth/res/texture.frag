#version 120


//-----------
varying vec2 texture_coordinate;
uniform sampler2D texturemap;
varying vec4 color;
//---------------
void main()
{

	//----------------------------------------------------
    vec4 cx = texture2D(texturemap, texture_coordinate);
    
    gl_FragColor = cx * color;
    //----------------------------------------------------
}
