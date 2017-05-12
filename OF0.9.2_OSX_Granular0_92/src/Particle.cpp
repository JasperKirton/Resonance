#include "Particle.h"

/* code modified from original by github.com/kylemcdonald/openFrameworksDemos 
 and help from github.com/shiffman/The-Nature-of-Code-Examples */

vector<Force>
Particle::forces;

vector<Particle>
Particle::particles;

ofColor
Particle::particleColour;

ofVec2f
Particle::centeringForce,
Particle::globalOffset,
Particle::externalForce;

float
Particle::speed,
Particle::spread,
Particle::viscosity,
Particle::independence,
Particle::neighborhood,
Particle::confusion,
Particle::indecision,
Particle::age,
Particle::globalDirection;

void Particle::setup(int n, float radius) {
    globalOffset.set(1. / 3, 2. / 3);
    speed = 15;
    spread = 20;
    viscosity = .21;
    independence = 1;
    neighborhood = 200;
    confusion = .1;
    indecision = 10.0;
    age = 0;
    globalDirection = speed;
    
    for(int i = 0; i < n; i++) {
        Particle::particles.push_back(Particle(radius));
    }
}

void Particle::drawAll() {
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    for(int i = 0; i < particles.size(); i++) {
        mesh.addVertex(particles[i].position);
        mesh.addColor(particles[i].particleColour);
    }
    mesh.drawVertices();
}

void Particle::updateAll(float dt = 1. / 60.) {
    for(int i = 0; i < particles.size(); i++) {
        particles[i].update(dt);
    }
    globalDirection += ofSignedNoise(indecision * age);
    ofVec2f direction(0, 1);
    direction.rotate(globalDirection);
    globalOffset += confusion * direction * dt;
    age += dt;
    forces.clear();
    
    
}

void Particle::addForce(ofVec2f position, float magnitude) {
    forces.push_back(Force(position, magnitude));
}
