#pragma once
#include "Node.h"
#include "cinder/app/App.h"
#include "../tools/Tools.h"

using namespace ci;

class Leg2D {
public:
    Leg2D();
    Leg2D(vec2 startPos, float floorY, float hipLength, float femurLength, float tibiaLength);
    void mouseDown(vec2 mousePos);
    void mouseDrag(vec2 mousePos);
    void mouseUp(vec2 mousePos);
    void moveWithIK(vec2 pos);
    void moveTo(vec2 pos);   
    void moveFeet(vec2 pos);
    vec2 getPos(); 
    vec2 getFeetPos();
    void draw();

    float mHipLength;
    float mFemurLength;
    float mTibiaLength;
private:
    std::vector<Node> mNodes;
    float mNodesLastIdx;
    int mSelNodeIdx; //Selected node index
   
    float mLegLength; //Total extended length of femur + tibia
    float mLegLengthSqr;
       
    float mHipLengthSqr;    
       
    float mFemurLengthSqr;
       
    float mTibiaLengthSqr; 
   
    vec2 mFeetLastPos;
   
    vec2 mHipToFemurOffset;        
};