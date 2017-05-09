 /* This is modified from an example of how to integrate maximilain into openFrameworks,
 including using audio received for input and audio requested for output.
 
 
 */

#include "ofApp.h"
#include "maximilian.h"/* include the lib */
#include "time.h"

//-------------------------------------------------------------
ofApp::~ofApp() {
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    /* This is stuff you always need.*/
    
    sender.setup(HOST, PORT);
    
    
    ofEnableAlphaBlending();
    ofSetupScreen();
    ofBackground(0, 0, 0);
    ofSetFrameRate(60);
    
    drumtrack.load(ofToDataPath("sais.wav"));
    
    sampleRate 	= 44100; /* Sampling Rate */
    bufferSize	= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    lAudioOut			= new float[initialBufferSize];/* outputs */
    rAudioOut			= new float[initialBufferSize];
    lAudioIn			= new float[initialBufferSize];/* inputs */
    rAudioIn			= new float[initialBufferSize];
    
    
    /* This is a nice safe piece of code */
    memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));
    
    
    fft.setup(1024, 512, 256);
    mfft.setup(1024, 512, 256);
    ifft.setup(1024, 512, 256);
    oct.setup(44100, 1024, 10);
    
    ofBackground(0,0,0);
    
    
    Particle::setup(50000, 100);
    
    int current = 0;
    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4); /* this has to happen at the end of setup - it switches on the DAC */
    isTraining=true;
    
    
    bHide = true;
    
    myfont.loadFont("arial.ttf", 18); //requires this to be in bin/data/
    myFont2.loadFont("arial.ttf", 12); //requires this to be in bin/data/
    
    /* Anything that you would normally find/put in maximilian's setup() method needs to go here. For example, Sample loading.
     
     */
    
    //ofSetSphereResolution(5);
    
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    
    //WEKINATOR - use machine learning to learn from existing snares
    if (!isTraining) {
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            // check for mouse moved message
            if(m.getAddress() == "/wek/outputs"){
                
                cout << m.getArgAsFloat(0);
                speed = ((double ) m.getArgAsFloat(0) * 4.0) - 2.0;
                grainLength = (m.getArgAsFloat(1) * 0.1) + 0.001;
                pos = ((double) m.getArgAsFloat(0) * 2.0);
                
            }
        }
        
    }
    
    
    //we have 256 bins so each bin represents roughly 86 hz
    //define kick and snare vars
    bool kBool, sBool;
    float kMag, sMag;
    std::tie(kBool, kMag) = isHit(fft.magnitudes, 5, 8, 0.1);
    std::tie(sBool, sMag) = isHit(fft.magnitudes, 30, 40, 0.1);
    //if kick = true
    if (kBool) {
        cout<<kMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 2 , ofGetHeight() / 2), kMag*100);
    }
    
    //if snare = true
    if(sBool) {
        cout<<sMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 2 , ofGetHeight() / 2), sMag*200);
    }
    
    
    Particle::updateAll(60 * ofGetLastFrameTime());
    
    //wek
    ofxOscMessage m;
    m.setAddress("/wek/inputs");
    
    sender.sendMessage(m);
    
    
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
    
    /*
    for(int i=0; i < oct.nAverages; i++) {
    Particle::particleColour = ofColor(200 + ((int)(ofGetFrameNum() * 0.8) % 255),
                   100 + ((int)(ofGetFrameNum() * 1.4) % 255),
                                       oct.averages[i] / 20.0 * 255.0);
    }
    /*/
    
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    ofNoFill();
    glPushMatrix();
    Particle::drawAll();
    glPopMatrix();
    
    
    glPointSize(1);
    glEnable(GL_POINT_SMOOTH);

    
    //ofSetColor(255);
    // ofRect(100, 100, 100, 100);
    //ofSetColor(255);
    //ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
    
    float horizWidth = 500.;
    float horizOffset = 100;
    float fftTop = 250;
    float mfccTop = 350;
    float chromagramTop = 450;
    
    ofSetColor(255, 0, 0,255);
    
    //FFT magnitudes:
    float xinc = horizWidth / fftSize * 2.0;
    for(int i=0; i < fftSize / 2; i++) {
        float height = mfft.magnitudes[i] * 100;
        ofRect(horizOffset + (i*xinc),250 - height,2, height);
    }
    myfont.drawString("FFT:", 30, 250);
    
    
    //MFCCs:
    ofSetColor(0, 255, 0,200);
    xinc = horizWidth / 13;
    for(int i=0; i < 13; i++) {
        float height = mfccs[i] * 100.0;
        ofRect(horizOffset + (i*xinc),mfccTop - height,40, height);
        //		cout << mfccs[i] << ",";
    }
    myfont.drawString("MFCCs:", 12, mfccTop);
    
    
    //Const-Q:
    ofSetColor(255, 0, 255,200);
    xinc = horizWidth / oct.nAverages;
    for(int i=0; i < oct.nAverages; i++) {
        float height = oct.averages[i] / 20.0 * 100;
        ofRect(horizOffset + (i*xinc),chromagramTop - height,2, height);
    }
    myfont.drawString("ConstQ:", 12, chromagramTop);
    
    
    ofSetColor(255, 255, 255,255);
    
    char peakString[255]; // an array of chars
    sprintf(peakString, "Peak Frequency: %.2f", peakFreq);
    myfont.drawString(peakString, 12, chromagramTop + 50);
    
    char centroidString[255]; // an array of chars
    sprintf(centroidString, "Spectral Centroid: %f", centroid);
    myfont.drawString(centroidString, 12, chromagramTop + 80);
    
    char rmsString[255]; // an array of chars
    sprintf(rmsString, "RMS: %.2f", RMS);
    myfont.drawString(rmsString, 12, chromagramTop + 110);
    
    
    char numInputsString[255]; // an array of chars
    sprintf(numInputsString, "Sending %d inputs total", numInputs);
    myfont.drawString(numInputsString, 450, 100);
    
    
    

    
    
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
    
    
    for (int i = 0; i < bufferSize; i++){
        
        wave = drumtrack.play();
        if (mfft.process(wave)) {
            
            mfft.magsToDB();
            oct.calculate(mfft.magnitudesDB);
            
            float sum = 0;
            float maxFreq = 0;
            int maxBin = 0;
            
            for (int i = 0; i < fftSize/2; i++) {
                sum += mfft.magnitudes[i];
                if (mfft.magnitudes[i] > maxFreq) {
                    maxFreq=mfft.magnitudes[i];
                    maxBin = i;
                }
            }
            centroid = sum / (fftSize / 2);
            peakFreq = (float)maxBin/fftSize * 44100;
            
            
            mfcc.mfcc(mfft.magnitudes, mfccs);
        }
        
        lAudioOut[i] = 0;
        rAudioOut[i] = 0;
    
        if (fft.process(wave)) {
            oct.calculate(fft.magnitudes);
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
    
    /* You can just grab this input and stick it in a double, then use it above to create output*/
    
    float sum = 0;
    for (int i = 0; i < bufferSize; i++){
        
        /* you can also grab the data out of the arrays*/
        
        lAudioIn[i] = input[i*2];
        rAudioIn[i] = input[i*2+1];
        
        sum += input[i*2] * input[i*2];
        
    }
    RMS = sqrt(sum);
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    isTraining=!isTraining;
    
    //cout << isTraining;
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
  
    
    float x1 =  myFilter.lopass(x,0.25);
    float y1 = myFilter2.lopass(y,0.25);
    
    if (isTraining) {
        
        speed = ((double ) x1 / ofGetWidth() * 4.0) - 2.0;
        grainLength = ((double) y1 / ofGetHeight() * 0.1) + 0.001;
        if (grainLength < 0.01 ) grainLength = 0.01;
        if (grainLength > 0.4 ) grainLength = 0.4;
        pos = ((double) x1 / ofGetWidth() * 2.0);
        //	cout << pos << endl;
        
        ofxOscMessage m;
        m.setAddress("/wekinator/control/outputs");
        m.addFloatArg((float)x/ofGetWidth());
        m.addFloatArg((float)y/ofGetHeight());
        sender.sendMessage(m);
        //cout << "messageSent" << "\n";
    }
    
    
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
