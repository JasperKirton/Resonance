#include "ofApp.h"
#include "maximilian.h"/* include the lib */
#include "time.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    /* This is stuff you always need.*/
    
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    drumtrack.load(ofToDataPath("sais.wav")); //demo track by Floating Points, Eglo Records, 2011
    
    sampleRate 	= 44100; /* Sampling Rate */
    bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    
    
    fft.setup(1024, 512, 256);
    oct.setup(44100, 1024, 10);
    
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    /* Anything that you would normally find/put in maximilian's setup() method needs to go here. For example, Sample loading.
     
     */
    
    ofSetSphereResolution(5);
    
    
    ofBackground(0,0,0);
    
    isStrobe = false;
    
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4); /* this has to happen at the end of setup - it switches on the DAC */
    
    Particle::setup(50000, 100);
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //we have 256 bins so each bin represents roughly 86 hz
    //define kick and snare vars
    bool kBool, sBool, hBool;
    float kMag, sMag, hMag;
    std::tie(kBool, kMag) = isHit(fft.magnitudesDB, 4, 12, 0.1);
    std::tie(sBool, sMag) = isHit(fft.magnitudesDB, 20, 30, 0.1);
    std::tie(hBool, hMag) = isHit(fft.magnitudesDB, 60, 100, 0.1);
    //if kick = true
    if (kBool) {
        //cout<<kMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , (ofGetHeight() / 2) -15 ), kMag*50); //position kick down the bottom
    }
    
    //if snare = true
    if(sBool) {
        //cout<<sMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , (ofGetHeight() / 20) -50 ), sMag*55); //position snare in the centre (roughly)
    }
    
    //if high = true
    if(hBool) {
        //cout<<hMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , (ofGetHeight() / 20) -300 ), hMag*50); //position highs up top
    }
    
    
    Particle::updateAll(60 * ofGetLastFrameTime());
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    if (isStrobe) //turns on the strobe when enter/return is pressed
    {
        if ((int)(ofGetFrameNum()) % 2 == 1){
            ofBackground(255, 255, 255);
        }
        else{
            ofBackground(0, 0, 0);
        }
    }
    
    if (isStrobe2)
    {
        for(int i=0; i < oct.nAverages; i++) { //cycle through the (most of the) colours, with offsets for aesthetic refining. Credit to Mick Grierson for the tip.
        Particle::particleColour = ofColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
                       100 + ((int)(ofGetFrameNum() * 1.1) % 245), //avoid super bright greens
                                          300 + ((int)(ofGetFrameNum() * 0.5) % 255),
                       (oct.averages[i] / 20.0) * 255.0); //transparency for strobe (reactive to octave energy)
        }
    }
    else
    {
    
        for(int i=0; i < oct.nAverages; i++) {
            Particle::particleColour = ofColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
                                               100 + ((int)(ofGetFrameNum() * 1.1) % 245),
                                               300 + ((int)(ofGetFrameNum() * 0.5) % 255));
        }
    }
    
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    ofNoFill();
    glPushMatrix();
    Particle::drawAll();
    glPopMatrix();
    
    /*
    glPointSize(1);
    glEnable(GL_POINT_SMOOTH);
     /*/

    
}

std::tuple<bool, float> ofApp::isHit(float * bins, int loRange, int hiRange, float threshold) { //return bool and average value of bins
    sum = 0;
    for (int i = loRange; i <= hiRange; i++){
        sum += bins[i];
        //cout<<sum<<endl;
        float average = sum/(hiRange - loRange);
        //cout<<average<<endl;
        if (average > threshold){
            return std::make_tuple(1, average);
    }
        else
            return std::make_tuple(0, 0);
    }
    
    
}

//--------------------------------------------------------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
    float sum = 0;
    
    for (int i = 0; i < bufferSize; i++){
        
        
           // this code is for demo playback (track loaded in through drumtrack)
        wave = drumtrack.play();
        if (fft.process(wave)) {
            
            fft.magsToDB();  // so all energy becomes relative to DB scale (more consistent for different tracks)
            oct.calculate(fft.magnitudesDB);
            
            
        }

        
        
        //play result
        mymix.stereo(wave, outputs, 0.5);
        //output[i*nChannels    ] = outputs[0];
        //output[i*nChannels + 1] = outputs[1];
        
       
        output[i*nChannels    ] = wave;
        output[i*nChannels + 1] = wave;
        
        
        /* You may end up with lots of outputs. add them here */
        
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    
    /* uncomment this code for input through soundcard
    
    for(int i = 0; i < bufferSize; i++){
        displayBuffer[i] = input[i*nChannels];
        wave = input[i*nChannels];
        
        if (fft.process(wave)) {
             fft.magsToDB();  // so all energy becomes relative to DB scale (more consistent for different tracks)
             oct.calculate(fft.magnitudesDB);
        }
     
     
    }
    /*/
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //here is the code for various functions of the keyboard that can be used when the program is running.

    if(key == 'f')
        ofToggleFullscreen();
    
    if (key == 'r')
        ofBackground(204, 0, 0);
    
    if (key == 'b')
        ofBackground(0, 0, 200);
    
    if (key == 'g')
        ofBackground(0, 51, 0);
    
    if (key == 'q')
        ofBackground(107, 0, 0);
    if (key == 'w')
        ofBackground(7, 61, 0);
    if (key == 'e')
        ofBackground(0 , 61, 61);
    if (key == 'r')
        ofBackground(0, 21, 60);
    if (key == 't')
        ofBackground(41, 0, 61);
    if (key == 'y')
        ofBackground(61, 0, 51);
    
    if (key == OF_KEY_DOWN)
        ofBackground(0, 0, 0);
    
    if(key == OF_KEY_LEFT)
        ofBackground(200, 0, 20);
    
    if(key == OF_KEY_UP)
        ofBackground(255, 255, 255);
    
    if(key == OF_KEY_RIGHT)
        ofBackground(0, 0, 255);
    
    if(key == OF_KEY_RETURN){
        isStrobe = !isStrobe; //toggle the strobe
        ofBackground(0,0,0); //switch to black when off
     }
    
    if(key == OF_KEY_SHIFT){
        isStrobe2 = !isStrobe2; //toggle the second strobe (transparency)
        ofBackground(0,0,0); //switch to black when off
    }
    
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
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
