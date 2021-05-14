#pragma once
#include "cinder/app/App.h"
#include <string>
#include <vector>
#include "Tools.h"
#include "Enums.h"

using namespace ci;

template <typename T>
class Keyframe
{
public:
    std::vector<T *> mTargets;
    std::vector<T> mValues;
    //In milliseconds
    int mDuration;

    //Duration is in milliseconds
    Keyframe(std::vector<T *> mTargets, std::vector<T> mValues, int duration);
};

template <typename T>
class Animation
{
private:
    std::vector<Keyframe<T>> mKeyframes;
    int mKeyframeIdx;
    int mFrameStartTime;
    bool mIsLoop = false;
    //timeLapsed is how much time passed after the previous frame
    double getTweenTime(double timeLapsed, double duration);

public:
    Animation(std::string name, std::vector<Keyframe<T>> keyframes, bool isLoop = false, TWEENTYPE tweenType = TWEENTYPE::LINEAR);
    void setLoop(bool isLoop);
    void addKeyframe(Keyframe<T> keyframe);
    void removeKeyframe(int idx);
    void replaceKeyframes(std::vector<Keyframe<T>> keyframes);
    void play();
    void update();
    void stop();

    std::string mName;
    bool mIsPlaying = false;
    TWEENTYPE mTweenType;
};

template class Keyframe<vec3>;
template class Animation<vec3>;