#include "GaitManager.h"
#include "cinder/CinderImGui.h"

std::string LEGNAMES[]{"FRONTRIGHT", "MIDRIGHT", "BACKRIGHT", "BACKLEFT", "MIDLEFT", "FRONTLEFT"};

std::ofstream debugFile;

GaitManager::GaitManager()
{
    initGaits();
    ImGui::Initialize();
    debugFile.open("debug.txt");
    debugFile.clear();
}

void GaitManager::initGaits()
{
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;

    mGaits.resize(4);
    mGaits[GAITTYPE::TRIPOD].setGroups({GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}),
                                        GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT})});

    mGaits[GAITTYPE::RIPPLE].setGroups({GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::FRONTLEFT})});

    mGaits[GAITTYPE::RIPPLE].setTimeOffset(-mStepDuration / 2);

    mGaits[GAITTYPE::TRIPLE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                        GaitGroup({LEG::MIDRIGHT}),
                                        GaitGroup({LEG::BACKLEFT}, 350),
                                        GaitGroup({LEG::FRONTRIGHT}),
                                        GaitGroup({LEG::MIDLEFT}),
                                        GaitGroup({LEG::BACKRIGHT}, 350)});

    mGaits[GAITTYPE::TRIPLE].setTimeOffset(-mStepDuration / 1.5);

    mGaits[GAITTYPE::WAVE].setGroups({GaitGroup({LEG::FRONTLEFT}),
                                      GaitGroup({LEG::MIDLEFT}),
                                      GaitGroup({LEG::BACKLEFT}),
                                      GaitGroup({LEG::FRONTRIGHT}),
                                      GaitGroup({LEG::MIDRIGHT}),
                                      GaitGroup({LEG::BACKRIGHT})});

    mGaits[GAITTYPE::WAVE].setTimeOffset(-mStepDuration / 2);
}

void GaitManager::setFeetBasePos(std::vector<vec3> feetBasePos)
{
    mFeetBasePos = feetBasePos;
    mFeetStartPos = feetBasePos;

    int footCount = feetBasePos.size();
    mFeetHalfStepDists.resize(footCount, mHalfStepDist);

    setFeetRadius();

    mFeetPrevAngles.resize(footCount);
    mFeetCurrAngles.resize(footCount);

    mFeetBaseRot.resize(footCount);
    setFeetBaseRot();
}

void GaitManager::setBodyBasePos(vec3 bodyBasePos)
{
    mBaseBodyPos = bodyBasePos;
    setFeetBaseRot();
}

void GaitManager::setFeetBaseRot()
{
    for (int i = 0; i < mFeetBaseRot.size(); i++)
    {
        vec3 diff = mFeetBasePos[i] - mBaseBodyPos;
        float angle = atan2(diff.z, diff.x);
        mFeetBaseRot[i] = angle;
    }
}

void GaitManager::setFeetRadius()
{
    mFeetRadius.resize(mFeetBasePos.size());
    for (int i = 0; i < mFeetRadius.size(); i++)
    {
        vec3 diff = mFeetBasePos[i] - mBaseBodyPos;
        mFeetRadius[i] = sqrt(dot(diff, diff));
    }
}

void GaitManager::setWalkProperties(float walkSpeed, float stepHeight, float stepDist)
{
    mWalkSpeed = walkSpeed;
    mStepHeight = stepHeight;
    mStepDist = stepDist;
    mHalfStepDist = stepDist / 2;
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;
}

void GaitManager::startGait(MOVETYPE moveType, GAITTYPE gaitType)
{
    if (_isStopping)
        return;

    mMoveType = moveType;

    mCurrGait = mGaits[gaitType];
    mCurrGaitGroupSize = mCurrGait.getGroupSize();

    int currTime = getCurrTime();
    mCurrGait.initStartTime(currTime, mStepDuration);
    updateForNextCycle(mFeetBasePos.data());
}

void GaitManager::updateForNextCycle(vec3 *currFeetPos, std::vector<LEG> exclude)
{
    for (int i = 0; i < mFeetStartPos.size(); i++)
    {
        if (std::count(exclude.begin(), exclude.end(), (LEG)i))
            continue;

        vec3 currFootPos = currFeetPos[i];

        vec2 diff = vec2(currFootPos.x - mFeetBasePos[i].x, currFootPos.z - mFeetBasePos[i].z);
        float dist = sqrt(dot(diff, diff));
        if (dist < 0.001)
            dist = 0;

        //Since the distance is used to calulate the arc positions, only half of the distance is needed
        if (_isStopping)
            mFeetHalfStepDists[i] = dist / 2;
        else
            mFeetHalfStepDists[i] = (mHalfStepDist + dist) / 2;

        currFeetPos[i].y = 0; //To make sure the foot is grounded
        mFeetStartPos[i] = currFootPos;
    }
}
//TODO: Add directional vector to change movement direction
//(eg. (+x) = turn right, -x = turn left, +z = forward, -z = backward, (+x, +z) = forward right, etc.)
void GaitManager::runGait(vec3 *currFeetPos, vec3 dir)
{
    int currTime = getCurrTime();

    for (int i = 0; i < mCurrGaitGroupSize; i++)
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

        //If it is not time to move the feet yet, update feet positions (because it will be moving backwards instead)
        //and exclude the legs that are not in the leg indices
        if (timeLapsedRatio < 0)
            updateForNextCycle(currFeetPos, excludedLegs);

        if (timeLapsedRatio >= 0 && timeLapsedRatio <= 1)
        {
            debugFile << "=== Group " << i << ", timeLapsedRatio: " << timeLapsedRatio << " ===" << std::endl;

            for (int j = 0; j < legIndices.size(); j++)
            {
                int footIdx = legIndices[j];

                if (mMoveType == MOVETYPE::WALK)
                    mMoveDir = mWalkDir;
                else if (mMoveType == MOVETYPE::ROTATE)
                {
                    mMoveDir = mFeetBaseRot[footIdx];
                    if (mRotateDir == ROTATEDIR::COUNTERCLOCKWISE)
                        mMoveDir -= M_PI / 2;
                    else
                        mMoveDir += M_PI / 2;

                    vec3 diff = currFeetPos[footIdx] - mBaseBodyPos;                    
                    mFeetPrevAngles[footIdx] = toPositiveAngle(atan2(diff.z, diff.x));
                }

                moveFoot(timeLapsedRatio, footIdx, &currFeetPos[footIdx]);
            }

            if (mMoveType == MOVETYPE::WALK)
                moveBody(currFeetPos, legIndices);
            else if (mMoveType == MOVETYPE::ROTATE)
                rotateBody(currFeetPos, legIndices);
        }

        if (timeLapsedRatio >= 1)
        {
            if (!_isStopping)
            {
                //Reset the start time to the current time
                GaitGroup *prevGroup = mCurrGait.getGroup(i > 0 ? i - 1 : mCurrGaitGroupSize - 1);
                currGroup->setStartTime(prevGroup->getStartTime() + prevGroup->getPauseDuration(), mStepDuration, mCurrGait.getTimeOffset());

                //Reset the feet start positions to the current positions
                updateForNextCycle(currFeetPos, excludedLegs);
            }
            //If stopping/changing direction, checks if all legs are stopped
            else if (!std::count(mStoppedGroups.begin(), mStoppedGroups.end(), i))
            {
                mStoppedGroups.push_back(i);
                if (mStoppedGroups.size() == mCurrGaitGroupSize)
                {
                    mMoveType = MOVETYPE::IDLE;
                    _isStopping = false;
                }
            }
        }
    }
}

void GaitManager::moveFoot(float timeLapsedRatio, int footIdx, vec3 *currFootPos)
{
    float halfStepDist = mFeetHalfStepDists[footIdx];

    if (_isStopping && halfStepDist <= 0.001)
        return;

    float distFromOrigin = mFeetRadius[footIdx];
    vec3 startFootPos = mFeetStartPos[footIdx];

    float cosWalkDir = cos(mMoveDir);
    float sinWalkDir = sin(mMoveDir);

    //The origin of the arc that is formed by the motion of the foot
    vec3 arcOrigin = vec3(startFootPos.x + halfStepDist * cosWalkDir, startFootPos.y, startFootPos.z + halfStepDist * sinWalkDir);

    //The angle between the line formed by the foot position to the origin and the ground
    float angleOfStepArc = timeLapsedRatio * M_PI;

    float y = sin(angleOfStepArc) * mStepHeight;

    //Calculate the offset of the foot on the XZ plane
    float forwardOffset = -cos(angleOfStepArc) * halfStepDist;

    //Take the drection of the movement into account
    float x = cosWalkDir * forwardOffset;
    float z = sinWalkDir * forwardOffset;

    //Directly add the offsets (x, y, z) to the arc origin since it is just a straight line
    if (mMoveType == MOVETYPE::WALK)
        *currFootPos = arcOrigin + vec3(x, y, z);
    //Calculate the curve on the XZ plane that is formed by the rotation motion of the foot
    else if (mMoveType == MOVETYPE::ROTATE)
    {
        //Get the angle of the new foot position in relation to 0 degrees
        float angle = toPositiveAngle(atan2(arcOrigin.z + z, arcOrigin.x + x));   

        mFeetCurrAngles[footIdx] = angle;

        //Calculate the correct x and z position that maintains the original foot distance from origin
        x = cos(angle) * distFromOrigin;
        z = sin(angle) * distFromOrigin;

        *currFootPos = vec3(x, arcOrigin.y + y, z);
    }
}

//Move the other feet so that the body will be centered
void GaitManager::moveBody(vec3 *currFeetPos, std::vector<LEG> exclude)
{
    int feetCount = mFeetStartPos.size();

    std::vector<vec2> currFeetPolygon;
    for (int i = 0; i < feetCount; i++)
        currFeetPolygon.push_back(vec2(currFeetPos[i].x, currFeetPos[i].z));

    //Get the center of mass
    vec2 feetCentroid = getCentroid(currFeetPolygon);
    vec3 offset = vec3(feetCentroid.x - mBaseBodyPos.x, 0, feetCentroid.y - mBaseBodyPos.z);

    //Move the feet positions to center the body
    for (int i = 0; i < feetCount; i++)
    {
        if (std::count(exclude.begin(), exclude.end(), (LEG)i))
            continue;

        currFeetPos[i] -= offset;
    }
}

//Move the other feet so that the body will be oriented correctly
void GaitManager::rotateBody(vec3 *currFeetPos, std::vector<LEG> rotatingFeet)
{
    int feetCount = mFeetStartPos.size();

    //Issue 1: It works only for gaits that are in sequence (tripod gait) but not for gaits that overlaps
    //- (if i can fix this, issue 2 should be fixed too)

    int movedFootIdx = rotatingFeet[0];

    //The number of feet the delta angle should be split among
    float denominator = feetCount / rotatingFeet.size() - 1;

    vec3 movedFootDiff = currFeetPos[movedFootIdx] - mBaseBodyPos;
    float movedFootAngle = toPositiveAngle(atan2(movedFootDiff.z, movedFootDiff.x));

    //Get the angle that was changed
    float deltaAngle = getSmallestAngle(movedFootAngle - mFeetPrevAngles[movedFootIdx]) / denominator;

    //debugFile << "=MovedFootAngle " << movedFootIdx << ": " << movedFootAngle << ",  prevAngle: " << mFeetPrevAngles[movedFootIdx] << "=" << std::endl;
    //debugFile << "=MovedFootAngle - prevAngle: " << movedFootAngle - mFeetPrevAngles[movedFootIdx] << "=" << std::endl;

    for (int i = 0; i < feetCount; i++)
    {
        if (std::count(rotatingFeet.begin(), rotatingFeet.end(), (LEG)i))
            continue;

        float footRadius = mFeetRadius[i];

        vec3 diff = currFeetPos[i] - mBaseBodyPos;
        float currFootAngle = atan2(diff.z, diff.x);

        // if (deltaAngle < M_PI / 18)
        //     debugFile << "-Foot " << i << ", currAngle: " << currFootAngle << ", deltaAngle: " << deltaAngle << "-" << std::endl;
        // else
        //     debugFile << "*Foot " << i << ", currAngle: " << currFootAngle << ", deltaAngle: " << deltaAngle << "*" << std::endl;

        //Subtract the change in angle to current foot angle so that it moves opposite to the feet that were lifted
        float newAngle = currFootAngle - deltaAngle;

        //Use the new angle to calculate the X and Z position that maintains the orginal foot distance from origin
        currFeetPos[i] = vec3(cos(newAngle) * footRadius, currFeetPos[i].y, sin(newAngle) * footRadius);
    }
}

//Store the directional value in a temporary variable first
//Only update the direction when the walk cycle has completed
void GaitManager::setWalkDir(float walkDir)
{
    mWalkDir = walkDir;
}

void GaitManager::setRotateDir(ROTATEDIR rotateDir)
{
    mRotateDir = rotateDir;
}

ROTATEDIR GaitManager::getRotateDir()
{
    return mRotateDir;
}

void GaitManager::stopGait()
{
    if (!_isStopping)
    {
        _isStopping = true;
        mStoppedGroups.resize(0);
    }
}

bool GaitManager::isWalking()
{
    return mMoveType == MOVETYPE::WALK;
}

bool GaitManager::isRotating()
{
    return mMoveType == MOVETYPE::ROTATE;
}

bool GaitManager::isStopping()
{
    return _isStopping;
}

bool GaitManager::isMoving()
{
    return mMoveType != MOVETYPE::IDLE;
}