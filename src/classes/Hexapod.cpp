#include "Hexapod.h"

const float HIP_LENGTH = 1.0;
const float FEMUR_LENGTH = 2.0;
const float TIBIA_LENGTH = 2.5;
const float BODY_THICKNESS = 0.5;
const float BODY_WIDTH = 3;
const float BODY_LENGTH = 4;
//The width of the head and back
/**
 * This is just used to determine the leg positions
 * Since it is an elongated hexagon, the width of is the length of the hexagon's sides with radius of BODY_WIDTH
*/
const float BODY_HALFWIDTH = BODY_WIDTH / 2;
const float BODY_HALFWIDTH_SQR = BODY_HALFWIDTH * BODY_HALFWIDTH;
const float BODY_FRONTBACK_WIDTH = sqrt(2 * BODY_HALFWIDTH_SQR - cos(M_PI / 3) * 2 * BODY_HALFWIDTH_SQR);
const int LEG_COUNT = 6;

//Leg Front Back X Offset
const float X_FB_LEG_OFFSET = BODY_FRONTBACK_WIDTH / 2 + 0.5;
const float X_MID_LEG_OFFSET = BODY_WIDTH / 2 + 0.5;
float Y_LEG_OFFSET = BODY_THICKNESS / 2;
//Leg Front Back Z offset
float Z_FB_LEG_OFFSET = BODY_LENGTH / 2;

//The distance of the foot is the radius of a circle and the body's center is the origin
float FOOT_DIST = BODY_WIDTH + 3;
float FOOT_Y = 0;

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

    //Create the 6 legs
    for (int i = 0; i < LEG_COUNT; i++)
    {
        //The right legs' positions are located in the evevn indices of the legOffsets array
        bool isRightLeg = i % 2;

        vec3 legPos = legOffsets[i];
        Leg3D *leg = new Leg3D(legOffsets[i], HIP_LENGTH, FEMUR_LENGTH, TIBIA_LENGTH, isRightLeg);
        leg->setParent(&mBody);
        leg->setup();

        float legAngle = atan2(legPos.z, legPos.x);
        vec3 footPos = vec3(FOOT_DIST * cos(legAngle), FOOT_Y, FOOT_DIST * sin(legAngle));
        leg->setFootTargetPos(footPos);

        mFootStartPos[i] = footPos;

        mFootPosX[i] = footPos.x;
        mFootPosY[i] = footPos.y;
        mFootPosZ[i] = footPos.z;

        mFootTargetPos[i] = footPos;

        mLegs.push_back(leg);
    }

    mGaitManager.setFeetBasePos(std::vector<vec3>(mFootStartPos, mFootStartPos + 6));
    mGaitManager.setBodyBasePos(vec3(mBody.mPosX, mBody.mPosY, mBody.mPosZ));

    ImGui::Initialize();
}

void Hexapod::drawGUI()
{
    ImGui::Begin("Hexapod Properties");
    ImGui::Text("Body Positions");
    ImGui::Columns(3);
    ImGui::DragFloat("X", &mBody.mPosX, 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("Y", &mBody.mPosY, 0.1f, 0, FEMUR_LENGTH + TIBIA_LENGTH);
    ImGui::NextColumn();
    ImGui::DragFloat("Z", &mBody.mPosZ, 0.1f, -FLT_MAX, FLT_MAX);

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
    ImGui::DragFloat("FL X", &mFootPosX[LEG::FRONTLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR X", &mFootPosX[LEG::FRONTRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML X", &mFootPosX[LEG::MIDLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR X", &mFootPosX[LEG::MIDRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL X", &mFootPosX[LEG::BACKLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR X", &mFootPosX[LEG::BACKRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Y", &mFootPosY[LEG::FRONTLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("FR Y", &mFootPosY[LEG::FRONTRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("ML Y", &mFootPosY[LEG::MIDLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("MR Y", &mFootPosY[LEG::MIDRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BL Y", &mFootPosY[LEG::BACKLEFT], 0.1f, 0, FLT_MAX);
    ImGui::DragFloat("BR Y", &mFootPosY[LEG::BACKRIGHT], 0.1f, 0, FLT_MAX);
    ImGui::NextColumn();
    ImGui::DragFloat("FL Z", &mFootPosZ[LEG::FRONTLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("FR Z", &mFootPosZ[LEG::FRONTRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("ML Z", &mFootPosZ[LEG::MIDLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("MR Z", &mFootPosZ[LEG::MIDRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BL Z", &mFootPosZ[LEG::BACKLEFT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::DragFloat("BR Z", &mFootPosZ[LEG::BACKRIGHT], 0.1f, -FLT_MAX, FLT_MAX);
    ImGui::NextColumn();
    if (ImGui::Button("Reset FL"))
    {
        mFootPosX[LEG::FRONTLEFT] = mFootStartPos[LEG::FRONTLEFT].x;
        mFootPosY[LEG::FRONTLEFT] = mFootStartPos[LEG::FRONTLEFT].y;
        mFootPosZ[LEG::FRONTLEFT] = mFootStartPos[LEG::FRONTLEFT].z;
    }

    if (ImGui::Button("Reset FR"))
    {
        mFootPosX[LEG::FRONTRIGHT] = mFootStartPos[LEG::FRONTRIGHT].x;
        mFootPosY[LEG::FRONTRIGHT] = mFootStartPos[LEG::FRONTRIGHT].y;
        mFootPosZ[LEG::FRONTRIGHT] = mFootStartPos[LEG::FRONTRIGHT].z;
    }

    if (ImGui::Button("Reset ML"))
    {
        mFootPosX[LEG::MIDLEFT] = mFootStartPos[LEG::MIDLEFT].x;
        mFootPosY[LEG::MIDLEFT] = mFootStartPos[LEG::MIDLEFT].y;
        mFootPosZ[LEG::MIDLEFT] = mFootStartPos[LEG::MIDLEFT].z;
    }

    if (ImGui::Button("Reset MR"))
    {
        mFootPosX[LEG::MIDRIGHT] = mFootStartPos[LEG::MIDRIGHT].x;
        mFootPosY[LEG::MIDRIGHT] = mFootStartPos[LEG::MIDRIGHT].y;
        mFootPosZ[LEG::MIDRIGHT] = mFootStartPos[LEG::MIDRIGHT].z;
    }

    if (ImGui::Button("Reset BL"))
    {
        mFootPosX[LEG::BACKLEFT] = mFootStartPos[LEG::BACKLEFT].x;
        mFootPosY[LEG::BACKLEFT] = mFootStartPos[LEG::BACKLEFT].y;
        mFootPosZ[LEG::BACKLEFT] = mFootStartPos[LEG::BACKLEFT].z;
    }

    if (ImGui::Button("Reset BR"))
    {
        mFootPosX[LEG::BACKRIGHT] = mFootStartPos[LEG::BACKRIGHT].x;
        mFootPosY[LEG::BACKRIGHT] = mFootStartPos[LEG::BACKRIGHT].y;
        mFootPosZ[LEG::BACKRIGHT] = mFootStartPos[LEG::BACKRIGHT].z;
    }

    ImGui::Columns(1);

    if (ImGui::Button("Reset Feet"))
        resetFeetPos();

    ImGui::Separator();
    ImGui::Text("Walk Cycle");

    ImGui::Columns(2);
    ImGui::Combo("Gait Type", &mComboGaitType, std::vector<std::string>{"Tripod", "Ripple", "Triple"});

    ImGui::NextColumn();
    if (ImGui::Button(mGaitManager.isWalking() ? "Stop" : "Play"))
    {
        if (mGaitManager.isWalking())
        {
            mGaitManager.stopGait();
            resetFeetPos();
        }
        else
        {
            mCurrGaitType = (GAITTYPE)mComboGaitType;
            mGaitManager.startGait(mCurrGaitType);
        }
    }

    ImGui::End();
}

void Hexapod::resetFeetPos()
{
    for (int i = 0; i < LEG_COUNT; i++)
    {
        mFootPosX[i] = mFootStartPos[i].x;
        mFootPosY[i] = mFootStartPos[i].y;
        mFootPosZ[i] = mFootStartPos[i].z;
    }
}

void Hexapod::resetBodyPos()
{
    mBody.mPosX = 0;
    mBody.mPosY = 1;
    mBody.mPosZ = 0;
}

void Hexapod::update()
{
    bool isWalking = mGaitManager.isWalking();
    if (mGaitManager.isWalking())
    {
        mGaitManager.runGait(mFootTargetPos);
        
        if (mCurrGaitType != mComboGaitType)
        {         
            mGaitManager.stopGait();
            resetFeetPos();
            return;
        }        
    }

    mBody.update();

    for (int i = 0; i < mLegs.size(); i++)
    {
        Leg3D *leg = mLegs[i];

        if (isWalking)
        {
            vec3 footTargetPos = mFootTargetPos[i];
            mFootPosX[i] = footTargetPos.x;
            mFootPosY[i] = footTargetPos.y;
            mFootPosZ[i] = footTargetPos.z;
        }
        else
            mFootTargetPos[i] = vec3(mFootPosX[i], mFootPosY[i], mFootPosZ[i]);

        vec3 footTargetPos = mFootTargetPos[i];
        leg->setFootTargetPos(footTargetPos);
        leg->update();
    }

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