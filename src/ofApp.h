#pragma once

#include "ofMain.h"
#include "MSAFluid.h"
#include "MSACore.h"
#include "ofxGui.h"
#include "ofxMultitouch.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    // Multi-touch events
    void touchDown(int x, int y, int id);
    void touchMoved(int x, int y, int id);
    void touchUp(int x, int y, int id);

    // Mouse events as fallback
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

    // GUI events
    void keyPressed(int key);

private:
    msa::fluid::Solver   fluidSolver;
    msa::fluid::DrawerGl fluidDrawer;

    // Basic fluid simulation parameters
    static const int FLUID_WIDTH = 150;
    float particleSize = 10.0f;

    // Touch/mouse tracking
    struct TouchPoint {
        ofVec2f pos;
        ofVec2f prevPos;
        bool isActive;
        ofFloatColor color;  // Persistent color per touch
        float startTime;     // For time-based effects
    };
    std::map<int, TouchPoint> touchPoints;

    // Helper functions
    void addToFluid(ofVec2f pos, ofVec2f vel, const ofFloatColor& color, bool addForce);
    ofVec2f windowToFluid(ofVec2f windowPos);
    void handleTouch(int x, int y, int id, bool isNew);

    // GUI
    ofxPanel gui;
    ofParameter<float> velocityMult{"Velocity Multiplier", 10.0f, 0.0f, 50.0f};
    ofParameter<float> fluidViscosity{"Viscosity", 0.00015f, 0.0f, 0.01f};
    ofParameter<float> fluidFadeSpeed{"Fade Speed", 0.002f, 0.0f, 0.05f};
    ofParameter<float> fluidDeltaT{"Delta T", 0.5f, 0.1f, 2.0f};
    ofParameter<float> colorDiffusion{"Color Diffusion", 0.0f, 0.0f, 0.1f};
    ofParameter<float> forceMult{"Force Multiplier", 1.0f, 0.0f, 10.0f};
    ofParameter<bool> showGui{"Show GUI", true};
    ofParameter<int> drawMode{"Draw Mode", 0, 0, 3};

    void onViscosityChanged(float& value) { fluidSolver.setVisc(value); }
    void onFadeSpeedChanged(float& value) { fluidSolver.setFadeSpeed(value); }
    void onDeltaTChanged(float& value) { fluidSolver.setDeltaT(value); }
    void onColorDiffusionChanged(float& value) { fluidSolver.setColorDiffusion(value); }
};
