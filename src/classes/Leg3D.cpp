#include "Leg3D.h"

Leg3D::Leg3D() {}

Leg3D::Leg3D(vec3 pos, float hipLength, float femurLength, float tibiaLength)
{
    mFemurLength = femurLength;
    mFemurLengthSqr = mFemurLength * mFemurLength;

    mTibiaLength = tibiaLength;
    mTibiaLengthSqr = mTibiaLength * mTibiaLength;

    mLegLength = femurLength + tibiaLength;
    mLegLengthSqr = mLegLength * mLegLength;

    //Hip servo
    mHipServo.setBase(pos, vec3(0));
    mHipServo.setColor(Color(1, 0, 0));

    //Femur servo
    mFemurServo.setBase(vec3(0, 0, hipLength), vec3(0, 0, M_PI / 2));
    mFemurServo.setColor(Color(0, 1, 0));

    //Tbia servo
    mTibiaServo.setBase(vec3(0, 0, femurLength), vec3(0));
    mTibiaServo.setColor(Color(0, 0, 1));

    //Foot
    mFoot.setBase(vec3(0, 0, tibiaLength), vec3(0));
    mFoot.setColor(Color(1, 1, 0));    

    mFootTargetMesh = gl::Batch::create(geom::WireCube().size(vec3(0.15)), gl::getStockShader(gl::ShaderDef().color()));
}

void Leg3D::setup()
{
    //Children have to be set here. Setting it in the contructor will result in different addresses
    mHipServo.setChild(&mFemurServo);
    mFemurServo.setChild(&mTibiaServo);
    mTibiaServo.setFoot(&mFoot);

    mFootTargetPos = mFoot.getPos();
    mTargetFootMatrix = translate(mFootTargetPos);
}

void Leg3D::setHipAngle(float angle)
{
    mHipServo.setServoAngle(toRadians(angle));
}

void Leg3D::setFemurAngle(float angle)
{
    mFemurServo.setServoAngle(toRadians(angle));
}

void Leg3D::setTibiaAngle(float angle)
{
    mTibiaServo.setServoAngle(toRadians(angle));
}

float Leg3D::getHipAngle()
{
    return mHipServo.getServoAngle();
}

float Leg3D::getFemurAngle()
{
    return mFemurServo.getServoAngle();
}

float Leg3D::getTibiaAngle()
{
    return mTibiaServo.getServoAngle();
}

vec3 Leg3D::getFootPos()
{
    return mFoot.getPos();
}

void Leg3D::setFootTargetPos(vec3 targetPos)
{
    mFootTargetPos = targetPos;
    mTargetFootMatrix = translate(mFootTargetPos);
}

vec3 Leg3D::getHipPos()
{
    return mHipServo.getPos();
}

void Leg3D::setHipPos(vec3 pos)
{    
    mHipServo.setPos(pos);
}

void Leg3D::calculateIK()
{
    //If it is the same then don't need to waste resources to calculate again
    if (mFoot.getPos() == mFootTargetPos)
        return;

    vec3 hipServoPos = mHipServo.getPos();
    vec3 femurServoPos = mFemurServo.getPos();
    vec3 tibiaServoPos = mTibiaServo.getPos();

    //Calculate hip rotation based on the foot X and Z distance from the hip servo
    //The hip servo considers the horizontal position only;
    vec3 diffTargetToHip = mFootTargetPos - hipServoPos;
    float hipAngle = atan2(diffTargetToHip.x, diffTargetToHip.z);

    //Calculate the distance between the target foot pos and femur servo
    vec3 diff = mFootTargetPos - femurServoPos;
    float dist = sqrt(dot(diff, diff));

    //Calculate the angle of the line formed (the hypotenuse) by the target pos and femur servo
    //The opposite side of the triangle is the Y axis
    //The adjacent side is the line formed on  the XZ plane (basically the hypotenuse of the triangle formed on the XZ plane)
    vec2 diffXZ = vec2(diff.x, diff.z);
    float distXZ = sqrt(dot(diffXZ, diffXZ));
    float femurToTargetAngle = atan2(diff.y, distXZ);

    //The distance cannot be longer than the leg length (femur length + tibia length)
    //Or else it will return the wrong angles
    if (dist > mLegLength)
        dist = mLegLength;
    //The distance cannot be less than the difference between the femur and tibia length
    else if (dist < abs(mFemurLength - mTibiaLength))
        dist = abs(mFemurLength - mTibiaLength);

    //The distance cannot be 0 because it will be divided
    //*The difference between femur and tibia might be 0
    if (dist <= 0)
        dist = 0.00001;

    float distSqr = dist * dist;

    //Use Law of Cosine to solve the femur and tibia servos' angles using
    //the triangle formed by the femur servo, tibia servo and foot positions
    float femurAngle = acos((mFemurLengthSqr + distSqr - mTibiaLengthSqr) / (2 * mFemurLength * dist)) + femurToTargetAngle;

    //If the distance is the same as the leg length then the tibia does not need to rotate. Only the femur servo needs to rotate
    float tibiaAngle = 0;
    if (dist < mLegLength)
        //The angle is subtracted by Pi because the angle calculate with Law of Cosines is the angle between the femur and tibia
        //By subtracting Pi, the actual servo rotation is calculated
        tibiaAngle = acos((mFemurLengthSqr + mTibiaLengthSqr - distSqr) / (2 * mFemurLength * mTibiaLength)) - M_PI;

    //Set the servo angles
    mHipServo.setServoAngle(hipAngle);
    mFemurServo.setServoAngle(femurAngle);
    mTibiaServo.setServoAngle(tibiaAngle);
}

void Leg3D::update()
{
    calculateIK();

    mHipServo.updateMatrix();
    mFemurServo.updateMatrix();
    mTibiaServo.updateMatrix();
    mFoot.updateMatrix();    
}

void Leg3D::draw()
{
    mHipServo.draw();
    mFemurServo.draw();
    mTibiaServo.draw();
    mFoot.draw();

    gl::ScopedColor color(Color(1,0.25,0));
    gl::ScopedModelMatrix matrix(mTargetFootMatrix);
    mFootTargetMesh->draw();
    //gl::drawCoordinateFrame(0.5f);
}