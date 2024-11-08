#pragma once

#include "ofMain.h"
#include "MSAFluid.h"
#include "MSACore.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();

    // Mouse events as fallback/testing
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

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
    };
    std::map<int, TouchPoint> touchPoints;

    // Helper functions
    void addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce);
    ofVec2f windowToFluid(ofVec2f windowPos);
};
