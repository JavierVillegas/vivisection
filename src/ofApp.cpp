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

const int Nob=6000;
const int Ntg=6000;

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
float var = 48;
float alpha = 0.45;

ofImage fondoFinal;
cv::Mat Transf;

//--------------------------------------------------------------
void ofApp::setup(){
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(Nw,Nh);
    colorImg.allocate(Nw, Nh);
    grayImage.allocate(Nw, Nh);
    
    ThDis =13.4;
    Ktar = 19.25;
    Kdam = 2.05;

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
    PorstPro.allocate(Rw, Rh,GL_RGBA32F);
    fboMaterial.allocate(Rw, Rh,GL_RGBA32F);
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
    fondoFinal.load("images/Balsamic.jpg");
    
    // calculating perspective transformation matrix
    
    cv::Point2f InPoints[4] = {
        cv::Point2f(Rw/2.0,0),
        cv::Point2f(Rw,Rh/2.0),
        cv::Point2f(Rw/2.0,Rh),
        cv::Point2f(0,Rh/2.0)
    };
    
    cv::Point2f OutPoints[4] = {
        cv::Point2f(1102,343),
        cv::Point2f(1447,585),
        cv::Point2f(1117,859),
        cv::Point2f(747,591)
    };
    
    Transf = cv::getPerspectiveTransform(InPoints,OutPoints);
    cout<<Transf<<endl;
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
    ofxCvGrayscaleImage ToShow;
    ofSetColor(255, 255, 255,255);
//    colorImg.draw(0,0,Nw,Nh);
    if (!TheDither.empty()){
        ToShow.allocate(TheDither.cols, TheDither.rows);
        ToShow.setFromPixels(TheDither.data,TheDither.cols, TheDither.rows);
        ToShow.draw(0, Nh,Nw,Nh);
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
      ballsVbo.draw(GL_POINTS, 0, Nob);
      GaussTexture.unbind();
      metaballs.end();
    
     metaballs.begin();
        GaussTextureDerivX.bind();
        metaballs.setUniform1i("TheChannel", 2);
        ballsVbo.draw(GL_POINTS, 0, Nob);
        GaussTextureDerivX.unbind();
     metaballs.end();
    
    metaballs.begin();
    GaussTextureDerivY.bind();
    metaballs.setUniform1i("TheChannel", 3);
    ballsVbo.draw(GL_POINTS, 0, Nob);
    GaussTextureDerivY.unbind();
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
    
    
    fboMaterial.begin();
    ofClear(0, 0, 0, 255);
    ofTexture tex2 = theFrame.getTexture();
    materialShader.begin();
    materialShader.setUniformTexture("tex1",  tex2, 1 );
    materialShader.setUniform1f("alpha", alpha);
    ofSetColor(0, 0, 0,255);
    theFrame.draw(0, 0, Rw, Rh);
    materialShader.end();
    fboMaterial.end();
    
    // material render
    
    
    
    
//    ofSetColor(0, 0, 0);
//    for (int sisi =0; sisi<Nob; sisi++) {
//        ofCircle(Nw + TheObjects[sisi].x, TheObjects[sisi].y, 4);
//    }

  //  theFrame.draw(Nw,0,2*320,2*240);
   // PorstPro.draw(Nw,2.2*Nh,2*320,2*240);
    ofClear(0, 0, 0,0);
   ofEnableAlphaBlending();

    
    
   ofSetColor(255, 255, 255,255);
    fondoFinal.draw(0,0,1920,1080);
    fboMaterial.draw(746,348,712,516);
    ofDisableAlphaBlending();
    
   glDisable(GL_BLEND);
 //   fboGaussianDerivativeX.draw(0,0,256,256);
  //   fboGaussian.draw(0,256,256,256);
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
    for (int j = 0; j < h; j++) {
        float ncj = (j/(float)h - 0.5);
        for (int i = 0; i < w; i++) {
            float nci = (i/(float)w - 0.5);
            uchar newVal;
            if ((ncj*ncj+nci*nci)<0.25){
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

