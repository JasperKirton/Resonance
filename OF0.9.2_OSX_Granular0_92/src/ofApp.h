#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include <sys/time.h>

#include "Particle.h"

#include "maxiMFCC.h"
#define HOST "localhost"
#define PORT 6448


class ofApp : public ofBaseApp{
    
public:
    ~ofApp();/* deconsructor is very useful */
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void audioOut(float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    
    std::tuple<bool, float> isHit(float * bins, int loRange, int hiRange, float threshold);
    
    float 	* lAudioOut; /* outputs */
    float   * rAudioOut;
    
    float   * lAudioIn; /* inputs */
    float   * rAudioIn;
    
    int numInputs = 2; //inputs for wek, take the MFCC and spectral centroid
    
    
    float displayBuffer[512];
    
    int		bufferSize;
    
    
    int		initialBufferSize; /* buffer size */
    int		sampleRate;
    
    
    /* stick your maximilian stuff below */
    
    int nAverages;
    float *ifftOutput;
    int ifftSize;
    
    float peakFreq = 0;
    float centroid = 0;
    float RMS = 0;
    
    ofxMaxiIFFT ifft;
    ofxMaxiFFT mfft;
    int fftSize;
    int bins, dataSize;
    
    maxiMFCC mfcc;
    double *mfccs;

    double wave,sample,outputs[2];
    maxiSample samp, samp2, samp3, samp4, samp5;
    ofxMaxiSample drumtrack;
    maxiMix mymix;
    double speed, grainLength;
    
    ofxMaxiFFT fft;
    ofxMaxiFFTOctaveAnalyzer oct;
    int current;
    double pos;
    
    //GUI STUFF
    bool bHide;
    
    
    ofTrueTypeFont myfont, myFont2;
    
    
    //osc
    ofxOscSender sender;
    ofxOscReceiver receiver;
    
    ofxMaxiFilter myFilter, myFilter2;
    
    bool isTraining;
    
    float sum = 0;
    
};


