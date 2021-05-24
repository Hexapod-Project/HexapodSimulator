#include "GaitManager.h"
#include "cinder/CinderImGui.h"

std::string LEGNAMES[]{"FRONTRIGHT", "MIDRIGHT", "BACKRIGHT", "BACKLEFT", "MIDLEFT", "FRONTLEFT"};

GaitManager::GaitManager(Hexapod *target)
{
    mTarget = target;
    initGaits();
    ImGui::Initialize();
}

void GaitManager::initGaits()
{
    mGaits.resize(4);
    mGaits[GAITTYPE::TRIPOD].setGroups({GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}),
                                        GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT})});

    mGaits[GAITTYPE::RIPPLE].setGroups({GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::FRONTLEFT})});

    mGaits[GAITTYPE::RIPPLE].setTimeOffset(-mTarget->mStepDuration / 2);

    mGaits[GAITTYPE::TRIPLE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}, 350),
                                        GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::BACKRIGHT}, 350)});

    mGaits[GAITTYPE::TRIPLE].setTimeOffset(-mTarget->mStepDuration / 1.5);

    mGaits[GAITTYPE::WAVE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                      GaitGroup({LEG::MIDLEFT}),
                                      GaitGroup({LEG::BACKLEFT}),
                                      GaitGroup({LEG::FRONTRIGHT}),
                                      GaitGroup({LEG::MIDRIGHT}),
                                      GaitGroup({LEG::BACKRIGHT})});

    mGaits[GAITTYPE::WAVE].setTimeOffset(-mTarget->mStepDuration / 2);
}

void GaitManager::startGait(MOVETYPE moveType, GAITTYPE gaitType)
{
    if (_isStopping)
        return;

    mMoveType = moveType;

    setGaitType(gaitType);

    mCurrGait.initStartTime(getCurrTime(), mTarget->mStepDuration);

    for (int grpIdx = 0; grpIdx < mCurrGait.getGroupSize(); grpIdx++)
    {
        //Set the walk/rotate directions
        if (mMoveType == MOVETYPE::WALK)
            mGroupMoveDir[grpIdx] = mWalkDir;
        else if (mMoveType == MOVETYPE::ROTATE)
            mGroupMoveDir[grpIdx] = mRotateDir;

        //Initialize the steps
        GaitGroup *currGroup = mCurrGait.getGroup(grpIdx);
        std::vector<LEG> legIndices = currGroup->getLegIndices();
        for (int footIdx = 0; footIdx < legIndices.size(); footIdx++)
            mTarget->setNextStep(legIndices[footIdx], mGroupMoveDir[grpIdx], currGroup->getStartTime());
    }
}

void GaitManager::runGait(vec3 dir)
{
    int currTime = getCurrTime();

    for (int grpIdx = 0; grpIdx < mCurrGaitGroupSize; grpIdx++)
    {
        GaitGroup *currGroup = mCurrGait.getGroup(grpIdx);

        float timeLapsed = currTime - currGroup->getStartTime();
        float timeLapsedRatio = timeLapsed / mTarget->mStepDuration;

        std::vector<LEG> currFeet = currGroup->getLegIndices();

        GAITGROUPSTATE currGroupState = mGroupState[grpIdx];

        if (timeLapsedRatio < 0)
        {
            //Updates the feet positions and states while it is not taking a step
            for (int i = 0; i < currFeet.size(); i++)
                mTarget->updateNextStep(currFeet[i], mGroupMoveDir[grpIdx], currGroupState != GAITGROUPSTATE::MOVING);
        }
        else if (timeLapsedRatio >= 1)
        {
            if (currGroupState == GAITGROUPSTATE::MOVING)
            {
                //Set the next step start time based on the previous group + pause duration + step duration + time offset
                GaitGroup *prevGroup = mCurrGait.getGroup(grpIdx > 0 ? grpIdx - 1 : mCurrGaitGroupSize - 1);
                currGroup->setStartTime(prevGroup->getStartTime() + prevGroup->getPauseDuration(), mTarget->mStepDuration, mCurrGait.getTimeOffset());

                //Pass the start time to the hexapod's feet
                for (int footIdx = 0; footIdx < currFeet.size(); footIdx++)
                    mTarget->setNextStep(currFeet[footIdx], mGroupMoveDir[grpIdx], currGroup->getStartTime());
            }
            else if (currGroupState == GAITGROUPSTATE::STOPPING)
            {
                //Checks how many group have stopped
                mGroupState[grpIdx] = GAITGROUPSTATE::STOPPED;
                mStoppedGroupCount++;

                //If all groups were stopped, then set the move type to idle
                if (mStoppedGroupCount >= mCurrGaitGroupSize)
                {
                    mMoveType = MOVETYPE::IDLE;
                    _isStopping = false;
                }
            }
            else if (currGroupState == GAITGROUPSTATE::CHANGEDIR)
            {
                mGroupState[grpIdx] = GAITGROUPSTATE::MOVING;
                if (mMoveType == MOVETYPE::WALK)
                    mGroupMoveDir[grpIdx] = mWalkDir;
                else if (mMoveType == MOVETYPE::ROTATE)
                    mGroupMoveDir[grpIdx] = mRotateDir;
            }
        }
    }
}

void GaitManager::stopGait()
{
    if (_isStopping)
        return;

    _isStopping = true;

    changeAllGroupState(GAITGROUPSTATE::STOPPING);

    mStoppedGroupCount = 0;
}

GAITTYPE GaitManager::getGaitType()
{
    return mCurrGaitType;
}

//TODO: Figure out how to do smooth gait transition
void GaitManager::setGaitType(GAITTYPE gaitType, GAITGROUPSTATE groupState)
{
    mCurrGaitType = gaitType;
    mCurrGait = mGaits[gaitType];
    mCurrGaitGroupSize = mCurrGait.getGroupSize();

    mGroupState.resize(mCurrGaitGroupSize);
    changeAllGroupState(groupState);

    mGroupMoveDir.resize(mCurrGaitGroupSize);
}

void GaitManager::setWalkDir(float walkDir)
{
    if (!compareFloats(mWalkDir, walkDir))
    {
        mWalkDir = walkDir;
        for (int i = 0; i < mGroupState.size(); i++)
            mGroupState[i] = GAITGROUPSTATE::CHANGEDIR;
    }
}

void GaitManager::setRotateDir(ROTATEDIR rotateDir)
{
    if (mRotateDir != rotateDir)
    {
        mRotateDir = rotateDir;

        for (int i = 0; i < mGroupState.size(); i++)
            mGroupState[i] = GAITGROUPSTATE::CHANGEDIR;
    }    
}

ROTATEDIR GaitManager::getRotateDir()
{
    return mRotateDir;
}

void GaitManager::changeAllGroupState(GAITGROUPSTATE groupState)
{
    for (int i = 0; i < mGroupState.size(); i++)
        mGroupState[i] = groupState;
}

MOVETYPE GaitManager::getMoveType()
{
    return mMoveType;
}

bool GaitManager::isWalking()
{
    return mMoveType == MOVETYPE::WALK;
}

bool GaitManager::isRotating()
{
    return mMoveType == MOVETYPE::ROTATE;
}

bool GaitManager::isMoving()
{
    return mMoveType != MOVETYPE::IDLE;
}

bool GaitManager::isStopping()
{
    return _isStopping;
}