#include "SceneHexapod.h"
#include "Tools.h"
#include "cinder/CinderImGui.h"

void SceneHexapod::setup()
{
    mCam.setPerspective(40.0, getWindowAspectRatio(), 0.01, 200.0);
    mCamUi = CameraUi(&mCam, getWindow());

    mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(2000)).subdivisions(vec2(2000)), gl::getStockShader(gl::ShaderDef().color()));
    mFloor = gl::Batch::create(geom::Plane().size(vec2(1000)).subdivisions(vec2(1000)), gl::getStockShader(gl::ShaderDef().color()));

    mWorldAxisMatrix = mat4(1.0f);
    mWorldAxisViewportSize = vec2(getWindowWidth() / 5, getWindowHeight() / 5);
    mWorldAxisViewportPos = vec2(getWindowWidth() - mWorldAxisViewportSize.x, 0);

    mHexapod.setup();

    mCamOffset = vec3(0, 5 * BODY_WIDTH, -10);
    mCam.lookAt(mHexapod.getPos() + mCamOffset, mHexapod.getPos());

    //createShadowMap();

    drawTexts();

    ImGui::Initialize();
}

//In progress
void SceneHexapod::createShadowMap()
{
    //Create framebuffer object
    glGenFramebuffers(1, &mDepthMapFBO);

    //Create 2D texture for the framebuffer's depth buffer
    glGenTextures(1, &mDepthMap);
    glBindTexture(GL_TEXTURE_2D, mDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Attach created texture to the framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, mDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneHexapod::update()
{
    drawGUI();

    mWorldAxisMatrix = translate(mCam.getEyePoint()) * toMat4(mCam.getOrientation()) * translate(vec3(0, 0, -5)) / toMat4(mCam.getOrientation());

    mHexapod.update();

    if (mFollowHexapod)
        mCam.lookAt(mHexapod.getPos() + mCamOffset, mHexapod.getPos());
    else
        mCamOffset = mCam.getEyePoint() - mHexapod.getPos();
}

void SceneHexapod::drawGUI()
{
    ImGui::Begin("Scene Properties");

    ImGui::Checkbox("Camera Follows Hexapod", &mFollowHexapod);

    ImGui::End();
}

void SceneHexapod::draw()
{
    gl::clear(Color(Color::gray(0.3f)));

    {
        gl::ScopedViewport viewport(0, 0, getWindowWidth(), getWindowHeight());

        gl::draw(mTextTexture);

        gl::ScopedMatrices matrices(mCam);

        gl::ScopedDepth depth(true);

        {
            gl::ScopedColor color(Color::gray(0.5));
            mWirePlane->draw();
        }

        mHexapod.draw();

        {

            gl::ScopedColor color(44 / 255.0, 56 / 255.0, 102 / 255.0);
            gl::ScopedModelMatrix matrix(translate(vec3(0, -0.1, 0)));
            mFloor->draw();
        }

        if (mIsDrawCoord)
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