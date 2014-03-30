#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxUDPManager.h"
#include "PGRCamera.h"

#define HOST "127.0.0.1"
#define PORT 12345

class testApp : public ofBaseApp{

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

	//PGR CAMERA
	PGRCamera mPgrCam;

    ofVideoGrabber 		vidGrabber;

    ofxCvColorImage			colorImg;
	ofImage	                imageTest;

    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;

    ofxCvContourFinder 	contourFinder;

    int 				threshold;
    bool				bLearnBakground;

    //UDP connection to the localhost
	ofxUDPManager udpConnection;
};
