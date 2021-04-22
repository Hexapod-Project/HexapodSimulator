#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "../classes/Leg2D.h"

using namespace ci;
using namespace ci::app;

class Scene2DLegIK {
public:
    void setup();
    void mouseDown(MouseEvent event);
    void mouseDrag(MouseEvent event);
    void mouseUp(MouseEvent event); 
    void drawGUI();
    void drawTexts();
    void update();
    void draw();
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
};