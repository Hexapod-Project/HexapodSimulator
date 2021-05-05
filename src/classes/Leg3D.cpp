#include "Leg3D.h"
#include "Tools.h"

vec3 DEFAULT_LEG_DIR = vec3(1, 0, 0);
Leg3D::Leg3D() {
    //Disable showing the coordinate frame
    mDisableShowCoord = true;
}

Leg3D::Leg3D(vec3 pos, float hipLength, float femurLength, float tibiaLength, bool isRightLeg)
{
    //Disable showing the coordinate frame
    mDisableShowCoord = true;

    //Set local variables
    mFemurLength = femurLength;
    mFemurLengthSqr = mFemurLength * mFemurLength;

    mTibiaLength = tibiaLength;
    mTibiaLengthSqr = mTibiaLength * mTibiaLength;

    mLegLength = femurLength + tibiaLength;
    mLegLengthSqr = mLegLength * mLegLength;

    //If it is left leg, rotate the hip 180 degrees
    vec3 rot = vec3(0, isRightLeg ? M_PI : 0, 0);
    //Set leg transform
    setBase(pos, rot);

    //Hip servo
    mHipServo.setBase(vec3(0), vec3(0));
    mHipServo.setColor(Color(1, 0, 0));

    //Femur servo
    mFemurServo.setBase(DEFAULT_LEG_DIR * hipLength, vec3(M_PI / 2, 0, 0));
    mFemurServo.setColor(Color(0, 1, 0));

    //Tbia servo
    mTibiaServo.setBase(DEFAULT_LEG_DIR * femurLength, vec3(0));
    mTibiaServo.setColor(Color(0, 0, 1));

    //Foot
    mFoot = Foot(DEFAULT_LEG_DIR);
    mFoot.setBase(DEFAULT_LEG_DIR * tibiaLength, vec3(0));
    mFoot.setColor(Color(1, 1, 0));

    mTargetFootPosMesh = gl::Batch::create(geom::WireCube().size(vec3(0.15)), gl::getStockShader(gl::ShaderDef().color()));
}

void Leg3D::setup()
{
    //Children have to be set here. Setting it in the contructor will result in different addresses
    addChild(&mHipServo);
    mHipServo.addChild(&mFemurServo);
    mFemurServo.addChild(&mTibiaServo);
    mTibiaServo.addChild(&mFoot);

    setFootTargetPos(getFootWorldPos());
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

void Leg3D::setFootTargetPos(vec3 targetWorldPos)
{
    mTargetFootPos = targetWorldPos;
    mTargetFootPosMatrix = translate(mTargetFootPos);
}

//This returns the world position relative to the leg's transformations and not the actual world transformation
vec3 Leg3D::getFootWorldPos()
{
    return mFoot.getWorldPos();
}

//Calculate the inverse kinematics
void Leg3D::calculateIK()
{
    mat4 inverseWorldMatrix = inverse(mWorldMatrix);

    //Convert the target foot pos to the leg's local space
    /**     
     * The target foot position has to be converted to the leg's local space
     * (basically translating the foot target position to the correct position relative to the leg's local space)
     * This is so that the servos' angles calculated will account for the body's rotations which affects the leg's postion
     */
    vec3 localTargetFootPos = inverseWorldMatrix * vec4(mTargetFootPos, 1);

    //For debugging purposes(has not effect on the actual IK calculations)
    mLocalFootPosMatrix = translate(localTargetFootPos);

    //Get the foot's local position relative to the hip node (excluding the leg's transformation's)
    vec3 localFootPos = mFoot.getLocalPos(inverseWorldMatrix);

    //If it is the same then don't need to waste resources to calculate again
    vec3 footDiff = glm::abs(localFootPos - localTargetFootPos);
    //*Cant do vec3 == vec3 method. It will always return false
    if (footDiff.x <= 0.0001 && footDiff.y <= 0.0001 && footDiff.z <= 0.0001)
        return;

    vec3 localHipServoPos = mHipServo.getLocalPos(inverseWorldMatrix);
    vec3 localFemurServoPos = mFemurServo.getLocalPos(inverseWorldMatrix);
    vec3 localTibiaServoPos = mTibiaServo.getLocalPos(inverseWorldMatrix);

    //Calculate hip rotation based on the foot X and Z distance from the hip servo
    //The hip servo considers the horizontal position only;
    vec3 diffTargetToHip = (localTargetFootPos - localHipServoPos);
    //*Negative because the positive X axis is pointing to the left
    float hipAngle = -atan2(diffTargetToHip.z, diffTargetToHip.x);

    if (!isnanf(hipAngle))
        mHipServo.setServoAngle(hipAngle);

    //Checks if the X and Z target position is valid
    /**
     * These 2 condition is where the target position lies in between the femur servo and hip servo, which is unreachable:
     * If Z = 0 and X distance is negative then it is invalid OR if X = 0 and Z distance is negative then it is invalid
    */
    vec3 diffVerification = glm::abs(localTargetFootPos) - glm::abs(localFemurServoPos);
    if (!(diffVerification.x < 0 && abs(diffVerification.z) <= 0.00001 || diffVerification.z < 0 && abs(diffVerification.x) <= 0.00001))
    {
        //Calculate the distance between the target foot pos and femur servo
        vec3 diff = localTargetFootPos - localFemurServoPos;
        float dist = sqrt(dot(diff, diff));

        //Calculate the angle of the line formed (the hypotenuse) by the target pos and femur servo
        /**
         * The opposite side of the triangle is the Y axis
         * The adjacent side is the line formed on  the XZ plane (basically the hypotenuse of the triangle formed on the XZ plane) 
         */
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
        float femurAcosVal = (mFemurLengthSqr + distSqr - mTibiaLengthSqr) / (2 * mFemurLength * dist);
        float femurAngle = acos(femurAcosVal) + femurToTargetAngle;

        //If the distance is the same as the leg length then the tibia does not need to rotate. Only the femur servo needs to rotate
        float tibiaAngle = 0;
        if (dist < mLegLength)
        {
            float tibiaAcosVal = (mFemurLengthSqr + mTibiaLengthSqr - distSqr) / (2 * mFemurLength * mTibiaLength);
            //The angle is subtracted by Pi because the angle calculated with Law of Cosines is the angle between the femur and tibia
            //So by subtracting Pi, the actual servo rotation required is obtained
            tibiaAngle = acos(tibiaAcosVal) - M_PI;
        }

        if (!isnanf(femurAngle))
            mFemurServo.setServoAngle(femurAngle);

        if (!isnanf(tibiaAngle))
            mTibiaServo.setServoAngle(tibiaAngle);
    }
}

void Leg3D::update()
{        
    updateMatrix();
    mHipServo.updateMatrix();
    mFemurServo.updateMatrix(); 

    //Somehow running this here has the least jittering from the slow update
    //Probably need a faster algorithm for calculating the IK
    calculateIK();

    mTibiaServo.updateMatrix();        
    mFoot.updateMatrix();    
}

void Leg3D::draw()
{
    //Draw the leg components
    {
        gl::ScopedModelMatrix matrix(mWorldMatrix);

        mHipServo.draw();
        mFemurServo.draw();
        mTibiaServo.draw();
        mFoot.draw();
    }

    //Draw the actual target foot position
    {
        gl::ScopedColor color(Color(1, 0.25, 0));
        gl::ScopedModelMatrix matrix(mTargetFootPosMatrix);
        mTargetFootPosMesh->draw();
    }

    //For debugging the local foot position calculated by the IK function
    /**
     * If it displayed in the same position as the actual target foot position, then the IK calculations are incorrect
     * This is supposed to show the foot position where the leg's transformations are not included
     */
    {
        gl::ScopedColor color(Color(0, 1, 1));
        gl::ScopedModelMatrix matrix(mLocalFootPosMatrix * scale(vec3(1.3)));
        //mTargetFootPosMesh->draw();
    }
}