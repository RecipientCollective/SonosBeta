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
 *   2. Copy Files build phase to new projects to copy the Syphon framework into the Frameworks folder of the built product.
 *
 *   3. Check and edit the location of the Syphon framework (addons/ofxSyphon/lib/Syphon/lib/osx) to 
 *      the Framework Search Paths Build Setting in your Xcode project.
 *      USE RELATIVE PATH: "$(SRCROOT)/../../../addons/ofxSyphon/libs/Syphon/lib/osx"
 *   
 *   4. Add Syphon.framework to Link Binary with Libraries Build Phase
 *   
 *   5. From ../../addons/ofxSyphon/libs/Syphon/src add to compile source phases the file: SyphonNameboundClient.m
 *
 ************************************************************/


#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxGui.h"

// Uncomment to activate VideoLive
#define _USE_LIVE_VIDEO
#define _USE_SYPHON_VIDEO

#ifdef _USE_SYPHON_VIDEO
#include "ofxSyphon.h"
#endif

class ofApp : public ofBaseApp
{

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
};
