#include <vector>
#include <algorithm>
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
