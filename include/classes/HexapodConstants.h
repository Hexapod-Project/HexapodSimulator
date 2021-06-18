#ifndef HEXAPOD_CONST
#define HEXAPOD_CONST

namespace HexapodConstants
{
    const int LEG_COUNT = 6;

    const float HIP_LENGTH = 1.0;
    const float FEMUR_LENGTH = 2.0;
    const float TIBIA_LENGTH = 2.5;
    const float BODY_THICKNESS = 0.5;
    const float BODY_WIDTH = 3;
    const float BODY_LENGTH = 4;
    //The width of the head and back
    /**
     * This is just used to determine the leg positions
     * Since it is an elongated hexagon, the width is the length of the hexagon's sides with radius of BODY_WIDTH
    */
    const float BODY_HALFWIDTH = BODY_WIDTH / 2;
    const float BODY_HALFWIDTH_SQR = BODY_HALFWIDTH * BODY_HALFWIDTH;
    const float BODY_FRONTBACK_WIDTH = sqrt(2 * BODY_HALFWIDTH_SQR - cos(M_PI / 3) * 2 * BODY_HALFWIDTH_SQR);

    //Leg Front Back X Offset
    const float X_FB_LEG_OFFSET = BODY_FRONTBACK_WIDTH / 2 + 0.5;
    const float X_MID_LEG_OFFSET = BODY_WIDTH / 2 + 0.5;
    const float Y_LEG_OFFSET = BODY_THICKNESS / 2;
    //Leg Front Back Z offset
    const float Z_FB_LEG_OFFSET = BODY_LENGTH / 2;

    //The distance of the foot is the radius of a circle and the body's center is the origin
    const float FOOT_DIST = BODY_WIDTH + 3;
    const float FOOT_Y = 0;

    const float FEET_CIRCUMFERENCE = 2 * M_PI * FOOT_DIST;

    const float RIGHT_FORWARD = 3 * M_PI / 4;
    const float FORWARD = M_PI / 2;
    const float LEFT_FORWARD = M_PI / 4;
    const float RIGHT = M_PI;
    const float LEFT = 0;
    const float RIGHT_BACKWARD = 5 * M_PI / 4;
    const float BACKWARD = 3 * M_PI / 2;
    const float LEFT_BACKWARD = 7 * M_PI / 4;

    const int BASE_STEP_DURATION = 1000;       //Duration in ms that each step takes to complete
    const float BASE_MAXROT_ANGLE = M_PI / 4; //Maximum angle of rotation per step
}
#endif