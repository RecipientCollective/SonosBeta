//
//  ofAppDraw.cpp
//  SonosBeta
//
//  Created by tucano on 27/05/15.
//
//

#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
// DRAW FUNCTIONS
//--------------------------------------------------------------

#ifdef _USE_SYPHON_VIDEO
void ofApp::syphonDraw()
{
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
    
    // END OF SCALE scalef (for input drawing)
    ofPopMatrix();
    
    if (showLabels)
    {
        ofPushMatrix();
        ofTranslate(margin, ofGetHeight() - 50);
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