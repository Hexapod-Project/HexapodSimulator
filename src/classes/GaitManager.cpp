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

    mGaits[GAITTYPE::TRIPOD].setStepDuration(BASE_STEP_DURATION / 2);
    mGaits[GAITTYPE::TRIPOD].setGroups({GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}),
                                        GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT})});

    mGaits[GAITTYPE::RIPPLE].setStepDuration(BASE_STEP_DURATION / 3);
    mGaits[GAITTYPE::RIPPLE].setTimeOffset(-BASE_STEP_DURATION / 6);
    mGaits[GAITTYPE::RIPPLE].setGroups({GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::FRONTLEFT})});

    mGaits[GAITTYPE::TRIPLE].setStepDuration(BASE_STEP_DURATION / 3);
    mGaits[GAITTYPE::TRIPLE].setTimeOffset(-mGaits[GAITTYPE::TRIPLE].getStepDuration() / 4.5);
    int triplePauseDuration = (7 / 20) * mGaits[GAITTYPE::TRIPLE].getStepDuration();
    mGaits[GAITTYPE::TRIPLE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}, triplePauseDuration),
                                        GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::BACKRIGHT}, triplePauseDuration)});

    mGaits[GAITTYPE::WAVE].setStepDuration(BASE_STEP_DURATION / 3);
    mGaits[GAITTYPE::WAVE].setTimeOffset(-mGaits[GAITTYPE::WAVE].getStepDuration() / 6);
    mGaits[GAITTYPE::WAVE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                      GaitGroup({LEG::MIDLEFT}),
                                      GaitGroup({LEG::BACKLEFT}),
                                      GaitGroup({LEG::FRONTRIGHT}),
                                      GaitGroup({LEG::MIDRIGHT}),
                                      GaitGroup({LEG::BACKRIGHT})});
}

void GaitManager::startGait(MOVESTATE moveType, GAITTYPE gaitType)
{
    if (_isStopping)
        return;

    mMoveState = moveType;

    setGaitType(gaitType);

    mCurrGait.initStartTime(getCurrTime(), mCurrGait.getStepDuration());

    for (int grpIdx = 0; grpIdx < mCurrGait.getGroupSize(); grpIdx++)
    {
        //Initialize the steps
        GaitGroup *currGroup = mCurrGait.getGroup(grpIdx);
        std::vector<LEG> legIndices = currGroup->getLegIndices();
        for (int footIdx = 0; footIdx < legIndices.size(); footIdx++)
            mTarget->setNextStep(legIndices[footIdx], currGroup->getStartTime());
    }
}

void GaitManager::runGait(vec3 dir)
{
    int currTime = getCurrTime();

    for (int grpIdx = 0; grpIdx < mCurrGaitGroupSize; grpIdx++)
    {
        GaitGroup *currGroup = mCurrGait.getGroup(grpIdx);

        float timeLapsed = currTime - currGroup->getStartTime();
        float timeLapsedRatio = timeLapsed / mCurrGait.getStepDuration();

        std::vector<LEG> currFeet = currGroup->getLegIndices();

        GAITGROUPSTATE currGroupState = mGroupState[grpIdx];
        if (timeLapsedRatio >= 1)
        {
            if (currGroupState == GAITGROUPSTATE::MOVING)
            {
                //Set the next step start time based on the previous group + pause duration + step duration + time offset
                GaitGroup *prevGroup = mCurrGait.getGroup(grpIdx > 0 ? grpIdx - 1 : mCurrGaitGroupSize - 1);
                currGroup->setStartTime(prevGroup->getStartTime() + prevGroup->getPauseDuration(), mCurrGait.getStepDuration(), mCurrGait.getTimeOffset());

                if (_isStopping && mGroupState[grpIdx] != GAITGROUPSTATE::STOPPING)
                    mGroupState[grpIdx] = GAITGROUPSTATE::STOPPING;
            }
            else if (currGroupState == GAITGROUPSTATE::STOPPING)
            {
                //Checks how many group have stopped
                mGroupState[grpIdx] = GAITGROUPSTATE::STOPPED;
                mStoppedGroupCount++;

                //If all groups were stopped, then set the move type to idle
                if (mStoppedGroupCount >= mCurrGaitGroupSize)
                {
                    mMoveState = MOVESTATE::IDLE;
                    _isStopping = false;
                }
            }
        }

        if (timeLapsedRatio < 0 || timeLapsedRatio >= 1)
        {
            //Pass the start time to the hexapod's feet
            for (int footIdx = 0; footIdx < currFeet.size(); footIdx++)
                mTarget->setNextStep(currFeet[footIdx], currGroup->getStartTime(), _isStopping);
        }
    }
}

void GaitManager::stopGait()
{
    if (_isStopping || mMoveState == MOVESTATE::IDLE)
        return;

    _isStopping = true;

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
    mWalkDir = walkDir;
    mTarget->changeDir(walkDir, mCurrGaitGroupSize, getCurrTime());
}

void GaitManager::setRotateDir(ROTATEDIR rotateDir)
{
    if (mRotateDir != rotateDir)
        mRotateDir = rotateDir;
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

MOVESTATE GaitManager::getMoveState()
{
    return mMoveState;
}

bool GaitManager::isWalking()
{
    return mMoveState == MOVESTATE::WALK;
}

bool GaitManager::isMoving()
{
    return mMoveState != MOVESTATE::IDLE;
}

bool GaitManager::isStopping()
{
    return _isStopping;
}

int GaitManager::getStepDuration()
{
    return mCurrGait.getStepDuration();
}