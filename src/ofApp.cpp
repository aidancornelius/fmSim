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

    // Enable multitouch
    ofxMultitouch::EnableTouch();
}

//--------------------------------------------------------------
void ofApp::exit() {
    ofxMultitouch::DisableTouch();
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update fluid simulation
    fluidSolver.update();

    float currentTime = ofGetElapsedTimef();

    // Update touch points and add forces/colors to fluid
    for(auto& pair : touchPoints) {
        TouchPoint& tp = pair.second;
        if(tp.isActive) {
            // Calculate touch duration
            float touchDuration = currentTime - tp.startTime;

            // Calculate velocity and scale it based on touch duration
            ofVec2f vel = (tp.pos - tp.prevPos) * velocityMult;

            // Create pulsing effect based on time
            float pulseIntensity = 0.5f + 0.5f * sin(touchDuration * 5.0f);
            ofFloatColor pulsingColor = tp.color;
            pulsingColor.setBrightness(tp.color.getBrightness() * pulseIntensity);

            // Add to fluid with time-based effects
            addToFluid(tp.pos, vel, pulsingColor, true);

            tp.prevPos = tp.pos;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);

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
void ofApp::touchDown(int x, int y, int id) {
    handleTouch(x, y, id, true);
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id) {
    handleTouch(x, y, id, false);
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id) {
    touchPoints[id].isActive = false;
}

//--------------------------------------------------------------
void ofApp::handleTouch(int x, int y, int id, bool isNew) {
    if(isNew) {
        TouchPoint tp;
        tp.pos = tp.prevPos = ofVec2f(x, y);
        tp.isActive = true;
        tp.color = ofFloatColor::fromHsb(ofRandom(1.0f), 1.0f, 1.0f, 1.0f);
        tp.startTime = ofGetElapsedTimef();
        touchPoints[id] = tp;

        addToFluid(tp.pos, ofVec2f(0, 0), tp.color, true);
    } else if(touchPoints.find(id) != touchPoints.end()) {
        TouchPoint& tp = touchPoints[id];
        tp.prevPos = tp.pos;
        tp.pos = ofVec2f(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    handleTouch(x, y, button, false);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    handleTouch(x, y, button, true);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    touchUp(x, y, button);
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
