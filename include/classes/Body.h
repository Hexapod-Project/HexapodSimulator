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
    void update();
    void draw();

    vec3 mStartPos;
private:
    mat4 mBodyMatrix;
    gl::BatchRef mBodyMesh;
};