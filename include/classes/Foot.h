#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "Node3D.h"

using namespace ci;

//Declare first to resolve compiler 'does not name a type error'
class Servo;

class Foot : public Node3D
{
public:
    Foot(){};
    Foot(vec3 dir);
    void setColor(Color color);       
    void setDir(vec3 dir);     
    void draw();

private:
    Color mColor;    
    vec3 mDir;

    gl::BatchRef mMesh;    
};