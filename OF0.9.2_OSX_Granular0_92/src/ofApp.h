#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxMaxim.h"
#include "maxiGrains.h"
#include "ofxOsc.h"
#include <sys/time.h>

#include "Particle.h"


class ofApp : public ofBaseApp{
    
public:
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
    
    bool isStrobe;
    bool isStrobe2;
    
    
    void audioOut(float * output, int bufferSize, int nChannels);
    void audioIn(float * input, int bufferSize, int nChannels);
    
    std::tuple<bool, float> isHit(float * bins, int loRange, int hiRange, float threshold);
    
    
    float displayBuffer[512];
    
    int		bufferSize;
    
    
    int		initialBufferSize; /* buffer size */
    int		sampleRate;
    
    
    /* stick your maximilian stuff below */
    
    double wave,sample,outputs[2];
    ofxMaxiSample drumtrack;
    maxiMix mymix;
    
    
    ofxMaxiFFT fft;
    ofxMaxiFFTOctaveAnalyzer oct;
    int current;
    double pos;
    
    
    float sum;
    
};


