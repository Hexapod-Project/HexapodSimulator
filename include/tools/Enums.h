#pragma once

//This follows the CW order
enum LEG
{    
    FRONTRIGHT,
    MIDRIGHT,
    BACKRIGHT, 
    BACKLEFT,    
    MIDLEFT,
    FRONTLEFT    
};

enum TWEENTYPE {
    LINEAR,
    SQUARE,
    CUBIC,
    SINE,
    COSINE
};

enum GAITTYPE
{
    TRIPOD,
    RIPPLE, //FR, MR, BR, BL, ML, FL
    TRIPLE, //Tripod + Ripple
    WAVE
};

enum MOVESTATE {
    WALK,     
    IDLE
};

enum ROTATEDIR {
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = -1
};

enum GAITGROUPSTATE {
    MOVING,    
    STOPPING,
    STOPPED
};