# Hexapod Simulator

A simulation created to test the algorithms for the hexapod's motions and inverse kinematics.

Disclaimer: This is mainly for experimental purposes, there may be a lot of undiscovered bugs but the algorithms are usable nonetheless.

#### Added Behaviours:
<ol>
    <li>Move if any directions based on the directional angles.</li>
    <li>Turns and walk towards specified direction.</li>
    <li>Rotates in-place towards given directional angles.</li>
</ol>

#### How to use:
<ol>
    <li>Download and install CMake.</li>
    <li>Download and install Cinder.</li>
    <li>Open the CMakeLists.txt and change the CINDER_PATH to the path where your Cinder library is installed.</li>
    <li>Run build.sh or enter the "make" command in yout termminal.</li>
    <li>Run the program (If you use the build.sh script, the program should be in the "build/Debug/HexapodSimulator" folder).</li>
</ol>
