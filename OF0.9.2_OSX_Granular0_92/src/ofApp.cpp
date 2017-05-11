 /* This is modified from an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.
 
 
 */

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
    
    drumtrack.load(ofToDataPath("sais.wav"));
    
    sampleRate 	= 44100; /* Sampling Rate */
    bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    
    fftSize = 1024;
    
    fft.setup(1024, 512, 256);
    //mfft.setup(1024, 512, 256);
    oct.setup(44100, 1024, 10);
    
    int current = 0;
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    /* Anything that you would normally find/put in maximilian's setup() method needs to go here. For example, Sample loading.
     
     */
    
    ofSetSphereResolution(5);
    
    isTraining=true;
    
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
    std::tie(kBool, kMag) = isHit(fft.magnitudes, 4, 12, 0.1);
    std::tie(sBool, sMag) = isHit(fft.magnitudes, 20, 30, 0.1);
    std::tie(hBool, hMag) = isHit(fft.magnitudes, 100, 200, 0.1);
    
    //if kick = true
    if (kBool) {
        cout<<kMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , (ofGetHeight() / 2.0) -15 ), kMag*50);
    }
    
    //if snare = true
    if(sBool) {
        cout<<sMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , ofGetHeight() / 20), sMag*100);
    }
    
    //if high = true
    
    if(hBool) {
        cout<<hMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 20 , (ofGetHeight() / 2.0) + 30 ), hMag*150);
        
    }
    
    
    Particle::updateAll(60 * ofGetLastFrameTime());
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    /* You can use any of the data from audio received and audiorequested to draw stuff here.
     Importantly, most people just use the input and output arrays defined above.
     Clever people don't do this. This bit of code shows that by default, each signal is going to flip
     between -1 and 1. You need to account for this somehow. Get the absolute value for example.
     */
    
    /*
    ofSetColor(160,32,240, 150);
    
    ofNoFill();
    for(int i=0; i < oct.nAverages; i++) {
        ofSetColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
                   100 + ((int)(ofGetFrameNum() * 1.4) % 255),
                   ofGetFrameNum() % 255,
                   oct.averages[i] / 20.0 * 255.0);
        glPushMatrix();
        glTranslatef(ofGetWidth()/2,ofGetHeight()/2, 0);
        glRotatef(0.01 * ofGetFrameNum() * speed * i, 0.01 * ofGetFrameNum() * speed * i,0.01 * ofGetFrameNum() * speed * i, 0);
        ofDrawSphere(0, 0, i * 5);
        glPopMatrix();
    }
    /*/
    
    /*
    for (int i = 0; i < oct.nAverages; i++){
        //ofSetColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
         //          100 + ((int)(ofGetFrameNum() * 1.4) % 255),
           //       ofGetFrameNum() % 255,
            //      oct.averages[i] / 20.0 * 255.0);
        ofDrawRectangle(i*((float)ofGetWidth()/oct.nAverages), ofGetHeight(), ((float)ofGetWidth()/oct.nAverages), -oct.averages[i]*100);
        //ofLog()<< ofToString(i) + "  " + ofToString(oct.averages[i]);
        
    }
     /*/
    
   // ofLog()<< ofToString(isHit(oct.averages, 0, 10, 0.15));
    
    //particle code
    //ofSetColor(255);
    //ofSetColor(160,32,240, 150);
    
    if (isStrobe)
    {
        if ((int)(ofGetFrameNum()) % 2 == 1){
            ofBackground(255, 255, 255);
        }
        else{
            ofBackground(0, 0, 0);
        }
    }
    
    for(int i=0; i < oct.nAverages; i++) {
    Particle::particleColour = ofColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
                   100 + ((int)(ofGetFrameNum() * 1.4) % 255),
                                      300 + ((int)(ofGetFrameNum() * 0.5) % 255));
                   //oct.averages[i] / 20.0 * 255.0);
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
    
    //ofSetColor(255);
    // ofRect(100, 100, 100, 100);
    //ofSetColor(255);
    //ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
    
    
    
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
        
        /* Stick your maximilian 'play()' code in here ! Declare your objects in testApp.h.
         
         For information on how maximilian works, take a look at the example code at
         
         http://www.maximilian.strangeloop.co.uk
         
         under 'Tutorials'.
         
         */
        
        
    
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
        
        /*
        double filteredSound[2];
        filteredSound[1] = myFilter.lopass(wave, 0.1);
        sum += filteredSound[i*2] * output[i*2];
        /*/
        
        
        /* You may end up with lots of outputs. add them here */
        
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    
    // samples are "interleaved"
    /*
    for(int i = 0; i < bufferSize; i++){
        displayBuffer[i] = input[i*nChannels];
        //wave = input[i*nChannels];
        
        if (fft.process(wave)) {
            oct.calculate(fft.magnitudes);
        }

        
    }
     /*/
    
}

//--------------------------------------------------------------
/*
int ofApp::getRMS(float * input, int bufferSize, int nChannels){
    
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        filteredSound = filter.lopass(input, 0.1)
        sum += input[i*2] * input[i*2];
        
    }
    
    
    return sqrt(sum);
    
    //case
}
/*/

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == 'f')
        ofToggleFullscreen();
    
    // if (key == 'r')
    //      ofBackground(204, 0, 0);
    
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
        isStrobe = !isStrobe;
        ofBackground(0,0,0);
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
