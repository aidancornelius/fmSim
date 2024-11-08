#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);

    // Setup fluid simulation
    fluidSolver.setup(FLUID_WIDTH, FLUID_WIDTH);
    fluidDrawer.setup(&fluidSolver);

    // Initialize fluid parameters
    fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5)
               .setVisc(0.00015).setColorDiffusion(0);
    fluidDrawer.setDrawMode(msa::fluid::kDrawColor);
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update fluid simulation
    fluidSolver.update();

    // Update touch points and add forces/colors to fluid
    for(auto& tp : touchPoints) {
        if(tp.second.isActive) {
            ofVec2f vel = (tp.second.pos - tp.second.prevPos) * 10.0; // Scale velocity
            addToFluid(tp.second.pos, vel, true, true);
            tp.second.prevPos = tp.second.pos;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofClear(0, 0, 0, 255);

    // Draw fluid
    fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());

    // Draw touch points
    ofSetColor(255);
    for(const auto& tp : touchPoints) {
        if(tp.second.isActive) {
            ofDrawCircle(tp.second.pos.x, tp.second.pos.y, particleSize);
        }
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    TouchPoint& tp = touchPoints[button];
    tp.pos = ofVec2f(x, y);
    if(!tp.isActive) {
        tp.prevPos = tp.pos;
        tp.isActive = true;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    TouchPoint tp;
    tp.pos = tp.prevPos = ofVec2f(x, y);
    tp.isActive = true;
    touchPoints[button] = tp;

    // Add initial splash to fluid
    addToFluid(tp.pos, ofVec2f(0, 0), true, true);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    touchPoints[button].isActive = false;
}

//--------------------------------------------------------------
void ofApp::addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce) {
    // Convert window coordinates to fluid coordinates
    ofVec2f fluidPos = windowToFluid(pos);

    // Calculate normalized coordinates
    float normalizedX = fluidPos.x;
    float normalizedY = fluidPos.y;

    if(addColor) {
        // Generate random color for each touch
        ofColor col = ofColor::fromHsb(ofRandom(255), 255, 255);
        ofFloatColor fluidColor(col.r/255.0f, col.g/255.0f, col.b/255.0f);

        fluidSolver.addColorAtPos(msa::Vec2f(normalizedX, normalizedY), fluidColor);
    }

    if(addForce) {
        // Scale velocity to fluid space
        ofVec2f fluidVel = vel * fluidSolver.getInvWidth();
        fluidSolver.addForceAtPos(msa::Vec2f(normalizedX, normalizedY),
                                 msa::Vec2f(fluidVel.x, fluidVel.y));
    }
}

//--------------------------------------------------------------
ofVec2f ofApp::windowToFluid(ofVec2f windowPos) {
    return ofVec2f(windowPos.x / ofGetWidth(),
                  windowPos.y / ofGetHeight());
}
