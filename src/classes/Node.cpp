#include "Node.h"
#include "Tools.h"

const float Node::sNodeRadius = 8.0f;
const float Node::sNodeRadiusSqr = Node::sNodeRadius * Node::sNodeRadius;

Node::Node() {}

Node::Node(vec2 pos, Color color)
{
    mPos = pos;
    mColor = color;
    mIsRotateable = true;
    mAxis = NULL;
    mChild = NULL;

    drawTexts();
}

Node::Node(vec2 pos, Color color, bool isRotateable)
{
    mPos = pos;
    mColor = color;
    mIsRotateable = isRotateable;
    mAxis = NULL;
    mChild = NULL;

    drawTexts();
}

vec2 Node::getPos()
{
    return mPos;
}

void Node::setChild(Node *child)
{
    mChild = child;
    if (mChild != NULL && mChild->mAxis != this)
        mChild->setAxis(this);
}

void Node::setAxis(Node *axis)
{
    mAxis = axis;
    if (mAxis->mChild != this)
        mAxis->setChild(this);
}

float Node::getRadians()
{
    return mRadians;
}

void Node::rotateAroundAxis(float radians)
{
    vec2 axisPos = mAxis->getPos();
    mDistToAxis = distance(mPos, axisPos);
    vec2 prevPos = mPos;
    mPos = rotateAround(axisPos, mDistToAxis, radians);    

    if (mChild != NULL)
        mChild->translate(mPos - prevPos);
}

void Node::rotate(float radians)
{
    mRadians = radians;

    if (mChild != NULL)
        mChild->rotateAroundAxis(radians);
}

void Node::translate(vec2 translation)
{
    mPos += translation;
    if (mChild != NULL)
        mChild->translate(translation);
}

void Node::moveTo(vec2 newPos)
{
    //If no axis then just translate the position and the child
    if (mAxis == NULL)
        translate(newPos - mPos);
    //If theres axis, calculate the angle from the mousePos and axis and rotate around the axis
    else if (mAxis->mIsRotateable)
    {
        //Note:Axis pos is retrieved every time the mouse is dragged instead of storing
        //it in a variable is because the axis pos might change
        vec2 axisPos = mAxis->getPos();
        vec2 diff = newPos - axisPos;
        rotateAroundAxis(atan2(diff.y, diff.x));
    }
}

void Node::drawTexts()
{
    TextBox textBox = TextBox();
    char text[4];
    std::sprintf(text, "%.1f\u00B0", toDegrees(mRadians));
    textBox.text(text).size(100, 50).setColor(mColor);
    mTextTexture = gl::Texture2d::create(textBox.render());
    gl::draw(mTextTexture, mPos + vec2(0, 10));
}

void Node::draw()
{
    gl::ScopedColor color(mColor);
    gl::drawStrokedCircle(mPos, Node::sNodeRadius);    
    
    drawTexts();    
}
