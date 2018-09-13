#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ThePoints.h"
#include "TheNode.h"

// nearest neighbour functions
//#include "ofxNearestNeighbour.h"


class TreeDLA : public ofBaseApp {
public:
    
    void setup();
    void update();
    void draw();
    void exit();
    
    
    void keyPressed  (int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void BWTest(const unsigned char * src, unsigned char * dst, int w, int h);
    
    // sombra
//    void CalculateTreeStructure(vector<vector<ofVec2f> >& PosList,ofVec2f TargetPoint, float CenFac);
  void CalculateTreeStructureOP(vector<vector<ofVec2f> >& PosList,ofVec2f TargetPoint, float CenFac);
//	int CalculateTreeStructure(vector< vector<ofVec2f> >& PosList,
//		vector<ofVec2f> TargetPoints, ofVbo& TheVbo, 
//		ofVbo& TheLeavesVbo);

//    int CopyToVbo (const vector<vector<ofVec2f> >& PosList,ofVbo& TheOfVbo, ofVbo& TheLeavesVbo, ofVec2f TargetPoint);
  int CopyToVbo (const vector< vector<ofVec2f> >& PosList, ofVbo& TheVbo, ofVbo& TheLeavesVbo);
  int CopyToVboTri(const vector< vector<ofVec2f> >& PosList, ofVbo& TheVbo, ofVbo& TheLeavesVbo);
	// calculate matching
	void MatchObjetTargetsPart1();
	void MatchObjetTargetsPart2();

    
    ofVideoGrabber 		vidGrabber;
    
    ofxCvColorImage		colorImg;
    
    ofxCvGrayscaleImage grayImage;			// grayscale depth image

     
    
    // textures
    ofImage Fondo;

    
    
    
   
    
    bool G_InputImage;

    
    int G_NewModo;
    
    int G_Continuous;
    float Ktar,Kdam;



    float ThDis;
    
    float G_alpha;
    float G_beta;
    unsigned char * px;
    unsigned char *fixedThresholdPx;
    priority_queue<ThePoints, vector<ThePoints>,greater<vector<ThePoints>::value_type> > PointQueue;

    float Kn,Kc;
    float joinTh;
    float TreeThres1;
    float TreeThres2;    
    int G_FrameCounter;
    
    
    // some control globals 
    float G_ThTarget;
    float G_ItFact;
    float G_background;
    
    
    // threshold for dithering
    float G_DithThreshold;
   /// the image
    cv::Mat DepthMap;
    
   
    // to save the inputs
    
    ofImage Colorsave;
    ofImage ArbolSave;
    
    bool G_capture;
    int G_SaveCounter;
    
    
    int G_TheSource;
    float G_Quality;
    float G_minDistance;
    
    int Xoffset;
 
    float G_ThresWhiter;
    

    // aspect control
    
  int  AlphaSombra;
  int  ColorSombra;
  float  ScaleHojas;
  float  SlopeSombra;
  float  ScaleXSombra;
  float  ScaleYSombra;
  float BrincoR;
  float BrincoG;  
    
    vector<ofVec2f> TheTargets;
    vector<int> TargetHits;
  
  void LeavesCentroid(TheNode*, const vector<ofVec3f> &, int lisInd);
  void generatVertexArray1(TheNode* Node);
  void generatVertexArray2(TheNode* Node);
  void generatVertexArray3(TheNode* Node);
  ofFbo theFrame;
  ofFbo PorstPro;
  ofFbo PorstPro2;
  ofShader postProShader;
  
  
//private:
//    ofxNearestNeighbour2D nn;
//    vector<NNIndex> indices;
//    
//    ofxNearestNeighbour2D nnOb;
//    vector<NNIndex> indicesOb;
//  
//    ofxNearestNeighbour2D nnTg;
//    vector<NNIndex> indicesTg;
    
};
  



