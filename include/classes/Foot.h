#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Servo.h"

using namespace ci;

//Declare first to resolve compiler 'does not name a type error'
class Servo;

class Foot
{
public:
    Foot();
    void setColor(Color color);
    void setBase(vec3 startPos, vec3 rotations);
    void setParent(Servo *parent);
    vec3 getPos();
    Servo* getParent();
    void updateMatrix();
    void draw();
private:
    Color mColor;
    mat4 mBaseMatrix;
    mat4 mLocalMatrix;

    gl::BatchRef mMesh;

    Servo *mParent = nullptr;
};