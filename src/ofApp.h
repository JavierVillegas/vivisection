#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ThePoints.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // images for reading input
        ofVideoGrabber         vidGrabber;
    ofxCvColorImage        colorImg;
    
    ofxCvGrayscaleImage grayImage;            // grayscale depth image
    
    // constans forobjects dynamics
    float Ktar,Kdam;
    float ThDis;
    
   // vectors for targets and targets hits
    
    vector<ofVec2f> TheTargets;
    vector<int> TargetHits;
    
    
    // dithering function
    void Dither(cv::Mat src, cv::Mat  &dst);
    void NonLinear(cv::Mat &src);
    float G_alpha;
    
    //matching functions
    void MatchObjetTargetsPart1();
    void MatchObjetTargetsPart2();



    // priority queue to save the darkest points
    priority_queue<ThePoints, vector<ThePoints>,greater<vector<ThePoints>::value_type> > PointQueue;
    
    //Fbos
    ofFbo theFrame;
    ofFbo PorstPro;
    ofFbo fboGaussian;
    ofFbo fboGaussianDerivativeX;
    ofFbo fboGaussianDerivativeY;
    ofFbo preGaussian;
    ofFbo fboMaterial;
    //shaders
    
    // metaballs shader
    ofShader metaballs;
    ofVbo ballsVbo;
    
    // postpro shader
    ofShader postProShader;
    // gaussian sprite shader
    ofShader gaussSprite;
    ofShader gaussSpriteDerivative;
    
    
    ofShader materialShader;
    
    
    
};
