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
    fluidSolver.enableRGB(true)
               .setFadeSpeed(fluidFadeSpeed)
               .setDeltaT(fluidDeltaT)
               .setVisc(fluidViscosity)
               .setColorDiffusion(colorDiffusion);

    fluidDrawer.setDrawMode(msa::fluid::kDrawColor);

    // Setup GUI
    gui.setup("Fluid Settings");
    gui.add(velocityMult);
    gui.add(fluidViscosity);
    gui.add(fluidFadeSpeed);
    gui.add(fluidDeltaT);
    gui.add(colorDiffusion);
    gui.add(forceMult);
    gui.add(drawMode);
    gui.add(showGui);

    // Setup parameter listeners
    fluidViscosity.addListener(this, &ofApp::onViscosityChanged);
    fluidFadeSpeed.addListener(this, &ofApp::onFadeSpeedChanged);
    fluidDeltaT.addListener(this, &ofApp::onDeltaTChanged);
    colorDiffusion.addListener(this, &ofApp::onColorDiffusionChanged);
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update fluid simulation
    fluidSolver.update();

    // Update touch points and add forces/colors to fluid
    for(auto& tp : touchPoints) {
        if(tp.second.isActive) {
            ofVec2f vel = (tp.second.pos - tp.second.prevPos) * velocityMult;
            addToFluid(tp.second.pos, vel, tp.second.color, true);
            tp.second.prevPos = tp.second.pos;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0); // Instead of ofClear

    // Draw fluid
    fluidDrawer.setDrawMode((msa::fluid::DrawMode)((int)drawMode));
    fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());

    // Draw touch points
    ofSetColor(255);
    for(const auto& tp : touchPoints) {
        if(tp.second.isActive) {
            ofSetColor(tp.second.color);
            ofDrawCircle(tp.second.pos.x, tp.second.pos.y, particleSize);
        }
    }

    // Draw GUI
    if(showGui) {
        gui.draw();
    }
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
    tp.color = ofFloatColor::fromHsb(ofRandom(1.0f), 1.0f, 1.0f, 1.0f);
    touchPoints[button] = tp;

    addToFluid(tp.pos, ofVec2f(0, 0), tp.color, true);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    touchPoints[button].isActive = false;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'g' || key == 'G') {
        showGui = !showGui;
    }
    if(key == 'd' || key == 'D') {
        drawMode = (drawMode + 1) % 4;
    }
}

//--------------------------------------------------------------
void ofApp::addToFluid(ofVec2f pos, ofVec2f vel, const ofFloatColor& color, bool addForce) {
    // Convert window coordinates to fluid coordinates
    ofVec2f fluidPos = windowToFluid(pos);

    // Add color
    fluidSolver.addColorAtPos(msa::Vec2f(fluidPos.x, fluidPos.y), color);

    if(addForce) {
        // Scale velocity to fluid space
        ofVec2f fluidVel = vel * fluidSolver.getInvWidth() * forceMult;
        fluidSolver.addForceAtPos(msa::Vec2f(fluidPos.x, fluidPos.y),
                                 msa::Vec2f(fluidVel.x, fluidVel.y));
    }
}

//--------------------------------------------------------------
ofVec2f ofApp::windowToFluid(ofVec2f windowPos) {
    return ofVec2f(windowPos.x / ofGetWidth(),
                  windowPos.y / ofGetHeight());
}
