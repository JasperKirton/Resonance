#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 10, 10, 10);
	
	camera.setPosition(ofVec3f(0, -10.f, -10.f));
	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));

	world.setup();
	world.enableGrabbing();
	world.enableDebugDraw();
	world.setCamera(&camera);
    
    
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                	sphere = new ofxBulletSphere();
                    sphere->create(world.world, ofVec3f(i*2, ((i*j)/40), j*2), 0.1, .25);
                    sphere->setProperties(1.0f, 2.0f);
                    sphere->add();
            }
        }

    // set quaternion, so we know which way is up //
    ofQuaternion groundQuat;
    groundQuat.set(0, 5, 0, 1.);
    ground.create( world.world, ofVec3f(0., 5.5, 0.), groundQuat, 0., 10.f, 1.f, 10.f );
    ground.setProperties(.25, .95);
    ground.add();
    // allows manipulation of object. The object will not react to other objects, but will apply forces to them //
    // just like a static object, except you can move it around //
    ground.enableKinematic();
    
    
    /*
    for (int k=0; k<3; k++){
        for (int i=0; i<(40-(k*6)); i++){
            for (int j=0; j<(40-(k*6)); j++){
                sphere *sphereShape = new btSphereShape(1);
                btMotionState *motionState = new btDefaultMotionState(btTransform(btQuaternion(0.0,0.0,0.0,1),btVector3((i*2)-39,k*4+((i*j)/40),(j*2)-39)));
                btScalar mass = 20;
                btVector3 fallInertia(1, 1, 1);
                sphereShape->calculateLocalInertia(mass,fallInertia);
                btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,motionState,sphereShape,fallInertia);
                btRigidBody *sphere = new btRigidBody(fallRigidBodyCI);
                sphere->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
                sphere->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
                sphere->setFriction(2.0f);
                sphere->setRestitution(1.0f);
                m_dynamicsWorld->addRigidBody(sphere);
                m_bodies.push_back(sphere);
            }
        }
    }
                /*/
	
    /*
	box = new ofxBulletBox();
	box->create(world.world, ofVec3f(7, 0, 0), .05, .5, .5, .5);
	box->add();
	
	cone = new ofxBulletCone();
	cone->create(world.world, ofVec3f(-1, -1, .2), .2, .4, 1.);
	cone->add();
	
	capsule = new ofxBulletCapsule();
	capsule->create(world.world, ofVec3f(1, -2, -.2), .4, .8, 1.2);
	capsule->add();
	
	cylinder = new ofxBulletCylinder();
	cylinder->create(world.world, ofVec3f(0, -2.4, 0), .8, .9, 1.8);
	cylinder->add();
	
	ground.create( world.world, ofVec3f(0., 5., 0.), 0., 100.f, 1.f, 100.f );
	ground.setProperties(.5, .95);
	ground.add();
    /*/
    
}

//--------------------------------------------------------------
void ofApp::update() {
    
    
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    
    //ground.setPosition(sin(ofGetElapsedTimeMillis()*1), .95);
    //ground.applyCentralForce(0, -10, 0);
    
    float t = ofGetElapsedTimeMillis();

    
    // store the position of the ground //
    ofVec3f pos = ground.getPosition();
    
    // create a bullet transform object and set the position to that of the object //
    btTransform trans;
    trans.setOrigin( btVector3( btScalar(pos.x), btScalar(pos.y), btScalar(pos.z)) );
    
    // get the rotation quaternion from the ground //
    ofQuaternion rotQuat = ground.getRotationQuat();
    // print out the angle
    cout << "rotation " << rotQuat.w() << endl;
    float newRot = rotQuat.w();
    // rotate it a bit //
    newRot += t*0.001f;
    // clamp values between PI and -PI so there is no jumping //
    if(newRot >= PI) newRot -= PI;
    if(newRot <= 0) newRot += PI;
    
    // set the rotation of the bullet transform to that of the axis of the stored quaternion
    // and apply the new rotation
    trans.setRotation( btQuaternion(btVector3(rotQuat.x(), rotQuat.y(), rotQuat.z()), newRot) );
    // apply the transform to the rigid body //
    //sphere->getRigidBody()->setCenterOfMassTransform(trans);
    ground.getRigidBody()->getMotionState()->setWorldTransform( trans );
    // tell the ofxBulletWorldRigid that we have moved rigid body and it should update the collision object //
    ground.activate();
    
    //cout << sin(ofGetElapsedTimeMillis()) << endl;
    
    world.update();
    

    
}

//--------------------------------------------------------------
void ofApp::draw() {
	glEnable( GL_DEPTH_TEST );
	camera.begin();
	
	ofSetLineWidth(1.f);
	ofSetColor(255, 0, 200);
	world.drawDebug();
	
	ofSetColor(100, 100, 100);
	ground.draw();
	
	ofSetColor(225, 225, 225);
	sphere->draw();
	
    
	
	camera.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
	
}
