cmake_minimum_required(VERSION 3.19)

project(HexapodSimulator)

get_filename_component(CINDER_PATH "~/Cinder" ABSOLUTE)

include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

set(APP_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/classes/Node.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Node3D.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Leg2D.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Leg3D.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Servo.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Foot.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Hexapod.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Body.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Animation.cpp
    ${PROJECT_SOURCE_DIR}/src/classes/Animator.cpp

    ${PROJECT_SOURCE_DIR}/src/scenes/Scene2DLegIK.cpp
    ${PROJECT_SOURCE_DIR}/src/scenes/Scene3DLegIK.cpp
    ${PROJECT_SOURCE_DIR}/src/scenes/SceneHexapod.cpp

    ${PROJECT_SOURCE_DIR}/src/tools/Tools.cpp

    ${PROJECT_SOURCE_DIR}/src/HexapodSimulator.cpp
)

set(APP_INCLUDES
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/classes
    ${PROJECT_SOURCE_DIR}/include/scenes
    ${PROJECT_SOURCE_DIR}/include/tools
)

ci_make_app(
    CINDER_PATH ${CINDER_PATH}
    SOURCES ${APP_SOURCE_FILES}
    INCLUDES ${APP_INCLUDES}
)