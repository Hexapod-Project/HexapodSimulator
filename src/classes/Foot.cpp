#include "Foot.h"

const float DEFAULT_FOOTHEIGHT = 0.5f;

Foot::Foot(vec3 dir)
{
    mMesh = gl::Batch::create(geom::Cone()
                                  .height(DEFAULT_FOOTHEIGHT)
                                  .ratio(0.35)
                                  .direction(dir)
                                  .origin(dir * -DEFAULT_FOOTHEIGHT),
                              gl::getStockShader(gl::ShaderDef().color().lambert()));
}

void Foot::setColor(Color color)
{
    mColor = color;
}

void Foot::draw()
{
    gl::ScopedModelMatrix matrix(mWorldMatrix);
    gl::ScopedColor color(mColor);
    mMesh->draw();    
}