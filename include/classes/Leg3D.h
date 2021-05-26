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
    void setFootTargetPos(vec3 targetPos);
    void resetFootTargetPos();
    vec3 getFootWorldPos();
    void calculateIK();            
    void setup();
    void update();
    void draw();

    //These are used by ImGui
    float mTargetFootPosX;
    float mTargetFootPosY;
    float mTargetFootPosZ;

    vec3 mTargetFootPos;

    vec3 mFootStartPos;
    float mFootStartAngle;

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
    mat4 mTargetFootPosMatrix;

    //For debugging purposes
    mat4 mLocalFootPosMatrix;        
};