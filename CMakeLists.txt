cmake_minimum_required(VERSION 3.19)

project(HexapodSimulator)

get_filename_component(CINDER_PATH "~/Cinder" ABSOLUTE)

include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

set(APP_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/classes/Node.cpp    
    ${PROJECT_SOURCE_DIR}/src/classes/TwoDLeg.cpp
    ${PROJECT_SOURCE_DIR}/src/scenes/SceneTwoDLegIK.cpp
    ${PROJECT_SOURCE_DIR}/src/tools/Tools.cpp
    ${PROJECT_SOURCE_DIR}/src/HexapodSimulator.cpp
)

ci_make_app(
    CINDER_PATH ${CINDER_PATH}
    SOURCES ${APP_SOURCE_FILES}
    INCLUDES ${PROJECT_SOURCE_DIR}/include   
)