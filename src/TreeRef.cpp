#include "TreeDLA.h"

#define Nw 320      
#define Nh 240
//#define Xoffset 0 testin as global variable
ofFbo FboBackground;

int NoutX;
int NoutY;


unsigned char Imin =0;
unsigned char Imax = 255;
ofDirectory dir;
int FondSize;
int FondSelect = 1;
const int Nob=6000;
const int Ntg=6000;

 int TargInd[Nob];
ofVec2f TheObjects[Nob];
//vector<ofVec2f>TheObjects;
ofVec2f TheVelocities[Nob];
int ImW,ImH;

ofVbo Tree1Vbo;
ofVbo Tree2Vbo;
ofVbo Tree3Vbo;

ofVbo TrunksVbo;

ofShader shaderStem;
int NumLineas[3] = {0,0,0};

float XYOffset[2];//={325.0,6.0};
float XYScale[2];//={2.25,1.8};

float XYOffsetB[2];//={325.0,6.0};
float XYScaleB[2];//={2.25,1.8};

// trunk texture

ofImage TextuTrunk;
ofShader TrunkShader;

// Shadow

ofShader ShadowShader;
ofVec2f MassXcenters[3];
int LeavesperTree[3];

ofShader BackShader;

// leaves

ofVbo Leaves1Vbo;
ofVbo Leaves2Vbo;
ofVbo Leaves3Vbo;

ofTexture LeaveTexture;

ofShader LeavesShader;
bool EquaHist = true;

const int HashRow = 12;
const int HashColumn = 16;


// target hash
vector <ofVec2f> TargetHash[HashRow][HashColumn];
// Object hash
list <ofVec2f> ObjectHash[HashRow][HashColumn];
// Index hash
list <int> ObjectIndHash[HashRow][HashColumn];

const float hashdec = 20.0;



// similar hash tables for leave matching

// Index hash
list <int> LeavesIndHash[HashRow][HashColumn];

int capX = 1920;
int capY = 1080;

vector<vector<ofVec2f> > PosList1;
vector<vector<ofVec2f> > PosList2;
vector<vector<ofVec2f> > PosList3;
int shadeHor=0;
//--------------------------------------------------------------
void TreeDLA::setup() {
    NoutX = 1280;//960;  // some fixed resolution tests
    NoutY = 720;//480;
   // NoutX = ofGetScreenWidth();
   //NoutY = ofGetScreenHeight();
    cout<< NoutX<<","<<NoutY<<endl;
    
    FboBackground.allocate(NoutX, NoutY,GL_RGBA);
     // postprocessing at the screen size
	PorstPro.allocate(capX, capY, GL_RGBA);
	PorstPro2.allocate(capX, capY, GL_RGBA);
	// texture

	FondSize = dir.listDir("fondos");
//	Fondo.loadImage(dir.getPath(FondSelect));
	Fondo.loadImage("fondos/primera.png");
//	Fondo.loadImage("bigfondo.jpg");

  theFrame.allocate(capX, capY);
  
    px    = new unsigned char [Nw*Nh];
    fixedThresholdPx  = new unsigned char [Nw*Nh];
    ofBackground(255, 255, 255);
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(Nw,Nh);
    
	colorImg.allocate(Nw, Nh);
	grayImage.allocate(Nw, Nh);

    
	ofSetFrameRate(60);

    
    G_InputImage =false;
  
    

    
    
    
    ThDis =13.4;
    Ktar = 19.25;
    Kdam = 2.05;
   
    TreeThres1 = Nw/3.0;
    TreeThres2 = 2*Nw/3.0;
    G_FrameCounter =1;
    

    
    


    for(int i =0;i< Nob;i++){
        
        ofVec2f TheValue(ofRandom(Nw),
                         ofRandom(Nh));
        TheObjects[i] =TheValue;//.set(0.0,0.0);//.push_back(ofVec2f(0.0,0.0));
 //       TheObjects.push_back(TheValue);
        TargInd[i] =-1;
        TheVelocities[i].set(0.0,0.0);
        
    }
    

    
    // control globals
	G_ThTarget =  25;
    Kn=2.9;
    Kc=5.0;
    joinTh=1;
    G_alpha = 0.215;
//    G_alpha = 0.44;
 //   G_beta =.63;
    G_ItFact=2/25.0;
    G_DithThreshold = 130.0;
 
    // the image
    DepthMap = cvCreateImage( cvSize(Nw,Nh),IPL_DEPTH_8U,1);
    ofHideCursor();
    

   
    Colorsave.allocate(Nw, Nh, OF_IMAGE_COLOR);
    G_capture  =   false;
    G_SaveCounter = 1;
    G_NewModo =0;
    
        
    G_TheSource = 0;
    G_Quality=0.001;
    G_minDistance=8;
    Xoffset =0;
    
    G_ThresWhiter = 0.5;
    
  
    
    // aparience definer variables.
    
    
 //   AlphaSombra = 24;
    AlphaSombra = 8;
    ColorSombra = 50;
 //   ScaleHojas = 0.56; //lamarca
    // 4*0.56  para la primera
    ScaleHojas = 0.86;
//    SlopeSombra = -1.37; //lamarca
    SlopeSombra = 17.0; //lamarca
//    ScaleXSombra = 1.0;  //lamarca
//    ScaleYSombra = 0.224; //lamarca
    ScaleXSombra = 5;  //lamarca
    ScaleYSombra = 0.084; //lamarca    
    BrincoR = 0.24;
    BrincoG = 0.35; 

    
    
    shaderStem.load("shaders/StemShad150.vert", "shaders/StemShad150.frag");
    
    TextuTrunk.loadImage("images/trunk3B.png");
    TextuTrunk.update();
    
    TrunkShader.load("shaders/TrunkShad150.vert", "shaders/TrunkShad150.frag");
    
//    ShadowShader.load("shaders/ShadowShad150.vert", "shaders/ShadowShad150.frag");
  
    
    BackShader.load("shaders/BackShad150.vert", "shaders/BackShad150.frag");
    
    ofDisableArbTex();
    ofLoadImage(LeaveTexture, "images/leaveCCalpha.png");
    
    LeavesShader.load("shaders/LeavesPSshad150.vert", "shaders/LeavesPSshad150.frag");
  
  // post processing
  postProShader.load("shaders/postProShad150.vert","shaders/postProShad150.frag");
  
  
}

//--------------------------------------------------------------
void TreeDLA::update() {
   
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
      // histogram equalization
        if (EquaHist == true){
            cv::equalizeHist(tempCv,tempCv);
        }
		/////////////////////////////////////////
		// note: hacer esto en un shader
        for (int i = 0; i < Nw*Nh; i++) {
            px[i] = (uchar)
            (Imin +255*((Imax -Imin)/255.0*(pow((float)(tempCv.data[i]/255.0),(float)G_alpha))));

        }
        
//        for (int i = 0; i < Nw*Nh; i++) {
//            px[i] = (uchar)( Imin +255*((Imax -Imin)/255.0*(pow((float)(grayImage.getPixels()[i]/255.0),(float)G_alpha))));
//        }
        
//        for (int i = 0; i < Nw*Nh; i++) {
//            px[i] = (uchar)(255*(G_beta + (1-G_beta )*pow((float)(grayImage.getPixels()[i]/255.0),(float)G_alpha)));
//        }

        while (!PointQueue.empty()){
            PointQueue.pop();
        }
        
        int cuantosBlack;  
        
        BWTest (px, fixedThresholdPx, Nw, Nh);
        DepthMap.data = (unsigned char *)fixedThresholdPx;
        cv::flip(DepthMap,DepthMap,1);
		

		MatchObjetTargetsPart1();

		MatchObjetTargetsPart2();
        
    // filling the tree structure
        

        ofVec2f CurrentPoint(320,10);
        float TheError = 10;
        int itThres = 0;
        float cordX1=0;
        float cordX2=0;
        float cordX3=0;
        while ((TheError>2)&&(itThres<5)) {
            PosList1.clear();
            PosList2.clear();
            PosList3.clear();
            // coping the objects to the list
            // finding te center of mass
            // only using X coordinate
            
            cordX1=0;
            cordX2=0;
            cordX3=0;
            
            itThres++;
            int sideCounter1=0;
            int sideCounter2=0;
            int sideCounter3=0;
            
            for (int k=0; k < Nob;k++){
                if (TheObjects[k].x<TreeThres1) {
                    cordX1+=TheObjects[k].x;
                    vector<ofVec2f> retempo;
                    retempo.push_back(ofVec2f(TheObjects[k].x,TheObjects[k].y));
                    PosList1.push_back(retempo);
                    retempo.clear();
                    sideCounter1++;
                }
                else if((TheObjects[k].x>=TreeThres1)&&(TheObjects[k].x<TreeThres2))
                {
                    cordX2+=TheObjects[k].x;
                    vector<ofVec2f> retempo;
                    retempo.push_back(ofVec2f(TheObjects[k].x,TheObjects[k].y));
                    PosList2.push_back(retempo);
                    retempo.clear();
                    sideCounter2++;
                }
                else{
                    cordX3+=TheObjects[k].x;
                    vector<ofVec2f> retempo;
                    retempo.push_back(ofVec2f(TheObjects[k].x,TheObjects[k].y));
                    PosList3.push_back(retempo);
                    retempo.clear();
                    sideCounter3++;
                }
            } // end for 0..Nob
            if (sideCounter1!=0){
                cordX1/=(float)sideCounter1;
            }
            else{
                cordX1 =  Nw/6.0;
            }
            if (sideCounter2!=0){
                cordX2/=(float)sideCounter2;
            }
            else{
                cordX2 =  Nw/2.0;
            }
            if (sideCounter3!=0){
                cordX3/=(float)sideCounter3;
            }
            else{
                cordX3 =  5*Nw/6.0;
            }       
            TheError = abs(TreeThres1-(cordX2+cordX1)/2.0)
            + abs(TreeThres2-(cordX3+cordX2)/2.0);
            
            TreeThres1 = cordX1  + (cordX2-cordX1)/2.0;
            TreeThres2 = cordX2  + (cordX3-cordX2)/2.0;
        
            } // end while of the k-means
        // calculating the vec of vecs structures
        LeavesperTree[0] = PosList1.size();
        LeavesperTree[1] = PosList2.size();
        LeavesperTree[2] = PosList3.size();
        
		//TODO
		//Kmeans recursivo para dibujar el arbol, subdividir y encontrar centros locales 



      glLineWidth(10.0);

    CalculateTreeStructureOP(PosList1,ofVec2f(cordX1,Nh*1.45), 0.85);
    CalculateTreeStructureOP(PosList2,ofVec2f(cordX2,Nh*1.58), 0.9);
    CalculateTreeStructureOP(PosList3,ofVec2f(cordX3,Nh*1.54), 0.85);
        MassXcenters[0] = ofVec2f(cordX1,Nh*1.45);
        MassXcenters[1] = ofVec2f(cordX2,Nh*1.58);
        MassXcenters[2] = ofVec2f(cordX3,Nh*1.54);
        NumLineas[0] =  CopyToVboTri(PosList1,Tree1Vbo, Leaves1Vbo);
        NumLineas[1] =  CopyToVboTri(PosList2,Tree2Vbo, Leaves2Vbo);
        NumLineas[2] =  CopyToVboTri(PosList3,Tree3Vbo, Leaves3Vbo);
        
    // Vertex for the trunks
        float deltaTronco1 = 2;
        float deltaTronco2 = 3.8;
        float deltaTronco3 = 3;
        
        ofVec3f TrunkVert[9] =
                {ofVec3f(cordX1,Nh,0.0),
                 ofVec3f(cordX1 -deltaTronco1,Nh*1.45,0.0),
                 ofVec3f(cordX1 +deltaTronco1,Nh*1.45,0.0),
                 ofVec3f(cordX2,Nh*.8,0.0),
                 ofVec3f(cordX2 -deltaTronco2,Nh*1.58,0.0),
                 ofVec3f(cordX2 +deltaTronco2,Nh*1.58,0.0),
                 ofVec3f(cordX3,Nh,0.0),
                 ofVec3f(cordX3 -deltaTronco3,Nh*1.54,0.0),
                 ofVec3f(cordX3 +deltaTronco3,Nh*1.54,0.0)
                };

      
      
        //Texture coordinates
      
        ofVec2f TrunkTexCoords[9]={
			ofVec2f(12.0*TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight() / 2.0),
			ofVec2f(11 * TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight()),
			ofVec2f(13.0*TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight()),
          
          ofVec2f(8.0*TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight()/2.0),
          ofVec2f(7*TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight()),
          ofVec2f(9.0*TextuTrunk.getWidth() / 16.0,TextuTrunk.getHeight()),
          ofVec2f(TextuTrunk.getWidth()/16.0,TextuTrunk.getHeight()/2.0),
          ofVec2f(0.0,TextuTrunk.getHeight()),
          ofVec2f(TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight())
          
//            ofVec2f(3*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight()/2.0),
//            ofVec2f(2*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight()),
//            ofVec2f(4*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight()),
//            ofVec2f(2*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight()/2.0),
//            ofVec2f(1*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight()),
//            ofVec2f(3*TextuTrunk.getWidth()/8.0,TextuTrunk.getHeight())
        };

        TrunksVbo.setVertexData( &TrunkVert[0], 9, GL_DYNAMIC_DRAW );
        TrunksVbo.setTexCoordData(&TrunkTexCoords[0],9,GL_STATIC_DRAW);

        
	}

}

//--------------------------------------------------------------
void TreeDLA::draw() {
    
      
    ofxCvGrayscaleImage ToShow;
    
    FboBackground.begin();
	ofClear(255, 255, 255, 255);
        BackShader.begin();
            // XYOffset and XYScale relative to background image
            // dimentions   (1280 x 720)
  XYOffset[0] = 325.0*capX/1280.0;
  XYOffset[1] = 6.0*capY/720.0;
  XYScale[0] = 2.25*capX/1280.0;
  XYScale[1] = 1.8*capY/720.0;
  XYOffsetB[0] = 325.0*NoutX / 1280.0;
  XYOffsetB[1] = 6.0*NoutY / 720.0;
  XYScaleB[0] = 2.25*NoutX / 1280.0;
  XYScaleB[1] = 1.8*NoutY / 720.0;
            BackShader.setUniform2fv("XYoffset", XYOffsetB);
            BackShader.setUniform2fv("XYScale", XYScaleB);
            float Cmass[2]={MassXcenters[0].x,MassXcenters[0].y};
            BackShader.setUniform2fv("Cmass1", Cmass);
            Cmass[0] = MassXcenters[1].x;
            Cmass[1] = MassXcenters[1].y;
            BackShader.setUniform2fv("Cmass2", Cmass);
            Cmass[0] = MassXcenters[2].x;
            Cmass[1] = MassXcenters[2].y;
            BackShader.setUniform2fv("Cmass3", Cmass);
            ofSetColor(255, 255, 255,255);
			//Fondo.draw(-1, -1, 2, 2);
            Fondo.draw(0, 0, NoutX,NoutY);
        BackShader.end();
    FboBackground.end();
    // shadow
    
    // Other scale factor relative to screen dimensions
  XYOffset[0] = 325.0*capX/1280.0;
  XYOffset[1] = 6.0*capY/720.0;
  XYScale[0] = 2.25*capX/1280.0;
  XYScale[1] = 1.8*capY/720.0;
    
  
  theFrame.begin();
  ofClear(255, 255, 255, 0);
  //glClear(GL_COLOR_BUFFER_BIT);
/*  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);*/ 

    ofSetColor(255, 255, 255,255);
 //   FboBackground.draw(0, 0, capX,capY);
//  ofPushMatrix();
//  ofScale(1.0,1.0);
//    ShadowShader.begin();
//        ShadowShader.setUniform2fv("XYoffset", XYOffset);
//        ShadowShader.setUniform2fv("XYScale", XYScale);
//        Cmass[0] = MassXcenters[0].x;
//        Cmass[1] = MassXcenters[0].y;
////        Cmass[2]={MassXcenters[0].x,MassXcenters[0].y};
//        ShadowShader.setUniform2fv("Cmass", Cmass);
//        Tree1Vbo.draw(GL_TRIANGLES,0,NumLineas[0]);
//        TrunksVbo.draw(GL_TRIANGLES, 0, 3);
//        Cmass[0] = MassXcenters[1].x;
//        Cmass[1] = MassXcenters[1].y;
//        ShadowShader.setUniform2fv("Cmass", Cmass);
//        Tree2Vbo.draw(GL_TRIANGLES,0,NumLineas[1]);
//        TrunksVbo.draw(GL_TRIANGLES, 3, 3);
//        Cmass[0] = MassXcenters[2].x;
//        Cmass[1] = MassXcenters[2].y;
//        ShadowShader.setUniform2fv("Cmass", Cmass);
//        Tree3Vbo.draw(GL_TRIANGLES,0,NumLineas[2]);
//        TrunksVbo.draw(GL_TRIANGLES, 6, 3);
//    ShadowShader.end();

   ofTexture tex2 = TextuTrunk.getTextureReference();
  //tex2.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST);
    shaderStem.begin();
        shaderStem.setUniformTexture("TrunkTextu",  tex2, 1 );
        shaderStem.setUniform2fv("XYoffset", XYOffset);
        shaderStem.setUniform2fv("XYScale", XYScale);
  
   //    glLineWidth(10.0);
        Tree1Vbo.draw( GL_TRIANGLES,0,NumLineas[0]);
        Tree2Vbo.draw( GL_TRIANGLES,0,NumLineas[1]);
        Tree3Vbo.draw( GL_TRIANGLES,0,NumLineas[2]);
    
    
    shaderStem.end();
  ofSetColor(30,20,15,100);
 
//  temppos.x = XYoffset.x + XYScale.x*temppos.x;
//  temppos.y = XYoffset.y + XYScale.y*temppos.y;

  
  
  
    
    TrunkShader.begin();
        TrunkShader.setUniform2fv("XYoffset", XYOffset);
        TrunkShader.setUniform2fv("XYScale", XYScale);
        TrunkShader.setUniformTexture("TrunkTextu",  tex2, 1 );
        TrunksVbo.draw(GL_TRIANGLES, 0, 9);
    TrunkShader.end();
   
    
    ofEnablePointSprites();
    //ofEnableAlphaBlending();
    ofSetColor(255,255,255,255);
    
 //   glDepthMask(GL_FALSE);
    LeavesShader.begin();
    LeavesShader.setUniform2fv("XYoffset", XYOffset);
    LeavesShader.setUniform2fv("XYScale", XYScale);
    float TheDims[2];
    TheDims[0] = NoutX;
    TheDims[1] = NoutY;
    LeavesShader.setUniform2fv("XYDim", TheDims);
    
	LeaveTexture.bind();
    Leaves1Vbo.draw(GL_POINTS, 0, LeavesperTree[0]);
	Leaves2Vbo.draw(GL_POINTS, 0, LeavesperTree[1]);
    Leaves3Vbo.draw(GL_POINTS, 0, LeavesperTree[2]);
	LeaveTexture.unbind();

	LeavesShader.end();
   
    ofDisablePointSprites();
 //   ofDisableAlphaBlending();
 //   glDepthMask(GL_TRUE);
  theFrame.end();

  PorstPro.begin();
  ofClear(255, 255, 255, 0);
  PorstPro.end();

  PorstPro2.begin();
  ofClear(255, 255, 255, 0);
  PorstPro2.end();

  //PorstPro.begin();
  //FboBackground.draw(0, 0, PorstPro.getWidth(), PorstPro.getHeight());
  //theFrame.draw(0, 0, PorstPro.getWidth(), PorstPro.getHeight());
  //PorstPro.end();

  
  PorstPro.begin();
  
  ofTexture tex3 = theFrame.getTexture();
  postProShader.begin();

   float stepsXY[2] = {1.0f, 1.0f};
   postProShader.setUniformTexture("justTree",  tex3, 0 );
   postProShader.setUniform2fv("steps", stepsXY);


    theFrame.draw(0, 0, capX, capY);
  
  
  postProShader.end();
  PorstPro.end();

  PorstPro2.begin();

  ofTexture tex4 = PorstPro.getTexture();
  postProShader.begin();

  float stepsXYB[2] = { 1.0f, 1.0f };
  postProShader.setUniformTexture("justTree", tex4, 0);
  postProShader.setUniform2fv("steps", stepsXYB);


  PorstPro.draw(0, 0, capX, capY);


  postProShader.end();
  PorstPro2.end();


  PorstPro.begin();

  ofTexture tex5 = PorstPro2.getTexture();
  postProShader.begin();

  float stepsXYC[2] = { 1.0f, 1.0f };
  postProShader.setUniformTexture("justTree", tex5, 0);
  postProShader.setUniform2fv("steps", stepsXYC);


  PorstPro2.draw(0, 0, capX, capY);


  postProShader.end();
  PorstPro.end();


  PorstPro2.begin();

  ofTexture tex6 = PorstPro.getTexture();
  postProShader.begin();

  float stepsXYD[2] = { 1.0f, 1.0f };
  postProShader.setUniformTexture("justTree", tex6, 0);
  postProShader.setUniform2fv("steps", stepsXYD);


  PorstPro.draw(0, 0, capX, capY);


  postProShader.end();
  PorstPro2.end();




  ofClear(255, 255, 255, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  FboBackground.draw(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
  PorstPro2.draw(0,0,ofGetScreenWidth(),ofGetScreenHeight());
  glDisable(GL_BLEND);
  if(G_capture){
    ofPixels pix;
    theFrame.readToPixels(pix);
    ofImage BigImage;
    BigImage.allocate(capX, capY, OF_IMAGE_COLOR_ALPHA);
    BigImage.setFromPixels(pix.getPixels(),
                         capX, capY, OF_IMAGE_COLOR_ALPHA);
    ofSetColor(255, 255, 255,255);
    BigImage.saveImage("treeCapture"
                     +
                     ofGetTimestampString()
                     +
                     ".png");
    G_capture = false;
  }
  
    if(G_InputImage){
        ofSetColor(255, 255, 255,255);
        ToShow.allocate(Nw, Nh);
        ToShow.setFromPixels(DepthMap.data,Nw,Nh);
        ToShow.draw(0, 0);
        
        ofSetColor(0, 0, 0);
        for (int sisi =0; sisi<Nob; sisi++) {
            ofCircle(Nw + TheObjects[sisi].x, TheObjects[sisi].y, 4);
        }
         ofSetColor(255, 255, 255);
    }  
}


int TreeDLA:: CopyToVbo (const vector< vector<ofVec2f> >& PosList, ofVbo& TheVbo, ofVbo& TheLeavesVbo){

    // coping the structure to the ofVbo
    
    int StruSize = 0;
    int LocalNumLineas =0;
    for (int sv =0; sv < PosList.size(); sv++) {
        StruSize += PosList[sv].size();
        LocalNumLineas += 2*(PosList[sv].size()-1);
    }
    
    ofVec3f * treeVec;
    treeVec    = new ofVec3f [StruSize];
    int elecont =0;
    
    unsigned int * treeInd;
    treeInd = new unsigned int[LocalNumLineas];
    int indicount=0;
    
    ofVec3f * leavesVec;
    leavesVec = new ofVec3f [PosList.size()];
    
    for (int sv =0; sv < PosList.size(); sv++) {
        for (int iv = 0; iv < PosList[sv].size(); iv++) {
            treeVec[elecont] = ofVec3f(PosList[sv][iv].x
                                        , PosList[sv][iv].y,0.0);
            if (iv ==(PosList[sv].size()-1)) {
                float ForangleX;
                float ForangleY;
                if((PosList[sv].size()-1)<10){
                 ForangleX= PosList[sv][iv].x -
                PosList[sv][0].x;
                 ForangleY= PosList[sv][iv].y -
                    PosList[sv][0].y;}
                else{
                    ForangleX= PosList[sv][10].x -
                    PosList[sv][0].x;
                    ForangleY= PosList[sv][10].y -
                    PosList[sv][0].y;
                
                }
                
                
                
                leavesVec[sv] = ofVec3f(PosList[sv][0].x
                                        , PosList[sv][0].y,atan2(ForangleX,ForangleY));
            }

            
            if (iv >0){
                treeInd[indicount] = elecont-1;
                indicount++;
                treeInd[indicount] = elecont;
                indicount++;
            }
            elecont++;
        }
    }
    
    
    
    TheVbo.setVertexData( &treeVec[0], StruSize, GL_DYNAMIC_DRAW );
    
    TheVbo.setIndexData( &treeInd[0], LocalNumLineas, GL_DYNAMIC_DRAW );
    
    TheLeavesVbo.setVertexData( &leavesVec[0], PosList.size(), GL_DYNAMIC_DRAW );
    
    delete [] treeVec;
    delete [] treeInd;
    delete [] leavesVec;

    return LocalNumLineas;

}



int TreeDLA:: CopyToVboTri (const vector< vector<ofVec2f> >& PosList, ofVbo& TheVbo, ofVbo& TheLeavesVbo){
  
  // creating the stems of triangles
  float theWidth = 0.2;

  
  

  
  vector<ofVec3f> treeVec;
  vector<ofVec2f>TexCoords;
  
  ofVec3f * leavesVec;
  leavesVec = new ofVec3f [PosList.size()];
  
  for (int sv =0; sv < PosList.size(); sv++) {
    theWidth = 0.2;
    for (int iv = 0; iv < PosList[sv].size()-1; iv++) {
      
//      from https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader
      ofVec2f p0 = ofVec2f(PosList[sv][iv].x,PosList[sv][iv].y)
      ;
      ofVec2f p1 = ofVec2f(PosList[sv][iv+1].x,PosList[sv][iv+1].y)
      ;
      ofVec2f line = p1 - p0;
      ofVec2f theNormal = ofVec2f( -line.y, line.x).getNormalized();
      ofVec2f a = p0 - theWidth * theNormal;
      ofVec2f b = p0 + theWidth * theNormal;
      theWidth += 0.008;
      ofVec2f c = p1 - theWidth * theNormal;
      ofVec2f d = p1 + theWidth * theNormal;
      
      // copping the 2 triangles
      treeVec.push_back(ofVec3f(a.x,a.y,0.0));
      treeVec.push_back(ofVec3f(b.x,b.y,0.0));
      treeVec.push_back(ofVec3f(c.x,c.y,0.0));
      
      treeVec.push_back(ofVec3f(c.x,c.y,0.0));
      treeVec.push_back(ofVec3f(b.x,b.y,0.0));
      treeVec.push_back(ofVec3f(d.x,d.y,0.0));
      
      // texture coordinates
      float parts = (float) PosList[sv].size();
      TexCoords.push_back(ofVec2f(0.0,
                      iv*TextuTrunk.getHeight()/parts
                          ));
      TexCoords.push_back(ofVec2f(shadeHor,
                                  iv*TextuTrunk.getHeight()/parts
                                  ));
      TexCoords.push_back(ofVec2f(0.0,
                                  (iv+1)*TextuTrunk.getHeight()/parts
                                  ));
      
      TexCoords.push_back(ofVec2f(0.0,
                                  (iv+1)*TextuTrunk.getHeight()/parts
                                  ));
      TexCoords.push_back(ofVec2f(shadeHor,
                                  iv*TextuTrunk.getHeight()/parts
                                  ));
      TexCoords.push_back(ofVec2f(shadeHor,
                                  (iv+1)*TextuTrunk.getHeight()/parts
                                  ));
      
      
      
      }
      
   
    }
  
  for (int sv =0; sv < PosList.size(); sv++) {
  
    float ForangleX;
    float ForangleY;
    if((PosList[sv].size()-1)<10){
      ForangleX= PosList[sv][PosList[sv].size()-1].x -
      PosList[sv][0].x;
      ForangleY= PosList[sv][PosList[sv].size()-1].y -
      PosList[sv][0].y;}
    else{
      ForangleX= PosList[sv][10].x -
      PosList[sv][0].x;
      ForangleY= PosList[sv][10].y -
      PosList[sv][0].y;
      
    }
    
    leavesVec[sv] = ofVec3f(PosList[sv][0].x
                            , PosList[sv][0].y,atan2(ForangleX,-ForangleY));
    
  }
  
  
  
  TheVbo.setVertexData( &treeVec[0], treeVec.size(), GL_DYNAMIC_DRAW );
  TheVbo.setTexCoordData(&TexCoords[0],TexCoords.size(),GL_DYNAMIC_DRAW);
  
  TheLeavesVbo.setVertexData( &leavesVec[0], PosList.size(), GL_DYNAMIC_DRAW );
  
  delete [] leavesVec;
  
  return treeVec.size();
  
}



// triing use of Cache on trr structure
void TreeDLA::CalculateTreeStructureOP(vector< vector<ofVec2f> >& PosList,ofVec2f TargetPoint, float CenFac){
  // create a bool vector for the state of each branch
  vector<bool> alive(PosList.size(),true);
  int it  = 0;
  int EndCount = PosList.size();
  while ((EndCount >0)&&(it < 80)) {
    // iteration loop starts here
    it++;
    float dt = .2 + it/10000.0;
    // clearing the hash tablles
    for (int roH = 0; roH < HashRow; roH++) {
      for (int coH = 0; coH < HashColumn; coH++) {
      LeavesIndHash[roH][coH].clear();
      }
    }
    // move objects to the hash cells
    for (int kp = 0; kp < PosList.size(); kp++) {
      // if the path is alive
      if (alive[kp]){
         int indrow = floor(PosList[kp][it-1].y / hashdec);
         indrow = MAX(0, indrow);
         indrow = MIN(HashRow - 1, indrow);
         int indcol = floor(PosList[kp][it-1].x / hashdec);
         indcol = MAX(0, indcol);
         indcol = MIN(HashColumn - 1, indcol);
         LeavesIndHash[indrow][indcol].push_back(kp);
      }
    }
  
   // run throught the index hash table
   for (int roH = 0; roH < HashRow; roH++) {
     for (int coH = 0; coH < HashColumn; coH++) {
       // while the cell is not empty
       while (!LeavesIndHash[roH][coH].empty()){
        std::list<int>::iterator
        itCell = LeavesIndHash[ roH][coH].begin();
        int refLeave = (*itCell++);
        float minDis =10000000;
        int minIndi = -1;
        while (itCell != LeavesIndHash[roH][coH].end())
        {
          int compLeave = (*itCell++);
          float currDist = PosList[refLeave][it-1].squareDistance(PosList[compLeave][it-1]);
            if (currDist < minDis) {
              minDis = currDist;
              minIndi = compLeave;
            }
        }
        if (minIndi != -1){ //if a matching leave was found
          ofVec2f TheF1;
          ofVec2f TheF2;
          ofVec2f LocalTarget;
          LocalTarget.x = TargetPoint.x;
          LocalTarget.y = TargetPoint.y - CenFac*(TargetPoint.y -MAX(PosList[refLeave][it-1].y, PosList[minIndi][it-1].y));
          ofVec2f VecN = PosList[minIndi][it-1]-PosList[refLeave][it-1];
          ofVec2f VecC1,VecC2;
          ofVec2f GradValueC;
          ofVec2f GradValueMinIndi;
          
          
          VecC1 = LocalTarget - PosList[refLeave][it-1];
          //      VecC1.normalize();
          VecC2 = LocalTarget - PosList[minIndi][it-1];
          //    VecC2.normalize();
          
          
          VecN.normalize();
          VecC1.normalize();
          VecC2.normalize();
          float lKc;
          lKc = ((float)it*(G_ItFact))*Kc;
          
          TheF1 =VecC1*lKc + VecN*Kn;
          
          
          TheF2 = VecC2*lKc + VecN*(-Kn);
          
          
          // update position
          
          ofVec2f newPos1 = PosList[refLeave][it-1]+ TheF1*dt;
          if(((newPos1.x - LocalTarget.x)*
              (PosList[refLeave][it-1].x - LocalTarget.x))<0){
            newPos1.x = LocalTarget.x;
          }
          if(newPos1.y>LocalTarget.y){
            newPos1.y=LocalTarget.y;
          }
          
          ofVec2f newPos2 = PosList[minIndi][it-1]+ TheF2*dt;
          if(((newPos2.x - LocalTarget.x)*
              (PosList[minIndi][it-1].x - LocalTarget.x))<0){
            newPos2.x = LocalTarget.x;
          }
          if(newPos2.y>LocalTarget.y){
            newPos2.y=LocalTarget.y;
          }
          
          
          // creating the new  list
          
          PosList[refLeave].push_back(newPos1);
          if (newPos1.squareDistance(newPos2)>joinTh){
            PosList[minIndi].push_back(newPos2);
          }
          else{
            PosList[minIndi].push_back(newPos1);
            alive[minIndi]=false;
            EndCount--;
            if(newPos1.squareDistance(LocalTarget)<G_ThTarget)
            {
              PosList[refLeave].push_back(LocalTarget);
              PosList[refLeave].push_back(TargetPoint);
              alive[refLeave]=false;
              EndCount--;
            }
          }
        }
        else{
          ofVec2f TheF1;
          ofVec2f VecC;
          ofVec2f LocalTarget;
          LocalTarget.x = TargetPoint.x;
          LocalTarget.y = TargetPoint.y - CenFac*(TargetPoint.y -PosList[refLeave][it-1].y);
          float lKc;
          lKc = ((float)it*(G_ItFact))*Kc;
          VecC = LocalTarget - PosList[refLeave][it-1];
          VecC.normalize();
          TheF1 = VecC*lKc;
          ofVec2f newPos1 = PosList[refLeave][it-1]+ TheF1*(dt);
          if(((newPos1.x - LocalTarget.x)*
              (PosList[refLeave][it-1].x - LocalTarget.x))<0){
            newPos1.x = LocalTarget.x;
          }
          if(newPos1.y>LocalTarget.y){
            newPos1.y=LocalTarget.y;
          }
          
          PosList[refLeave].push_back(newPos1);
          if(newPos1.squareDistance(LocalTarget)<G_ThTarget)
          {
            PosList[refLeave].push_back(LocalTarget);
            PosList[refLeave].push_back(TargetPoint);
            alive[refLeave]=false;
            EndCount--;
          }
        }
        // delete element for the list
        // remove both leaves from the cell
         std::list<int>::iterator
        itCell2 = LeavesIndHash[roH][coH].begin();
        LeavesIndHash[roH][coH].erase(itCell2);
        // point to the begin again
         if (!LeavesIndHash[roH][coH].empty()){
           itCell2 = LeavesIndHash[ roH][coH].begin();
           while (itCell2!=LeavesIndHash[roH][coH].end()) {
             int val = (*itCell2);
             if (val==minIndi){
              LeavesIndHash[roH][coH].erase(itCell2);
               break;
             }
             itCell2++;
           }
         }// end if cell not empty
        
       }
      
      } // for loops cell
     } // for loop cell
  } // while iterations
} // close function



//--------------------------------------------------------------
void TreeDLA::exit() {
    ////	kinect.setCameraTiltAngle(0); // zero the tilt on exit
    ////	kinect.close();
    //	kinectPlayer.close();
    //	kinectRecorder.close();
}


//--------------------------------------------------------------
void TreeDLA::keyPressed (int key) {
	switch (key) {

        case 'a':
            G_alpha-= .005;
            if (G_alpha<0.0){G_alpha=0.0;}
            cout<<"alpha:"<<G_alpha<<"\n";
            break;        
        
        case 'z':
            G_alpha+= .005;
           // if (G_alpha>1.0){G_alpha=1.0;}
            cout<<"alpha:"<<G_alpha<<"\n";           
            break;
               
        case 's':
            Imin++;
            if (Imin>=Imax){Imin =Imax-1;}
            cout<<"Imin:"<<Imin<<"\n";
            
            break;
        case 'x':
            Imin--;
            if (Imin<=0){Imin=0;}
            cout<<"Imin:"<<Imin<<"\n";
            break;    
        case OF_KEY_UP:
            XYOffset[1]++;
            
            cout<<"Yoffset:"<<XYOffset[1]<<"\n";
            
            break;
        case OF_KEY_DOWN:
            XYOffset[1]--;
            cout<<"Yoffset:"<<XYOffset[1]<<"\n";
            break;
            
        case OF_KEY_RIGHT:
            shadeHor++;
      
            cout<<"shadeHor:"<<shadeHor<<"\n";
            
            break;
        case OF_KEY_LEFT:
            shadeHor--;
            cout<<"shadeHor:"<<shadeHor<<"\n";
            break;    
        case 'v':
            G_DithThreshold++;
            cout <<"G_DithThreshold:"<<G_DithThreshold<<"\n";
            if (G_DithThreshold>255){G_DithThreshold=255;}
            break;
        case 'f':
            G_DithThreshold--;
            cout <<"G_DithThreshold:"<<G_DithThreshold<<"\n";
            if (G_DithThreshold<0){G_DithThreshold=0;}
            break;
        
        case '1':
            G_NewModo++;
            if (G_NewModo>3){G_NewModo=0;}
            break;  
        case '2':
            G_NewModo--;
            if (G_NewModo<0){G_NewModo=3;}
            break;   
        case '6':
            G_InputImage=!G_InputImage;
            
			break;
        case '5':
           
            break;
        case ' ':
            G_capture=true;
            break;
            
        case 'o':
            XYScale[0]-= 0.05;
            cout<<"Xscale:"<<XYScale[0]<<"\n";
            
            break;
        case 'p':
            XYScale[0]+= 0.05;
            cout<<"Xscale:"<<XYScale[0]<<"\n";
            break;
        case 'k':
            XYScale[1]-= 0.05;
            cout<<"Yscale:"<<XYScale[1]<<"\n";
            break;
        case 'l':
            XYScale[1]+= 0.05;
            cout<<"Yscale:"<<XYScale[1]<<"\n";
            break;
            
            
        case 'c':
            Imax--;
            if (Imax<=Imin){Imax =Imin+1;}
            cout<<"Imax:"<<Imax<<"\n";
            break; 
            
        case 'd':
            Imax++;
            if (Imax>=255){Imax =255;}
            cout<<"Imax:"<<Imax<<"\n";
            break; 
        case 'n':
            FondSelect--;
            if (FondSelect <0){
                FondSelect = FondSize-1;
            }
            Fondo.loadImage(dir.getPath(FondSelect));
            cout<<dir.getPath(FondSelect)<<endl;
            break;  
        case 'm':
            FondSelect++;
            if (FondSelect >FondSize-1){
                FondSelect = 0;
            }
            Fondo.loadImage(dir.getPath(FondSelect)); 
            cout<<dir.getPath(FondSelect)<<endl;
            break;

        case 'g':
            EquaHist = !EquaHist;
          
            break;  
        case 'y':

            break;  
            
  

	}
}

//--------------------------------------------------------------
void TreeDLA::mouseMoved(int x, int y) {
    //	pointCloudRotationY = x;
}

//--------------------------------------------------------------
void TreeDLA::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void TreeDLA::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void TreeDLA::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void TreeDLA::windowResized(int w, int h)
{}





void TreeDLA::BWTest(const unsigned char * src, unsigned char * dst, int w, int h) {
    
    int pxPos;
    int oldPx = 0;
    int newPx = 0;
    int qError = 0;
    
    int threshold = G_DithThreshold;
    memcpy(dst, src, w*h);
    int r1, r2,  bl1, bc1, br1, bc2;
    int new1,new2;
    for (int j = 0; j < h-2; j++) {
        for (int i = 1; i < w-2; i++) {
            pxPos = j*w+i;
            oldPx = dst[j*w+i];
            newPx = oldPx < threshold ? 0 : 255;
            dst[j*w+i] = newPx;
            
            qError = oldPx - newPx;
            
            //if (qError <0)
            {qError*=2*G_ThresWhiter;}
            
            // super duper ugly 2 part value clipping (to avoid values beyond 0-255)
            r1  = MIN(dst[j*w+(i+1)]     + (int)(.125f * qError), 255);
            r2  = MIN(dst[j*w+(i+2)]     + (int)(.125f * qError), 255);
            bl1 = MIN(dst[(j+1)*w+(i-1)] + (int)(.125f * qError), 255);
            bc1 = MIN(dst[(j+1)*w+(i)]   + (int)(.125f * qError), 255);
            br1 = MIN(dst[(j+1)*w+(i+1)] + (int)(.125f * qError), 255);
            bc2 = MIN(dst[(j+2)*w+(i)]   + (int)(.125f * qError), 255);
//            new1 = MIN(dst[(j+1)*w+(i+2)] + (int)(.125f * qError), 255);
//            new2 = MIN(dst[(j+2)*w+(i+1)]   + (int)(.125f * qError), 255);
            
            
            dst[j*w+(i+1)]     = MAX(r1 , 0);
            dst[(j+1)*w+(i-1)] = MAX(bl1, 0);
            dst[(j+1)*w+(i)]   = MAX(bc1, 0);
            dst[(j+1)*w+(i+1)] = MAX(br1, 0);
            dst[(j+2)*w+(i)]   = MAX(bc2, 0);
//            dst[(j+1)*w+(i+2)] = MAX(new1, 0);
//            dst[(j+2)*w+(i+1)]   = MAX(new2, 0);            
            if (qError>0){
                PointQueue.push(ThePoints(Nw-i,j,qError));
            }
            
        }
    }
}



void TreeDLA::MatchObjetTargetsPart1() {

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
} // End of function




void TreeDLA::MatchObjetTargetsPart2() {
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
