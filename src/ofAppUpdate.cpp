//
//  ofAppUpdate.cpp
//  SonosBeta
//
//  Created by tucano on 27/05/15.
//
//

#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
// UPDATE FUNCTIONS
//--------------------------------------------------------------

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