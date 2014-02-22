#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

#ifdef _USE_LIVE_VIDEO
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
#else
    vidPlayer.loadMovie("fingers.mov");
    vidPlayer.play();
#endif

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

	bLearnBakground = true;
	threshold = 80;

	//create the socket and set to send to host:port
    udpConnection.Create();
    udpConnection.Connect(HOST,PORT);
    udpConnection.SetNonBlocking(true);
	printf("Open UDP connection");
}

//--------------------------------------------------------------
void testApp::update(){

    ofBackground(100,100,100);

    bool bNewFrame = false;

#ifdef _USE_LIVE_VIDEO
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
#else
    vidPlayer.update();
    bNewFrame = vidPlayer.isFrameNew();
#endif

	if (bNewFrame){

#ifdef _USE_LIVE_VIDEO
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
#else
        colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
#endif

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
	}

}

//--------------------------------------------------------------
void testApp::draw(){
    // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(20,20);
	grayImage.draw(360,20);
	grayBg.draw(20,280);
	grayDiff.draw(360,280);

	// then draw the contours:

	ofFill();
	ofSetHexColor(0x333333);
	ofRect(360,540,320,240);
	ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(360,540);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    //for (int i = 0; i < contourFinder.nBlobs; i++){
    if (contourFinder.nBlobs > 0) {
        contourFinder.blobs[0].draw(360,540);

		// draw over the centroid if the blob is a hole
		ofSetColor(255);
		if(contourFinder.blobs[0].hole){
			ofDrawBitmapString("hole",
                               contourFinder.blobs[0].boundingRect.getCenter().x + 360,
                               contourFinder.blobs[0].boundingRect.getCenter().y + 540);
		}
		//Retrieve data into a stringstream
		std::ostringstream ss;
        ss << contourFinder.blobs[0].boundingRect.getCenter().x;
        ss << "&";
        ss << contourFinder.blobs[0].boundingRect.getCenter().y;
        ss << ";";
		//Send data to the server
		string message = ss.str();
		udpConnection.Send(message.c_str(), message.length());
    }

	// finally, a report:
	ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << "bg subtraction and blob detection" << endl
    << "press ' ' to capture bg" << endl
    << "threshold " << threshold << " (press: +/-)" << endl
    << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 20, 600);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

void testApp::exit() {
	printf("Close UDP connection");
	udpConnection.Close();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
