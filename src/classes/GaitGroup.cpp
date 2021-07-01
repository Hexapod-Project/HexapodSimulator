#include "GaitGroup.h"

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