#pragma once
#include <algorithm>
#include <vector>
#include "Enums.h"

class GaitGroup
{
private:
    std::vector<LEG> mLegIndices;
    int mStartTime;
    int mPauseDuration;

public:
    GaitGroup(){};
    GaitGroup(std::vector<LEG> legIndices, int pauseDuration = 0);
    void setStartTime(int startTime, int stepDuration = 0, int timeOffset = 0);
    int getStartTime();
    int getPauseDuration();
    std::vector<LEG> getLegIndices();
};