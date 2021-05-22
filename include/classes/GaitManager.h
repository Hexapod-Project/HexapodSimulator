#pragma once
#include "cinder/app/App.h"
#include <chrono>
#include "Tools.h"
#include "Enums.h"
#include "Gait.h"

using namespace ci;
using namespace std::chrono;

class GaitManager
{
public:
    GaitManager();
    void setWalkProperties(float walkSpeed, float stepHeight, float stepDist);
    void setFeetBasePos(std::vector<vec3> feetBasePos);
    void setBodyBasePos(vec3 bodyBasePos);
    void startGait(MOVETYPE moveType, GAITTYPE gaitType = GAITTYPE::TRIPOD);
    void runGait(vec3 *currFeetPos, vec3 dir = vec3(0));
    void stopGait();
    void setWalkDir(float walkDir);
    void setRotateDir(ROTATEDIR rotateDir);
    ROTATEDIR getRotateDir();
    bool isWalking();
    bool isRotating();
    bool isStopping();
    bool isMoving();

private:
    vec3 mBaseBodyPos;
    std::vector<vec3> mFeetBasePos;
    std::vector<float> mFeetBaseRot;
    std::vector<vec3> mFeetStartPos;
    std::vector<float> mFeetHalfStepDists;        
    std::vector<float> mFeetRadius;
    std::vector<float> mFeetPrevAngles;
    std::vector<float> mFeetCurrAngles;

    const int BASE_STEP_DURATION = 800; //Each step takes 800ms to complete

    float mWalkSpeed = 1.0;
    float mStepHeight = 1.0;
    float mStepDist = 2.0;
    float mHalfStepDist = 1;

    int mStepDuration;

    Gait mCurrGait;
    int mCurrGaitGroupSize;

    MOVETYPE mMoveType = MOVETYPE::IDLE;    
    ROTATEDIR mRotateDir = ROTATEDIR::COUNTERCLOCKWISE;

    float mWalkDir = M_PI/2;
    float mMoveDir = M_PI / 2;

    bool _isStopping = false;

    std::vector<int> mStoppedGroups;

    std::vector<Gait> mGaits;

    void initGaits();
    void restartGait();
    void setFeetBaseRot();
    void setFeetRadius();
    void updateForNextCycle(vec3 *currFootPos, std::vector<LEG> exclude = {});
    void moveFoot(float timeLapsedRatio, int footIdx, vec3 *currFootPos);
    void moveBody(vec3 *currFeetPos, std::vector<LEG> exclude = {});
    void rotateBody(vec3 *currFeetPos, std::vector<LEG> rotatingFeet = {});

    int mCurrGroupIdx = 0;
};