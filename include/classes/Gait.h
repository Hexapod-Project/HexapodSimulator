#pragma once

#include <vector>
#include "GaitGroup.h"

class Gait
{
private:
    std::vector<GaitGroup> mGaitGroups;
    int mTimeOffset;
    int mStepDuration;

public:
    void setGroups(std::vector<GaitGroup> groups);
    int getGroupSize();
    void initStartTime(int startTime, int stepDuration);
    void setTimeOffset(int timeOffset);
    int getTimeOffset();
    void setStepDuration(int stepDuration);
    int getStepDuration();
    GaitGroup *getGroup(int idx);
};
