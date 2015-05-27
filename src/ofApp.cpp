#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
// OPEN FRAMEWORKS FUNCTIONS
//--------------------------------------------------------------
void ofApp::setup()
{
    
#ifdef DEBUG
    ofSetLogLevel(OF_LOG_VERBOSE);
#else
    ofSetLogLevel(OF_LOG_ERROR);
#endif
    
    ofLogNotice("START SETUP");
    showDebug = true;
    showLabels = true;
    
    ofSetup();
    cameraSetup();
    guiSetup();
    blobSetup();
    oscSetup();
#ifdef _USE_SYPHON_VIDEO
    syphonSetup();
#endif
    
    // setup draw
    ofBackground(0);
    
    ofLogNotice("END SETUP");
}

//--------------------------------------------------------------
void ofApp::update()
{
    blobUpdate();
    oscUpdate();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    if (showDebug)
    {
        ofPushStyle();
        drawDebugVideo();
        ofPopStyle();
    }
    
    if (showGui)
    {
        ofPushStyle();
        guiSonos->draw();
        ofPopStyle();
    }

#ifdef _USE_SYPHON_VIDEO
    syphonDraw();
#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
    // ofxUI
    guiSonos ->saveSettings(GUISETTINGS);
    delete guiSonos;
    
    //magari c'è da chiudere la cam o i video da verificare;
    OF_EXIT_APP(0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key)
    {
        case 'g':
            showGui = !showGui;
            break;
        case 'h':
            showDebug = !showDebug;
            break;
        case ' ':
            bLearnBackground = true;
            break;
        case 'l':
            showLabels = !showLabels;
            break;
        default:
            break;
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
void ofApp::mousePressed(int x, int y, int button)
{
    if (button == 2)
    {
        ofImage tmpImg;
        tmpImg.grabScreen(x, y, 1, 1);
        ofPixels ofpx = tmpImg.getPixelsRef();
        targetColor = ofpx.getColor(0,0);
        contourFinder.setTargetColor(targetColor, TRACK_COLOR_RGB);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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