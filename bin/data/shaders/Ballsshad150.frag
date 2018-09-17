#version 150
uniform sampler2D tex;
uniform int TheChannel;
uniform float scale;
in vec2 Pointcord;


out vec4 outputColor;
void main()
{
   
    vec4 thecolor = scale*texture(tex,gl_PointCoord);
    if (TheChannel == 1){
    outputColor = vec4(thecolor.r,0,0,1.0);
    }
    else if (TheChannel == 2){
    outputColor = vec4(0,thecolor.g,0,1.0);
    }
    else if (TheChannel == 3) {
     outputColor = vec4(0,0,thecolor.b,1.0);
        
    }
    else {
        outputColor = vec4(thecolor.rgb+.05,0.6);
        
    }
        
        

}
