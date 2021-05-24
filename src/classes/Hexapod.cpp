#include "Hexapod.h"

const vec2 BTN_SIZE = vec2(100, 25);

void Hexapod::setup()
{
    //Leg offsets from the body's origin
    vec3 legOffsets[LEG_COUNT];
    legOffsets[LEG::FRONTLEFT] = vec3(X_FB_LEG_OFFSET, Y_LEG_OFFSET, Z_FB_LEG_OFFSET);
    legOffsets[LEG::FRONTRIGHT] = vec3(-X_FB_LEG_OFFSET, Y_LEG_OFFSET, Z_FB_LEG_OFFSET);
    legOffsets[LEG::MIDLEFT] = vec3(X_MID_LEG_OFFSET, Y_LEG_OFFSET, 0);
    legOffsets[LEG::MIDRIGHT] = vec3(-X_MID_LEG_OFFSET, Y_LEG_OFFSET, 0);
    legOffsets[LEG::BACKLEFT] = vec3(X_FB_LEG_OFFSET, Y_LEG_OFFSET, -Z_FB_LEG_OFFSET);
    legOffsets[LEG::BACKRIGHT] = vec3(-X_FB_LEG_OFFSET, Y_LEG_OFFSET, -Z_FB_LEG_OFFSET);

    mBody = Body(BODY_THICKNESS, BODY_WIDTH, BODY_LENGTH);
    mBody.setBase(vec3(0), vec3(0));

    mLegs.clear();
    //Create the 6 legs
    for (int footIdx = 0; footIdx < LEG_COUNT; footIdx++)
    {
        //The right legs' positions are located in the evevn indices of the legOffsets array
        bool isRightLeg = footIdx % 2;

        vec3 legPos = legOffsets[footIdx];
        Leg3D *leg = new Leg3D(legOffsets[footIdx], HIP_LENGTH, FEMUR_LENGTH, TIBIA_LENGTH, isRightLeg);
        leg->setParent(&mBody);
        leg->setup();

        float footAngle = atan2(legPos.z, legPos.x);
        vec3 footPos = vec3(FOOT_DIST * cos(footAngle), FOOT_Y, FOOT_DIST * sin(footAngle));
        leg->setFootTargetPos(footPos);

        mFeetRadius[footIdx] = FOOT_DIST;
        mFeetStartPos[footIdx] = footPos;
        mFeetStartRot[footIdx] = footAngle;

        mFeetCurrPosX[footIdx] = footPos.x;
        mFeetCurrPosY[footIdx] = footPos.y;
        mFeetCurrPosZ[footIdx] = footPos.z;

        mFeetOffsetPos[footIdx] = footPos;

        mLegs.push_back(leg);
    }

    // mGaitManager->setFeetBasePos(std::vector<vec3>(mFootStartPos, mFootStartPos + 6));
    // mGaitManager->setBodyBasePos(vec3(mBody.mPosX, mBody.mPosY, mBody.mPosZ));
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;

    mStepStartTimes.resize(6, 0);

    mGaitManager = new GaitManager(this);

    ImGui::Initialize();
}

void Hexapod::drawGUI()
{
    ImGui::Begin("Hexapod Properties");
    ImGui::Text("Body Positions");
    ImGui::Columns(3);
    ImGui::DragFloat("X", &mBody.mLocalPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("Y", &mBody.mLocalPosY, 0.1f, 0, FEMUR_LENGTH + TIBIA_LENGTH);
    ImGui::NextColumn();
    ImGui::DragFloat("Z", &mBody.mLocalPosZ, 0.1f, -FLT_MAX, FLT_MAX);

    ImGui::Columns(1);
    if (ImGui::Button("Reset Body Positions"))
    {
        resetBodyPos();
    }

    ImGui::Separator();
    ImGui::Text("Body Rotations");
    ImGui::Columns(3);
    ImGui::DragFloat("Roll", &mBody.mRoll, 1, -180, 180);
    ImGui::NextColumn();
    ImGui::DragFloat("Yaw", &mBody.mYaw, 1, -180, 180);
    ImGui::NextColumn();
    ImGui::DragFloat("Pitch", &mBody.mPitch, 1, -180, 180);

    ImGui::Columns(1);
    if (ImGui::Button("Reset Body Rotations"))
    {
        mBody.mRoll = 0;
        mBody.mYaw = 0;
        mBody.mPitch = 0;
    }

    ImGui::Separator();
    ImGui::Text("Foot Positions");
    ImGui::Columns(4);
    ImGui::DragFloat("FL X", &mFeetCurrPosX[LEG::FRONTLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR X", &mFeetCurrPosX[LEG::FRONTRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML X", &mFeetCurrPosX[LEG::MIDLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR X", &mFeetCurrPosX[LEG::MIDRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL X", &mFeetCurrPosX[LEG::BACKLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR X", &mFeetCurrPosX[LEG::BACKRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Y", &mFeetCurrPosY[LEG::FRONTLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("FR Y", &mFeetCurrPosY[LEG::FRONTRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("ML Y", &mFeetCurrPosY[LEG::MIDLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("MR Y", &mFeetCurrPosY[LEG::MIDRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BL Y", &mFeetCurrPosY[LEG::BACKLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BR Y", &mFeetCurrPosY[LEG::BACKRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Z", &mFeetCurrPosZ[LEG::FRONTLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR Z", &mFeetCurrPosZ[LEG::FRONTRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML Z", &mFeetCurrPosZ[LEG::MIDLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR Z", &mFeetCurrPosZ[LEG::MIDRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL Z", &mFeetCurrPosZ[LEG::BACKLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR Z", &mFeetCurrPosZ[LEG::BACKRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    if (ImGui::Button("Reset FL"))
    {
        mFeetCurrPosX[LEG::FRONTLEFT] = mFeetStartPos[LEG::FRONTLEFT].x;
        mFeetCurrPosY[LEG::FRONTLEFT] = mFeetStartPos[LEG::FRONTLEFT].y;
        mFeetCurrPosZ[LEG::FRONTLEFT] = mFeetStartPos[LEG::FRONTLEFT].z;
    }

    if (ImGui::Button("Reset FR"))
    {
        mFeetCurrPosX[LEG::FRONTRIGHT] = mFeetStartPos[LEG::FRONTRIGHT].x;
        mFeetCurrPosY[LEG::FRONTRIGHT] = mFeetStartPos[LEG::FRONTRIGHT].y;
        mFeetCurrPosZ[LEG::FRONTRIGHT] = mFeetStartPos[LEG::FRONTRIGHT].z;
    }

    if (ImGui::Button("Reset ML"))
    {
        mFeetCurrPosX[LEG::MIDLEFT] = mFeetStartPos[LEG::MIDLEFT].x;
        mFeetCurrPosY[LEG::MIDLEFT] = mFeetStartPos[LEG::MIDLEFT].y;
        mFeetCurrPosZ[LEG::MIDLEFT] = mFeetStartPos[LEG::MIDLEFT].z;
    }

    if (ImGui::Button("Reset MR"))
    {
        mFeetCurrPosX[LEG::MIDRIGHT] = mFeetStartPos[LEG::MIDRIGHT].x;
        mFeetCurrPosY[LEG::MIDRIGHT] = mFeetStartPos[LEG::MIDRIGHT].y;
        mFeetCurrPosZ[LEG::MIDRIGHT] = mFeetStartPos[LEG::MIDRIGHT].z;
    }

    if (ImGui::Button("Reset BL"))
    {
        mFeetCurrPosX[LEG::BACKLEFT] = mFeetStartPos[LEG::BACKLEFT].x;
        mFeetCurrPosY[LEG::BACKLEFT] = mFeetStartPos[LEG::BACKLEFT].y;
        mFeetCurrPosZ[LEG::BACKLEFT] = mFeetStartPos[LEG::BACKLEFT].z;
    }

    if (ImGui::Button("Reset BR"))
    {
        mFeetCurrPosX[LEG::BACKRIGHT] = mFeetStartPos[LEG::BACKRIGHT].x;
        mFeetCurrPosY[LEG::BACKRIGHT] = mFeetStartPos[LEG::BACKRIGHT].y;
        mFeetCurrPosZ[LEG::BACKRIGHT] = mFeetStartPos[LEG::BACKRIGHT].z;
    }

    ImGui::Columns(1);

    if (ImGui::Button("Reset Feet"))
        resetFeetPos();

    //Walk/Rotate cycle
    ImGui::Separator();
    ImGui::Text("Walk/Rotate Cycle");

    ImGui::Columns(3);
    ImGui::Combo("Gait", &mComboGaitType, std::vector<std::string>{"Tripod", "Ripple", "Triple", "Wave"});

    ImGui::NextColumn();
    const char *walkBtnStr = mGaitManager->isWalking() ? "Stop" : "Walk";
    const char *rotateBtnStr = mGaitManager->isRotating() ? "Stop" : "Rotate";

    if (mGaitManager->isStopping())
    {
        walkBtnStr = "Stopping";
        rotateBtnStr = "Stopping";
    }

    if (ImGui::Button(walkBtnStr, BTN_SIZE))
    {
        if (mGaitManager->isWalking())
            mGaitManager->stopGait();
        else if (!mGaitManager->isMoving())
            mGaitManager->startGait(MOVETYPE::WALK, (GAITTYPE)mComboGaitType);
    }

    ImGui::NextColumn();
    if (ImGui::Button(rotateBtnStr, BTN_SIZE))
    {
        if (mGaitManager->isRotating())
            mGaitManager->stopGait();
        else if (!mGaitManager->isMoving())
            mGaitManager->startGait(MOVETYPE::ROTATE, (GAITTYPE)mComboGaitType);
    }

    //Walk Directions buttons
    ImGui::Columns(1);
    ImGui::Text("Walk Directions");

    //TODO: Walk facing the walk direction
    ImGui::Checkbox("Crab Mode", &mCrabMode);

    ImGui::Columns(3);
    if (ImGui::Button("Left Forward", BTN_SIZE))
        mGaitManager->setWalkDir(LEFT_FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Forward", BTN_SIZE))
        mGaitManager->setWalkDir(FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Right Forward", BTN_SIZE))
        mGaitManager->setWalkDir(RIGHT_FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Left", BTN_SIZE))
        mGaitManager->setWalkDir(LEFT);

    ImGui::NextColumn();
    ImGui::NextColumn();

    if (ImGui::Button("Right", BTN_SIZE))
        mGaitManager->setWalkDir(RIGHT);

    ImGui::NextColumn();

    if (ImGui::Button("Left Backward", BTN_SIZE))
        mGaitManager->setWalkDir(LEFT_BACKWARD);

    ImGui::NextColumn();

    if (ImGui::Button("Backward", BTN_SIZE))
        mGaitManager->setWalkDir(BACKWARD);

    ImGui::NextColumn();

    if (ImGui::Button("Right Backward", BTN_SIZE))
        mGaitManager->setWalkDir(RIGHT_BACKWARD);

    //Rotate Directions buttons
    ImGui::Columns(1);
    ImGui::Text("Rotate Directions");

    if (ImGui::Button(mGaitManager->getRotateDir() == ROTATEDIR::CLOCKWISE ? "CW" : "CCW", BTN_SIZE))
    {
        if (mGaitManager->getRotateDir() == ROTATEDIR::CLOCKWISE)
            mGaitManager->setRotateDir(ROTATEDIR::COUNTERCLOCKWISE);
        else
            mGaitManager->setRotateDir(ROTATEDIR::CLOCKWISE);
    }

    ImGui::End();
}

void Hexapod::resetFeetPos()
{
    for (int i = 0; i < LEG_COUNT; i++)
    {
        mFeetCurrPosX[i] = mFeetStartPos[i].x;
        mFeetCurrPosY[i] = mFeetStartPos[i].y;
        mFeetCurrPosZ[i] = mFeetStartPos[i].z;
    }
}

void Hexapod::resetBodyPos()
{
    mBody.mLocalPosX = 0;
    mBody.mLocalPosY = 1;
    mBody.mLocalPosZ = 0;
}

void Hexapod::update()
{
    bool isMoving = mGaitManager->isMoving();

    //Manage the feet positions if the hexapod is currently moving
    if (isMoving)
        mGaitManager->runGait();

    //Take a step towards the new feet positions
    stepTowardsTarget();
    setFeetToCurrPos();

    //Center/Orient the body
    if (mGaitManager->isWalking())
        centerBody();
    else if (mGaitManager->isRotating())
        orientToFront();

    //Update the legs transformation visually
    for (Leg3D *leg : mLegs)
        leg->update();

    //Update the body transformations visually
    mBody.update();

    drawGUI();
}

void Hexapod::draw()
{
    mBody.draw();

    for (Leg3D *leg : mLegs)
        leg->draw();
}

void Hexapod::drawCoord()
{
    mBody.drawCoord();
}

void Hexapod::setWalkProperties(float walkSpeed, float stepHeight, float stepDist)
{
    mWalkSpeed = walkSpeed;
    mStepHeight = stepHeight;
    mStepDist = stepDist;
    mHalfStepDist = stepDist / 2;
    mStepDuration = BASE_STEP_DURATION / mWalkSpeed;
}

void Hexapod::setFootTarget(int footIdx, vec3 footPos)
{
    mFeetOffsetPos[footIdx] = footPos;
}

void Hexapod::stepTowardsTarget()
{
    mRotatedLegs.clear();
    mDeltaAngle = 0;

    MOVETYPE moveType = mGaitManager->getMoveType();

    for (int footIdx = 0; footIdx < mLegs.size(); footIdx++)
    {
        float timeLapsedRatio = getTimeLapsedRatio(mStepStartTimes[footIdx], mStepDuration);
        
        vec3 newPos = vec3(mFeetCurrPosX[footIdx], mFeetCurrPosY[footIdx], mFeetCurrPosZ[footIdx]);

        vec3 stepStartPos = mFeetStepStartPos[footIdx];

        //Calculate the step height based on the time lapsed
        float y = sin(timeLapsedRatio * M_PI) * mStepHeight - stepStartPos.y * timeLapsedRatio;

        if (timeLapsedRatio >= 0 && timeLapsedRatio <= 1)
        {
            if (moveType == MOVETYPE::WALK)
            {
                //Calculate the new position based on the time lapsed and the offset
                vec3 offset = mFeetOffsetPos[footIdx] * timeLapsedRatio;
                offset.y = y;                
                newPos = stepStartPos + offset;
            }
            else if (moveType == MOVETYPE::ROTATE)
            {
                //Calculate the new angle of the foot based on the current time lapsed and offset angle
                float startAngle = mFeetStepStartRot[footIdx];
                float offsetAngle = mFeetOffsetRot[footIdx] * timeLapsedRatio;
                float newAngle = toPositiveAngle(startAngle + offsetAngle);

                //Set the new position based on the new angle
                float footRadius = mFeetRadius[footIdx];
                newPos = vec3(cos(newAngle) * footRadius, y, sin(newAngle) * footRadius);

                //Calculate the changed in angle compared to the frame before this
                float currAngle = toPositiveAngle(atan2(mFeetCurrPosZ[footIdx], mFeetCurrPosX[footIdx]));
                float deltaAngle = getSmallestAngle(newAngle - currAngle);

                //Add the delta angle to the global variable to be used to orient the body
                mDeltaAngle += deltaAngle;
                //Add the rotated foot indices to be skipped from orienting the body
                mRotatedLegs.push_back(footIdx);
            }
        }

        //Update the feet current position to the new position
        mFeetCurrPosX[footIdx] = newPos.x;
        mFeetCurrPosY[footIdx] = newPos.y;
        mFeetCurrPosZ[footIdx] = newPos.z;

        //Update the feet current radius(distance from the body's origin)
        vec2 footRadiusDiff = vec2(newPos.x - mBody.mLocalPosX, newPos.z - mBody.mLocalPosZ);
        mFeetRadius[footIdx] = sqrt(dot(footRadiusDiff, footRadiusDiff));
    }

    mDeltaAngle = mDeltaAngle / mRotatedLegs.size();
}

void Hexapod::setNextStep(int footIdx, double dir, int startTime, bool isStop)
{
    updateNextStep(footIdx, dir, isStop);
    mStepStartTimes[footIdx] = startTime;
}

void Hexapod::updateNextStep(int footIdx, double dir, bool isStop)
{
    vec3 currFootPos = vec3(mFeetCurrPosX[footIdx], mFeetCurrPosY[footIdx], mFeetCurrPosZ[footIdx]);

    MOVETYPE moveType = mGaitManager->getMoveType();

    if (moveType == MOVETYPE::WALK)
    {
        vec2 diff = vec2(currFootPos.x - mFeetStartPos[footIdx].x, currFootPos.z - mFeetStartPos[footIdx].z);

        if (!isStop)
        {
            //Get the difference of the foot's distance from the initial position
            float diffDist = sqrt(dot(diff, diff));
            //Calculate the step distance to be taken
            float stepDist = mHalfStepDist + diffDist;

            //Calculate the position offset for the next step
            mFeetOffsetPos[footIdx] = vec3(cos(dir) * stepDist, 0, sin(dir) * stepDist);
        }
        else
            //Set the foot position offset so that it moves back to the initial position
            mFeetOffsetPos[footIdx] = vec3(-diff.x, 0, -diff.y);
    }
    else if (moveType == MOVETYPE::ROTATE)
    {
        //Calculate the difference of the current foot angle to the initial foot angle
        float currAngle = atan2(currFootPos.z - mBody.mLocalPosZ, currFootPos.x - mBody.mLocalPosX);
        float diffAngle = getSmallestAngle(currAngle - mFeetStartRot[footIdx]);

        if (!isStop)
            //Set the foot rotation offset
            mFeetOffsetRot[footIdx] = mHalfStepAngle * dir - diffAngle;
        else
            //Set the foot rotation offset so that it moves back to the initial rotation
            mFeetOffsetRot[footIdx] = -diffAngle;

        mFeetStepStartRot[footIdx] = currAngle;
    }

    //Set the next step start position to the current foot position
    mFeetStepStartPos[footIdx] = currFootPos;
}

void Hexapod::setFeetToCurrPos()
{
    //Set the feet to the current positions
    for (int i = 0; i < mLegs.size(); i++)
    {
        Leg3D *leg = mLegs[i];

        leg->setFootTargetPos(vec3(mFeetCurrPosX[i], mFeetCurrPosY[i], mFeetCurrPosZ[i]));
    }
}

void Hexapod::centerBody()
{
    std::vector<vec2> currFeetPolygon;
    for (int i = 0; i < LEG_COUNT; i++)
        currFeetPolygon.push_back(vec2(mFeetCurrPosX[i], mFeetCurrPosZ[i]));

    //Get the center of mass
    vec2 centroid = getCentroid(currFeetPolygon);

    //Change the feet position so that the body is centered
    for (int legIdx = 0; legIdx < LEG_COUNT; legIdx++)
    {
        mFeetCurrPosX[legIdx] -= centroid.x - mBody.mBasePos.x;
        mFeetCurrPosZ[legIdx] -= centroid.y - mBody.mBasePos.z;
    }
}

void Hexapod::orientToFront()
{
    if (mRotatedLegs.size() <= 0)
        return;

    //Divider to divide the delta angle among the non-rotating/pushing feet
    int divider = LEG_COUNT / mRotatedLegs.size() - 1;

    for (int footIdx = 0; footIdx < LEG_COUNT; footIdx++)
    {
        //Skip the feet that are currently taking a step
        if (std::count(mRotatedLegs.begin(), mRotatedLegs.end(), footIdx))
            continue;

        //Calculate the new angle by adding the current angle with the delta angle of the current frame and the previous frame
        float currAngle = atan2(mFeetCurrPosZ[footIdx], mFeetCurrPosX[footIdx]);
        float newAngle = currAngle - mDeltaAngle / divider;

        //Set the new foot position
        float footRadius = mFeetRadius[footIdx];
        vec3 newPos = vec3(cos(newAngle) * footRadius, mFeetCurrPosY[footIdx], sin(newAngle) * footRadius);

        mFeetCurrPosX[footIdx] = newPos.x;
        mFeetCurrPosZ[footIdx] = newPos.z;
    }
}