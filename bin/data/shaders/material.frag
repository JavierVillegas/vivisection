#version 150
uniform sampler2D tex1;
uniform float alpha;
in vec2 texCoordVarying;
out vec4 outputColor;

vec4 BaseColor = vec4(.25,.25,.2,1.0);

vec3 Palette1(float Gray){
    vec3 TheValue;
    float modGray = Gray;
    if (Gray < 0.05){
        modGray = 0.05;
    }
    
    TheValue.r = pow(modGray,.30);
    TheValue.g =pow(modGray,1.0);//clamp(3.0*Gray/2.0-0.5,0.0,1.0);
    TheValue.b = pow(modGray,4.0);//clamp(3.0*Gray-2.0,0.0,1.0);
//    TheValue.r = pow(modGray,.30);
//    TheValue.g =pow(modGray,10.0);
//    TheValue.b =pow(modGray,9.0);
    return TheValue;
}

vec3 Palette3(float Gray){
    vec3 TheValue;
    TheValue.r = Gray;
    TheValue.g = Gray;//clamp(3.0*Gray/2.0-0.5,0.0,1.0);
    TheValue.b = Gray;
    return TheValue;
    
}

vec3 Palette2(float Gray){
    vec3 TheValue;
    float modGray = pow(Gray,1.0);
  
    modGray = clamp(modGray, .0,.85);
    float tempR,tempG,tempB;
    tempR = 0.0;
    tempG = 0.0;
    tempB = 0.0;
    
//    // coefficients
//    float a = 0.48328;
//    float b = 0.73088;
//    float c = -0.48352;
//    float d = -8.56955;
//
//    temp = a * exp(b*modGray) + c * exp(d*modGray);
    float aR = -7.7910125384771001E-01;
    float bR = 2.3755644433662226E+00;
    float OffsetR = 8.1829892728916898E-01;
    
    tempR = aR * pow(modGray, bR);
    tempR += OffsetR;

    float aG = -8.4818385826418585E-01;
    float bG = 9.8987768471391557E-01;
    float OffsetG = 8.0530559451921446E-01;
    
    tempG = aG * pow(modGray, bG);
    tempG += OffsetG;
    
    float aB = -1.0485360732603013E+00;
    float bB = 1.6556654920643583E+00;
    float OffsetB = 2.3000000000000001E-01;

    tempB = aB * pow(modGray, bB);
    tempB += OffsetB;
    tempB = clamp(tempB,0.0,1.0);
    
    
    TheValue.r = tempR;
    TheValue.g = tempG;//clamp(3.0*Gray/2.0-0.5,0.0,1.0);
    TheValue.b = tempB;//clamp(3.0*Gray-2.0,0.0,1.0);
    //    TheValue.r = pow(modGray,.30);
    //    TheValue.g =pow(modGray,10.0);
    //    TheValue.b =pow(modGray,9.0);
    return TheValue;
}


vec3 ADSModelFrag(vec3 base, vec3 NorMs, vec3 eyevec, vec3 leye){

    // calculates the ADS light model

    float thedist;
    vec3 thenorm;
    thenorm = NorMs;
    vec3 eyevertex = eyevec;
    // just for testing light defined in model
    vec3 lighEye = leye;
    vec3 s = normalize(lighEye-eyevertex);
    vec3 v = normalize(-eyevertex);
    vec3 r = reflect( -s, thenorm );
    float sDotN = clamp( dot(s,thenorm), 0.0 , 1.0 );
    base = base + 0.1*base*sDotN;
    if( sDotN > 0.0 ){
        base += 0.2*pow( max( dot(r,v), 0.0 ),364.0);
    }

    return base;

}

void main()
{
    
    
    
    vec3 col;
    vec2 TheCoords;
    TheCoords.x = texCoordVarying.x;
    TheCoords.y = texCoordVarying.y;
    
    vec2 TheCoordsX;
    vec2 TheCoordsY;
    TheCoordsX = TheCoords + vec2(0.001,0.0);
    TheCoordsY = TheCoords + vec2(0.0,0.001);

    // reading adjacent to get the normals
    vec4 fromTex = texture(tex1, TheCoords);
//    vec4 fromTexX = texture(tex1, TheCoordsX);
//    vec4 fromTexY = texture(tex1, TheCoordsY);

//    vec3 a = vec3(.001,0,fromTexX.r-fromTex.r);
//    vec3 b = vec3(0,.001,fromTexY.r-fromTex.r);
     vec3 a = vec3 (1.0, 0.0, fromTex.g);
     vec3 b = vec3 (0.0, 1.0, fromTex.b);
    
    col = Palette2(fromTex.r);
    vec3 TheEye = vec3(0.5,0.5,5.0);
 vec3 TheEyev = vec3(0.0,0.0,0.0);

    vec3 TheNormal = normalize(cross(b,a));
//
    vec3 TheLight = vec3(0,1.5,2.4);
 //   vec3 TheNormal = normalize(vec3(fromTex.g,fromTex.b,-.008));

    col = ADSModelFrag(col,TheNormal,vec3(-TheCoords.x,TheCoords.y,5.8),-TheLight+TheEye);

    //  float dif = clamp(0.5 + 0.5*dot( TheNormal,normalize(vec3(VinSphe,bF)-TheLight) ),0.0,1.0);
    //     col *= 1.0 + 0.2*dif*col;
    //    col += 0.3 * pow(TheNormal.z,64.0);

    // vignetting
   // col *= 1.0 - 0.1*length(TheCoords.xy);
    //col *= pow(TheCoords.x,alpha);
    float radi = length(TheCoords - vec2(0.5));
    if (radi < 0.45){
       outputColor = vec4(col,1);
    }
    else{
    outputColor = vec4(col,40.0*(0.5-radi));
    }
}
