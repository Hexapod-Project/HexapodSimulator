#include "HexapodSimulator.h"
#include "cinder/CinderImGui.h"

const int START_SCENE_IDX = 0;

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

    updateScene();

    mCurrScene->update();
}

void HexapodSimulator::updateScene()
{
    if (mComboSceneIdx != mSceneIdx)
    {
        mSceneIdx = mComboSceneIdx;

        switch (mSceneIdx)
        {
        default:
        case 0:
            mScene3DLegIK.toggleCamUi(false);
            break;
        case 1:
            mScene3DLegIK.toggleCamUi(true);
            break;
        case 2:
            mScene3DLegIK.toggleCamUi(false);
            break;
        }

        mCurrScene = mScenes[mSceneIdx];

        mCurrScene->setup();
    }
}

void HexapodSimulator::drawGUI()
{
    ImGui::Begin("Hexapod Simulation");
    ImGui::Combo("Scene", &mComboSceneIdx, mSceneNames);
    ImGui::End();
}

void HexapodSimulator::draw()
{
    mCurrScene->draw();
}

CINDER_APP(HexapodSimulator, RendererGl, prepareSettings)