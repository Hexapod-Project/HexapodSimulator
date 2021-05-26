#include "Gait.h"

GaitGroup::GaitGroup(std::vector<LEG> legIndices, int pauseDuration)
{
    mLegIndices = legIndices;
    mPauseDuration = pauseDuration;
}

void GaitGroup::setStartTime(int startTime, int stepDuration, int timeOffset)
{
    mStartTime = startTime + timeOffset + stepDuration;
}

int GaitGroup::getStartTime()
{
    return mStartTime;
}

int GaitGroup::getPauseDuration()
{
    return mPauseDuration;
}

std::vector<LEG> GaitGroup::getLegIndices()
{
    return mLegIndices;
}

/*------------------------------Gait--------------------------------------*/

void Gait::setGroups(std::vector<GaitGroup> groups)
{
    mGaitGroups = groups;
}

void Gait::initStartTime(int startTime, int stepDuration)
{
    for (int i = 0; i < mGaitGroups.size(); i++)
    {
        if (i > 0)
        {
            GaitGroup prevGroup = mGaitGroups[i - 1];
            int pauseDur = prevGroup.getPauseDuration();
            mGaitGroups[i].setStartTime(prevGroup.getStartTime() + pauseDur, stepDuration, mTimeOffset);
        }
        else
            mGaitGroups[i].setStartTime(startTime);
    }
}

void Gait::setTimeOffset(int timeOffset)
{
    mTimeOffset = timeOffset;
}

int Gait::getTimeOffset()
{
    return mTimeOffset;
}

int Gait::getGroupSize()
{
    return mGaitGroups.size();
}

GaitGroup *Gait::getGroup(int idx)
{
    return &mGaitGroups[idx];
}

void Gait::setStepDuration(int stepDuration)
{
    mStepDuration = stepDuration;
}

int Gait::getStepDuration()
{
    return mStepDuration;
}