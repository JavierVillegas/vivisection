#version 150
uniform sampler2D tex1;
uniform float var;
uniform int dir;
in vec2 texCoordVarying;
out vec4 outputColor;
void main()
{
    vec2 st = texCoordVarying;
    // centering coordinates
    vec2 cco = st - vec2(0.5);
    float gaud;
    if (dir == 0){
    gaud = -2*var*cco.x*exp(-var*(cco.x*cco.x+cco.y*cco.y));
    }
    else{
    gaud = -2*var*cco.y*exp(-var*(cco.x*cco.x+cco.y*cco.y));  
    }
    outputColor = vec4(gaud,gaud,gaud,1.0);
    //colorBase;
}
