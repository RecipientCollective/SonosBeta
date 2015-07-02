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
    syphonBlobTextureServer.setName("SonosBlob");
}
#endif

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
    // setup of std::map
    for (int i =0; i<BLOBMAX; i++)
    {
        vbuffer.insert(pair<int, vector<float> >(i, vector<float>(BUFFERSIZE)));
    }
    
#ifdef DEBUG
    for ( map<int, vector<float> >::const_iterator it = vbuffer.begin(); it != vbuffer.end(); ++it)
    {
        cerr << "Actor: " << it->first << " vbuffer size: " << it->second.size() << endl;
    }
#endif
    
    ofLogNotice() << "sending osc messages to " << ofToString(HOST) << ":" << ofToString(PORT) << endl;
}