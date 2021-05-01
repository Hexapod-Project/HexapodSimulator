#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Servo.h"
#include "Foot.h"

using namespace ci;

class Leg3D
{
public:
    Leg3D();
    Leg3D(vec3 pos, float hipLength, float femurLength, float tibiaLength);
    void setHipAngle(float angle);
    void setFemurAngle(float angle);
    void setTibiaAngle(float angle);
    float getHipAngle();
    float getFemurAngle();
    float getTibiaAngle();
    vec3 getFootPos();
    void setFootTargetPos(vec3 targetPos);
    void calculateIK();
    vec3 getHipPos();
    void setHipPos(vec3 pos);
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
    
    gl::BatchRef mFootTargetMesh;
    vec3 mFootTargetPos;
    mat4 mTargetFootMatrix;    
};