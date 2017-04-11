#include "ofApp.h"

void ofApp::setup() {
    //    ofSetBackgroundAuto(false);
    //    ofSetVerticalSync(true);
    Particle::setup(50000, 1000);
    ofBackground(0);
    // if (mousePressed(ofBackground(10,100,100);
}

void ofApp::update() {
    if(ofGetMousePressed()) {
        Particle::addForce(ofVec2f(mouseX - ofGetWidth() / 2, mouseY - ofGetHeight() / 2), 100);
    }
    Particle::updateAll(60 * ofGetLastFrameTime());
}

void ofApp::draw() {
    ofPushMatrix();
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
    ofSetColor(255);
    //glPointSize(2);
    //glEnable(GL_POINT_SMOOTH);
    Particle::drawAll();
    ofPopMatrix();
    
    ofSetColor(255);
    // ofRect(100, 100, 100, 100);
    ofSetColor(255);
    //ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void ofApp::keyPressed(int key) {
    if(key == 'f')
        ofToggleFullscreen();
    
    if (key == 'r')
        ofBackground(204, 0, 0);
    
    if (key == 'b')
        ofBackground(0, 0, 200);
    
    if (key == 'g')
        ofBackground(0, 51, 0);
    
    if (key == OF_KEY_DOWN)
        ofBackground(0, 0, 0);
}

