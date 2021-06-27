#ifndef HEXAPOD_CONST
#define HEXAPOD_CONST

namespace HexapodConstants
{
    const int LEG_COUNT = 6;

    const float AXIS_RADIUS = 1.0f;
    const float AXIS_HEIGHT = 4.0f;
    const vec3 AXIS = vec3(0, 1, 0);
    const float FOOTHEIGHT = AXIS_HEIGHT / 2; //This is for visualization only

    const float HIP_LENGTH = 5.2;
    const float FEMUR_LENGTH = 4.7;
    const float TIBIA_LENGTH = 9;
    const float BODY_THICKNESS = 8;
    const float BODY_WIDTH = 16;
    const float BODY_LENGTH = 16;

    const float LEG_DIST_FROM_ORIGIN = 6.7;

    //The width of the head and back
    /**
     * This is just used to determine the leg positions
     * Since it is an elongated hexagon, the width is the length of the hexagon's sides with radius of BODY_WIDTH
    */
    const float BODY_HALFWIDTH = BODY_WIDTH / 2;
    const float BODY_HALFWIDTH_SQR = BODY_HALFWIDTH * BODY_HALFWIDTH;
    const float BODY_FRONTBACK_WIDTH = sqrt(2 * BODY_HALFWIDTH_SQR - cos(M_PI / 3) * 2 * BODY_HALFWIDTH_SQR);

    const float BODY_Y_OFFSET = 1.5;
    const float BODY_START_Y = 1;

    //Leg Front Back X Offset
    const float X_FB_LEG_OFFSET = cos(M_PI_4) * LEG_DIST_FROM_ORIGIN;
    const float X_MID_LEG_OFFSET = LEG_DIST_FROM_ORIGIN;
    const float Y_LEG_OFFSET = AXIS_HEIGHT / 2 - BODY_Y_OFFSET;
    //Leg Front Back Z offset
    const float Z_FB_LEG_OFFSET = sin(M_PI_4) * LEG_DIST_FROM_ORIGIN;

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

    //Walk properties
    const int BASE_STEP_DURATION = 1000;      //Duration in ms that each step takes to complete
    const float BASE_MAXROT_ANGLE = M_PI / 4; //Maximum angle of rotation per step

    const float STEP_HEIGHT = 1.0;
    const float STEP_DIST = 2;

    const int JOYSTICK_POSITIVE_MAXPOS = 255;
    const int JOYSTICK_NEGATIVE_MAXPOS = 1;
    const int JOYSTICK_MAXDIST = 127;
    const int JOYSTICK_ZERO_POS = 128;    
}
#endif