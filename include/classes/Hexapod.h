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

class GaitManager;

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

    vec3 mLegStartPos[LEG_COUNT];    

    std::vector<std::vector<LEG>> mLegSequences = {
        {LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT},
        {LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}
    };

    vec3 mBodyStepStartPos;

    int mLegSeqIdx;
    int mStepStartTime;
    int mBaseStepDuration, mStepDuration;
    float mStepDistMulti;

    ivec2 mJoystickMovePos, mJoystickRotatePos;    

    float mFaceDir = FORWARD, mTargetFaceDir = FORWARD;
    float mMoveDir = 0;
    float mCosMoveDir, mSinMoveDir;

    int mComboGaitType = GAITTYPE::TRIPOD;

    //Walk properties
    float mStepHeight = 1.0;
    float mPrevStepHeight;
    vec2 mStepDist;
    vec2 mPrevStepDist;

    float mStepRotAngle;
    float mBodyStepStartYaw;

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