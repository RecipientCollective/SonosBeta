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
    ofBackground(0, 0, 0);
    
    if (showDebug)
    {
        ofPushStyle();
        drawDebugVideo();
        ofPopStyle();
    }
    
    if (showGui)
    {
        ofPushStyle();
        gui.draw();
        ofPopStyle();
    }
    
    ofPushStyle();
    drawHelp();
    ofPopStyle();

#ifdef _USE_SYPHON_VIDEO
    syphonDraw();
#endif
}

//--------------------------------------------------------------
void ofApp::exit()
{
    //magari c'è da chiudere la cam o i video da verificare;
    minAreaRadius.removeListener(this, & ofApp::minRadiusChanged);
    maxAreaRadius.removeListener(this, &ofApp::maxRadiusChanged);
    contourThreshold.removeListener(this, &ofApp::contourThresholdChanged);
    blobPersistence.removeListener(this, &ofApp::blobPersistenceChanged);
    blobMaxDistance.removeListener(this, &ofApp::blobMaxDistanceChanged);
    useTargetColor.removeListener(this, &ofApp::useTargetColorChanged);
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

//--------------------------------------------------------------
// SONOS PRIVATE FUNCTIONS
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

#ifdef _USE_SYPHON_VIDEO
void ofApp::syphonDraw()
{
    syphonScreenServer.publishScreen();
    syphonDiffTextureServer.publishTexture(&diff.getTextureReference());
    // draw static into texture.
    ofPushStyle();
    ofPushMatrix();
    blobsFbo.begin();
    ofClear(0,0,0,0);
    ofSetHexColor(0xffffff);
    for(int i = 0; i < contourFinder.size(); i++)
    {
        ofPolyline contour = toOf(contourFinder.getContour(i));
        ofPath path;
        for( int i = 0; i < contour.getVertices().size(); i++)
        {
            if(i == 0) {
                path.newSubPath();
                path.moveTo(contour.getVertices()[i] );
            } else {
                path.lineTo( contour.getVertices()[i] );
            }
        }
        path.simplify();
        path.draw();
    }
    blobsFbo.end();
    ofPopMatrix();
    ofPopStyle();
    syphonBlobTextureServer.publishTexture(&blobsFbo.getTextureReference());
}
#endif

// draw debug video
void ofApp::drawDebugVideo()
{
    ofPushMatrix();
    ofScale(0.5, 0.5, 1.0);
#ifdef _USE_LIVE_VIDEO
    vidGrabber.draw(margin, margin);
#else
    vidPlayer.draw(margin,margin);
#endif
    diff.draw(spacerx+margin, margin);
    
    ofSetBackgroundAuto(showLabels);
    RectTracker& tracker = contourFinder.getTracker();
    
    // translate coordinates to spacerx/spacery
    ofTranslate(margin, spacery);
    ofSetColor(255);
    input.draw(0,0);
    contourFinder.draw();
    
    ofTranslate(spacerx, margin);
    
    for(int i = 0; i < contourFinder.size(); i++)
    {
        ofPoint center = toOf(contourFinder.getCenter(i));
        ofPushMatrix();
        ofPolyline contour = toOf(contourFinder.getContour(i));
        ofPath path;
        for( int i = 0; i < contour.getVertices().size(); i++)
        {
            if(i == 0) {
                path.newSubPath();
                path.moveTo(contour.getVertices()[i] );
            } else {
                path.lineTo( contour.getVertices()[i] );
            }
        }
        path.simplify();
        path.draw();
        ofNoFill();
        ofRect(toOf(contourFinder.getBoundingRect(i)));
        ofPopMatrix();
        
        if (showLabels)
        {
            ofPushMatrix();
            ofTranslate(center.x, center.y);
            int label = contourFinder.getLabel(i);
            string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            ofSetColor(0,255,0);
            ofDrawBitmapString(msg, 0, 0);
            ofVec2f velocity = toOf(contourFinder.getVelocity(i));
            ofScale(5, 5);
            ofLine(0, 0, velocity.x, velocity.y);
            ofPopMatrix();
        }
    }
    
    
    if (showLabels)
    {
        ofPushMatrix();
        ofScale(4.0, 4.0);
        // this chunk of code visualizes the creation and destruction of labels
        const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
        const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
        const vector<unsigned int>& newLabels = tracker.getNewLabels();
        const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
        ofSetColor(cyanPrint);
        for(int i = 0; i < currentLabels.size(); i++) {
            int j = currentLabels[i];
            ofLine(j, 0, j, 4);
        }
        ofSetColor(magentaPrint);
        for(int i = 0; i < previousLabels.size(); i++) {
            int j = previousLabels[i];
            ofLine(j, 4, j, 8);
        }
        ofSetColor(yellowPrint);
        for(int i = 0; i < newLabels.size(); i++) {
            int j = newLabels[i];
            ofLine(j, 8, j, 12);
        }
        ofSetColor(ofColor::white);
        for(int i = 0; i < deadLabels.size(); i++) {
            int j = deadLabels[i];
            ofLine(j, 12, j, 16);
        }
        ofPopMatrix();
    }
    
    // END OF SCALE scalef (for input drawing)
    ofPopMatrix();
    
    if (showLabels)
    {
        ofPushMatrix();
        ofTranslate(margin, spacery + margin*2);
        ofDrawBitmapString("LABELS LIFE LEGEND:",0,0);
        ofSetColor(cyanPrint);
        ofDrawBitmapString("[current labels]",0,margin);
        ofSetColor(magentaPrint);
        ofDrawBitmapString("[previous labels]",0,margin*2);
        ofSetColor(yellowPrint);
        ofDrawBitmapString("[new labels]",0,margin*3);
        ofSetColor(ofColor::white);
        ofDrawBitmapString("[dead labels]",0,margin*4);
        ofPopMatrix();
    }
    
    if (useTargetColor)
    {
        ofPushMatrix();
        ofTranslate(OUTPUT_WIDTH - 150, OUTPUT_HEIGHT - 100);
        ofSetColor(255);
        ofDrawBitmapString("target color",-margin,-margin);
        ofFill();
        ofSetColor(128);
        ofRect(-3, -3, 64+6, 64+6);
        ofSetColor(targetColor);
        ofRect(0, 0, 64, 64);
        ofPopMatrix();
    }
}

// draw help (always)
void ofApp::drawHelp()
{
    ofSetHexColor(0xffffff);
    ofDrawBitmapString(helpStr, margin, OUTPUT_HEIGHT - margin*2);
}

// osc update
void ofApp::oscUpdate()
{
    RectTracker& tracker = contourFinder.getTracker();
    for(int i = 0; i < contourFinder.size(); i++)
    {
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
        ofVec2f velocity = toOf(contourFinder.getVelocity(i));
        ofRectangle boundingBox = toOf(contourFinder.getBoundingRect(i));
        float area = boundingBox.width * boundingBox.height;
        int age = tracker.getAge(label);
        
        stringstream addr;
        addr.str(std::string());
        addr << "/blob/" << i << "/centroid";
        m.setAddress(addr.str());
        m.addFloatArg(center.x);
        m.addFloatArg(center.y);
        sender.sendMessage(m);
        m.clear();
        
        addr.str(std::string());
        addr << "/blob/" << i << "/area";
        m.setAddress(addr.str());
        m.addFloatArg(area);
        sender.sendMessage(m);
        m.clear();
        
        addr.str(std::string());
        addr << "/blob/" << i << "/boundingrect";
        m.setAddress(addr.str());
        m.addFloatArg(boundingBox.width);
        m.addFloatArg(boundingBox.height);
        sender.sendMessage(m);
        m.clear();
        
        addr.str(std::string());
        addr << "/blob/" << i << "/velocity";
        m.setAddress(addr.str());
        m.addFloatArg(velocity.x);
        m.addFloatArg(velocity.y);
        sender.sendMessage(m);
        m.clear();
        
        addr.str(std::string());
        addr << "/blob/" << i << "/velocity_length";
        m.setAddress(addr.str());
        m.addFloatArg(velocity.length());
        sender.sendMessage(m);
        m.clear();
    }
}

// update blobs
void ofApp::blobUpdate()
{
    // we nned this due to the multi compilation paths
    bNewFrame = false;
#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    if (vidGrabber.isFrameNew())
    {
        bNewFrame = true;
        copy(vidGrabber, input);
    }
#else
    vidPlayer.update();
    if (vidPlayer.isFrameNew())
    {
        bNewFrame = true;
        copy(vidPlayer, input);
    }
#endif
    
    if (bNewFrame)
    {
        if (blurInputImage > 0)
        {
            blur(input, blurInputImage);
        }
        input.update();
        if (bLearnBackground == true)
        {
            copy(input, background);
            bLearnBackground = false;
        }
        absdiff(background, input, diff);
        if (blurLevel > 0)
        {
            blur(diff,blurLevel);
        }
        diff.update();
        contourFinder.findContours(diff);
    }
}