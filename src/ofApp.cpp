#include "ofApp.h"
#define Nw 320
#define Nh 240

unsigned char Imin =0;
unsigned char Imax = 255;

// Render Size
const int Rw = 320;
const int Rh = 240;

const int Gw = 128;
const int Gh = 128;

const int Nob=5000;
const int Ntg=5000;

int TargInd[Nob];
ofVec2f TheObjects[Nob];
ofVec2f TheVelocities[Nob];

const int HashRow = 12;
const int HashColumn = 16;
const float hashdec = 20.0;

// target hash
vector <ofVec2f> TargetHash[HashRow][HashColumn];
// Object hash
list <ofVec2f> ObjectHash[HashRow][HashColumn];
// Index hash
list <int> ObjectIndHash[HashRow][HashColumn];

// histogram equalization

int EquaHist = 0;

// image with dither
cv::Mat TheDither;



// gauss texture
ofTexture GaussTexture;
ofTexture GaussTextureDerivX;
ofTexture GaussTextureDerivY;
float var = 48;//168?
float alpha = 0.45;

ofImage fondoFinal;
cv::Mat Transf;
int TheInd = 0;
//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(Nw,Nh);
    colorImg.allocate(Nw, Nh);
    grayImage.allocate(Nw, Nh);
    
    ThDis =13.4;
    Ktar = 19.25;
    Kdam = 1.05;

    for(int i =0;i< Nob;i++){
        ofVec2f TheValue(ofRandom(Nw),
                         ofRandom(Nh));
        TheObjects[i] = TheValue;
        TargInd[i] = -1;
        TheVelocities[i].set(0.0,0.0);
    }
    G_alpha = 0.215;
    metaballs.load("shaders/Ballsshad150.vert", "shaders/Ballsshad150.frag");
    postProShader.load("shaders/PostProshad150.vs", "shaders/PostProshad150.fs");
    gaussSprite.load("shaders/gaussShader.vs", "shaders/gaussShader.fs");
    gaussSpriteDerivative.load("shaders/gaussShader.vs", "shaders/gaussShaderDeriv.fs");
    materialShader.load("shaders/material.vert", "shaders/material.frag");
    
    ofDisableArbTex();
   // ofLoadImage(GaussTexture, "images/gauss1.png");
   
    theFrame.allocate(Rw, Rh,GL_RGBA32F);
    theFrameAux.allocate(Rw, Rh,GL_RGBA32F);
    PorstPro.allocate(Rw, Rh,GL_RGBA32F);
    fboMaterial.allocate(Rw, Rh,GL_RGBA32F);
    fboMaterialAux.allocate(Rw, Rh,GL_RGBA32F);
    fboGaussian.allocate(Gw, Gh,GL_RGBA32F);
    fboGaussianDerivativeX.allocate(Gw, Gh,GL_RGBA32F);
    fboGaussianDerivativeY.allocate(Gw, Gh,GL_RGBA32F);
    preGaussian.allocate(Gw, Gh,GL_RGBA32F);
    
    preGaussian.begin();
     ofClear(255, 255, 255, 255);
    preGaussian.end();
    
    
    // gaussian texture
    
    fboGaussian.begin();
    ofClear(255, 255, 255, 0);
    ofTexture atex = preGaussian.getTexture();
    gaussSprite.begin();
    gaussSprite.setUniformTexture("tex1",  atex, 1 );
    gaussSprite.setUniform1f("var",  var);
    ofSetColor(255, 255, 255,255);
    preGaussian.draw(0, 0, Gw, Gh);
    gaussSprite.end();
    fboGaussian.end();
    
    // gaussian derivative texture
    
    fboGaussianDerivativeX.begin();
    ofClear(255, 255, 255, 0);
    gaussSpriteDerivative.begin();
    gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
    gaussSpriteDerivative.setUniform1f("var",var);
    gaussSpriteDerivative.setUniform1i("dir",0);
    ofSetColor(255, 255, 255,255);
    preGaussian.draw(0, 0, Gw, Gh);
    gaussSpriteDerivative.end();
    fboGaussianDerivativeX.end();
    
    
    
    GaussTexture = fboGaussian.getTexture();
    GaussTextureDerivX = fboGaussianDerivativeX.getTexture();
    
    fboGaussianDerivativeY.begin();
    ofClear(255, 255,255, 0);
    gaussSpriteDerivative.begin();
    gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
    gaussSpriteDerivative.setUniform1f("var",var);
    gaussSpriteDerivative.setUniform1i("dir",1);
    ofSetColor(255, 255, 255,255);
    preGaussian.draw(0, 0, Gw, Gh);
    gaussSpriteDerivative.end();
    fboGaussianDerivativeY.end();
    
    GaussTextureDerivY = fboGaussianDerivativeY.getTexture();
    
    PorstPro.begin();
    ofClear(255, 255, 255, 0);
    PorstPro.end();
    
    fboMaterial.begin();
    ofClear(255, 255, 255, 0);
    fboMaterial.end();
    fboMaterialAux.begin();
    ofClear(255, 255, 255, 0);
    fboMaterialAux.end();
    
    fondoFinal.load("images/Balsamic.jpg");
    
    // calculating perspective transformation matrix
//
//    cv::Point2f InPoints[4] = {
//        cv::Point2f(Rw/2.0,0),
//        cv::Point2f(Rw,Rh/2.0),
//        cv::Point2f(Rw/2.0,Rh),
//        cv::Point2f(0,Rh/2.0)
//    };
    
//    cv::Point2f OutPoints[4] = {
//        cv::Point2f(1102,343),
//        cv::Point2f(1447,585),
//        cv::Point2f(1117,859),
//        cv::Point2f(747,591)
//    };
    
//    Transf = cv::getPerspectiveTransform(InPoints,OutPoints);
//    cout<<Transf<<endl;

    fboScreen1.allocate(Rw, Rh,GL_RGBA);
    fboScreen2.allocate(Rw, Rh,GL_RGBA);
    
    fboScreen1.begin();
    ofClear(255, 255, 255, 0);
    fboScreen1.end();
    
    fboScreen2.begin();
    ofClear(255, 255, 255, 0);
    fboScreen2.end();
    
    
    TheScreens.push_back("RGB");
    TheScreens.push_back("Gray");
    TheScreens.push_back("Dither");
    TheScreens.push_back("Darkest");
    TheScreens.push_back("TimeCoher");
    TheScreens.push_back("Gauss");
    TheScreens.push_back("Blend");
    TheScreens.push_back("Normals");
    TheScreens.push_back("Palette");
    TheScreens.push_back("ADS");
    TheScreens.push_back("Final");
    
    
    //Fonts
    Fuentes[0].load("helveticaBold.ttf", 34,true,true,false);
    Fuentes[1].load("helvetica.ttf", 30,true,true,false);
    Fuentes[2].load("helveticaBold.ttf", 20,true,true,false);
    Fuentes[3].load("helvetica.ttf", 20,true,true,true);
    Fuentes[4].load("helveticaBold.ttf", 40,true,true,false);
    
}

//--------------------------------------------------------------
void ofApp::update(){
 ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    ofBackground(100, 100, 100);
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    if (bNewFrame){
        // load grayscale depth image from the kinect source
        ofPixels ThePixels;
        ThePixels = vidGrabber.getPixels();
        colorImg.setFromPixels(ThePixels);
        grayImage = colorImg;   
        TheTargets.clear();
        TargetHits.clear();
        cv::Mat tempCv = grayImage.getCvImage();
      //  cv::flip(tempCv,tempCv,1);
        // histogram equalization
        if (EquaHist == 1){
            cv::equalizeHist(tempCv,tempCv);
        }
        else if (EquaHist == 2){
            int clim = 4;
            cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
            clahe->setClipLimit(clim);
            cv::Mat dst;
            clahe->apply(tempCv, tempCv);
         }
        
        
        
        
        while (!PointQueue.empty()){
            PointQueue.pop();
        }
        
        
      
        TheDither.release();
        TheDither = cv::Mat(tempCv.rows,tempCv.cols,CV_8UC1);
        NonLinear(tempCv);
        Dither(tempCv,TheDither);
        MatchObjetTargetsPart1();
        MatchObjetTargetsPart2();
        
        // set the vertex buffer object
        
        vector<ofVec3f> pointsVec;
        for (int sisi =0; sisi<Nob; sisi++) {
            pointsVec.push_back(ofVec3f(TheObjects[sisi].x*Rw/(float)Nw,
                                TheObjects[sisi].y*Rh/(float)Nh,0.0));
        }
        
//        pointsVec.push_back(ofVec3f(Rw/4.0,Rh/2.0,0.0));
//        pointsVec.push_back(ofVec3f(Rw/2.0,Rh/4.0,0.0));
//        pointsVec.push_back(ofVec3f(3*Rw/4.0,Rh/2.0,0.0));
//        pointsVec.push_back(ofVec3f(Rw/2.0,3*Rh/4.0,0.0));


        ballsVbo.setVertexData( &pointsVec[0], Nob, GL_DYNAMIC_DRAW );
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
//    ofxCvGrayscaleImage ToShow;
    ofBackground(255, 255, 255);
    ofSetColor(255, 255, 255,255);
//    colorImg.draw(0,0,Nw,Nh);
//    if (!TheDither.empty()){
//        ToShow.allocate(TheDither.cols, TheDither.rows);
//        ToShow.setFromPixels(TheDither.data,TheDither.cols, TheDither.rows);
//        ToShow.draw(0, Nh,Nw,Nh);
//    }
    if((TheScreens[TheInd].compare("Gauss")==0)||(TheScreens[TheInd].compare("Blend")==0)){
    theFrameAux.begin();
    ofEnablePointSprites();
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_ADD);
 //   glBlendFunc(GL_ONE, GL_ONE);
  //  glDisable(GL_BLEND);
    ofClear(0, 0, 0, 0);
    metaballs.begin();
    GaussTexture.bind();
    metaballs.setUniform1i("TheChannel", 4);
    metaballs.setUniform1f("scale", .9);
    ballsVbo.draw(GL_POINTS, 0, Nob);
    GaussTexture.unbind();
    metaballs.end();
    ofDisablePointSprites();
    theFrameAux.end();
 }
    
    else{
        
        theFrameAux.begin();
        
        ofEnablePointSprites();
        
        glEnable(GL_BLEND);
        glBlendEquation(GL_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        
        ofClear(0, 0, 0, 0);
        metaballs.begin();
        GaussTexture.bind();
        metaballs.setUniform1i("TheChannel", 1);
        metaballs.setUniform1f("scale", 0.01);
        ballsVbo.draw(GL_POINTS, 0, Nob);
        GaussTexture.unbind();
        metaballs.end();
        
        metaballs.begin();
        GaussTextureDerivX.bind();
        metaballs.setUniform1i("TheChannel", 2);
        metaballs.setUniform1f("scale", 0.01);
        ballsVbo.draw(GL_POINTS, 0, Nob);
        GaussTextureDerivX.unbind();
        metaballs.end();
        
        metaballs.begin();
        GaussTextureDerivY.bind();
        metaballs.setUniform1i("TheChannel", 3);
        metaballs.setUniform1f("scale", 0.01);
        ballsVbo.draw(GL_POINTS, 0, Nob);
        GaussTextureDerivY.unbind();
        metaballs.end();
        
        glDisable(GL_BLEND);
        ofDisablePointSprites();
        
        theFrameAux.end();
        }
    
    
    theFrame.begin();

      ofEnablePointSprites();

        glEnable(GL_BLEND);
        glBlendEquation(GL_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        ofClear(0, 0, 0, 0);
      metaballs.begin();
      GaussTexture.bind();
      metaballs.setUniform1i("TheChannel", 1);
      metaballs.setUniform1f("scale", 0.01);
      ballsVbo.draw(GL_POINTS, 0, Nob);
      GaussTexture.unbind();
      metaballs.end();
    
    
      metaballs.begin();
        GaussTexture.bind();
        metaballs.setUniform1i("TheChannel", 2);
       metaballs.setUniform1f("scale", 0.01);
        ballsVbo.draw(GL_POINTS, 0, Nob);
         GaussTexture.unbind();
         metaballs.end();
    
    metaballs.begin();
    GaussTexture.bind();
    metaballs.setUniform1i("TheChannel", 3);
    metaballs.setUniform1f("scale", 0.01);
    ballsVbo.draw(GL_POINTS, 0, Nob);
    GaussTexture.unbind();
    metaballs.end();
    
    glDisable(GL_BLEND);
    ofDisablePointSprites();

    theFrame.end();
    
    


//    PorstPro.begin();
//     ofClear(0, 0, 0, 0);
//      ofTexture tex3 = theFrame.getTexture();
//    postProShader.begin();
//    postProShader.setUniformTexture("tex1",  tex3, 1 );
//    postProShader.setUniform1f("alpha", alpha);
//    ofSetColor(255, 255, 255,255);
//     theFrame.draw(0, 0, Rw, Rh);
//      postProShader.end();
//    PorstPro.end();
    
    
    fboMaterialAux.begin();
    ofClear(0, 0, 0, 255);
    ofTexture tex2 = theFrameAux.getTexture();
    materialShader.begin();
    materialShader.setUniformTexture("tex1",  tex2, 1 );
    materialShader.setUniform1f("alpha", alpha);
    if(TheScreens[TheInd].compare("Final")==0){
        materialShader.setUniform1i("ADSOn",1);
        materialShader.setUniform1i("circleOn",1);
    }
    else{
    materialShader.setUniform1i("ADSOn",0);
    materialShader.setUniform1i("circleOn",0);
    }
    ofSetColor(0, 0, 0,255);
    theFrameAux.draw(0, 0, Rw, Rh);
    materialShader.end();
    fboMaterialAux.end();
    
    
    fboMaterial.begin();
    ofClear(0, 0, 0, 255);
    materialShader.begin();
    materialShader.setUniformTexture("tex1",  tex2, 1 );
    materialShader.setUniform1f("alpha", alpha);
    materialShader.setUniform1i("ADSOn",1);
    materialShader.setUniform1i("circleOn",0);
    ofSetColor(0, 0, 0,255);
    theFrameAux.draw(0, 0, Rw, Rh);
    materialShader.end();
    fboMaterial.end();
    
    
    // material render
    
    
    
    
//    ofSetColor(0, 0, 0);
//    for (int sisi =0; sisi<Nob; sisi++) {
//        ofCircle(Nw + TheObjects[sisi].x, TheObjects[sisi].y, 4);
//    }

  //  theFrame.draw(Nw,0,2*320,2*240);
   // PorstPro.draw(Nw,2.2*Nh,2*320,2*240);
//    ofClear(0, 0, 0,0);
//   ofEnableAlphaBlending();
//
//
//
//   ofSetColor(255, 255, 255,255);
//    fondoFinal.draw(0,0,1920,1080);
//    fboMaterial.draw(746,348,712,516);
//    ofDisableAlphaBlending();
//
//   glDisable(GL_BLEND);
 //   fboGaussianDerivativeX.draw(0,0,256,256);
  //   fboGaussian.draw(0,256,256,256);
    
    // defining what will be on screen
    
    // The title:
    ofSetColor(0,0,0,255);
    float theWidth = ofGetWidth();
    float theHeight = ofGetHeight();
    Fuentes[4].drawString("A Vivisection of Analysis/Synthesis", theWidth/14.0, theHeight/14.0);
    ofSetColor(255,255,255,255);
    onScreen(TheScreens[TheInd]);
    
    ofPushMatrix();
    ofTranslate(400, 300);
    ofPushMatrix();
    ofRotate(25, 0, 1, 0);
    if (TheInd != 1){
    //ofNoFill();
    //ofSetLineWidth(28.0);
    ofSetColor(0,0,0,255);
    ofDrawRectangle(-fboScreen1.getWidth()/2.0-6,
                    -fboScreen1.getHeight()/2.0-6,320*3+12,240*3+12);
    }
    ofSetColor(255,255,255,255);
    fboScreen1.draw(-fboScreen1.getWidth()/2.0,
                    -fboScreen1.getHeight()/2.0,320*3,240*3);
    ofPopMatrix();
    ofPopMatrix();
    
    if (TheInd == 3){
         ofDisableAlphaBlending();
    }
    else{ ofEnableAlphaBlending();}
    
    ofPushMatrix();
    ofTranslate(800, 300);
    ofPushMatrix();

    ofRotate(25, 0, 1, 0);
    if (TheInd != 0){
        //ofNoFill();
        //ofSetLineWidth(28.0);
        ofSetColor(0,0,0,255);
        ofDrawRectangle(-fboScreen1.getWidth()/2.0-6,
                        -fboScreen1.getHeight()/2.0-6,320*3+12,240*3+12);
    }
    ofSetColor(255,255,255,255);
    ofEnableAlphaBlending();
    fboScreen2.draw(-fboScreen2.getWidth()/2.0,
                    -fboScreen2.getHeight()/2.0,320*3,240*3);
    ofDisableAlphaBlending();
    ofPopMatrix();
    ofPopMatrix();
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == OF_KEY_RIGHT){
        
        float paso = (var < 10)?0.1:1.0;
        var -= paso;
        fboGaussian.begin();
        ofClear(255, 255, 255, 0);
        ofTexture atex = preGaussian.getTexture();
        gaussSprite.begin();
        gaussSprite.setUniformTexture("tex1",  atex, 1 );
        gaussSprite.setUniform1f("var",  var);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSprite.end();
        fboGaussian.end();
        
        GaussTexture = fboGaussian.getTexture();
        
        fboGaussianDerivativeX.begin();
        ofClear(255, 255, 255, 0);
        gaussSpriteDerivative.begin();
        gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
        gaussSpriteDerivative.setUniform1f("var",  var);
        gaussSpriteDerivative.setUniform1i("dir",0);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSpriteDerivative.end();
        fboGaussianDerivativeX.end();
    
        GaussTextureDerivX = fboGaussianDerivativeX.getTexture();
        
        fboGaussianDerivativeY.begin();
        ofClear(255, 255, 255, 0);
        gaussSpriteDerivative.begin();
        gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
        gaussSpriteDerivative.setUniform1f("var", var);
        gaussSpriteDerivative.setUniform1i("dir",1);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSpriteDerivative.end();
        fboGaussianDerivativeY.end();
        
        GaussTextureDerivY = fboGaussianDerivativeY.getTexture();
        
        cout<<var<<endl;
        
    }
    if (key == OF_KEY_LEFT){
        
        float paso = (var < 10)?0.1:1.0;
        var += paso;
        fboGaussian.begin();
        ofClear(255, 255, 255, 0);
        ofTexture atex = preGaussian.getTexture();
        gaussSprite.begin();
        gaussSprite.setUniformTexture("tex1",  atex, 1 );
        gaussSprite.setUniform1f("var",  var);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSprite.end();
        fboGaussian.end();
        
        GaussTexture = fboGaussian.getTexture();
        
        fboGaussianDerivativeX.begin();
        ofClear(0, 0, 0, 0);
        gaussSpriteDerivative.begin();
        gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
        gaussSpriteDerivative.setUniform1f("var",var);
        gaussSpriteDerivative.setUniform1i("dir",0);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSpriteDerivative.end();
        fboGaussianDerivativeX.end();
        
        GaussTextureDerivX = fboGaussianDerivativeX.getTexture();
        
        fboGaussianDerivativeY.begin();
        ofClear(0, 0, 0, 0);
        gaussSpriteDerivative.begin();
        gaussSpriteDerivative.setUniformTexture("tex1",  atex, 1 );
        gaussSpriteDerivative.setUniform1f("var",var);
        gaussSpriteDerivative.setUniform1i("dir",1);
        ofSetColor(255, 255, 255,255);
        preGaussian.draw(0, 0, Gw, Gh);
        gaussSpriteDerivative.end();
        fboGaussianDerivativeY.end();
        
        GaussTextureDerivY = fboGaussianDerivativeY.getTexture();
        
        cout<<var<<endl;
        
    }
    if (key == OF_KEY_UP){
        float val;
        val = (alpha>2.5)?1.0:0.05;
        alpha += val;
        cout<<"alpha:" <<alpha<<endl;
    }
    if (key == OF_KEY_DOWN){
        float val;
        val = (alpha>2.5)?1.0:0.05;
        alpha -= val;
        cout<<"alpha:" <<alpha<<endl;
    }
    if (key == 'g'){
        
        EquaHist ++;
        if (EquaHist>2){EquaHist = 0;}
        
    }
    
    if (key == 'a'){
        TheInd++;
        cout<<TheInd<<endl;
        
    }
    if (key == 'z'){
        TheInd--;
        cout<<TheInd<<endl;
        
    }

}



// sets the fbos to play on screen
void ofApp::onScreen(string screenName){
    float thew = ofGetWidth();
    float theh = ofGetHeight();
    if (screenName.compare("RGB")==0){
        
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Analysis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("1. A colored RGB Image is captured", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("from the Camera",3.05*thew/4.0, 1.5*theh/3.0);
        
        
        
        ofSetColor(255, 255, 255, 255);
        
        
        
        
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        ofPushMatrix();
        ofScale(-1,1);
        colorImg.draw(-fboScreen1.getWidth(),0,fboScreen1.getWidth(),
                      fboScreen1.getHeight());
        ofPopMatrix();
        fboScreen1.end();
        
        fboScreen2.begin();
         ofClear(255,255,255,0);
        // split to RGB
        
        cv::Mat bgr[3];   //destination array
        cv::Mat tempCv = colorImg.getCvImage();
        cv::flip(tempCv,tempCv, 1);
        split(tempCv,bgr);
        ofxCvGrayscaleImage ToShow;
        if (!bgr[0].empty()){
           ToShow.allocate(bgr[0].cols, bgr[0].rows);
           ToShow.setFromPixels(bgr[0].data,bgr[0].cols, bgr[0].rows);
            ofSetColor(255, 0, 0, 255);
            ToShow.draw(0,fboScreen1.getHeight()/2.0,fboScreen1.getWidth()/2.0,
                          fboScreen1.getHeight()/2.0);
          }
        if (!bgr[1].empty()){
            ToShow.allocate(bgr[1].cols, bgr[1].rows);
            ToShow.setFromPixels(bgr[1].data,bgr[1].cols, bgr[1].rows);
            ofSetColor(0, 255, 0, 255);
            ToShow.draw(fboScreen1.getWidth()/4.0,fboScreen1.getHeight()/4.0,fboScreen1.getWidth()/2.0,
                         fboScreen1.getHeight()/2.0);
        }
        if (!bgr[2].empty()){
            ToShow.allocate(bgr[2].cols, bgr[2].rows);
            ToShow.setFromPixels(bgr[2].data,bgr[2].cols, bgr[2].rows);
            ofSetColor(0, 0, 255, 255);
            ToShow.draw(fboScreen1.getWidth()/2.0,0,fboScreen1.getWidth()/2.0,
                        fboScreen1.getHeight()/2.0);
        }
        fboScreen2.end();
    }
    if (screenName.compare("Gray")==0)
    
    {
        
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Analysis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("2. The RGB Image is converted", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("to gray values",3.05*thew/4.0, 1.5*theh/3.0);
        
        
        
        ofSetColor(255, 255, 255, 255);
        
       fboScreen1.begin();
        ofClear(255,255,255,0);
        // split to RGB
        
        cv::Mat bgr[3];   //destination array
        cv::Mat tempCv = colorImg.getCvImage();
        cv::flip(tempCv,tempCv, 1);
        split(tempCv,bgr);
        ofxCvGrayscaleImage ToShow;
        if (!bgr[0].empty()){
            ToShow.allocate(bgr[0].cols, bgr[0].rows);
            ToShow.setFromPixels(bgr[0].data,bgr[0].cols, bgr[0].rows);
            ofSetColor(255, 0, 0, 255);
            ToShow.draw(0,fboScreen1.getHeight()/2.0,fboScreen1.getWidth()/2.0,
                        fboScreen1.getHeight()/2.0);
        }
        if (!bgr[1].empty()){
            ToShow.allocate(bgr[1].cols, bgr[1].rows);
            ToShow.setFromPixels(bgr[1].data,bgr[1].cols, bgr[1].rows);
            ofSetColor(0, 255, 0, 255);
            ToShow.draw(fboScreen1.getWidth()/4.0,fboScreen1.getHeight()/4.0,fboScreen1.getWidth()/2.0,
                        fboScreen1.getHeight()/2.0);
        }
        if (!bgr[2].empty()){
            ToShow.allocate(bgr[2].cols, bgr[2].rows);
            ToShow.setFromPixels(bgr[2].data,bgr[2].cols, bgr[2].rows);
            ofSetColor(0, 0, 255, 255);
            ToShow.draw(fboScreen1.getWidth()/2.0,0,fboScreen1.getWidth()/2.0,
                        fboScreen1.getHeight()/2.0);
        }
        fboScreen1.end();
        
        
        fboScreen2.begin();
         ofClear(255,255,255,0);
        ofSetColor(255, 255, 255, 255);
        ofPushMatrix();
        ofScale(-1,1);
        grayImage.draw(-fboScreen1.getWidth(),0,fboScreen1.getWidth(),
                      fboScreen1.getHeight());
        ofPopMatrix();
        fboScreen2.end();
    }
    if (screenName.compare("Dither")==0)
        
    {
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Analysis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("3. A black and white version", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("is created, using density ",3.05*thew/4.0, 1.5*theh/3.0);
        Fuentes[3].drawString("to represent darker areas",3.05*thew/4.0, 1.6*theh/3.0);
        ofSetColor(255, 255, 255, 255);
        fboScreen1.begin();
        ofClear(255,255,255,0);
        ofSetColor(255, 255, 255, 255);
        ofPushMatrix();
        ofScale(-1,1);
        grayImage.draw(-fboScreen1.getWidth(),0,fboScreen1.getWidth(),
                       fboScreen1.getHeight());
        ofPopMatrix();
        fboScreen1.end();
        
        fboScreen2.begin();
        ofClear(255,255,255,0);
        ofxCvGrayscaleImage ToShow;
        cv::Mat tempCv;
        TheDither.copyTo(tempCv);
        cv::flip(tempCv,tempCv, 1);
        if (!tempCv.empty()){
            ToShow.allocate(tempCv.cols, tempCv.rows);
            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
            ToShow.draw(0,0,fboScreen1.getWidth(),
                        fboScreen1.getHeight());
        }

        fboScreen2.end();
    }
    
    if (screenName.compare("Darkest")==0)
        
    {
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Analysis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("4. Points that represent", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("the darkest regions are",3.05*thew/4.0, 1.5*theh/3.0);
        Fuentes[3].drawString("selected",3.05*thew/4.0, 1.6*theh/3.0);
        ofSetColor(255, 255, 255, 255);
        
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofxCvGrayscaleImage ToShow;
        cv::Mat tempCv;
        TheDither.copyTo(tempCv);
        cv::flip(tempCv,tempCv, 1);
        if (!tempCv.empty()){
            ToShow.allocate(tempCv.cols, tempCv.rows);
            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
            ToShow.draw(0,0,fboScreen1.getWidth(),
                        fboScreen1.getHeight());
        }
        
        fboScreen1.end();
        fboScreen2.begin();
        ofEnableAlphaBlending();
        ofClear(255,255,255,255);
        ofDrawRectangle(0, 0, fboScreen1.getWidth(),
                        fboScreen1.getHeight());
//        if (!tempCv.empty()){
//            ToShow.allocate(tempCv.cols, tempCv.rows);
//            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
//            ofSetColor(255,255,255,255);
//            ToShow.draw(0,0,fboScreen1.getWidth(),
//                        fboScreen1.getHeight());
//            ofNoFill();
//
//            ofSetColor(255,255,255,255);
//        }
        ofSetColor(120,0,0,70);
        for (int k = 0; k< TheTargets.size(); k++){
            
            ofDrawCircle(TheTargets[k].x*fboScreen1.getWidth()/(float)Nw, TheTargets[k].y*fboScreen1.getHeight()/(float)Nh, 3);
        }
        ofDisableAlphaBlending();
        fboScreen2.end();
    }
    if(screenName.compare("TimeCoher")==0){
        
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("1. Points in consecutive frames are", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("matched and their trajectory",3.05*thew/4.0, 1.5*theh/3.0);
        Fuentes[3].drawString("is smoothed",3.05*thew/4.0, 1.6*theh/3.0);
        ofSetColor(255, 255, 255, 255);
        fboScreen1.begin();
        ofEnableAlphaBlending();
        ofClear(255,255,255,255);
        ofDrawRectangle(0, 0, fboScreen1.getWidth(),
                        fboScreen1.getHeight());
        //        if (!tempCv.empty()){
        //            ToShow.allocate(tempCv.cols, tempCv.rows);
        //            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
        //            ofSetColor(255,255,255,255);
        //            ToShow.draw(0,0,fboScreen1.getWidth(),
        //                        fboScreen1.getHeight());
        //            ofNoFill();
        //
        //            ofSetColor(255,255,255,255);
        //        }
        ofSetColor(120,0,0,70);
        for (int k = 0; k< TheTargets.size(); k++){
            
            ofDrawCircle(TheTargets[k].x*fboScreen1.getWidth()/(float)Nw, TheTargets[k].y*fboScreen1.getHeight()/(float)Nh, 3);
        }
        ofDisableAlphaBlending();
        fboScreen1.end();
        fboScreen2.begin();
        ofEnableAlphaBlending();
        ofClear(255,255,255,255);
        ofDrawRectangle(0, 0, fboScreen1.getWidth(),
                        fboScreen1.getHeight());
        //        if (!tempCv.empty()){
        //            ToShow.allocate(tempCv.cols, tempCv.rows);
        //            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
        //            ofSetColor(255,255,255,255);
        //            ToShow.draw(0,0,fboScreen1.getWidth(),
        //                        fboScreen1.getHeight());
        //            ofNoFill();
        //
        //            ofSetColor(255,255,255,255);
        //        }
        ofSetColor(0,0,150,70);
        for (int k = 0; k< Nob; k++){
            
            ofDrawCircle(TheObjects[k].x*fboScreen1.getWidth()/(float)Nw, TheObjects[k].y*fboScreen1.getHeight()/(float)Nh, 3);
        }
        ofDisableAlphaBlending();
        fboScreen2.end();
        
    }

    if (screenName.compare("Gauss")==0)
        
    {
        
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("2. Points are replaced by", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("a 2D Gaussian function",3.05*thew/4.0, 1.5*theh/3.0);
        ofSetColor(255, 255, 255, 255);
        
        fboScreen1.begin();
        ofEnableAlphaBlending();
        ofClear(255,255,255,255);
        ofDrawRectangle(0, 0, fboScreen1.getWidth(),
                        fboScreen1.getHeight());
        //        if (!tempCv.empty()){
        //            ToShow.allocate(tempCv.cols, tempCv.rows);
        //            ToShow.setFromPixels(tempCv.data,tempCv.cols, tempCv.rows);
        //            ofSetColor(255,255,255,255);
        //            ToShow.draw(0,0,fboScreen1.getWidth(),
        //                        fboScreen1.getHeight());
        //            ofNoFill();
        //
        //            ofSetColor(255,255,255,255);
        //        }
        ofSetColor(0,0,150,70);
        for (int k = 0; k< Nob; k++){
            
            ofDrawCircle(TheObjects[k].x*fboScreen1.getWidth()/(float)Nw, TheObjects[k].y*fboScreen1.getHeight()/(float)Nh, 3);
        }
        ofDisableAlphaBlending();
        fboScreen1.end();
        
        
        fboScreen2.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
    
        theFrameAux.draw(0,0,fboScreen1.getWidth(),
                       fboScreen1.getHeight());;
        
        fboScreen2.end();
    }
    if (screenName.compare("Blend")==0)
        
    {
        // subtitle
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("3. All the gaussians are blended", 3*thew/4.0, 1.4*theh/3.0);

        ofSetColor(255, 255, 255, 255);
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        theFrameAux.draw(0,0,fboScreen1.getWidth(),
                         fboScreen1.getHeight());;
        
        fboScreen1.end();
        fboScreen2.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        theFrame.draw(0,0,fboScreen1.getWidth(),
                         fboScreen1.getHeight());;
        
        fboScreen2.end();
        
        
    }

    if (screenName.compare("Normals")==0)
        
    {
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("4. Other surfaces (in green and blue)", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("are derived from the Gaussian blend",3.05*thew/4.0, 1.5*theh/3.0);
        Fuentes[3].drawString("(in red)",3.05*thew/4.0, 1.6*theh/3.0);
        
        ofSetColor(255, 255, 255, 255);
        
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        theFrame.draw(0,0,fboScreen1.getWidth(),
                         fboScreen1.getHeight());;
        
        fboScreen1.end();
        fboScreen2.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        theFrameAux.draw(0,0,fboScreen1.getWidth(),
                      fboScreen1.getHeight());;
        
        fboScreen2.end();
    }

    if (screenName.compare("Palette")==0)
        
    {
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("5. Each gray value of the", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("blend, is mapped to an",3.05*thew/4.0, 1.5*theh/3.0);
        Fuentes[3].drawString("RGB value",3.05*thew/4.0, 1.6*theh/3.0);
        
        ofSetColor(255, 255, 255, 255);
        
        
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        theFrameAux.draw(0,0,fboScreen1.getWidth(),
                         fboScreen1.getHeight());;
        
        fboScreen1.end();
        
        fboScreen2.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        fboMaterialAux.draw(0,0,fboScreen1.getWidth(),
                      fboScreen1.getHeight());;
        
        fboScreen2.end();

    }
    if (screenName.compare("ADS")==0)
        
    {
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("6. Light reflections are", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("calculated and added",3.05*thew/4.0, 1.5*theh/3.0);
        
        ofSetColor(255, 255, 255, 255);
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        fboMaterialAux.draw(0,0,fboScreen1.getWidth(),
                            fboScreen1.getHeight());;
        
        fboScreen1.end();
        fboScreen2.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        fboMaterial.draw(0,0,fboScreen1.getWidth(),
                            fboScreen1.getHeight());;
        
        fboScreen2.end();
        
    }
    if (screenName.compare("Final")==0)
        
    {
        ofSetColor(0, 0, 0, 255);
        Fuentes[0].drawString("Synthesis", 3.2*thew/4.0, 1.2*theh/3.0);
        Fuentes[3].drawString("7. Final image is embedded", 3*thew/4.0, 1.4*theh/3.0);
        Fuentes[3].drawString("into a scene",3.05*thew/4.0, 1.5*theh/3.0);
        
        ofSetColor(255, 255, 255, 255);
        fboScreen1.begin();
        ofClear(255,255,255,255);
        ofSetColor(255, 255, 255, 255);
        
        fboMaterial.draw(0,0,fboScreen1.getWidth(),
                            fboScreen1.getHeight());;
        
        fboScreen1.end();
        fboScreen2.begin();
        ofClear(0, 0, 0,0);
           ofEnableAlphaBlending();
            ofSetColor(255, 255, 255,255);
        fondoFinal.draw(0,0,fboScreen1.getWidth(),
                        fboScreen1.getHeight());
            fboMaterialAux.draw(746/1920.0*fboScreen1.getWidth(),348/1080.0*fboScreen1.getHeight(),712/1920.0*fboScreen1.getWidth(),516/1080.0*fboScreen1.getHeight());
            ofDisableAlphaBlending();
        fboScreen2.end();
        
    }
    
    


}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::NonLinear(cv::Mat &src) {
    int h = src.rows;
    int w = src.cols;
    float radi;
    radi = (TheInd < 10)?1.0:0.25;
    for (int j = 0; j < h; j++) {
        float ncj = (j/(float)h - 0.5);
        for (int i = 0; i < w; i++) {
            float nci = (i/(float)w - 0.5);
            uchar newVal;
            if ((ncj*ncj+nci*nci)<radi){
            newVal = (Imin +255*((Imax -Imin)/255.0*(pow((float)(src.at<uchar>(j,i)/255.0),(float)G_alpha))));
            }
            else{
                newVal = 255;
            }
                src.at<uchar>(j,i) = newVal;

        }
    }
    
    
}

void ofApp::Dither(const cv::Mat src, cv::Mat  &dst) {
    
    int pxPos;
    int oldPx = 0;
    int newPx = 0;
    int qError = 0;
    
    int threshold = 130;
    src.copyTo(dst);
    int r1, r2,  bl1, bc1, br1, bc2;
    int new1,new2;
    
    int h = src.rows;
    int w = src.cols;
    for (int j = 0; j < h-2; j++) {
        for (int i = 1; i < w-2; i++) {
            oldPx = dst.at<uchar>(j,i);
            newPx = oldPx < threshold ? 0 : 255;
            dst.at<uchar>(j,i) = newPx;
            qError = oldPx - newPx;

            //if (qError <0)
            {qError*=2*0.5;}

            // super duper ugly 2 part value clipping (to avoid values beyond 0-255)
            r1  = MIN(dst.at<uchar>(j,(i+1))  + (int)(.125f * qError), 255);
            r2  = MIN(dst.at<uchar>(j,(i+2))     + (int)(.125f * qError), 255);
            bl1 = MIN(dst.at<uchar>(j+1,(i-1)) + (int)(.125f * qError), 255);
            bc1 = MIN(dst.at<uchar>(j+1,i)   + (int)(.125f * qError), 255);
            br1 = MIN(dst.at<uchar>(j+1,(i+1)) + (int)(.125f * qError), 255);
            bc2 = MIN(dst.at<uchar>(j+2,i)   + (int)(.125f * qError), 255);
            //            new1 = MIN(dst[(j+1)*w+(i+2)] + (int)(.125f * qError), 255);
            //            new2 = MIN(dst[(j+2)*w+(i+1)]   + (int)(.125f * qError), 255);


            dst.at<uchar>(j,(i+1))     = MAX(r1 , 0);
            dst.at<uchar>(j+1,(i-1)) = MAX(bl1, 0);
            dst.at<uchar>(j+1,i)   = MAX(bc1, 0);
            dst.at<uchar>(j+1,i+1) = MAX(br1, 0);
            dst.at<uchar>(j+2,i)   = MAX(bc2, 0);
            //            dst[(j+1)*w+(i+2)] = MAX(new1, 0);
            //            dst[(j+2)*w+(i+1)]   = MAX(new2, 0);
            if (qError>0){
                PointQueue.push(ThePoints(Nw-i,j,qError));
            }
            
        }
    }
}

void ofApp::MatchObjetTargetsPart1() {
    
    ThePoints tempoPoint;
    int TheLimit = MIN(Ntg, PointQueue.size());
    
    for (int k = 0; k < TheLimit; k++) {
        tempoPoint = PointQueue.top();
        TheTargets.push_back(ofVec2f(tempoPoint.Pos.x, tempoPoint.Pos.y));
        PointQueue.pop();
        //     TargetHits.push_back(tempoPoint.Error);
        TargetHits.push_back(0);
    }
    
    for (int i = 0; i< Nob; i++) {
        TargInd[i] = -1;
    }
    
    if (!TheTargets.empty()) {
        // empty the hash tables
        
        for (int roH = 0; roH < HashRow; roH++) {
            for (int coH = 0; coH < HashColumn; coH++) {
                
                ObjectHash[roH][coH].clear();
                
                ObjectIndHash[roH][coH].clear();
                
            }
        }
        
        
        
        
        
        
        // move objects to the hash cells
        for (int ko = 0; ko < Nob; ko++) {
            int indrow = floor(TheObjects[ko].y / hashdec);
            indrow = MAX(0, indrow);
            indrow = MIN(HashRow - 1, indrow);
            int indcol = floor(TheObjects[ko].x / hashdec);
            indcol = MAX(0, indcol);
            indcol = MIN(HashColumn - 1, indcol);
            
            ObjectHash[indrow][indcol].push_back(TheObjects[ko]);
            ObjectIndHash[indrow][indcol].push_back(ko);
        }
        // running through the targets and finding a close free object
        
        for (int kt = 0; kt < TheTargets.size(); kt++) {
            int indrow = floor(TheTargets[kt].y / hashdec);
            int indcol = floor(TheTargets[kt].x / hashdec);
            indrow = MAX(0, indrow);
            indrow = MIN(HashRow - 1, indrow);
            indcol = MAX(0, indcol);
            indcol = MIN(HashColumn - 1, indcol);
            int ww = 0; // window width
            int wh = 0; // window height
            bool MatchFound = false;
            int winsize = 0;
            while (!MatchFound) {
                ww = winsize;
                wh = winsize;
                float MinDis = 10000 * 10000;
                int MinIndi = 0;
                for (int n = -ww; n <= ww; n++) {
                    for (int m = -wh; m <= wh; m++) {
                        // if the cell is not empty
                        if ((indcol + n >= 0) &&
                            (indrow + m >= 0) &&
                            (indcol + n < 16) &&
                            (indrow + m < 12) &&
                            (!ObjectHash[indrow + m][indcol + n].empty())
                            ) {
                            std::list<ofVec2f>::iterator
                            it1 = ObjectHash[indrow + m][indcol + n].begin();
                            std::list<int>::iterator
                            it2 = ObjectIndHash[indrow + m][indcol + n].begin();
                            int elemcount = 0;
                            int Theelement = 0;
                            while (it1 != ObjectHash[indrow + m][indcol + n].end())
                            {
                                ofVec2f theobj = (*it1);
                                float currDist = TheTargets[kt].squareDistance(theobj);
                                if (currDist < MinDis) {
                                    MinDis = currDist;
                                    MinIndi = (*it2);
                                    Theelement = elemcount;
                                }
                                it1++;
                                it2++;
                                elemcount++;
                            }
                            TargInd[MinIndi] = kt;
                            MatchFound = true;
                            // delete element for the list
                            it1 = ObjectHash[indrow + m][indcol + n].begin();
                            it2 = ObjectIndHash[indrow + m][indcol + n].begin();
                            for (int ec = 0; ec < Theelement; ec++) {
                                it1++;
                                it2++;
                            }
                            ObjectHash[indrow + m][indcol + n].erase(it1);
                            ObjectIndHash[indrow + m][indcol + n].erase(it2);
                            n = ww + 1;
                            m = wh + 1;
                            break;
                        }
                        else { // increment area counter
                            // if last window cell
                            if ((n == ww) && (m == wh)) {
                                winsize++;
                            }
                        }
                    }// for m
                }// for n
            } // while
        } // loop targets
    }  // end if targets non empty
}


void ofApp::MatchObjetTargetsPart2() {
    float dt = 0.25;
    for (int ko = 0; ko<Nob; ko++) {
        
        float MinHits = 10000;
        ofVec2f UpdateVec;
        float MinDis = 10000 * 10000;
        int MinIndi = 0;
        
        if (TargInd[ko] == -1) {
            MinDis = 10000 * 10000;
            for (int kt = 0; kt<TheTargets.size(); kt++) {
                ofVec2f ErrorVec;
                ErrorVec = TheTargets[kt] - TheObjects[ko];
                float dis = TargetHits[kt] * ErrorVec.lengthSquared();
                if (dis < MinDis) {
                    MinDis = dis;
                    MinIndi = kt;
                }
                
            }
            
            TargetHits[MinIndi]++;
            TargInd[ko] = MinIndi;
        }
        
        UpdateVec = TheTargets[TargInd[ko]] - TheObjects[ko];
        float newDis = UpdateVec.length();
        UpdateVec.normalize();
        
        
        ofVec2f acc;
        
        if (newDis < ThDis) {
            acc = (newDis / 10.0)*(Ktar*UpdateVec) - Kdam*TheVelocities[ko];
        }
        else {
            acc = (Ktar*UpdateVec) - Kdam*TheVelocities[ko];
        }
        TheVelocities[ko] = TheVelocities[ko] - (-dt)*acc;
        TheObjects[ko] = TheObjects[ko] - (-dt)*TheVelocities[ko];
    }
    
}// closing function

