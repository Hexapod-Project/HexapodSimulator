#include "Body.h"

const vec3 BODY_FORWARD_DIR = vec3(0, 0, 1);

Body::Body()
{
    mBodyMesh = gl::Batch::create(geom::Cylinder()
                                      .subdivisionsAxis(6)
                                      .height(0.5)
                                      .radius(1.5),
                                  gl::getStockShader(gl::ShaderDef().color().lambert()));

    //The values here are based on personal preferences.
    //*This should not be hardcoded but for now it works
    //This is mainly just to stretch and rotate the body mesh
    mBodyMatrix = scale(vec3(1, 1, 1.5));
}

//*Length is the vertical length
Body::Body(float thickness, float width, float length)
{
    float hexagonRadius = width/2;

    mBodyMesh = gl::Batch::create(geom::Cylinder()
                                      .subdivisionsAxis(8)
                                      .height(thickness)
                                      .radius(hexagonRadius),
                                  gl::getStockShader(gl::ShaderDef().color().lambert()));
                                  
    /**
     * The ratio determines how much the body will stretch relative to the forward direction of the body
     * so that the body's length will match the given length
     * It has to subtract 1 because the initial scale is already 1
     * 
     * *The hexagon forms 6 triangles, to calculate the the height, just calculate one of the triangle's height x 2
     * *hexagonHeight = sin(180(total triangle angles) - 90 (right angle side) - (180/3)/2 (hexagon angle / 2)) * hexagon radius
     * 
     * *The body mesh is actually a cylinder even though it shows a hexagonal prism, thats why the scale ratio has to take into account
     * *the difference between the hexagon's height and the cylinder's width/diameter
     */
    float hexagonHeight = sin(M_PI - M_PI/2 - M_PI/6) * hexagonRadius * 2;    
    float scaleRatio = length/hexagonHeight - 1;    
    mBodyMatrix = scale(vec3(1) + BODY_FORWARD_DIR * scaleRatio) * rotate((float)M_PI_2 / 4, vec3(0, 1, 0));
}

void Body::update()
{
    updateMatrix();
}

void Body::draw()
{
    gl::ScopedModelMatrix matrix(mWorldMatrix);

    {
        gl::ScopedModelMatrix matrix(mLocalMatrix * mBodyMatrix);
        gl::ScopedColor color(Color(1, 0.5, 0));
        mBodyMesh->draw();        
    }
}