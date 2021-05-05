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
const float FOOT_DIST_FROM_HIP = HIP_LENGTH + FEMUR_LENGTH;

void Hexapod::setup()
{
    //Leg offsets from the body's origin
    vec3 legOffsets[LEG_COUNT];
    float xLegFrontBackOffset = BODY_FRONTBACK_WIDTH / 2 + 0.5;
    float xLegMidOffset = BODY_WIDTH / 2 + 0.5;
    float yLegOffset = BODY_THICKNESS / 2;
    float zLegFrontBackOffset = BODY_LENGTH / 2;
    legOffsets[LEG::FRONTLEFT] = vec3(xLegFrontBackOffset, yLegOffset, zLegFrontBackOffset);
    legOffsets[LEG::FRONTRIGHT] = vec3(-xLegFrontBackOffset, yLegOffset, zLegFrontBackOffset);
    legOffsets[LEG::MIDLEFT] = vec3(xLegMidOffset, yLegOffset, 0);
    legOffsets[LEG::MIDRIGHT] = vec3(-xLegMidOffset, yLegOffset, 0);
    legOffsets[LEG::BACKLEFT] = vec3(xLegFrontBackOffset, yLegOffset, -zLegFrontBackOffset);
    legOffsets[LEG::BACKRIGHT] = vec3(-xLegFrontBackOffset, yLegOffset, -zLegFrontBackOffset);

    vec3 footOffsets[LEG_COUNT];    
    float xFootFrontBackOffSet = cos(M_PI/4) * FOOT_DIST_FROM_HIP;
    float xFootMidOffset = FOOT_DIST_FROM_HIP;
    float yFootOffset = -yLegOffset;
    float zFootFrontBackOffset = sin(M_PI/4) * FOOT_DIST_FROM_HIP;
    footOffsets[LEG::FRONTLEFT] = vec3(xFootFrontBackOffSet, yFootOffset, zFootFrontBackOffset);
    footOffsets[LEG::FRONTRIGHT] = vec3(-xFootFrontBackOffSet, yFootOffset, zFootFrontBackOffset);
    footOffsets[LEG::MIDLEFT] = vec3(xFootMidOffset, yFootOffset, 0);
    footOffsets[LEG::MIDRIGHT] = vec3(-xFootMidOffset, yFootOffset, 0);
    footOffsets[LEG::BACKLEFT] = vec3(xFootFrontBackOffSet, yFootOffset, -zFootFrontBackOffset);
    footOffsets[LEG::BACKRIGHT] = vec3(-xFootFrontBackOffSet, yFootOffset, -zFootFrontBackOffset);

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
        leg->setFootTargetPos(legPos + footOffsets[i]);

        mLegs.push_back(leg);
    }

    ImGui::Initialize();
}

void Hexapod::drawGUI()
{
    ImGui::Begin("Hexapod Properties");
    ImGui::Text("Rotations");
    ImGui::DragFloat("Roll", &mBody.mRoll, 1, -180, 180);
    ImGui::DragFloat("Yaw", &mBody.mYaw, 1, -180, 180);
    ImGui::DragFloat("Pitch", &mBody.mPitch, 1, -180, 180);
    ImGui::End();
}

void Hexapod::update()
{
    mBody.update();

    for (Leg3D *leg : mLegs)
        leg->update();

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