#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Node3D.h"

using namespace ci;

class Servo : public Node3D
{
public:
    Servo();
    void setColor(Color color);    
    void setServoAngle(float angle);
    float getServoAngle();                
    void updateMatrix() override;
    void draw();    

private:   
    Color mColor;
    gl::BatchRef mAxisMesh;

    float mServoAngle = 0.0f;
};