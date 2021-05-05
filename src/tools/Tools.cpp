#include "Tools.h"

vec2 rotateAround(vec2 origin, float distance, float radians)
{
    return vec2(origin.x + distance * cos(radians), origin.y + distance * sin(radians));
}

bool isPointInCircleSqr(vec2 pos, vec2 origin, float radiusSqr)
{
    vec2 diff = pos - origin;
    return dot(diff, diff) <= radiusSqr;
}

gl::TextureRef createTextTexture(std::string texts, vec2 size, ColorA color, ColorA bgColor)
{
    TextBox textBox = TextBox().size(size.x, size.y);
    textBox.text(texts);
    textBox.setColor(color);
    textBox.setBackgroundColor(bgColor);

    return gl::Texture2d::create(textBox.render());
}

mat4 eulerRotate(mat4 target, vec3 rotations)
{
    target *= rotate(rotations.x, vec3(1, 0, 0));
    target *= rotate(rotations.y, vec3(0, 1, 0));
    target *= rotate(rotations.z, vec3(0, 0, 1));

    return target;
}

vec3 getPosFromMatrix(mat4 matrix)
{
    return vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}

//Note: There may be some decrepancies with the euler angles (eg. vec3(0, PI, 0) might be converted to its equivalent vec3(PI, 0, PI))
//If the exact angles of the axis is required then this method is not the best option
vec3 getRotFromMatrix(mat4 matrix)
{    
    return glm::eulerAngles(toQuat(matrix));
}

bool compareFloats(double a, double b, double precision)
{
    return abs(a - b) <= precision;
}