#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "Leg3D.h"
#include "Body.h"
#include "Enums.h"
#include "HexapodConstants.h"

using namespace ci;
using namespace HexapodConstants;

struct GaitGroup
{
    std::vector<LEG> mLegIndices;
    int mStartTime;
    float mStepTimeOffset = 0;
    float mStepDuration;

    GaitGroup(){};
    GaitGroup(std::vector<LEG> legIndices, float stepTimeOffset = 0);
};

struct Gait {
    std::vector<GaitGroup> mGaitGroups;
};

class Hexapod
{
public:
    void drawGUI();
    void setup();
    void update();
    void draw();
    void changeDir(double dir, float grpSize, int startTime);
    vec3 getPos();
    void drawCoord();

private:
    Body mBody;

    MOVETYPE mMoveTypeBtn = MOVETYPE::WALK;
    MOVETYPE mMoveType = MOVETYPE::WALK;

    MOVESTATE mMoveState = MOVESTATE::STOPPED;

    std::vector<Leg3D *> mLegs;

    std::vector<Gait> mGaits;
    GAITTYPE mGaitType = GAITTYPE::TRIPOD;
    std::vector<LEG> mLegIndices;    
    Gait* mCurrGait;
    int mGaitGrpIdx;
    int mGroupStoppedCount = 0;
    int mCurrGaitGrpSize;
    float mStepTimeOffset;

    vec3 mBodyStepStartPos;
    vec3 mLegStepStartPos[LEG_COUNT];
    float mLegStepStartRot[LEG_COUNT];
    vec3 mStepDist[LEG_COUNT];
    float mPrevStepHeight[LEG_COUNT];    

    float mStepRotAngle;
    float mBodyStepStartYaw;
    
    int mStepStartTime;
    int mStepDuration;
    float mStepDistMulti;    
    float mStepHeight;

    ivec2 mJoystickMovePos, mJoystickRotatePos;

    float mFaceDir = FORWARD, mTargetFaceDir = FORWARD;
    float mMoveDir = 0;
    float mCosMoveDir, mSinMoveDir;

    int mComboGaitType = GAITTYPE::TRIPOD;

    bool mNaturalWalkMode = true;

    void initGaits();
    void checkJoystickPos();
    void walk();
    void setNextStepRot();
    void setNextStep();
    void initStep();
    void centerBody();
    void orientToFront();
    void resetFeetPos();
    void resetBodyPos();
    void resetBodyRot();
    void stepTowardsTarget();
    void orientBody();
    void setFeetToCurrPos();
    void drawMoveJoystick();
    void drawRotateJoystick();
};