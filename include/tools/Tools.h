#pragma once

#include "cinder/app/App.h"

using namespace ci;

vec2 rotatePoint(vec2 axis, float distance, float radians);
bool isPointInCircleSqr(vec2 pos, vec2 origin, float radiusSqr); //Use squared radius for faster calculations
//float distance(vec2 posA, vec2 posB);