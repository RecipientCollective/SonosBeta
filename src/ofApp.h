#pragma once

/************************************************************
 * INCLUDES INFO:
 * 
 * ofxCv:
 *
 * Even if we use ofxCv we need from ofxOpenCv: opencv.a (../../addons/ofxOpenCv/libs/opencv/lib/osx)
 * Opencv headers in HEADER_SEARCH_PATH
 *    
 *    ../../../addons/ofxOpenCv/libs/opencv/include
 *    ../../../addons/ofxCv/libs/CLD/include
 *    ../../../addons/ofxCv/libs/CLD/include
 *
 *
 * ofxSyphon:
 * 
 *   1. Drag addons/ofxSyphon/lib/Syphon/lib/osx/Syphon.framework to addons/ofxSyphon group
 *
 *   2. Copy Files build phase to new projects to copy the Syphon framework into the 
 *      Frameworks folder of the built product.
 *
 *   3. Check and edit the location of the Syphon framework (addons/ofxSyphon/lib/Syphon/lib/osx) 
 *      to the Framework Search Paths Build Setting in your Xcode project.
 *      USE RELATIVE PATH: "$(SRCROOT)/../../../addons/ofxSyphon/libs/Syphon/lib/osx"
 *   
 *   4. Add Syphon.framework to Link Binary with Libraries Build Phase
 *   
 *   5. From ../../addons/ofxSyphon/libs/Syphon/src add 
 *      to compile source phases the file: SyphonNameboundClient.m
 *
 *
 * DEBUG: the DEBUG preprocessing value is set in Build Settings --> Preprocessor Macro
 ************************************************************/


#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxUI.h"


// Uncomment to activate VideoLive
#define _USE_LIVE_VIDEO
#define _USE_SYPHON_VIDEO

#ifdef _USE_SYPHON_VIDEO
#include "ofxSyphon.h"
#endif


#define HOST "localhost"
#define PORT 8000
#define OUTPUT_WIDTH  1024
#define OUTPUT_HEIGHT 768
#define CAMWIDTH 640
#define CAMHEIGHT 480
#define MARGIN 10
#define DEVICE_NAME "DV-VCR"
#define FRAMERATE 60
#define CIRCLE_RESOLUTION 40
#define FILENAME "videos/Movie_okjpg.mov"
#define SCALE_FACTOR 0.5

// ofxUI GUI
#define GUIWIDTH 200
#define GUISETTINGS "GUI/ofxui_settings.xml"

// limit the max number of tracked blobs for velocity buffers
#define BLOBMAX 5
#define BUFFERSIZE 20

//------------------------------------------------------------------
// OPENCV DEFAULTS
//------------------------------------------------------------------

// blur is perfomed over a square BLUR_LEVEL * BLUR_LEVEL (10x10)
// this is the blur for diff image
#define BLUR_LEVEL 10
// this is the blur for input image
#define BLUR_INPUT_IMAGE 0

// contour finder area radius limits
#define MIN_AREA_RADIUS 10
#define MAX_AREA_RADIUS 100
// by default, the threshold is
// 128. when finding bright regions, 128 is halfway between white and black. when
// tracking a color, 0 means "exactly similar" and 255 is "all colors".
#define CONTOUR_THRESHOLD 10
// persistence determines how many frames an
// object can last without being seen until the tracker forgets about it.
#define BLOB_PERSISTENCE 15
// maximumDistance determines how far an object can move until the tracker
// considers it a new object.
#define BLOB_MAX_DISTANCE 32


class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
private:
    
#ifdef _USE_LIVE_VIDEO
    ofVideoGrabber vidGrabber;
#else
    ofVideoPlayer vidPlayer;
#endif
    
    // FUNCTIONS
    void ofSetup();
    void guiSetup();
    void cameraSetup();
#ifdef _USE_SYPHON_VIDEO
    void syphonSetup();
    void syphonDraw();
#endif
    void drawDebugVideo();
    void blobSetup();
    void blobUpdate();
    void oscSetup();
    void oscUpdate();
    
    // CAMERA
    int camWidth;
    int camHeight;
    string filename;
    string deviceName;
    
    // DEBUG DRAW
    bool showDebug;
    bool showLabels;
    int spacerx;
    int spacery;
    int margin;
    
    // OPENCV 2
    ofPixels background;
    ofImage diff;
    ofImage input;
    bool bLearnBackground;
    ofxCv::ContourFinder contourFinder;
    bool bNewFrame;
    ofColor targetColor;
    
    // ofxUI
    bool showGui;
    ofxUISuperCanvas *guiSonos;
    bool useTargetColor;
    int blurLevel;
    int blurInputImage;
    int minAreaRadius;
    int maxAreaRadius;
    float contourThreshold;
    int blobPersistence;
    int blobMaxDistance;
    void guiEvent(ofxUIEventArgs &e);
    
    // SYPHON
#ifdef _USE_SYPHON_VIDEO
    ofxSyphonServer syphonBlobTextureServer;
    ofFbo blobsFbo;
#endif
    
    // OSC
    ofxOscSender sender;
    ofxOscMessage m;
    bool bOscContour;
    
    // VELOCITY BUFFERS
    map<int, vector <float> > vbuffer;
    map<int, vector <ofVec2f> > vpbuffer;
};
