#include "Gait.h"

GaitGroup::GaitGroup(std::vector<LEG> legIndices, int pauseDuration)
{
    mLegIndices = legIndices;
    mPauseDuration = pauseDuration;
}

void GaitGroup::setStartTime(int startTime, int stepDuration, int timeOffset, int idx)
{
    mStartTime = startTime + timeOffset * (idx + 1) + (stepDuration) * idx;
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
        mGaitGroups[i].setStartTime(startTime + mTimeOffset * i, stepDuration, 0, i);    
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
