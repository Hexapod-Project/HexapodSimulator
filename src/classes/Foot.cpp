#include "Foot.h"

const float DEFAULT_FOOTHEIGHT = 0.5f;

Foot::Foot()
{
    mMesh = gl::Batch::create(geom::Cone()
                                  .height(DEFAULT_FOOTHEIGHT)
                                  .ratio(0.35)
                                  .direction(vec3(0, 0, 1))
                                  .origin(vec3(0, 0, -DEFAULT_FOOTHEIGHT)),
                              gl::getStockShader(gl::ShaderDef().color().lambert()));
}

void Foot::setColor(Color color)
{
    mColor = color;
}

void Foot::setBase(vec3 startPos, vec3 rotations)
{
    mBaseMatrix = translate(startPos) * toMat4(quat(rotations));
    mLocalMatrix = mBaseMatrix;
}

void Foot::setParent(Servo *parent)
{
    mParent = parent;
    mLocalMatrix = mParent->getLocalMatrix() * mBaseMatrix;

    if(mParent->getFoot() != this)
        mParent->setFoot(this);
}

Servo* Foot::getParent()
{
    return mParent;
}

vec3 Foot::getPos()
{
    return vec3(mLocalMatrix[3][0], mLocalMatrix[3][1], mLocalMatrix[3][2]);
}

void Foot::updateMatrix()
{
    if(mParent)
        mLocalMatrix = mParent->getLocalMatrix() * mBaseMatrix;
    else
        mLocalMatrix = mBaseMatrix;
}

void Foot::draw()
{
    gl::ScopedModelMatrix matrix(mLocalMatrix);
    gl::ScopedColor color(mColor);
    mMesh->draw();
    gl::drawCoordinateFrame();
}