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
    target *= rotate(rotations.x, vec3(1,0,0));
    target *= rotate(rotations.y, vec3(0,1,0));
    target *= rotate(rotations.z, vec3(0,0,1));

    return target;
}