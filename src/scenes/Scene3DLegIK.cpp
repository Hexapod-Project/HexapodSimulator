#include "Scene3DLegIK.h"
#include "cinder/CinderImGui.h"
#include "Tools.h"

void Scene3DLegIK::setup()
{
    vec3 legPos = vec3(-2, 0, 0);
    mLeg = Leg3D(legPos, 1.0f, 2.0f, 2.5f, true);
    mLeg.setup();
    mLeg.setParent(&mBody);    

    mCamera.lookAt(vec3(0, 10, 15), vec3(0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100.0f);
    mCameraUi = CameraUi(&mCamera, getWindow());

    mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(50)).subdivisions(vec2(50)), gl::getStockShader(gl::ShaderDef().color()));

    mWorldAxisMatrix = mat4(1.0f);
    mWorldAxisViewportSize = vec2(getWindowWidth() / 5, getWindowHeight() / 5);
    mWorldAxisViewportPos = vec2(getWindowWidth() - mWorldAxisViewportSize.x, 0);    

    drawTexts();

    ImGui::Initialize();
}

void Scene3DLegIK::drawGUI()
{
    ImGui::Begin("Leg Properties");
    ImGui::DragFloat("Hip Angle", &mHipAngle, 0.5, -360, 360, "%.1f");
    ImGui::DragFloat("Femur Angle", &mFemurAngle, 0.5, -360, 360, "%.1f");
    ImGui::DragFloat("Tibia Angle", &mTibiaAngle, 0.5, -360, 360, "%.1f");
    ImGui::Separator();

    ImGui::Text("Parent Position");
    ImGui::Columns(3);
    ImGui::DragFloat("pX", &mBody.mLocalPosX, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("pY", &mBody.mLocalPosY, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("pZ", &mBody.mLocalPosZ, 0.1, -FLT_MAX, FLT_MAX, "%.2f");

    ImGui::Columns(1);

    ImGui::Text("Parent Rotation");
    ImGui::Columns(3);
    ImGui::DragFloat("Roll", &mBody.mRoll, 0.5, -360, 360, "%.1f");
    ImGui::NextColumn();
    ImGui::DragFloat("Yaw", &mBody.mYaw, 0.5, -360, 360, "%.1f");
    ImGui::NextColumn();
    ImGui::DragFloat("Pitch", &mBody.mPitch, 0.5, -360, 360, "%.1f");

    ImGui::Columns(1);
    
    ImGui::Separator();
    ImGui::Text("Foot Position");
    ImGui::Columns(3);
    ImGui::DragFloat("ftX", &mLeg.mTargetFootPosX, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("ftY", &mLeg.mTargetFootPosY, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("ftZ", &mLeg.mTargetFootPosZ, 0.1, -FLT_MAX, FLT_MAX, "%.2f");

    ImGui::End();
}

void Scene3DLegIK::update()
{
    mWorldAxisMatrix = translate(mCamera.getEyePoint()) * toMat4(mCamera.getOrientation()) * translate(vec3(0, 0, -5)) / toMat4(mCamera.getOrientation());
    
    mBody.updateMatrix();
    
    mLeg.update();

    mHipAngle = mLeg.getHipAngle();
    mFemurAngle = mLeg.getFemurAngle();
    mTibiaAngle = mLeg.getTibiaAngle();

    drawGUI();
}

void Scene3DLegIK::draw()
{
    gl::clear(Color::gray(0.5f));

    {
        gl::ScopedViewport viewport(0, 0, getWindowWidth(), getWindowHeight());

        //Text texture has to be drawn before setting the camera matrices.
        //This is because it will draw into the 3D space once the camera matrices are set.
        gl::draw(mTextTexture);

        gl::ScopedMatrices matrices(mCamera);

        gl::color(Color::white());

        gl::ScopedDepth depth(true);

        mWirePlane->draw();

        mLeg.draw();

        if(mIsDrawCoord)
            //The body will draw the child coordinates as well so just calling body is sufficient
            mBody.drawCoord();
    }

    {
        //Camera matrices are defined twice because of the text texture above which has to be drawn before the
        //camera is declared and has to be within the viewport above
        gl::ScopedMatrices matrices(mCamera);

        gl::ScopedViewport viewport(mWorldAxisViewportPos.x, mWorldAxisViewportPos.y, mWorldAxisViewportSize.x, mWorldAxisViewportSize.y);
        gl::ScopedModelMatrix matrix(mWorldAxisMatrix);
        gl::drawCoordinateFrame(1.5, 0.4, 0.1);
    }
}

void Scene3DLegIK::drawTexts()
{
    mTextTexture = createTextTexture("3D Leg Inverse Kinematics", vec2(getWindowWidth() / 3, 150), Color(1, 1, 1), ColorA(0, 0, 0, 0));
}

void Scene3DLegIK::toggleCamUi(bool enable)
{
    if (!enable && mCameraUi.isEnabled())
        mCameraUi.disable();
    else if (enable && !mCameraUi.isEnabled())
        //Somehow using .enable() function does not work so just reinitialize
        mCameraUi = CameraUi(&mCamera, getWindow());
}