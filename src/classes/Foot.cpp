#include "Foot.h"
#include "HexapodConstants.h"

using namespace HexapodConstants;

Foot::Foot(vec3 dir)
{
    mMesh = gl::Batch::create(geom::Cone()
                                  .height(FOOTHEIGHT)
                                  .ratio(AXIS_RADIUS / FOOTHEIGHT)
                                  .direction(dir)
                                  .origin(dir * -FOOTHEIGHT),
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