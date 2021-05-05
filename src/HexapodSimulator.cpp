#include "HexapodSimulator.h"
#include "cinder/CinderImGui.h"

//This index change which scenes is shown first on start
//*Can be used to set to the scene index that is currently being developed
const int START_SCENE_IDX = 2;

void prepareSettings(App::Settings *settings)
{
    settings->setWindowSize(1024, 768);
}

void HexapodSimulator::setup()
{
    ImGui::Initialize();

    mSceneIdx = START_SCENE_IDX;
    mComboSceneIdx = START_SCENE_IDX;
    mCurrScene = mScenes[START_SCENE_IDX];

    mCurrScene->setup();
}

void HexapodSimulator::mouseDown(MouseEvent event)
{
    mCurrScene->mouseDown(event);
}

void HexapodSimulator::mouseDrag(MouseEvent event)
{
    mCurrScene->mouseDrag(event);
}

void HexapodSimulator::mouseUp(MouseEvent event)
{
    mCurrScene->mouseUp(event);
}

void HexapodSimulator::update()
{
    drawGUI();

    checkChangeScene();

    mCurrScene->update();
}

void HexapodSimulator::checkChangeScene()
{
    if (mComboSceneIdx != mSceneIdx)
    {
        mSceneIdx = mComboSceneIdx;

        //Turns off the camera ui before switching scene so that 2D scenes mouse cursor can be used
        mCurrScene->toggleCamUi(false);
        mCurrScene = mScenes[mSceneIdx];
        //Turns on the camera ui of the new scene if it has a camera ui
        mCurrScene->toggleCamUi(true);

        mCurrScene->setup();
    }
}

void HexapodSimulator::drawGUI()
{
    ImGui::Begin("Hexapod Simulation");
    ImGui::Combo("Scene", &mComboSceneIdx, mSceneNames);
    ImGui::Checkbox("Draw Coord Frame", &mCurrScene->mIsDrawCoord);
    ImGui::End();
}

void HexapodSimulator::draw()
{
    mCurrScene->draw();
}

CINDER_APP(HexapodSimulator, RendererGl, prepareSettings)