#pragma once

enum LEG
{
    FRONTLEFT,
    FRONTRIGHT,
    MIDLEFT,
    MIDRIGHT,
    BACKLEFT,
    BACKRIGHT,
    NONE
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
};