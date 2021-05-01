#pragma once
#include "cinder/app/App.h"
#include "cinder/CameraUi.h"
#include "../tools/Tools.h"
#include "Leg3D.h"
#include "Scene.h"

using namespace ci;
using namespace ci::app;

class Scene3DLegIK : public Scene
{
public:
    void drawGUI();
    void calculateIK();
    void setup() override;
    void update() override;
    void draw() override;
    void toggleCamUi(bool enable);

private:
    Leg3D mLeg;

    CameraPersp mCamera;
    CameraUi mCameraUi;

    gl::TextureRef mTextTexture;
    gl::BatchRef mWirePlane;

    mat4 mWorldAxisMatrix;
    vec2 mWorldAxisViewportPos;
    vec2 mWorldAxisViewportSize;

    float mHipAngle;
    float mFemurAngle;
    float mTibiaAngle;

    float mFootX;
    float mFootY;
    float mFootZ;

    float mHipX;
    float mHipY;
    float mHipZ;

    void drawTexts();
};