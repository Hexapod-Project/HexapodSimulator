#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;

vec2 rotateAround(vec2 origin, float distance, float radians);
bool isPointInCircleSqr(vec2 pos, vec2 origin, float radiusSqr); //Use squared radius for faster calculations
gl::TextureRef createTextTexture(std::string texts, vec2 size, ColorA color, ColorA bgColor);
mat4 eulerRotate(mat4 target, vec3 rotations);
vec3 getPosFromMatrix(mat4 matrix);
vec3 getRotFromMatrix(mat4 matrix);
bool compareFloats(double a, double b, double precision = 0.00001);