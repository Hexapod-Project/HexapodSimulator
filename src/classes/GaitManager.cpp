#include "GaitManager.h"
#include "cinder/CinderImGui.h"

GaitManager::GaitManager()
{
    initGaits();
    ImGui::Initialize();
}

void GaitManager::initGaits()
{
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;

    mGaits.resize(3);
    mGaits[GAITTYPE::TRIPOD].setGroups({GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}),
                                        GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT})});    

    mGaits[GAITTYPE::RIPPLE].setGroups({GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::FRONTLEFT})});

    mGaits[GAITTYPE::RIPPLE].setTimeOffset(-mStepDuration/2);

    mGaits[GAITTYPE::TRIPLE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}, 1000),                                        
                                        GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::BACKRIGHT}, 1000)});

    mGaits[GAITTYPE::TRIPLE].setTimeOffset(-mStepDuration/2);
}

void GaitManager::setFeetBasePos(std::vector<vec3> feetBasePos)
{
    mFeetBasePos = feetBasePos;
    mFeetStartPos = feetBasePos;
    mFeetStepDists.resize(feetBasePos.size(), mHalfStepDist);
}

void GaitManager::setBodyBasePos(vec3 bodyBasePos)
{
    mBaseBodyPos = bodyBasePos;
}

void GaitManager::setWalkProperties(float walkSpeed, float stepHeight, float stepDist)
{
    mWalkSpeed = walkSpeed;
    mStepHeight = stepHeight;
    mStepDist = stepDist;
    mHalfStepDist = stepDist / 2;
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;
}

void GaitManager::startGait(GAITTYPE gaitType)
{
    mIsWalking = true;
    mCurrGaitType = gaitType;
    mCurrGait = mGaits[gaitType];

    int currTime = getCurrTime();
    mCurrGait.initStartTime(currTime, mStepDuration);
    setFeetStartPos(mFeetBasePos.data());
}

void GaitManager::setFeetStartPos(vec3 *currFeetPos, std::vector<LEG> exclude)
{
    for (int i = 0; i < mFeetStartPos.size(); i++)
    {
        if (std::count(exclude.begin(), exclude.end(), (LEG)i))
            continue;

        vec2 diff = vec2(currFeetPos[i].x - mFeetBasePos[i].x, currFeetPos[i].z - mFeetBasePos[i].z);
        float dist = sqrt(dot(diff, diff));
        mFeetStepDists[i] = mHalfStepDist + dist;
        mFeetStartPos[i] = currFeetPos[i];
    }
}

void GaitManager::runGait(vec3 *currFeetPos)
{
    int currTime = getCurrTime();

    for (int i = 0; i < mCurrGait.getGroupSize(); i++)
    {        
        GaitGroup *currGroup = mCurrGait.getGroup(i);

        float timeLapsed = currTime - currGroup->getStartTime();
        float timeLapsedRatio = timeLapsed / mStepDuration;

        std::vector<LEG> legIndices = currGroup->getLegIndices();
        std::vector<LEG> excludedLegs;
        for (int k = 0; k < mFeetStartPos.size(); k++)
        {
            if (std::count(legIndices.begin(), legIndices.end(), (LEG)k))
                continue;

            excludedLegs.push_back((LEG)k);
        }

        if (timeLapsedRatio < 0)
            setFeetStartPos(currFeetPos, excludedLegs);

        if (timeLapsedRatio >= 0 && timeLapsedRatio <= 1)
        {
            for (int j = 0; j < legIndices.size(); j++)
            {
                int footIdx = legIndices[j];
                moveFoot(timeLapsedRatio, mFeetStepDists[footIdx], mFeetStartPos[footIdx], &currFeetPos[footIdx]);
                moveBody(currFeetPos);
            }
        }

        if (timeLapsedRatio >= 1)
        {   
            currGroup->setStartTime(currTime, mStepDuration, mCurrGait.getTimeOffset(), mCurrGait.getGroupSize() - 1);

            setFeetStartPos(currFeetPos, excludedLegs);
        }
    }
}

void GaitManager::moveFoot(float timeLapsedRatio, float stepDist, vec3 startFootPos, vec3 *currFootPos)
{
    vec3 arcOrigin = vec3(startFootPos.x, startFootPos.y, startFootPos.z + stepDist / 2);
    float angle = timeLapsedRatio * M_PI;
    float y = sin(angle) * mStepHeight;
    float forwardOffset = -cos(angle) * stepDist / 2;

    *currFootPos = arcOrigin + vec3(0, y, forwardOffset);
}

void GaitManager::moveBody(vec3 *currFeetPos)
{
    std::vector<vec2> currFeetPolygon;
    for (int i = 0; i < mFeetStartPos.size(); i++)
        currFeetPolygon.push_back(vec2(currFeetPos[i].x, currFeetPos[i].z));

    vec2 feetCentroid = getCentroid(currFeetPolygon);
    vec3 offset = vec3(feetCentroid.x - mBaseBodyPos.x, 0, feetCentroid.y - mBaseBodyPos.z);

    for (int i = 0; i < mFeetStartPos.size(); i++)
        currFeetPos[i] -= offset;
}

void GaitManager::stopGait()
{
    mIsWalking = false;
    mFeetStartPos = mFeetBasePos;
    //TODO:: Reset the foot position or something
}

bool GaitManager::isWalking()
{
    return mIsWalking;
}