#include "Servo.h"
#include "Tools.h"
#include "HexapodConstants.h"

using namespace HexapodConstants;

/**
 * @brief 
 * This script is not complete. There are many problems with the rotation function.
 * So it has been replaced with Node3D script which uses a simpler 3D model.
 */



Servo::Servo()
{
    mAxisMesh = gl::Batch::create(geom::Cylinder()
                                      .direction(AXIS)
                                      .radius(AXIS_RADIUS)
                                      .height(AXIS_HEIGHT)
                                      .origin(vec3(0, -AXIS_HEIGHT / 2, 0)),
                                  gl::getStockShader(gl::ShaderDef().color().lambert()));
}

void Servo::setColor(Color color)
{
    mColor = color;
}

void Servo::setServoAngle(float angle)
{
    mServoAngle = angle;
}

float Servo::getServoAngle()
{
    return toDegrees(mServoAngle);
}

void Servo::updateMatrix()
{
    //Reset the mLocalMatrix back to the base matrix
    mLocalMatrix = mBaseMatrix;

    //Rotate based on the servo's angle while excluding the base Y rotation so that the servo rotates to the correct angle
    mLocalMatrix *= rotate(mServoAngle, AXIS);

    if (mParent)
        mWorldMatrix = mParent->mWorldMatrix * mLocalMatrix;
    else
        mWorldMatrix = mLocalMatrix;

    if (mChildren.size())
    {
        for (Node3D *child : mChildren)
            child->updateMatrix();
    }
}

void Servo::draw()
{
    gl::ScopedModelMatrix matrix(mWorldMatrix);
    {
        gl::ScopedColor color(mColor);
        mAxisMesh->draw();
    }
}