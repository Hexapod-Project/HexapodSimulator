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
    mSceneTwoDLegIK.setup();    
}

void HexapodSimulator::mouseDown(MouseEvent event)
{
    mSceneTwoDLegIK.mouseDown(event);
}

void HexapodSimulator::mouseDrag(MouseEvent event)
{
    mSceneTwoDLegIK.mouseDrag(event);
}

void HexapodSimulator::mouseUp(MouseEvent event)
{
    mSceneTwoDLegIK.mouseUp(event);
}

void HexapodSimulator::update()
{
    mSceneTwoDLegIK.update();
}

void HexapodSimulator::draw()
{
    mSceneTwoDLegIK.draw();
}

CINDER_APP(HexapodSimulator, RendererGl, prepareSettings)