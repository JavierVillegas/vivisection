#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.setGLVersion(3, 2); //we define the OpenGL version we want to use
    settings.windowMode  = OF_FULLSCREEN;
        settings.width = 1920;
       settings.height = 1080;
    ofCreateWindow(settings);
    
    //    ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);
    //       ofSetupOpenGL(1280 , 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
