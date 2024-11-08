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
    gui.add(inputSize);
    gui.add(showVectors);
    gui.add(showMotion);
    gui.add(showColor);
    gui.add(showSpeed);
    gui.add(drawOpacity);
    gui.add(disableSystemGestures);
    gui.add(showGui);

    // Setup parameter listeners
    fluidViscosity.addListener(this, &ofApp::onViscosityChanged);
    fluidFadeSpeed.addListener(this, &ofApp::onFadeSpeedChanged);
    fluidDeltaT.addListener(this, &ofApp::onDeltaTChanged);
    colorDiffusion.addListener(this, &ofApp::onColorDiffusionChanged);
    disableSystemGestures.addListener(this, &ofApp::onGesturesChanged);

    // Enable multitouch
    ofxMultitouch::EnableTouch();

    // Initial gesture control
    bool initialValue = disableSystemGestures.get();
    onGesturesChanged(initialValue);
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

    // Draw combined fluid modes
    drawCombinedModes();

    // Draw touch points
    ofSetColor(255);
    for(const auto& tp : touchPoints) {
        if(tp.second.isActive) {
            ofSetColor(tp.second.color);
            ofDrawCircle(tp.second.pos.x, tp.second.pos.y, particleSize * inputSize);
        }
    }

    // Draw GUI
    if(showGui) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::drawCombinedModes() {
    ofEnableAlphaBlending();

    if(showColor) {
        ofSetColor(255, 255 * drawOpacity);
        fluidDrawer.setDrawMode(msa::fluid::kDrawColor);
        fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    if(showMotion) {
        ofSetColor(255, 255 * drawOpacity);
        fluidDrawer.setDrawMode(msa::fluid::kDrawMotion);
        fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    if(showSpeed) {
        ofSetColor(255, 255 * drawOpacity);
        fluidDrawer.setDrawMode(msa::fluid::kDrawSpeed);
        fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    if(showVectors) {
        ofSetColor(255, 255 * drawOpacity);
        fluidDrawer.setDrawMode(msa::fluid::kDrawVectors);
        fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
    }

    ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::exit() {
    // Re-enable system gestures
    bool enable = false;
    onGesturesChanged(enable);
    ofxMultitouch::DisableTouch();
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
    switch(key) {
        case 'g':
        case 'G':
            showGui = !showGui;
            break;
        case 'v':
        case 'V':
            showVectors = !showVectors;
            break;
        case 'm':
        case 'M':
            showMotion = !showMotion;
            break;
        case 'c':
        case 'C':
            showColor = !showColor;
            break;
        case 's':
        case 'S':
            showSpeed = !showSpeed;
            break;
        case '[':
            inputSize = max(inputSize.get() - 0.1f, 0.1f);
            break;
        case ']':
            inputSize = min(inputSize.get() + 0.1f, 5.0f);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::addToFluid(ofVec2f pos, ofVec2f vel, const ofFloatColor& color, bool addForce) {
    // Convert window coordinates to fluid coordinates
    ofVec2f fluidPos = windowToFluid(pos);

    // Scale the input size
    float scaledSize = inputSize * 0.5f; // Base multiplier for force area

    // Add color with scaled size
    for(float x = -scaledSize; x <= scaledSize; x += 0.25) {
        for(float y = -scaledSize; y <= scaledSize; y += 0.25) {
            ofVec2f offset(x/FLUID_WIDTH, y/FLUID_WIDTH);
            ofVec2f pos = fluidPos + offset;

            // Ensure we're within bounds
            if(pos.x >= 0 && pos.x <= 1 && pos.y >= 0 && pos.y <= 1) {
                fluidSolver.addColorAtPos(msa::Vec2f(pos.x, pos.y), color);

                if(addForce) {
                    // Scale force based on distance from center
                    float dist = sqrt(x*x + y*y);
                    float forceFactor = ofMap(dist, 0, scaledSize, 1, 0, true);
                    ofVec2f scaledVel = vel * forceFactor;

                    fluidSolver.addForceAtPos(msa::Vec2f(pos.x, pos.y),
                                            msa::Vec2f(scaledVel.x * fluidSolver.getInvWidth() * forceMult,
                                                      scaledVel.y * fluidSolver.getInvWidth() * forceMult));
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::onGesturesChanged(bool& value) {
    if(value) {
        system("gsettings set org.gnome.desktop.interface enable-gestures false");
        system("gsettings set org.gnome.shell.overrides edge-tiling false");
    } else {
        system("gsettings set org.gnome.desktop.interface enable-gestures true");
        system("gsettings set org.gnome.shell.overrides edge-tiling true");
    }
}

//--------------------------------------------------------------
ofVec2f ofApp::windowToFluid(ofVec2f windowPos) {
    return ofVec2f(windowPos.x / ofGetWidth(),
                  windowPos.y / ofGetHeight());
}
