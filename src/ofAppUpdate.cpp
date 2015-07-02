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

// update blobs
void ofApp::blobUpdate()
{
    // we need this due to the multi compilation paths
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
        float areaDelta = 0.0f;
        
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

        if (tracker.existsPrevious(label))
        {
            ofRectangle boundingBoxPrevious = toOf(tracker.getPrevious(label));
            float areaPrevious = boundingBoxPrevious.width * boundingBoxPrevious.height;
            areaDelta = areaPrevious - area;
            addr.str(std::string());
            addr << "/blob/" << i << "/delta/area";
            m.setAddress(addr.str());
            m.addFloatArg(areaDelta);
            sender.sendMessage(m);
            m.clear();
        }

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
        addr << "/blob/" << i << "/instantvelocity";
        m.setAddress(addr.str());
        m.addFloatArg(velocity.length());
        sender.sendMessage(m);
        m.clear();
        
        // age
        if (tracker.existsCurrent(label))
        {
            int age = tracker.getAge(label);
            addr.str(std::string());
            addr << "/blob/" << i << "/age";
            m.setAddress(addr.str());
            m.addFloatArg(age);
            sender.sendMessage(m);
            m.clear();
        }
        
        // contour point
        if (bOscContour)
        {
            ofPolyline pl = toOf(contourFinder.getContour(i));
            for (int n = 0; n < pl.getVertices().size(); n++)
            {
//                ofLogNotice() << "ACTOR: " << i << "LABEL: " << label << " POINT: " << pl.getVertices()[i];
                addr.str(std::string());
                addr << "/blob/" << i << "/contour";
                m.setAddress(addr.str());
                m.addIntArg(n);
                m.addFloatArg(pl.getVertices()[i].x);
                m.addFloatArg(pl.getVertices()[i].y);
                sender.sendMessage(m);
                m.clear();
            }
        }
        
        // VELOCITY BUFFER
        if (i < BLOBMAX)
        {
            // record velocity for vbuffer meanvelocity
            vector<float> v = vbuffer[i];
            // erase first element
            v.erase(v.begin());
            // add current v as last element
            v.push_back(velocity.length());
            // push back
            vbuffer[i] = v;
            float sum = accumulate(v.begin(), v.end(), 0.0);
            float mean = sum/v.size();
            addr.str(std::string());
            addr << "/blob/" << i << "/meanvelocity";
            m.setAddress(addr.str());
            m.addFloatArg(mean);
            sender.sendMessage(m);
            m.clear();
            
            // vpbuffer
            vector<ofVec2f> vp = vpbuffer[i];
            vp.erase(vp.begin());
            vp.push_back(velocity);
            vpbuffer[i] = vp;
            ofVec2f centroid = centroid.average(vp.data(), vp.size());
            addr.str(std::string());
            addr << "/blob/" << i << "/centroidvelocity";
            m.setAddress(addr.str());
            m.addFloatArg(centroid.x);
            m.addFloatArg(centroid.y);
            sender.sendMessage(m);
            m.clear();
            
        }
    }
}