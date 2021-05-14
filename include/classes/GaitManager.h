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
    void startGait(GAITTYPE gaitType = GAITTYPE::TRIPOD);        
    void runGait(vec3* currFeetPos);
    void stopGait();    
    bool isWalking();

private:    
    vec3 mBaseBodyPos;
    std::vector<vec3> mFeetBasePos;
    std::vector<vec3> mFeetStartPos;
    std::vector<float> mFeetStepDists;

    bool mIsWalking = false;

    const int BASE_STEP_DURATION = 1000; //Each step takes 1000ms to complete

    float mWalkSpeed = 1.0;
    float mStepHeight = 1.0;
    float mStepDist = 2.0;
    float mHalfStepDist = 1;

    int mStepDuration;
    int mPauseTime;

    Gait mCurrGait;
    GAITTYPE mCurrGaitType;

    std::vector<Gait> mGaits;  

    void initGaits();     
    void setFeetStartPos(vec3* currFootPos, std::vector<LEG> exclude = {});
    void moveFoot(float timeLapsedRatio, float stepDist, vec3 startFootPos, vec3* currFootPos);
    void moveBody(vec3*currFeetPos);

    int mCurrGroupIdx = 0;
};