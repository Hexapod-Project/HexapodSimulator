#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Node3D.h"

using namespace ci;

class Body : public Node3D
{
public:
    Body();
    Body(float thickness, float width, float length);
    void updateMatrix() override;    
    void update();
    void draw();

    float mRoll = 0;
    float mYaw = 0;
    float mPitch = 0;

    float mLocalPosX = 0;
    float mLocalPosY = 0.5;
    float mLocalPosZ = 0;

private:
    mat4 mBodyMatrix;
    gl::BatchRef mBodyMesh;
};