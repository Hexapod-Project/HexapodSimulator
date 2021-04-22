#include "HexapodSimulator.h"
#include "cinder/CinderImGui.h"

bool test = false;
double testDbl = 0.0;

void prepareSettings(App::Settings *settings)
{
    settings->setWindowSize(1024, 768);
}

void HexapodSimulator::setup()
{
    mScene2DLegIK.setup();    
}

void HexapodSimulator::mouseDown(MouseEvent event)
{
    mScene2DLegIK.mouseDown(event);
}

void HexapodSimulator::mouseDrag(MouseEvent event)
{
    mScene2DLegIK.mouseDrag(event);
}

void HexapodSimulator::mouseUp(MouseEvent event)
{
    mScene2DLegIK.mouseUp(event);
}

void HexapodSimulator::update()
{
    mScene2DLegIK.update();
}

void HexapodSimulator::draw()
{
    mScene2DLegIK.draw();
}

CINDER_APP(HexapodSimulator, RendererGl, prepareSettings)