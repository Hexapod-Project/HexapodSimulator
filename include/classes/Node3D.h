#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;

class Node3D
{
public:
    virtual void setBase(vec3 pos, vec3 rot);
    virtual void setParent(Node3D *parent);
    virtual void addChild(Node3D *child);
    virtual void removeChild(Node3D *child);
    virtual bool checkChildExists(Node3D *child);
    virtual void updateMatrix();
    //This returns the node's local position
    virtual vec3 getLocalPos();
    //This returns the node's local position in relation to the given root's inverse matrix
    //*The root matrix has to be from the parent or one of it's ancestors
    virtual vec3 getLocalPos(mat4 rootInverseMat);
    virtual vec3 getWorldPos();
    virtual void setBasePos(vec3 pos);
    void drawCoord();

    bool mDisableShowCoord = false;

    vec3 mBasePos;
    vec3 mBaseRot;

    //This is to track any changes in rotation after the initialization
    vec3 mLocalRot;
    vec3 mLocalPos;

    //The base matrix defines the initial matrix of the Servo
    //This will be used to reset the local matrix to the initial transformation before calculating the changes
    mat4 mBaseMatrix;
    //The local matrix is used to calculate changes in the update
    mat4 mLocalMatrix;
    //The world matrix does not represent the actual world's matrix
    //It represents the node's world matrix which includes all matrices of it's parent and ancestors
    mat4 mWorldMatrix;

    Node3D *mParent = nullptr;
    std::vector<Node3D *> mChildren;
};