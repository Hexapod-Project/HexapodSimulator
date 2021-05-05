#pragma once
#include "cinder/app/App.h"

using namespace ci;
using namespace ci::app;

class Scene
{
public:    
    virtual void setup(){};
    virtual void update(){};
    virtual void draw(){};
    virtual void drawTexts(){};
    virtual void mouseDown(MouseEvent event){};
    virtual void mouseDrag(MouseEvent event){};
    virtual void mouseUp(MouseEvent event){};
    virtual void toggleCamUi(bool enable){};

    bool mIsDrawCoord = false;
};