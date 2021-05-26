#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "Leg3D.h"
#include "Body.h"
#include "Animator.h"
#include "GaitManager.h"
#include "Enums.h"
#include "HexapodConstants.h"

using namespace ci;
using namespace HexapodConstants;

class GaitManager;

class Hexapod
{
public:
    void drawGUI();
    void setup();
    void update();
    void draw();
    void setWalkProperties(float walkSpeed, float stepHeight, float stepDist);
    void setNextStep(int footIdx, int startTime, bool isStop = false);
    void changeDir(double dir, float grpSize, int startTime);
    vec3 getPos();
    void drawCoord();

    int mStepDuration;

private:
    Body mBody;

    std::vector<Leg3D *> mLegs;

    bool mCrabMode = true;

    float mStartDir = FORWARD;
    float mCurrDir = FORWARD;
    float mChangeStartDir;
    float mChangeOffsetDir;
    float mChangDirStartTime;

    vec3 mStepStartPos[LEG_COUNT];
    float mStepFootAngle[LEG_COUNT];
    vec3 mStepOffsetPos[LEG_COUNT];
    int mStepStartTimes[LEG_COUNT];

    int mComboGaitType = GAITTYPE::TRIPOD;

    GaitManager *mGaitManager;

    //Walk properties
    const int BASE_STEP_DURATION = 1000;       //Duration in ms that each step takes to complete
    const float BASE_MAXROT_ANGLE = M_PI / 4; //Maximum angle of rotation per step

    float mWalkSpeed = 1.0;
    float mStepHeight = 1.0;
    float mStepDist = 2;    

    void walk(float walkDir);
    void centerBody();
    void orientToFront();
    void resetFeetPos();
    void resetBodyPos();
    void resetBodyRot();
    void stepTowardsTarget();
    void orientBody();
    void setFeetToCurrPos();
};