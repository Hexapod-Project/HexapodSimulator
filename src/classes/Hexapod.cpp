#include "Hexapod.h"
#include "Tools.h"

const vec2 BTN_SIZE = vec2(100, 25);
const char *MOVE_STATES_STRING[] = {"MOVING", "STOPSTARTED", "STOPPING", "STOPPED"};

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

        leg->mFootStartAngle = atan2(legPos.z, legPos.x);
        leg->mFootStartPos = vec3(cos(leg->mFootStartAngle) * FOOT_DIST, FOOT_Y, sin(leg->mFootStartAngle) * FOOT_DIST);
        leg->setFootTargetPos(leg->mFootStartPos);

        mLegs.push_back(leg);
    }

    mStepHeight = STEP_HEIGHT;

    mJoystickMovePos = ivec2(JOYSTICK_ZERO_POS);
    mJoystickRotatePos = ivec2(JOYSTICK_ZERO_POS);

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

            if (!compareFloats(mTargetFaceDir, mFaceDir) && mMoveState != MOVESTATE::MOVING)
            {
                setNextStepRot();
                initStep();
            }
        }
        else if (mMoveState == MOVESTATE::MOVING && mStepDistMulti == 0)
            mMoveState = MOVESTATE::STOPSTARTED;
    }
    else if (mMoveState == MOVESTATE::MOVING && mStepDistMulti == 0)
        mMoveState = MOVESTATE::STOPSTARTED;

    if (mJoystickMovePos == ivec2(JOYSTICK_ZERO_POS))
    {
        if (mMoveState == MOVESTATE::MOVING && mStepDistMulti > 0)
            mMoveState = MOVESTATE::STOPSTARTED;
    }
    else
    {
        vec2 pos = normalizeJoystickPos(mJoystickMovePos);
        float moveDir = atan2(pos.y, pos.x);

        if (mStepDistMulti <= 0)
        {
            mLegSeqIdx = mLegSequences.size() - 1; //Set to mLegSequences.size() - 1 so that when calling setNextStep, it will set it to 0
            mMoveDir = moveDir;

            mCosMoveDir = cos(mMoveDir);
            mSinMoveDir = sin(mMoveDir);

            mPrevStepDist = vec2(0);
            mStepDistMulti = sqrt(dot(pos, pos));
            float baseStepDist = STEP_DIST * mStepDistMulti;
            mStepDist = vec2(mCosMoveDir * baseStepDist, mSinMoveDir * baseStepDist);

            initStep();
        }
        else if (!compareFloats(mMoveDir, moveDir))
        {
            mMoveDir = moveDir;
            mCosMoveDir = cos(mMoveDir);
            mSinMoveDir = sin(mMoveDir);
            setNextStep();
        }
    }
}

void Hexapod::initStep()
{
    mStepStartTime = getCurrTime(); //Set it to current time so that the time offset added in setNextStep will be rmeoved
    mStepDuration = BASE_STEP_DURATION;
    mMoveState = MOVESTATE::MOVING;
}

void Hexapod::setNextStepRot()
{
    float diffAngle = getSmallestAngle(mTargetFaceDir - mFaceDir);
    mStepRotAngle = fabs(diffAngle) > MAXRAD_PERSTEP ? copysign(MAXRAD_PERSTEP, diffAngle) : diffAngle;
    mBodyStepStartYaw = toPositiveAngle(mBody.mYaw);
}

void Hexapod::setNextStep()
{
    //Reset the step start time
    int currTime = getCurrTime();
    //Calculate the durations left over from the previous step that was not able to complete due to changing directions
    mStepStartTime = currTime;
    mStepDuration = BASE_STEP_DURATION - (currTime - mStepStartTime);

    //Go to the next leg sequence group
    mLegSeqIdx++;
    if (mLegSeqIdx >= mLegSequences.size())
        mLegSeqIdx = 0;

    vec3 rootPos = mBody.getLocalPos();

    if (mStepDistMulti > 0)
    {
        Leg3D *currLeg = mLegs[mLegSequences[mLegSeqIdx][0]];
        vec3 footWorldPos = currLeg->getFootWorldPos();
        mPrevStepDist = vec2(currLeg->mFootStartPos.x + rootPos.x - footWorldPos.x, currLeg->mFootStartPos.z + rootPos.z - footWorldPos.z);

        mPrevStepHeight = footWorldPos.y;
        float baseStepDist = STEP_DIST * mStepDistMulti;
        mStepDist = vec2(mCosMoveDir * baseStepDist, mSinMoveDir * baseStepDist);

        if (mMoveState != MOVESTATE::STOPSTARTED)
            mStepDist += mPrevStepDist;

        //Update the body position vector to match the body matrix
        mBodyStepStartPos.x = rootPos.x;
        mBodyStepStartPos.z = rootPos.z;
    }

    if (mMoveState == MOVESTATE::STOPSTARTED)
        mMoveState = MOVESTATE::STOPPING;

    if (!compareFloats(mFaceDir, mTargetFaceDir))
    {
        mFaceDir += mStepRotAngle;
        mBody.mYaw = mBodyStepStartYaw - mStepRotAngle;
        setNextStepRot();
    }
}

void Hexapod::walk()
{
    float normalizedTimelapsed = normalizeTimelapsed(mStepStartTime, mStepDuration);

    if (normalizedTimelapsed >= 1)
    {
        if (mMoveState != MOVESTATE::STOPPING)
        {
            setNextStep();
            normalizedTimelapsed = 0;
        }
        else //Stops the walk cycle
        {
            mMoveState = MOVESTATE::STOPPED;
            mStepDistMulti = 0;

            return;
        }
    }

    if (normalizedTimelapsed >= 0)
    {
        float footYOffset = sin(M_PI * normalizedTimelapsed) * STEP_HEIGHT + mPrevStepHeight * (1 - normalizedTimelapsed);
        float footXOffset = mStepDist.x * normalizedTimelapsed - mPrevStepDist.x + mBodyStepStartPos.x;
        float footZOffset = mStepDist.y * normalizedTimelapsed - mPrevStepDist.y + mBodyStepStartPos.z;

        vec3 offset = vec3(footXOffset, footYOffset, footZOffset);

        float legAngle = toPositiveAngle(-mBody.mYaw);
        float cosLegAngle = cos(legAngle);
        float sinLegAngle = sin(legAngle);
        //Set the new foot target position
        for (int i = 0; i < 3; i++)
        {
            int idx = mLegSequences[mLegSeqIdx][i];            
            vec3 newPos = mLegs[idx]->mFootStartPos + offset;

            newPos.x = cosLegAngle * newPos.x - sinLegAngle * newPos.z;
            newPos.z = cosLegAngle * newPos.z + sinLegAngle * newPos.x;

            mLegs[idx]->setFootTargetPos(newPos);
        }

        //Move the body forward
        if (mMoveState != MOVESTATE::STOPPING)
        {
            float baseBodyOffset = STEP_DIST * normalizedTimelapsed * mStepDistMulti;
            mBody.mLocalPosX = mBodyStepStartPos.x + mCosMoveDir * baseBodyOffset;
            mBody.mLocalPosZ = mBodyStepStartPos.z + mSinMoveDir * baseBodyOffset;

            mBody.mYaw = mBodyStepStartYaw - mStepRotAngle * normalizedTimelapsed;
        }
    }
}

void Hexapod::orientBody()
{
}

vec3 Hexapod::getPos()
{
    return mBody.mLocalPos;
}