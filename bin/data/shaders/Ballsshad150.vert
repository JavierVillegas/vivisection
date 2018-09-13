#version 150
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
out vec2 Pointcord;


void main()
{
	  gl_PointSize  = 60.0;
    vec4 temppos= position;
    Pointcord = temppos.xy;
    temppos.z = 0.0;
  //  gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = modelViewProjectionMatrix*temppos;
} 

