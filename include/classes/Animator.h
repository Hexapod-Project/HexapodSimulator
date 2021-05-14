#pragma  once
#include "Animation.h"
#include <iostream>

/**
 * @brief 
 * The animator is used to interpolate the given values in the keyframes over a given duration.
 * It is not meant to be used to play actual animations.
 */

template <typename T>
class Animator
{
public:
    void addAnimation(Animation<T> anim);
    void removeAnimation(std::string name);
    void removeAnimation(int idx);
    void play(std::string name);
    void play(int idx);
    //Play the last played animation / the latest added animation if no animations were played before
    void play();
    void update();
    void stop();
    bool isPlaying();
private:
    void setAnimIdxFromName(); 

    std::vector<Animation<T>> mAnimations;
    int mAnimIdx;
    std::string mAnimName;
};

template class Animator<vec3>;