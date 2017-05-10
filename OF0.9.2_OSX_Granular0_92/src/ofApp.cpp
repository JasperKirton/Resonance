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

    
    /* This is a nice safe piece of code */
    //memset(lAudioOut, 0, initialBufferSize * sizeof(float));
    //memset(rAudioOut, 0, initialBufferSize * sizeof(float));
    
    memset(lAudioIn, 0, initialBufferSize * sizeof(float));
    memset(rAudioIn, 0, initialBufferSize * sizeof(float));

    
    fft.setup(1024, 512, 256);
    mfft.setup(1024, 512, 256);
    oct.setup(44100, 1024, 10);
    
    

    ofBackground(0,0,0);
    
    
    Particle::setup(50000, 100);
    
    int current = 0;
    
    
    
    /* Anything that you would normally find/put in maximilian's setup() method needs to go here. For example, Sample loading.
     
     */
    
    //ofSetSphereResolution(5);
    
    
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();

    ofxMaxiSettings::setup(sampleRate, 2, initialBufferSize);
    ofSoundStreamSetup(2,2,this, sampleRate, bufferSize, 4); /* this has to happen at the end of setup - it switches on the DAC */
    
    if (isTraining) { //train to listen for snares
        train();
    }
    isTraining = false;
    
    
}

//--------------------------------------------------------------
void ofApp::train(){
    /* stuff to analyse sounds and train model. plays sound, waits for analysis, sends osc to wek, plays next sound.*/
    
    ofDirectory dir(sPath); //set directory to snares folder
    dir.listDir();
    for (soundCount = 0; soundCount < dir.size(); soundCount++) { // for each sound in folder get centroid and ZCR
        //populate the directory object
        ofLogNotice(dir.getPath(soundCount));
        if ( (soundCount = 0) || (currentSound.isEnded()) )
            currentSound.load(dir.getPath(soundCount)); //load the file at iterator pointer
            ofSoundSetVolume(0.1);
        
        
        ofxOscMessage m;
        m.setAddress("/wekinator/inputs"); //set address to send data as input
        m.addFloatArg(centroid);
        sender.sendMessage(m);
        cout << "messageSent" << "\n";
    }
    
}


//--------------------------------------------------------------
void ofApp::update(){
    
    
    
    //WEKINATOR - listen to the stream for percussive sounds, if it sounds like snare, match freq boundaries to nearest neighbour
    if (!isTraining) {
        while(receiver.hasWaitingMessages()){
            // get the next message
            ofxOscMessage m;
            receiver.getNextMessage(&m);
            
            if(m.getAddress() == "/wek/outputs"){
                
               // cout << m.getArgAsFloat(0);
            }
        }
        
    }
    
    
    //we have 256 bins so each bin represents roughly 86 hz
    //define kick and snare vars
    //snare boundaries will change
    bool kBool, sBool;
    float kMag, sMag;
    std::tie(kBool, kMag) = isHit(fft.magnitudes, 5, 8, 0.1);
    std::tie(sBool, sMag) = isHit(fft.magnitudes, 30, 40, 0.1);
    //if kick = true
    if (kBool) {
        //cout<<kMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 2 , ofGetHeight() / 2), kMag*100);
    }
    
    //if snare = true
    if(sBool) {
        //cout<<sMag<<endl;
        Particle::addForce(ofVec2f(ofGetWidth() / 2 , ofGetHeight() / 2), sMag*200);
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

    
    
    ofSetColor(255, 0, 0,255);
    
    
    
    
}

//--------------------------------------------------------------

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
    
    if (!isTraining){ //if not training then perform analysis on music output
        for (int i = 0; i < bufferSize; i++){
            
            
                wave = drumtrack.play();
                /*
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
                 
                }
                /*/
                //lAudioOut[i] = 0;
                //rAudioOut[i] = 0;
            
                if (fft.process(wave)) {
                    oct.calculate(fft.magnitudes);
                }
                
                //play result
                mymix.stereo(wave, outputs, 0.5);
                //output[i*nChannels    ] = outputs[0];
                //output[i*nChannels + 1] = outputs[1];
                
                
                output[i*nChannels    ] = wave;
                output[i*nChannels + 1] = wave;
            }
    }
        else {
            for (int i = 0; i < bufferSize; i++){
                wave = currentSound.playOnce();
                if (mfft.process(wave)) {
                    
                    mfft.magsToDB();
                    oct.calculate(mfft.magnitudesDB);
                    
                    float sum = 0;
                    float maxFreq = 0;
                    int maxBin = 0;
                    
                    if (!currentSound.isEnded()) { //if currents sound is still playing
                        for (int i = 0; i < fftSize/2; i++) { //analyse freqs under nyquist
                            sum += mfft.magnitudes[i];
                            if (mfft.magnitudes[i] > maxFreq) {
                                maxFreq=mfft.magnitudes[i];
                                maxBin = i;
                            }
                        }
                    }
                    else { // if currentsound has finished, before moving on to next sound, calculate centroid
                        centroid = sum / (fftSize / 2);
                        cout << "centr is " << centroid << endl;
                        
                    }
            
            
        }
        
    
        
        
        
        /* You may end up with lots of outputs. add them here */
        
        
    }
    
}
    
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    /* You can just grab this input and stick it in a double, then use it above to create output*/
    
    
    //float sum = 0;
    //for (int i = 0; i < bufferSize; i++){
        
        /* you can also grab the data out of the arrays*/
        
        //lAudioIn[i] = input[i*2];
       // rAudioIn[i] = input[i*2+1];
    
        
   // }

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
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
