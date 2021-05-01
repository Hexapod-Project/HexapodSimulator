#include "Scene2DLegIK.h"
#include "cinder/CinderImGui.h"
#include "cinder/CameraUi.h"

void Scene2DLegIK::setup()
{
    //And display info
    mBgColor = Color(0, 0, 0);
    mFloorHeight = 100.0f;
    mFloorY = getWindowHeight() - mFloorHeight;

    mHipLength = 100.0f;
    mFemurLength = 300.0f;
    mTibiaLength = 350.0f;

    mLegX = getWindowWidth() / 2 - 160.0f;
    mLegY = mFloorY - 100.0f;
    mLegPos = vec2(mLegX, mLegY);

    mLeg = Leg2D(mLegPos, mFloorY, mHipLength, mFemurLength, mTibiaLength);

    mFeetPos = mLeg.getFootPos();
    mFeetX = mFeetPos.x;
    mFeetY = mFeetPos.y;    

    drawTexts();
}

void Scene2DLegIK::mouseDown(MouseEvent event)
{
    mLeg.mouseDown(event.getPos());
}

void Scene2DLegIK::mouseDrag(MouseEvent event)
{
    mLeg.mouseDrag(processMousePos(event.getPos()));
}

void Scene2DLegIK::mouseUp(MouseEvent event)
{
    mLeg.mouseUp(processMousePos(event.getPos()));
}

vec2 Scene2DLegIK::processMousePos(vec2 mousePos)
{
    if (mousePos.y >= mFloorY)
        mousePos.y = mFloorY;

    return mousePos;
}

void Scene2DLegIK::drawGUI()
{
    ImGui::Begin("Properties");
    ImGui::InputFloat("Floor Height", &mFloorHeight, 1, 5, 0);

    ImGui::Separator();
    ImGui::Columns(2);
    ImGui::DragFloat("Leg X", &mLegX, 1.0f, 0, getWindowWidth(), "%.1f");
    ImGui::DragFloat("Foot X", &mFeetX, 1.0f, 0, mFloorY, "%.1f");
    ImGui::NextColumn();
    ImGui::DragFloat("Leg Y", &mLegY, 1.0f, 0, mFloorY, "%.1f");
    ImGui::DragFloat("Foot Y", &mFeetY, 1.0f, 0, mFloorY, "%.1f");

    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::InputFloat("Hip Length", &mHipLength, 1.0f, 1.0f, 1);
    ImGui::InputFloat("Femur Length", &mFemurLength, 1.0f, 1.0f, 1);
    ImGui::InputFloat("Tibia Length", &mTibiaLength, 1.0f, 1.0f, 1);

    ImGui::End();
}

void Scene2DLegIK::update()
{
    drawGUI();
    vec2 currLegPos = mLeg.getPos();

    //Move entire leg based on the floor height
    if (getWindowHeight() - mFloorHeight != mFloorY)
    {
        float prevFloorY = mFloorY;
        mFloorY = getWindowHeight() - mFloorHeight;
        mLeg.moveTo(currLegPos + vec2(0, mFloorY - prevFloorY));
    }

    //Update the leg position values if it were changed (eg. by mouse drag)
    vec2 newLegPos = vec2(mLegX, mLegY);
    if (currLegPos != mLegPos)
    {
        mLegPos = currLegPos;
        mLegX = mLegPos.x;
        mLegY = mLegPos.y;
    }
    //Move the leg based on the new position from the input
    else if (newLegPos != mLegPos)
        mLeg.moveWithIK(newLegPos);

    vec2 currFeetPos = mLeg.getFootPos();
    vec2 newFeetPos = vec2(mFeetX, mFeetY);
    //Update the feet position values if the values were change
    if (currFeetPos != mFeetPos)
    {
        mFeetPos = currFeetPos;
        mFeetX = mFeetPos.x;
        mFeetY = mFeetPos.y;
    }
    //Move the feet position based on the new feet position from the input
    else if (newFeetPos != mFeetPos)
        mLeg.moveFoot(newFeetPos);

    //Update leg lengths
    if (mHipLength != mLeg.mHipLength || mFemurLength != mLeg.mFemurLength || mTibiaLength != mLeg.mTibiaLength)
    {
        mLeg = Leg2D(mLegPos, mFloorY, mHipLength, mFemurLength, mTibiaLength);
        mLeg.moveFoot(mFeetPos);
    }
}

void Scene2DLegIK::draw()
{
    gl::ScopedMatrices matrix();

    gl::clear(mBgColor, true);

    gl::color(Color::white());
    gl::draw(mTextTexture);

    //Draw floor line
    gl::color(Color::white());
    gl::drawLine(vec2(0, mFloorY), vec2(getWindowWidth(), mFloorY));

    //Draw leg
    mLeg.draw();
}

void Scene2DLegIK::drawTexts()
{
    mTextTexture = createTextTexture("2D Leg Inverse Kinematics", vec2(getWindowWidth() / 3, 150), Color(1, 1, 1), ColorA(0, 0, 0, 0));
}