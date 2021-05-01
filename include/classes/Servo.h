#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "../tools/Tools.h"
#include "Foot.h"

using namespace ci;

//Declare first to resolve compiler 'does not name a type error'
class Foot;

class Servo
{
public:
    Servo();
    void setColor(Color color);
    void setBase(vec3 startPos, vec3 rotation);
    void setServoAngle(float angle);
    float getServoAngle();
    void setParent(Servo *parent);
    void setChild(Servo *child);
    void setFoot(Foot *foot);
    vec3 getPos();
    void setPos(vec3 pos);
    Foot* getFoot();
    mat4 getLocalMatrix();
    void updateMatrix();
    void draw();    

private:
    vec3 mPosition;
    vec3 mRotation;
    
    Color mColor;
    gl::BatchRef mAxisMesh;
    //The base matrix defines the initial matrix of the Servo
    //This will be used to reset the local matrix to the initial transformation before calculating the changes
    mat4 mBaseMatrix;
    //The local matrix is used to calculate changes in the update
    mat4 mLocalMatrix;

    float mServoAngle = 0.0f;

    Servo *mParent = nullptr;
    Servo *mChild = nullptr;    
    Foot *mFoot = nullptr;
};