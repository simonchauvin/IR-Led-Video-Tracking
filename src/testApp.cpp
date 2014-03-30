#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	//Firefly MV
	mPgrCam.setup();

	// Webcam
	//vidGrabber.setVerbose(true);
    //vidGrabber.initGrabber(320,240);

	// Webcam
    //colorImg.allocate(320, 240);
	// Firefly MV
    grayImage.allocate(752,480);
	grayBg.allocate(752,480);
	grayDiff.allocate(752,480);

	bLearnBakground = true;
	threshold = 80;
	
	// Open UDP connection to the localhost
	udpConnection.Create();
	udpConnection.Connect(HOST, PORT);
    udpConnection.SetNonBlocking(true);
	printf("Open UDP connection to the localhost\n");
}

//--------------------------------------------------------------
void testApp::update(){
	// Background
	ofBackground(100,100,100);

	// Firefly MV
	mPgrCam.update();
	// Webcam
	//vidGrabber.update();

	// TODO prendre une image de référence (bLearnBackground = true) de temps en temps pour ne pas prendre en compte le bruit (rayon, objets, etc.)

	Image rawImage;

	Camera& camera = *mPgrCam.mCamera;
	camera.RetrieveBuffer(&rawImage);

	// Firefly MV
	grayImage.clear();
	grayImage.allocate(752,480);
	memcpy(grayImage.getPixels(), rawImage.GetData(), 752 * 480);

	// Webcam
	//colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
	//grayImage = colorImg;
    //grayImage = imageTest;
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

//--------------------------------------------------------------
void testApp::draw(){
	//mPgrCam.draw();

    // draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	//colorImg.draw(340,0);
	grayImage.draw(20, 20);
	grayBg.draw(792, 20);
	grayDiff.draw(20, 520);

	// then draw the contours:
	ofFill();
	ofSetHexColor(0x333333);
	ofRect(792, 520, 752, 480);
	ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(792, 520);

	// TODO ne chercher que les blobs de la taille (plus ou moins) de la led en situation normale

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    //for (int i = 0; i < contourFinder.nBlobs; i++){
    if (contourFinder.nBlobs > 0) {
        contourFinder.blobs[0].draw(792, 520);

		// draw over the centroid if the blob is a hole
		ofSetColor(255);
		if(contourFinder.blobs[0].hole){
			ofDrawBitmapString("hole",
                               contourFinder.blobs[0].boundingRect.getCenter().x + 792,
                               contourFinder.blobs[0].boundingRect.getCenter().y + 520);
		}
		//Retrieve data into a stringstream
		std::ostringstream ss;
        ss << contourFinder.blobs[0].boundingRect.getCenter().x;
        ss << "&";
        ss << contourFinder.blobs[0].boundingRect.getCenter().y;
        
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
		case 'z':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case 's':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

void testApp::exit() {
	printf("Close UDP connections");
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
