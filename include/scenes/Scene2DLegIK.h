#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "../tools/Tools.h"
#include "../classes/Leg2D.h"
#include "../scenes/Scene.h"

using namespace ci;
using namespace ci::app;

class Scene2DLegIK : public Scene
{
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void mouseDrag(MouseEvent event) override;
    void mouseUp(MouseEvent event) override;    
    void update() override;
    void draw() override;

private:
    Leg2D mLeg;
    Color mBgColor;

    float mFloorHeight;
    float mFloorY;

    float mHipLength;
    float mFemurLength;
    float mTibiaLength;

    vec2 mLegPos;
    float mLegX;
    float mLegY;

    vec2 mFeetPos;
    float mFeetX;
    float mFeetY;

    gl::TextureRef mTextTexture;    

    vec2 processMousePos(vec2 mousePos);
    void drawGUI();
    void drawTexts();
};