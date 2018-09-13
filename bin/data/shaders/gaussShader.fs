#version 150
uniform sampler2D tex1;
uniform float var;
in vec2 texCoordVarying;
out vec4 outputColor;
void main()
{
    vec2 st = texCoordVarying;
    // centering coordinates
    vec2 cco = st - vec2(0.5);
    float gau= exp(-var*(cco.x*cco.x+cco.y*cco.y));
    //if ((cco.x*cco.x+cco.y*cco.y) > 1/var){gau = 0.0;}
    outputColor = vec4(gau,gau,gau,1.0);
    //colorBase;
}
