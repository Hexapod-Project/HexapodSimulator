#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Leg3D.h"
#include "Body.h"
#include "cinder/CinderImGui.h"

using namespace ci;

class Hexapod
{
public:
    void drawGUI();
    void setup();
    void update();
    void draw();
    void drawCoord();

    enum LEG {FRONTLEFT, FRONTRIGHT, MIDLEFT, MIDRIGHT, BACKLEFT, BACKRIGHT};

private:
    Body mBody;

    std::vector<Leg3D*> mLegs;

    float mBodyRoll;
    float mBodyYaw;
    float mBodyPitch;
};