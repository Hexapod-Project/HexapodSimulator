#include "Animation.h"

template <typename T>
Keyframe<T>::Keyframe(std::vector<T *> targets, std::vector<T> values, int duration)
{
    mTargets = targets;
    mValues = values;
    mDuration = duration;
}

template <typename T>
Animation<T>::Animation(std::string name, std::vector<Keyframe<T>> keyframes, bool isLoop, TWEENTYPE tweenType)
{
    mName = name;
    mKeyframes = keyframes;
    mKeyframeIdx = 0;
    mIsLoop = isLoop;
    mTweenType = tweenType;
}

template <typename T>
void Animation<T>::addKeyframe(Keyframe<T> keyframe)
{
    mKeyframes.push_back(keyframe);
}

template <typename T>
void Animation<T>::removeKeyframe(int idx)
{
    mKeyframes.erase(mKeyframes.begin() + idx);
}

template <typename T>
void Animation<T>::replaceKeyframes(std::vector<Keyframe<T>> keyframes)
{
    mKeyframes = keyframes;
}

template <typename T>
void Animation<T>::setLoop(bool isLoop)
{
    mIsLoop = isLoop;
}

template <typename T>
void Animation<T>::play()
{
    if (!mIsPlaying)
    {
        mFrameStartTime = getCurrTime();
        mKeyframeIdx = 0;
        mIsPlaying = true;
    }
}

template <typename T>
double Animation<T>::getTweenTime(double diffTime, double duration)
{
    float tweenTime = diffTime / duration;

    switch (mTweenType)
    {
    default:
    case TWEENTYPE::LINEAR:
        return tweenTime;
    case TWEENTYPE::SQUARE:
        return tweenTime * tweenTime;
    case TWEENTYPE::CUBIC:
        return tweenTime * tweenTime * tweenTime;
    case TWEENTYPE::SINE:
        return (sin(2 * M_PI * tweenTime) + 1)/2;
    case TWEENTYPE::COSINE:
        return (cos(2 * M_PI * tweenTime) + 1)/2;
    }
}

template <typename T>
void Animation<T>::update()
{
    if (!mIsPlaying)
        return;

    if (mKeyframeIdx >= mKeyframes.size())
    {
        if (mIsLoop)
            mKeyframeIdx = 0;
        else
            return stop();
    }

    Keyframe<T> currKeyframe = mKeyframes[mKeyframeIdx];

    int currTime = getCurrTime();
    double timeLapsed = currTime - mFrameStartTime;
    float tweenTiming = getTweenTime(timeLapsed, currKeyframe.mDuration);

    if (timeLapsed >= currKeyframe.mDuration)
    {
        for (int i = 0; i < currKeyframe.mTargets.size(); i++)
            *currKeyframe.mTargets[i] = currKeyframe.mValues[i];

        mFrameStartTime = currTime;
        mKeyframeIdx++;
    }
    else
    {
        for (int i = 0; i < currKeyframe.mTargets.size(); i++)
        {
            T diffValue = currKeyframe.mValues[i] - *currKeyframe.mTargets[i];
            //This will interpolate the value based on the duration and the current time lapsed
            T increment = diffValue * tweenTiming;
            *currKeyframe.mTargets[i] += increment;
        }
    }
}

template <typename T>
void Animation<T>::stop()
{
    mIsPlaying = false;
}