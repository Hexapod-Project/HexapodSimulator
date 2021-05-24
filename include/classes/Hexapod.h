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
    void setFootTarget(int footIdx, vec3 footPos);
    void setNextStep(int footIdx, double dir, int startTime, bool isStop = false);
    void updateNextStep(int footIdx, double dir, bool isStop = false);
    void centerBody();
    void orientToFront();
    void drawCoord();

    int mStepDuration;

private:
    Body mBody;

    std::vector<Leg3D *> mLegs;

    bool mCrabMode = true;

    float mBodyRoll;
    float mBodyYaw;
    float mBodyPitch;

    float mFeetCurrPosX[LEG_COUNT];
    float mFeetCurrPosY[LEG_COUNT];
    float mFeetCurrPosZ[LEG_COUNT];

    vec3 mFeetStartPos[LEG_COUNT];
    float mFeetStartRot[LEG_COUNT];
    vec3 mFeetStepStartPos[LEG_COUNT];
    float mFeetStepStartRot[LEG_COUNT];
    vec3 mFeetOffsetPos[LEG_COUNT];
    float mFeetOffsetRot[LEG_COUNT];

    float mFeetRadius[LEG_COUNT];

    std::vector<int> mRotatedLegs;
    float mDeltaAngle;

    int mComboGaitType = GAITTYPE::TRIPOD;    

    GaitManager *mGaitManager;

    //Walk properties
    const int BASE_STEP_DURATION = 1000; //Each step takes 800ms to complete

    float mWalkSpeed = 1.0;
    float mStepHeight = 1.0;    
    float mStepDist = 2.0;    
    float mHalfStepDist = 1;
    float mHalfStepAngle = M_PI/18; //Rotates 10 degrees

    std::vector<int> mStepStartTimes;
    

    void createWalkAnimations();
    void resetFeetPos();
    void resetBodyPos();
    void stepTowardsTarget();
    void setFeetToCurrPos();
};