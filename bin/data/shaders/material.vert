#version 150
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 texCoordVarying;

void main()
{
    mat4 persp = mat4(2.386330625391558,0.1067743374281633,0,0.0002129695203333939,
                      -0.4006221683509157,1.865471585905073,0,-0.0004165191258319149,
                      0.0,0.0,1.0,0.0,
                      757.7378857625365,337.6038732873912,0,1.0);
    
//    [2.386330625391558, -0.4006221683509157, 757.7378857625365;
//     0.1067743374281633, 1.865471585905073, 337.6038732873912;
//     0.0002129695203333939, -0.0004165191258319149, 1]
    texCoordVarying = texcoord;
    gl_Position = modelViewProjectionMatrix*position;
}
