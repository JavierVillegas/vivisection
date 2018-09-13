#version 150
uniform sampler2D tex1;
uniform float alpha;
in vec2 texCoordVarying;
out vec4 outputColor;
void main()
{
    vec2 st = texCoordVarying;
    vec4 colorBase = texture(tex1, st);
//    float TheVal = pow(colorBase.a,alpha);
//    if (TheVal < 0.0){
//        outputColor = vec4(TheVal,TheVal,TheVal,1.0);
//    }
//    else{        outputColor = vec4(1.0,1.0,1.0,1.0);
//    }
    outputColor = colorBase;//vec4(st.x,st.y,0.0,1.0);
    //colorBase;
}
