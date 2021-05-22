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

enum MOVETYPE {
    WALK,
    ROTATE,      
    IDLE
};

enum ROTATEDIR {
    CLOCKWISE,
    COUNTERCLOCKWISE
};