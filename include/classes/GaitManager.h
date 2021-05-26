#pragma once
#include "cinder/app/App.h"
#include <chrono>
#include "Tools.h"
#include "Enums.h"
#include "Gait.h"
#include "Hexapod.h"

using namespace ci;
using namespace std::chrono;

class Hexapod;

class GaitManager
{
public:
    GaitManager(Hexapod *target);
    void initGaits();
    void startGait(MOVESTATE moveType, GAITTYPE gaitType = GAITTYPE::TRIPOD);
    void runGait(vec3 dir = vec3(0));
    void stopGait();
    GAITTYPE getGaitType();
    void setGaitType(GAITTYPE gaitType, GAITGROUPSTATE groupState = GAITGROUPSTATE::MOVING); 
    void setWalkDir(float walkDir);
    void setRotateDir(ROTATEDIR rotateDir);
    ROTATEDIR getRotateDir();    
    MOVESTATE getMoveState();
    bool isWalking();
    bool isMoving();
    bool isStopping();

private:    
    Hexapod *mTarget;

    GAITTYPE mCurrGaitType;
    Gait mCurrGait;
    int mCurrGaitGroupSize;    

    MOVESTATE mMoveState = MOVESTATE::IDLE;
    ROTATEDIR mRotateDir = ROTATEDIR::COUNTERCLOCKWISE;

    float mWalkDir = M_PI / 2;    
    
    bool _isStopping = false;

    int mStoppedGroupCount = 0;
    std::vector<GAITGROUPSTATE> mGroupState;
    std::vector<float> mGroupMoveDir;

    std::vector<Gait> mGaits;

    void restartGait();
    void setFeetBaseRot();
    void setFeetRadius();   
    void changeAllGroupState(GAITGROUPSTATE groupState); 

    int mCurrGroupIdx = 0;
};