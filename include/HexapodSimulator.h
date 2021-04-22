#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "scenes/Scene2DLegIK.h"

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
    Scene2DLegIK mScene2DLegIK;
};