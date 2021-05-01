#include "Servo.h"

/**
 * @brief 
 * This script is not complete. There are many problems with the rotation function.
 * So it has been replaced with Node3D script which uses a simpler 3D model.
 */

const float DEFAULT_AXIS_RADIUS = 0.25f;
const float AXIS_HEIGHT = 0.5f;
const vec3 AXIS = vec3(0, 1, 0);

Servo::Servo()
{
    mAxisMesh = gl::Batch::create(geom::Cylinder()
                                      .direction(AXIS)
                                      .radius(DEFAULT_AXIS_RADIUS)
                                      .height(AXIS_HEIGHT)
                                      .origin(vec3(0, -AXIS_HEIGHT / 2, 0)),
                                  gl::getStockShader(gl::ShaderDef().color().lambert()));
}

void Servo::setColor(Color color)
{
    mColor = color;
}

void Servo::setBase(vec3 startPos, vec3 rotation)
{
    mPosition = startPos;
    mRotation = rotation;

    mBaseMatrix = translate(startPos) * toMat4(quat(rotation));
    mLocalMatrix = mBaseMatrix;
}

void Servo::setParent(Servo *parent)
{
    mParent = parent;
    mLocalMatrix = mParent->mLocalMatrix * mBaseMatrix;

    if (mParent->mChild != this)
        mParent->setChild(this);
}

void Servo::setChild(Servo *child)
{
    mChild = child;

    if (mChild->mParent != this)
        mChild->setParent(this);
}

void Servo::setFoot(Foot *foot)
{
    mFoot = foot;

    if (mFoot->getParent() != this)
        mFoot->setParent(this);
}

Foot *Servo::getFoot()
{
    return mFoot;
}

void Servo::setServoAngle(float angle)
{
    mServoAngle = angle;
}

float Servo::getServoAngle()
{
    return toDegrees(mServoAngle);
}

mat4 Servo::getLocalMatrix()
{
    return mLocalMatrix;
}

vec3 Servo::getPos()
{
    return vec3(mLocalMatrix[3][0], mLocalMatrix[3][1], mLocalMatrix[3][2]);
}

void Servo::setPos(vec3 pos)
{
    setBase(pos, mRotation);
}

void Servo::updateMatrix()
{
    //Resets the local matrix back to the initial transformation
    mLocalMatrix = mBaseMatrix;

    //Calculate the new rotation;
    mLocalMatrix *= rotate(mServoAngle, AXIS);

    //Add the parent's transformation
    //This is so that any rotation or translation of the parent will be passed to this object
    if (mParent)
        mLocalMatrix = mParent->mLocalMatrix * mLocalMatrix;

    //Update child after all the calculations have been made
    if (mChild)
        mChild->updateMatrix();

    //Update foot matrix if servo is attached to foot
    if (mFoot)
        mFoot->updateMatrix();
}

void Servo::draw()
{
    gl::ScopedModelMatrix matrix(mLocalMatrix);
    {
        gl::ScopedColor color(mColor);
        mAxisMesh->draw();
    }

    {
        gl::ScopedDepth depth(false);
        gl::drawCoordinateFrame();
    }
}