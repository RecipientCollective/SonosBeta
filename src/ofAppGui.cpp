//
//  ofAppGui.cpp
//  SonosBeta
//
//  Created by tucano on 27/05/15.
//
//

#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
// GUI FUNCTIONS
//--------------------------------------------------------------

// setup gui
void ofApp:: guiSetup()
{
    // ofxUI
    margin = MARGIN;
    showGui = true;
    useTargetColor = false;
    blurLevel = 0;
    blurInputImage = 0;
    minAreaRadius = MIN_AREA_RADIUS;
    maxAreaRadius = MAX_AREA_RADIUS;
    contourThreshold = CONTOUR_THRESHOLD;
    blobPersistence = BLOB_PERSISTENCE;
    blobMaxDistance = BLOB_MAX_DISTANCE;
    
    guiSonos = new ofxUISuperCanvas("IR Capturing: ");
    guiSonos->setPosition(ofGetWidth() - (GUIWIDTH + margin), margin);
    guiSonos->addSpacer();
    guiSonos->addTextInput("INSTANCE_NAME", "default")->setAutoClear( false );
    guiSonos->addSpacer();
    guiSonos->addIntSlider("BLUR DIFF", 0, camWidth, &blurLevel);
    guiSonos->addIntSlider("BLUR INPUT", 0, camWidth, &blurInputImage);
    guiSonos->addSpacer();
    guiSonos->addToggle("USE_TARGET_COLOR", &useTargetColor);
    guiSonos->addIntSlider("MIN AREA RADIUS", 0, camWidth, &minAreaRadius);
    guiSonos->addIntSlider("MAX AREA RADIUS", 0, camWidth, &maxAreaRadius);
    guiSonos->addSlider("CONTOUR THRESHOLD", 0.0f, 255.0f, &contourThreshold);
    guiSonos->addIntSlider("BLOB PERSISTENCE", 0, FRAMERATE*10, &blobPersistence);
    guiSonos->addIntSlider("BLOB MAX DISTANCE", 0, camWidth, &blobMaxDistance);
    guiSonos->addSpacer();
    guiSonos->addLabel("OSC:");
    guiSonos->addToggle("OSC SEND CONTOUR", &bOscContour);
    guiSonos->addSpacer();
    guiSonos->addLabel("KEYS:");
    guiSonos->addLabel("[g] interface");
    guiSonos->addLabel("[h] debug view");
    guiSonos->addLabel("[space] reset background");
    guiSonos->addLabel("[Mouse DX] target color");
    guiSonos->addSpacer();
    guiSonos->addLabel("LABELS LIFE:");
    guiSonos->addLabel("[current labels]")->setColorFill(cyanPrint);
    guiSonos->addLabel("[previous labels]")->setColorFill(magentaPrint);
    guiSonos->addLabel("[new labels]")->setColorFill(yellowPrint);
    guiSonos->addLabel("[dead labels]")->setColorFill(ofColor::white);
    guiSonos->addSpacer();
    guiSonos->addFPS();
    guiSonos->addSpacer();
    guiSonos->autoSizeToFitWidgets();
    ofAddListener(guiSonos->newGUIEvent, this, &ofApp::guiEvent);
    guiSonos->loadSettings(GUISETTINGS);
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
    int kind = e.getKind();
    ofLogNotice() << "got event from: " << name << endl;
    
    if (name == "USE_TARGET_COLOR")
    {
        ofxUIToggle *t = (ofxUIToggle *) e.widget;
        contourFinder.setUseTargetColor(t->getValue());
    }
    else if (name == "MIN AREA RADIUS")
    {
        ofxUIIntSlider *s = (ofxUIIntSlider *) e.widget;
        contourFinder.setMinAreaRadius(s->getValue());
    }
    else if (name == "MAX AREA RADIUS")
    {
        ofxUIIntSlider *s = (ofxUIIntSlider *) e.widget;
        contourFinder.setMaxAreaRadius(s->getValue());
    }
    else if (name == "CONTOUR THRESHOLD")
    {
        ofxUISlider *s = (ofxUISlider *) e.widget;
        contourFinder.setThreshold(s->getValue());
    }
    else if (name == "BLOB PERSISTENCE")
    {
        ofxUIIntSlider *s = (ofxUIIntSlider *) e.widget;
        contourFinder.getTracker().setPersistence(s->getValue());
    }
    else if (name == "BLOB MAX DISTANCE")
    {
        ofxUIIntSlider *s = (ofxUIIntSlider *) e.widget;
        contourFinder.getTracker().setMaximumDistance(s->getValue());
    }
}

