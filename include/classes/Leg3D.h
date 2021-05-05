#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Servo.h"
#include "Foot.h"
#include "Node3D.h"

using namespace ci;

class Leg3D:public Node3D
{
public:
    Leg3D();
    Leg3D(vec3 pos, float hipLength, float femurLength, float tibiaLength, bool isRightLeg = false);
    float getHipAngle();
    float getFemurAngle();
    float getTibiaAngle();
    vec3 getFootWorldPos();
    void setFootTargetPos(vec3 targetPos);
    void calculateIK();            
    void setup();
    void update();
    void draw();

private:    
    float mFemurLength;
    float mFemurLengthSqr;
    float mTibiaLength;
    float mTibiaLengthSqr;
    float mLegLength;
    float mLegLengthSqr;
    
    Servo mHipServo;
    Servo mFemurServo;
    Servo mTibiaServo;
    Foot mFoot;
    
    gl::BatchRef mTargetFootPosMesh;
    vec3 mTargetFootPos;
    mat4 mTargetFootPosMatrix;

    //For debugging purposes
    mat4 mLocalFootPosMatrix;        
};