#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "Leg3D.h"
#include "Body.h"
#include "Animator.h"
#include "GaitManager.h"
#include "Enums.h"

using namespace ci;

class GaitManager;

class Hexapod
{
public:
    void drawGUI();
    void setup();
    void createWalkAnimations();
    void resetFeetPos();
    void resetBodyPos();
    void update();
    void draw();
    void drawCoord();

private:
    Body mBody;

    std::vector<Leg3D *> mLegs;

    float mBodyRoll;
    float mBodyYaw;
    float mBodyPitch;

    float mFootPosX[6];
    float mFootPosY[6];
    float mFootPosZ[6];

    vec3 mFootStartPos[6];
    vec3 mFootTargetPos[6];

    float mWalkSpeed = 1.0f;
    float mStepHeight = 1.0f;
    float mStepDist = 1.0f;

    int mComboGaitType = GAITTYPE::TRIPOD;
    GAITTYPE mCurrGaitType = GAITTYPE::TRIPOD;
    GaitManager mGaitManager;
};