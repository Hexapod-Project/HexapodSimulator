#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "scenes/Scene2DLegIK.h"
#include "scenes/Scene3DLegIK.h"
#include "scenes/SceneHexapod.h"
#include "scenes/Scene.h"

using namespace ci;
using namespace ci::app;

class HexapodSimulator:public App {
public:    
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void mouseDrag(MouseEvent event) override;
    void mouseUp(MouseEvent event) override;
    void update() override;
    void draw() override;
private:
    int mSceneIdx;
    int mComboSceneIdx;
    Scene *mCurrScene;
    Scene2DLegIK mScene2DLegIK;
    Scene3DLegIK mScene3DLegIK;
    SceneHexapod mSceneHexapod;

    const std::vector<std::string> mSceneNames = {"2D Leg IK", "3D Leg IK", "Hexapod"};
    const std::vector<Scene*> mScenes = {&mScene2DLegIK, &mScene3DLegIK, &mSceneHexapod};

    void checkChangeScene();
    void drawGUI();
};