#include "Scene3DLegIK.h"
#include "cinder/CinderImGui.h"

bool printUpdate2 = true;

void Scene3DLegIK::setup()
{
    vec3 legPos = vec3(0, 0.25, -2);
    mLeg = Leg3D(legPos, 1.0f, 2.0f, 2.5f);
    mLeg.setup();

    mCamera.lookAt(vec3(-15, 2, 0), vec3(0));
    mCamera.setPerspective(40.0f, getWindowAspectRatio(), 0.01f, 100.0f);
    mCameraUi = CameraUi(&mCamera, getWindow());

    mWirePlane = gl::Batch::create(geom::WirePlane().size(vec2(50)).subdivisions(vec2(50)), gl::getStockShader(gl::ShaderDef().color()));

    mWorldAxisMatrix = mat4(1.0f);
    mWorldAxisViewportSize = vec2(getWindowWidth() / 5, getWindowHeight() / 5);
    mWorldAxisViewportPos = vec2(getWindowWidth() - mWorldAxisViewportSize.x, 0);

    vec3 footPos = mLeg.getFootPos();
    mFootX = footPos.x;
    mFootY = 0;
    mFootZ = footPos.z - 2.5 / 2;

    vec3 hipPos = mLeg.getHipPos();
    mHipX = hipPos.x;
    mHipY = hipPos.y;
    mHipZ = hipPos.z;

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
    ImGui::Text("Hip Position");
    ImGui::Columns(3);
    ImGui::DragFloat("Hip X", &mHipX, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("Hip Y", &mHipY, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("Hip Z", &mHipZ, 0.1, -FLT_MAX, FLT_MAX, "%.2f");

    ImGui::Columns(1);

    ImGui::Separator();
    ImGui::Text("Foot Position");
    ImGui::Columns(3);
    ImGui::DragFloat("Foot X", &mFootX, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("Foot Y", &mFootY, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::NextColumn();
    ImGui::DragFloat("Foot Z", &mFootZ, 0.1, -FLT_MAX, FLT_MAX, "%.2f");
    ImGui::End();
}

void Scene3DLegIK::update()
{
    mWorldAxisMatrix = translate(mCamera.getEyePoint()) * toMat4(mCamera.getOrientation()) * translate(vec3(0, 0, -5)) / toMat4(mCamera.getOrientation());

    mLeg.setHipPos(vec3(mHipX, mHipY, mHipZ));
    mLeg.setFootTargetPos(vec3(mFootX, mFootY, mFootZ));
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