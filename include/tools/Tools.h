#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include <chrono>

using namespace ci;

vec2 rotateAround(vec2 origin, float distance, float radians);
bool isPointInCircleSqr(vec2 pos, vec2 origin, float radiusSqr); //Use squared radius for faster calculations
gl::TextureRef createTextTexture(std::string texts, vec2 size, ColorA color, ColorA bgColor);
mat4 eulerRotate(mat4 target, vec3 rotations);
vec3 getPosFromMatrix(mat4 matrix);
vec3 getRotFromMatrix(mat4 matrix);
bool compareFloats(double a, double b, double precision = 0.00001);
int getCurrTime();
vec2 getCentroid(std::vector<vec2> polygon);
double toPositiveAngle(double angle);
double getSmallestAngle(double angle);
double clampAngleTo360(double angle);
double normalizeTimelapsed(int startTime, int duration);
vec2 normalizeJoystickPos(vec2 pos);
ivec2 degreesToJoyStickPos(float deg);
vec3 rotateAroundY(vec3 point, float rad);