#include "Hexapod.h"
#include "Tools.h"

const vec2 BTN_SIZE = vec2(100, 25);
const char *MOVE_STATES_STRING[] = {"MOVING", "STOPSTARTED", "STOPPING", "STOPPED"};
std::vector<std::string> GAITTYPE_STRINGS = {"Tripod", "Triple", "Wave", "Ripple"};

GaitGroup::GaitGroup(std::vector<LEG> legIndices, float stepTimeOffset)
{
    mLegIndices = legIndices;
    mStepTimeOffset = stepTimeOffset;
    mStepDuration = BASE_STEP_DURATION * (1 + (mLegIndices.size() - 1) * mStepTimeOffset);
};

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
    mBody.mLocalPosY = BODY_Y_OFFSET + BODY_START_Y;

    mBody.mStartPos = vec3(0, mBody.mLocalPosY, 0);

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

        leg->mFootStartAngle = toPositiveAngle(atan2(legPos.z, legPos.x));
        leg->mFootStartPos = vec3(cos(leg->mFootStartAngle) * FOOT_DIST, FOOT_Y, sin(leg->mFootStartAngle) * FOOT_DIST);
        leg->setFootTargetPos(leg->mFootStartPos);

        mLegs.push_back(leg);
    }

    mStepHeight = STEP_HEIGHT;

    mJoystickMovePos = ivec2(JOYSTICK_ZERO_POS);
    mJoystickRotatePos = ivec2(JOYSTICK_ZERO_POS);

    initGaits();

    ImGui::Initialize();
}

void Hexapod::initGaits()
{
    mGaits.resize(4);

    mGaits[GAITTYPE::TRIPOD].mGaitGroups = {{GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}),
                                             GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT})}};

    mGaits[GAITTYPE::TRIPLE].mGaitGroups = {{GaitGroup({LEG::FRONTLEFT, LEG::MIDRIGHT, LEG::BACKLEFT}, 0.5),
                                             GaitGroup({LEG::FRONTRIGHT, LEG::MIDLEFT, LEG::BACKRIGHT}, 0.5)}};

    mGaits[GAITTYPE::WAVE].mGaitGroups = {GaitGroup({LEG::FRONTRIGHT, LEG::MIDRIGHT, LEG::BACKRIGHT}, 0.6),
                                          GaitGroup({LEG::FRONTLEFT, LEG::MIDLEFT, LEG::BACKLEFT}, 0.6)};

    mGaits[GAITTYPE::RIPPLE].mGaitGroups = {GaitGroup({LEG::FRONTRIGHT, LEG::MIDRIGHT, LEG::BACKRIGHT}, 0.4),
                                            GaitGroup({LEG::BACKLEFT, LEG::MIDLEFT, LEG::FRONTLEFT}, 0.4)};

    mCurrGaitGrpSize = mGaits[mGaitType].mGaitGroups.size();
    mCurrGait = &mGaits[mGaitType];
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
    ImGui::Combo("Gait", &mComboGaitType, GAITTYPE_STRINGS);
    ImGui::NextColumn();
    ImGui::Text(MOVE_STATES_STRING[mMoveState]);
    ImGui::NextColumn();

    ImGui::NextColumn();
    ImGui::Text("Walk Directions");

    ImGui::NextColumn();
    if (ImGui::Button("Reset"))
    {
        resetBodyPos();
        resetFeetPos();
    }

    ImGui::NextColumn();

    //Walk buttons
    ImGui::InputFloat("Step Dist: ", &mStepDistMulti);
    ImGui::NextColumn();

    drawMoveJoystick();

    ImGui::NextColumn();

    ImGui::Separator();

    ImGui::Text("Rotate Directions");
    ImGui::NextColumn();
    if (ImGui::Button("Reset"))
    {
        resetBodyPos();
        resetFeetPos();
    }
    ImGui::NextColumn();
    float faceDir = toDegrees(mFaceDir);
    ImGui::InputFloat("Face Dir", &faceDir);
    ImGui::NextColumn();

    drawRotateJoystick();

    ImGui::End();
}

void Hexapod::drawMoveJoystick()
{
    if (ImGui::Button("Left Forward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(45);

    ImGui::NextColumn();
    if (ImGui::Button("Forward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(90);

    ImGui::NextColumn();
    if (ImGui::Button("Right Forward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(135);

    ImGui::NextColumn();
    if (ImGui::Button("Left", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(0);

    ImGui::NextColumn();
    if (ImGui::Button("Stop", BTN_SIZE))
        mJoystickMovePos = ivec2(JOYSTICK_ZERO_POS);

    ImGui::NextColumn();
    if (ImGui::Button("Right", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(180);

    ImGui::NextColumn();
    if (ImGui::Button("Left Backward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(315);

    ImGui::NextColumn();
    if (ImGui::Button("Backward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(270);

    ImGui::NextColumn();
    if (ImGui::Button("Right Backward", BTN_SIZE))
        mJoystickMovePos = degreesToJoyStickPos(225);
}

void Hexapod::drawRotateJoystick()
{
    if (ImGui::Button("45", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(45);

    ImGui::NextColumn();
    if (ImGui::Button("90", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(90);

    ImGui::NextColumn();
    if (ImGui::Button("135", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(135);

    ImGui::NextColumn();
    if (ImGui::Button("0", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(0);

    ImGui::NextColumn();
    if (ImGui::Button("RStop", BTN_SIZE))
        mJoystickRotatePos = ivec2(JOYSTICK_ZERO_POS);

    ImGui::NextColumn();
    if (ImGui::Button("180", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(180);

    ImGui::NextColumn();
    if (ImGui::Button("315", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(315);

    ImGui::NextColumn();
    if (ImGui::Button("270", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(270);

    ImGui::NextColumn();
    if (ImGui::Button("225", BTN_SIZE))
        mJoystickRotatePos = degreesToJoyStickPos(225);
}

void Hexapod::resetFeetPos()
{
    for (int i = 0; i < LEG_COUNT; i++)
        mLegs[i]->resetFootTargetPos();
}

void Hexapod::resetBodyPos()
{
    mBody.mLocalPosX = mBody.mStartPos.x;
    mBody.mLocalPosY = mBody.mStartPos.y;
    mBody.mLocalPosZ = mBody.mStartPos.z;

    mBodyStepStartPos = vec3(0);
}

void Hexapod::resetBodyRot()
{
    mBody.mRoll = 0;
    mBody.mYaw = 0;
    mBody.mPitch = 0;
    mFaceDir = FORWARD;
}

void Hexapod::update()
{
    drawGUI();

    checkJoystickPos();

    if (mMoveState != MOVESTATE::STOPPED)
        walk();
    else if (mGaitType != mComboGaitType)
    {
        mGaitType = (GAITTYPE)mComboGaitType;
        mCurrGait = &mGaits[mGaitType];
        mCurrGaitGrpSize = mCurrGait->mGaitGroups.size();
    }

    //Update the legs transformation visually
    for (Leg3D *leg : mLegs)
        leg->update();

    //Update the body transformations visually
    mBody.update();
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

void Hexapod::checkJoystickPos()
{
    if (mJoystickRotatePos != ivec2(JOYSTICK_ZERO_POS))
    {
        vec2 rot = normalizeJoystickPos(mJoystickRotatePos);
        float rotDir = toPositiveAngle(atan2(rot.y, rot.x));

        if (!compareFloats(rotDir, mFaceDir))
        {
            mTargetFaceDir = rotDir;
            mFaceDir = toPositiveAngle(mFaceDir);

            if (mMoveState == MOVESTATE::STOPPED)
            {
                initStep();
                setNextStep();
                setNextStepRot();
            }
        }
        else if (mMoveState == MOVESTATE::MOVING && mStepDistMulti == 0)
            mMoveState = MOVESTATE::STOPSTARTED;
    }
    else if (mMoveState == MOVESTATE::MOVING)
    {
        if (mStepDistMulti == 0)
            mMoveState = MOVESTATE::STOPSTARTED;
        else
            mTargetFaceDir = mFaceDir + mStepRotAngle;
    }

    if (mJoystickMovePos == ivec2(JOYSTICK_ZERO_POS))
    {
        if (mMoveState == MOVESTATE::MOVING && mStepDistMulti > 0)
        {
            if (compareFloats(mTargetFaceDir, mFaceDir))
                mMoveState = MOVESTATE::STOPSTARTED;
            else
                mStepDistMulti = 0;
        }
    }
    else
    {
        vec2 pos = normalizeJoystickPos(mJoystickMovePos);
        float moveDir = atan2(pos.y, pos.x);

        if (mStepDistMulti <= 0)
        {
            mMoveDir = moveDir;

            mCosMoveDir = cos(mMoveDir);
            mSinMoveDir = sin(mMoveDir);

            mStepDistMulti = sqrt(dot(pos, pos));

            if (compareFloats(mTargetFaceDir, mFaceDir))
            {
                mBodyDistMulti = mStepDistMulti;
                initStep();
                setNextStep();
            }
        }
        else if (!compareFloats(mMoveDir, moveDir))
        {
            mMoveDir = moveDir;
            mCosMoveDir = cos(mMoveDir);
            mSinMoveDir = sin(mMoveDir);

            setNextStep();
            setNextStepRot();
        }
    }
}

void Hexapod::initStep()
{
    mGaitGrpIdx = mCurrGaitGrpSize - 1;
    mMoveState = MOVESTATE::MOVING;
    mStepHeight = STEP_HEIGHT;
}

void Hexapod::setNextStepRot()
{
    if (compareFloats(mFaceDir, mTargetFaceDir))
        return;

    //Store the values after being rotated
    mFaceDir = clampAngleTo360(toPositiveAngle(FORWARD - mBody.mYaw));

    float diffAngle = getSmallestAngle(mTargetFaceDir - mFaceDir);
    mStepRotAngle = fabs(diffAngle) > MAXRAD_PERSTEP ? copysign(MAXRAD_PERSTEP, diffAngle) : diffAngle;
    mBodyStepStartYaw = toPositiveAngle(mBody.mYaw);
    mStepHeight = mStepDistMulti > 0 ? STEP_HEIGHT : fabs(mStepRotAngle) * STEP_HEIGHT / MAXRAD_PERSTEP;
    if (mStepHeight < MIN_STEP_HEIGHT)
        mStepHeight = MIN_STEP_HEIGHT;
}

void Hexapod::setNextStep()
{
    //Go to the next leg sequence group
    mGaitGrpIdx++;
    if (mGaitGrpIdx >= mCurrGaitGrpSize)
        mGaitGrpIdx = 0;

    mStepStartTime = getCurrTime();
    GaitGroup *currGroup = &mCurrGait->mGaitGroups[mGaitGrpIdx];
    mLegIndices = currGroup->mLegIndices;
    mStepTimeOffset = currGroup->mStepTimeOffset;
    mStepDuration = currGroup->mStepDuration;

    vec3 rootPos = mBody.getLocalPos();

    float baseStepDist = STEP_DIST * mStepDistMulti;

    for (int i = 0; i < mLegIndices.size(); i++)
    {
        int legIdx = mLegIndices[i];
        Leg3D *currLeg = mLegs[legIdx];

        //Store the current position of the leg as the step starting position
        mLegStepStartPos[legIdx] = currLeg->getFootWorldPos();
        //Store the current leg height to offset the step lifting position
        mPrevStepHeight[legIdx] = mLegStepStartPos[legIdx].y;
        //Set the step start Y position to 0 to prevent the step Y position from getting skewed
        mLegStepStartPos[legIdx].y = 0;

        vec3 bodyPosXZ = vec3(rootPos.x, 0, rootPos.z);
        //Calculate the step distance
        vec3 footStartPos = rotateAroundY(currLeg->mFootStartPos, -mBody.mYaw) + bodyPosXZ;
        vec3 footDiff = mLegStepStartPos[legIdx] - footStartPos;

        //Include the foot differences if it is still moving
        if (mMoveState == MOVESTATE::MOVING)
            mStepDist[legIdx] = vec3(mCosMoveDir * baseStepDist, 0, mSinMoveDir * baseStepDist) - footDiff;
        else
            mStepDist[legIdx] = -footDiff;
    }

    //Update the body position vector to match the body matrix
    mBodyStepStartPos.x = rootPos.x;
    mBodyStepStartPos.z = rootPos.z;
    mBodyDistMulti = mStepDistMulti;

    if (mMoveState == MOVESTATE::STOPSTARTED)
        mMoveState = MOVESTATE::STOPPING;
}

void Hexapod::walk()
{
    float stepNormTimeLapsed = normalizeTimelapsed(mStepStartTime, mStepDuration);

    if (stepNormTimeLapsed >= 1)
    {
        if (mGroupStoppedCount < mCurrGaitGrpSize - 1)
        {
            setNextStep();
            setNextStepRot();
            stepNormTimeLapsed = 0;

            if (mMoveState == MOVESTATE::STOPPING)
                mGroupStoppedCount++;
        }
        else //Stops the walk cycle
        {
            mMoveState = MOVESTATE::STOPPED;
            mStepDistMulti = 0;
            mBodyDistMulti = 0;
            mStepRotAngle = 0;
            mTargetFaceDir = mFaceDir;
            mGroupStoppedCount = 0;

            return;
        }
    }

    if (stepNormTimeLapsed >= 0)
    {
        //Set the new foot target position
        for (int i = 0; i < mLegIndices.size(); i++)
        {
            float legNormTimeLapsed = normalizeTimelapsed(mStepStartTime + BASE_STEP_DURATION * mStepTimeOffset * i, BASE_STEP_DURATION);

            if (legNormTimeLapsed < 0 || legNormTimeLapsed > 1)
                continue;

            int legIdx = mLegIndices[i];

            float footYOffset = sin(M_PI * legNormTimeLapsed) * mStepHeight + mPrevStepHeight[legIdx] * (1 - legNormTimeLapsed);
            float footXOffset = mStepDist[legIdx].x * legNormTimeLapsed;
            float footZOffset = mStepDist[legIdx].z * legNormTimeLapsed;

            vec3 offset = vec3(footXOffset, footYOffset, footZOffset);

            vec3 rootPos = vec3(mBody.mLocalPosX, 0, mBody.mLocalPosZ);
            vec3 stepStartPos = rotateAroundY(mLegStepStartPos[legIdx] - rootPos, mStepRotAngle * legNormTimeLapsed) + rootPos;
            vec3 newPos = stepStartPos + offset;

            mLegs[legIdx]->setFootTargetPos(newPos);
        }

        //Move the body forward
        if (mMoveState != MOVESTATE::STOPPING)
        {
            float baseBodyOffset = STEP_DIST * stepNormTimeLapsed * mBodyDistMulti;
            mBody.mLocalPosX = mBodyStepStartPos.x + mCosMoveDir * baseBodyOffset;
            mBody.mLocalPosZ = mBodyStepStartPos.z + mSinMoveDir * baseBodyOffset;

            mBody.mYaw = mBodyStepStartYaw - mStepRotAngle * stepNormTimeLapsed;
        }
    }
}

vec3 Hexapod::getPos()
{
    return mBody.mLocalPos;
}