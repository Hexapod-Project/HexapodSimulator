#include "SceneHexapod.h"
#include "Tools.h"

void SceneHexapod::setup()
{
    mCam.lookAt(vec3(0, 15, 20), vec3(0));
    mCam.setPerspective(40.0, getWindowAspectRatio(), 0.01, 100.0);
    mCamUi = CameraUi(&mCam, getWindow());

    mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(1000)).subdivisions(vec2(1000)), gl::getStockShader(gl::ShaderDef().color()));
    mFloor = gl::Batch::create(geom::Plane().size(vec2(1000)).subdivisions(vec2(1000)), gl::getStockShader(gl::ShaderDef().color()));

    mWorldAxisMatrix = mat4(1.0f);
    mWorldAxisViewportSize = vec2(getWindowWidth() / 5, getWindowHeight() / 5);
    mWorldAxisViewportPos = vec2(getWindowWidth() - mWorldAxisViewportSize.x, 0);

    mHexapod.setup();

    drawTexts();
}

void SceneHexapod::update()
{
    mWorldAxisMatrix = translate(mCam.getEyePoint()) * toMat4(mCam.getOrientation()) * translate(vec3(0, 0, -5)) / toMat4(mCam.getOrientation());

    mHexapod.update();
}

void SceneHexapod::draw()
{
    gl::clear(Color(Color::gray(0.3f)));
    {        
        gl::ScopedViewport viewport(0, 0, getWindowWidth(), getWindowHeight());

        gl::draw(mTextTexture);

        gl::ScopedMatrices matrices(mCam);

        gl::ScopedDepth depth(true);

        mWirePlane->draw();
        mHexapod.draw();

        gl::ScopedColor color(44/255.0, 56/255.0, 102/255.0);
        gl::ScopedModelMatrix matrix(translate(vec3(0, -0.1, 0)));
        mFloor->draw();

        if(mIsDrawCoord)
            mHexapod.drawCoord();
    }

    {
        //Camera matrices are defined twice because of the text texture above which has to be drawn before the
        //camera is declared and has to be within the viewport above
        gl::ScopedMatrices matrices(mCam);

        gl::ScopedViewport viewport(mWorldAxisViewportPos.x, mWorldAxisViewportPos.y, mWorldAxisViewportSize.x, mWorldAxisViewportSize.y);
        gl::ScopedModelMatrix matrix(mWorldAxisMatrix);
        gl::drawCoordinateFrame(1.5, 0.4, 0.1);
    }
}

void SceneHexapod::drawTexts()
{
    mTextTexture = createTextTexture("Hexapod", vec2(getWindowWidth() / 3, 150), Color(1, 1, 1), ColorA(0, 0, 0, 0));
}

void SceneHexapod::toggleCamUi(bool enable)
{
    if (!enable && mCamUi.isEnabled())
        mCamUi.disable();
    else if (enable && !mCamUi.isEnabled())
        //Somehow using .enable() function does not work so just reinitialize
        mCamUi = CameraUi(&mCam, getWindow());
}