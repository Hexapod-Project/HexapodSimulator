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
    mBody.mLocalPosY = 1;

    mBody.mStartPos = vec3(0, 1, 0);

    mLegs.clear();

    //Create the 6 legs
    for (int footIdx = 0; footIdx < LEG_COUNT; footIdx++)
    {
        //The right legs' positions are located in the even indices of the legOffsets array
        bool isRightLeg = footIdx % 2;

        vec3 legPos = legOffsets[footIdx];
        Leg3D *leg = new Leg3D(legOffsets[footIdx], HIP_LENGTH, FEMUR_LENGTH, TIBIA_LENGTH, isRightLeg);
        leg->setParent(&mBody);
        leg->setup();

        leg->mFootStartAngle = atan2(legPos.z, legPos.x);
        leg->mFootStartPos = vec3(cos(leg->mFootStartAngle) * FOOT_DIST, FOOT_Y, sin(leg->mFootStartAngle) * FOOT_DIST);
        leg->setFootTargetPos(leg->mFootStartPos);

        mLegs.push_back(leg);
    }

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
        resetBodyRot();
    }

    ImGui::Separator();
    ImGui::Text("Foot Positions");
    ImGui::Columns(4);
    ImGui::DragFloat("FL X", &mLegs[LEG::FRONTLEFT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR X", &mLegs[LEG::FRONTRIGHT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML X", &mLegs[LEG::MIDLEFT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR X", &mLegs[LEG::MIDRIGHT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL X", &mLegs[LEG::BACKLEFT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR X", &mLegs[LEG::BACKRIGHT]->mTargetFootPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Y", &mLegs[LEG::FRONTLEFT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("FR Y", &mLegs[LEG::FRONTRIGHT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("ML Y", &mLegs[LEG::MIDLEFT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("MR Y", &mLegs[LEG::MIDRIGHT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BL Y", &mLegs[LEG::BACKLEFT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BR Y", &mLegs[LEG::BACKRIGHT]->mTargetFootPosY, 0.1f, 0, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Z", &mLegs[LEG::FRONTLEFT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR Z", &mLegs[LEG::FRONTRIGHT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML Z", &mLegs[LEG::MIDLEFT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR Z", &mLegs[LEG::MIDRIGHT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL Z", &mLegs[LEG::BACKLEFT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR Z", &mLegs[LEG::BACKRIGHT]->mTargetFootPosZ, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    if (ImGui::Button("Reset FL"))
        mLegs[FRONTLEFT]->resetFootTargetPos();

    if (ImGui::Button("Reset FR"))
        mLegs[FRONTRIGHT]->resetFootTargetPos();

    if (ImGui::Button("Reset ML"))
        mLegs[MIDLEFT]->resetFootTargetPos();

    if (ImGui::Button("Reset MR"))
        mLegs[MIDRIGHT]->resetFootTargetPos();

    if (ImGui::Button("Reset BL"))
        mLegs[BACKLEFT]->resetFootTargetPos();

    if (ImGui::Button("Reset BR"))
        mLegs[BACKRIGHT]->resetFootTargetPos();

    ImGui::Columns(1);

    if (ImGui::Button("Reset Feet"))
        resetFeetPos();

    //Walk/Rotate cycle
    ImGui::Separator();
    ImGui::Text("Walk Cycle");

    ImGui::Columns(3);
    ImGui::Combo("Gait", &mComboGaitType, std::vector<std::string>{"Tripod", "Ripple", "Triple", "Wave"});

    ImGui::NextColumn();
    ImGui::Checkbox("Crab Mode", &mCrabModeCheckbox);

    ImGui::NextColumn();
    if (ImGui::Button("Reset"))
    {
        resetBodyPos();
        resetBodyRot();
        resetFeetPos();
    }

    //Walk Directions buttons
    ImGui::Columns(1);
    ImGui::Text("Walk Directions");

    ImGui::Columns(3);
    float moveDir = toDegrees(mMoveDir);
    ImGui::InputFloat("Move", &moveDir);
    ImGui::NextColumn();
    float faceDir = toDegrees(mFaceDir);
    ImGui::InputFloat("Face", &faceDir);
    ImGui::NextColumn();
    float targetDir = toDegrees(mTargetDir);
    ImGui::InputFloat("Target", &targetDir);
    ImGui::NextColumn();

    if (ImGui::Button("Left Forward", BTN_SIZE))
        walk(LEFT_FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Forward", BTN_SIZE))
        walk(FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Right Forward", BTN_SIZE))
        walk(RIGHT_FORWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Left", BTN_SIZE))
        walk(LEFT);

    ImGui::NextColumn();
    const char *stopBtnStr = mGaitManager->isStopping() ? "Stopping" : "Stop";

    if (ImGui::Button(stopBtnStr, BTN_SIZE))
        mGaitManager->stopGait();

    ImGui::NextColumn();
    if (ImGui::Button("Right", BTN_SIZE))
        walk(RIGHT);

    ImGui::NextColumn();
    if (ImGui::Button("Left Backward", BTN_SIZE))
        walk(LEFT_BACKWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Backward", BTN_SIZE))
        walk(BACKWARD);

    ImGui::NextColumn();
    if (ImGui::Button("Right Backward", BTN_SIZE))
        walk(RIGHT_BACKWARD);

    ImGui::End();
}

void Hexapod::resetFeetPos()
{
    for (int i = 0; i < LEG_COUNT; i++)
    {
        mLegs[i]->resetFootTargetPos();
    }
}

void Hexapod::resetBodyPos()
{
    mBody.mLocalPosX = mBody.mStartPos.x;
    mBody.mLocalPosY = mBody.mStartPos.y;
    mBody.mLocalPosZ = mBody.mStartPos.z;
}

void Hexapod::resetBodyRot()
{
    mBody.mRoll = 0;
    mBody.mYaw = 0;
    mBody.mPitch = 0;
    mMoveDir = mStartDir;
    mTargetDir = mStartDir;
    mFaceDir = mStartDir;
}

void Hexapod::update()
{
    bool isMoving = mGaitManager->isMoving();

    //Manage the feet positions if the hexapod is currently moving
    if (isMoving)
    {
        mGaitManager->runGait();

        //Take a step towards the new feet positions
        stepTowardsTarget();

        //Center/Orient the body
        centerBody();

        if (!mCrabMode)
            orientBody();
    }

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

void Hexapod::setWalkProperties(float stepHeight, float stepDist)
{
    mStepHeight = stepHeight;
    mStepDist = stepDist;
}

void Hexapod::walk(float walkDir)
{
    if (mGaitManager->isStopping())
        return;

    if (!mGaitManager->isMoving())
    {
        mCrabMode = mCrabModeCheckbox;

        if (!mCrabMode)
            mTargetDir = mFaceDir;

        for (int i = 0; i < LEG_COUNT; i++)
        {
            mStepStartPos[i] = mLegs[i]->mTargetFootPos;
            mStepOffsetPos[i] = vec3(0);
        }

        mGaitManager->setWalkDir(walkDir);
        mGaitManager->startGait(MOVESTATE::WALK, (GAITTYPE)mComboGaitType);
    }
    else if (!compareFloats(mTargetDir, walkDir))
        mGaitManager->setWalkDir(walkDir);
}

void Hexapod::stepTowardsTarget()
{
    MOVESTATE moveType = mGaitManager->getMoveState();

    float newDir = mMoveDir;

    for (int footIdx = 0; footIdx < mLegs.size(); footIdx++)
    {
        float timeLapsedRatio = getTimeLapsedRatio(mStepStartTimes[footIdx], mGaitManager->getStepDuration());

        if (timeLapsedRatio >= 0 && timeLapsedRatio <= 1)
        {
            vec3 startFootPos = mLegs[footIdx]->mFootStartPos + mBody.mLocalPos;
            vec3 newPos = mLegs[footIdx]->mTargetFootPos;
            vec3 stepStartPos = mStepStartPos[footIdx];
            vec3 stepOffset = mStepOffsetPos[footIdx];
            vec3 bodyStartPos = mStepBodyStartPos[footIdx];

            if (!mCrabMode && !compareFloats(mMoveDir, mTargetDir) && !mStepFootIsStop[footIdx])
            {
                float newFootAngle = mLegs[footIdx]->mFootStartAngle + getSmallestAngle(mMoveDir - mStartDir);
                startFootPos.x = cos(newFootAngle) * FOOT_DIST + bodyStartPos.x;
                startFootPos.z = sin(newFootAngle) * FOOT_DIST + bodyStartPos.z;

                vec3 footDiff = stepStartPos - startFootPos;
                float footDist = sqrt(footDiff.x * footDiff.x + footDiff.z * footDiff.z);

                stepOffset.x = cos(mMoveDir) * mStepDist - footDiff.x;
                stepOffset.z = sin(mMoveDir) * mStepDist - footDiff.z;
            }

            //Calculate the new position based on the time lapsed and the offset
            vec3 offset = stepOffset * timeLapsedRatio;
            offset.y = sin(timeLapsedRatio * M_PI) * mStepHeight - stepStartPos.y * timeLapsedRatio;

            newPos = stepStartPos + offset;

            //Update the feet current position to the new position
            mLegs[footIdx]->mTargetFootPosX = newPos.x;
            mLegs[footIdx]->mTargetFootPosY = newPos.y;
            mLegs[footIdx]->mTargetFootPosZ = newPos.z;
        }
    }
}

void Hexapod::setNextStep(int footIdx, int startTime, bool isStop)
{
    vec3 startFootPos = mLegs[footIdx]->mFootStartPos + mBody.mLocalPos;
    vec3 currFootPos = mLegs[footIdx]->mTargetFootPos;

    float newFootAngle = mLegs[footIdx]->mFootStartAngle + getSmallestAngle(mFaceDir - mStartDir);
    startFootPos.x = cos(newFootAngle) * FOOT_DIST + mBody.mLocalPosX;
    startFootPos.z = sin(newFootAngle) * FOOT_DIST + mBody.mLocalPosZ;

    vec3 footDiff = currFootPos - startFootPos;
    float footDist = sqrt(footDiff.x * footDiff.x + footDiff.z * footDiff.z);

    float offsetX = cos(mMoveDir) * mStepDist - footDiff.x;
    float offsetZ = sin(mMoveDir) * mStepDist - footDiff.z;

    if (!isStop)
        mStepOffsetPos[footIdx] = vec3(offsetX, 0, offsetZ);
    else
        mStepOffsetPos[footIdx] = vec3(-footDiff.x, 0, -footDiff.z);

    //Set the next step start position to the current foot position
    mStepStartPos[footIdx] = currFootPos;
    mStepBodyStartPos[footIdx] = mBody.mLocalPos;
    mStepFootIsStop[footIdx] = isStop;

    if (mStepStartTimes[footIdx] != startTime)
        mStepStartTimes[footIdx] = startTime;
}

void Hexapod::changeDir(double dir, float grpSize, int startTime)
{
    if (!mCrabMode)
    {
        float maxAngleRot = BASE_MAXROT_ANGLE / grpSize;
        mChangeOffsetDir = getSmallestAngle(dir - mFaceDir);
        if (std::abs(mChangeOffsetDir) > maxAngleRot)
            mChangeOffsetDir = std::copysignf(maxAngleRot, mChangeOffsetDir);

        mChangeStartDir = mFaceDir;
        mChangDirStartTime = startTime;
        mChangeDirDuration = mGaitManager->getStepDuration();
    }
    else
        mMoveDir = dir;

    mTargetDir = dir;
}

void Hexapod::orientBody()
{
    float timeLapsedRatio = getTimeLapsedRatio(mChangDirStartTime, mChangeDirDuration);

    if (timeLapsedRatio >= 0 && timeLapsedRatio <= 1)
    {
        float newDir = mChangeStartDir + mChangeOffsetDir * timeLapsedRatio;

        mMoveDir = newDir;
        mFaceDir = newDir;
    }
    else if (timeLapsedRatio > 1 && !compareFloats(toPositiveAngle(mFaceDir), mTargetDir))
        changeDir(mTargetDir, mGaitManager->mCurrGaitGroupSize, getCurrTime());

    mBody.mYaw = toDegrees(-getSmallestAngle(mFaceDir - mStartDir));
}

void Hexapod::centerBody()
{
    std::vector<vec2> currFeetPolygon;
    for (int i = 0; i < LEG_COUNT; i++)
        currFeetPolygon.push_back(vec2(mLegs[i]->mTargetFootPosX, mLegs[i]->mTargetFootPosZ));

    //Get the center of mass
    vec2 centroid = getCentroid(currFeetPolygon);

    mBody.mLocalPosX = centroid.x;
    mBody.mLocalPosZ = centroid.y;
}

vec3 Hexapod::getPos()
{
    return mBody.mLocalPos;
}