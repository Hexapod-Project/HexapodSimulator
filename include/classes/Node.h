#pragma once
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;

class Node {    
public:
    Node();
    Node(vec2 pos, Color color);
    Node(vec2 pos, Color color, bool isRotateable);
    void setAxis(Node *axis);
    void setChild(Node *child);    
    vec2 getPos();    
    float getRadians();
    void rotateAroundAxis(float radians);
    void rotate(float radians);
    void translate(vec2 translation);
    void moveTo(vec2 newPos);    
    void drawTexts();
    void draw();       

    static const float sNodeRadius;
    static const float sNodeRadiusSqr; //For performing distance calculations
    bool mIsRotateable;
    Color mColor;
private:
    vec2 mPos;        
    float mRadians;
    float mDistToAxis;    
    Node *mAxis;
    Node *mChild;

    gl::TextureRef mTextTexture;
};
