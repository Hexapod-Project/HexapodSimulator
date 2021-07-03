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
    TRIPLE, //Tripod + Ripple
    WAVE,
    RIPPLE, //FR, MR, BR, BL, ML, FL        
};

enum MOVESTATE {
    MOVING,
    STOPSTARTED,
    STOPPING,
    STOPPED,
};

enum MOVETYPE {
    WALK,
    ROTATE
};

enum ROTATEDIR {
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = -1
};