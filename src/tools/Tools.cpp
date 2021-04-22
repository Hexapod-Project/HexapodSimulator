#include "Tools.h"

vec2 rotatePoint(vec2 axis, float distance, float radians)
{
    return vec2(axis.x + distance * cos(radians), axis.y + distance * sin(radians));
}

bool isPointInCircleSqr(vec2 pos, vec2 origin, float radiusSqr)
{
    vec2 diff = pos - origin;
    return dot(diff, diff) <= radiusSqr;
}