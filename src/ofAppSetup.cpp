//
//  ofAppSetup.cpp
//  SonosBeta
//
//  Created by tucano on 27/05/15.
//
//

#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
// SETUP FUNCTIONS
//--------------------------------------------------------------

// setup camera
void ofApp::cameraSetup()
{
#ifdef _USE_LIVE_VIDEO
    
#ifdef DEBUG
    vidGrabber.setVerbose(true);
#else
    vidGrabber.setVerbose(false);
#endif
    
    deviceName = DEVICE_NAME;
    
    // search for camera
    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    
    for(vector<ofVideoDevice>::iterator it = devices.begin(); it != devices.end(); ++it)
    {
        ofLogNotice() << "DEVICE: " << it->deviceName << " ID: " << it->id;
        
        if(it->deviceName == deviceName)
        {
            vidGrabber.setDeviceID(it->id);
            break;
        }
    }
    
    vidGrabber.initGrabber(CAMWIDTH, CAMHEIGHT);
    camWidth  = vidGrabber.getWidth();
    camHeight = vidGrabber.getHeight();
    spacerx = camWidth + margin;
    spacery = camHeight + margin;
#else
    // FILENAME
    filename = FILENAME;
    
    //check if file exists
    bool bFileThere = false;
    fstream fin;
    string fileNameInOF = ofToDataPath(filename, false); // since OF files are in the data directory, we need to do this
    fin.open(fileNameInOF.c_str(),ios::in);
    
    if ( fin.is_open() )
    {
        ofLogNotice() << "Founded file " << fileNameInOF << endl;
        bFileThere =true;
    }
    fin.close();
    
    if (bFileThere)
    {
        vidPlayer.loadMovie(filename);
        vidPlayer.play();
        camWidth = vidPlayer.getWidth();
        camHeight = vidPlayer.getHeight();
        spacerx = vidPlayer.getWidth() + margin;
        spacery = vidPlayer.getHeight() + margin;
    }
    else
    {
        ofLogError() << "File " << fileNameInOF << " is not here!" << endl;
        ofApp::exit();
    }
#endif
    
    ofLogNotice() << "Input size: width =" << camWidth << " height = " << camHeight << endl;
}

// setup openframeworks
void ofApp::ofSetup()
{
    ofSetVerticalSync(true);
    ofSetCircleResolution(CIRCLE_RESOLUTION); 	   //set resolution of circle
    ofEnableSmoothing();
    ofSetFrameRate(FRAMERATE);
}

#ifdef _USE_SYPHON_VIDEO
// syphon setup
void ofApp::syphonSetup()
{
    blobsFbo.allocate(camWidth,camHeight,GL_RGBA);
    syphonScreenServer.setName("SonosDebug");
    syphonDiffTextureServer.setName("SonosDiff");
    syphonBlobTextureServer.setName("SonosBlob");
}
#endif

// setup gui
void ofApp:: guiSetup()
{
    helpStr = "KEYS: [g] show/hide interface [h] show/hide debug view [space] reset background [l] show labels [Mouse DX] change target color";
    margin = MARGIN;
    showGui = true;
    
    minAreaRadius.addListener(this, &ofApp::minRadiusChanged);
    maxAreaRadius.addListener(this, &ofApp::maxRadiusChanged);
    contourThreshold.addListener(this, &ofApp::contourThresholdChanged);
    blobPersistence.addListener(this, &ofApp::blobPersistenceChanged);
    blobMaxDistance.addListener(this, &ofApp::blobMaxDistanceChanged);
    useTargetColor.addListener(this, &ofApp::useTargetColorChanged);
    
    gui.ofxGuiGroup::setup(APP_TITLE);
    gui.setPosition(OUTPUT_WIDTH - 300, margin);
    gui.add(guiTitle.setup("name",APP_INSTANCE));
    gui.add(useTargetColor.setup("target color", false));
    gui.add(blurLevel.setup("blur diff",BLUR_LEVEL,0,camWidth));
    gui.add(blurInputImage.setup("blur input",BLUR_INPUT_IMAGE,0,camWidth));
    gui.add(minAreaRadius.setup("min radius",MIN_AREA_RADIUS,0,camWidth));
    gui.add(maxAreaRadius.setup("max radius",MAX_AREA_RADIUS,0,camWidth));
    gui.add(contourThreshold.setup("contour threshold",CONTOUR_THRESHOLD,0,255));
    // max set to 10 seconds
    gui.add(blobPersistence.setup("persistence", BLOB_PERSISTENCE,0,FRAMERATE*10));
    gui.add(blobMaxDistance.setup("max distance",BLOB_MAX_DISTANCE,0,camWidth));
#ifdef _USE_SYPHON_VIDEO
    gui.add(sendToSyphon.setup("syphon", true));
#endif
}

// gui events
void ofApp::minRadiusChanged(int & minRadius)
{
    contourFinder.setMinAreaRadius(minRadius);
}

void ofApp::maxRadiusChanged(int & maxRadius)
{
    contourFinder.setMaxAreaRadius(maxRadius);
}

void ofApp::contourThresholdChanged(float &threshold)
{
    contourFinder.setThreshold(threshold);
}

void ofApp::blobPersistenceChanged(int &persistence)
{
    contourFinder.getTracker().setPersistence(persistence);
}

void ofApp::blobMaxDistanceChanged(int & distance)
{
    contourFinder.getTracker().setMaximumDistance(distance);
}

void ofApp::useTargetColorChanged(bool & bChanged)
{
    contourFinder.setUseTargetColor(bChanged);
}

// setup blobs (opencv)
void ofApp::blobSetup()
{
#ifdef _USE_LIVE_VIDEO
    // opencv2
    imitate(background, vidGrabber);
    imitate(diff, vidGrabber);
    imitate(input, vidGrabber);
#else
    // opencv2
    imitate(background, vidPlayer);
    imitate(diff, vidPlayer);
    imitate(input, vidPlayer);
#endif
    bLearnBackground = true;
    targetColor = ofColor(255,255,255);
    contourFinder.setTargetColor(targetColor);
    contourFinder.setUseTargetColor(useTargetColor);
    contourFinder.setMinAreaRadius(minAreaRadius);
    contourFinder.setMaxAreaRadius(maxAreaRadius);
    contourFinder.setThreshold(contourThreshold);
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(blobPersistence);
    // an object can move up to 32 pixels per frame
    contourFinder.getTracker().setMaximumDistance(blobMaxDistance);
}

// osc setup
void ofApp::oscSetup()
{
    sender.setup(HOST, PORT);
    ofLogNotice() << "sending osc messages to " << ofToString(HOST) << ":" << ofToString(PORT) << endl;
}