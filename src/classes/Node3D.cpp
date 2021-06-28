#include "Node3D.h"
#include "Tools.h"

void Node3D::setBase(vec3 pos, vec3 rot)
{
    mBasePos = pos;
    mBaseRot = rot;    

    mBaseMatrix = translate(mBasePos) * toMat4(quat(mBaseRot));
    mLocalMatrix = mBaseMatrix;
    mWorldMatrix = mLocalMatrix;
}

void Node3D::updateMatrix()
{
    mLocalRot = vec3(mPitch, mYaw, mRoll);
    mLocalPos = vec3(mLocalPosX, mLocalPosY, mLocalPosZ);

    mLocalMatrix = mBaseMatrix;

    mLocalMatrix *= translate(mLocalPos) * toMat4(quat(mLocalRot));

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

void Node3D::setParent(Node3D *parent)
{
    if (mParent)
        mParent->removeChild(this);

    mParent = parent;

    //Update the matrix since the parents local matrix affects this object's local matrix
    updateMatrix();

    mParent->addChild(this);
}

void Node3D::addChild(Node3D *child)
{
    if (checkChildExists(child))
        return;

    mChildren.push_back(child);

    child->updateMatrix();

    if (child->mParent != this)
        child->setParent(this);
}

void Node3D::removeChild(Node3D *child)
{
    for (int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i] == child)
        {
            mChildren.erase(mChildren.begin() + i);
            child->updateMatrix();
        }
    }
}

bool Node3D::checkChildExists(Node3D *child)
{
    for (int i = 0; i < mChildren.size(); i++)
    {
        if (mChildren[i] == child)
            return true;
    }

    return false;
}

vec3 Node3D::getLocalPos()
{
    return getPosFromMatrix(mLocalMatrix);
}

vec3 Node3D::getLocalPos(mat4 rootInverseMat)
{
    return getPosFromMatrix(rootInverseMat * mWorldMatrix);
}

vec3 Node3D::getWorldPos()
{
    return getPosFromMatrix(mWorldMatrix);
}

void Node3D::setBasePos(vec3 pos)
{
    setBase(pos, mBaseRot);
}

void Node3D::drawCoord()
{
    if (!mDisableShowCoord)
    {
        gl::ScopedDepth depth(false);
        gl::ScopedModelMatrix matrix(mWorldMatrix);
        gl::drawCoordinateFrame();
    }

    for (Node3D *child : mChildren)
        child->drawCoord();
}