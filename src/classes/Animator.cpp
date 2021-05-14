#include "Animator.h"

template <typename T>
void Animator<T>::addAnimation(Animation<T> anim)
{
    mAnimations.push_back(anim);
    mAnimIdx = mAnimations.size() - 1;
    mAnimName = anim.mName;
}

template <typename T>
void Animator<T>::removeAnimation(std::string name)
{
    for (int i = 0; i < mAnimations.size(); i++)
    {
        if (mAnimations[i].mName == name)
            return removeAnimation(i);
    }
}

template <typename T>
void Animator<T>::removeAnimation(int idx)
{
    if (idx >= mAnimations.size())
        return;

    mAnimations.erase(mAnimations.begin() + idx);
}

template <typename T>
void Animator<T>::play(std::string name)
{
    if (mAnimName != name)
    {
        for (int i = 0; i < mAnimations.size(); i++)
        {            
            if (mAnimations[i].mName == mAnimName)
            {
                mAnimIdx = i;
                break;
            }
        }

        //Check if the animation with the given name was found.
        if (mAnimations[mAnimIdx].mName == name)
            mAnimName = name;
        else
            std::cout << "Animation with the name \"" << name << "\" was not found." << std::endl;
    }

    //Play aimation if animation name was found
    if (mAnimName == name)
        play();
}

template <typename T>
void Animator<T>::play(int idx)
{
    if (mAnimIdx != idx)
    {
        if (idx >= 0 && idx < mAnimations.size())
            mAnimIdx = idx;
        else
            std::cout << "Animation at index " << idx << " is not valid." << std::endl;
    }

    //Play animation if animation index is correct
    if (mAnimIdx == idx)
        play();
}

template <typename T>
void Animator<T>::play()
{
    if(mAnimations.size() > 0) {
        mAnimations[mAnimIdx].play();
    } else 
        std::cout << "Failed to play animation. There are no animation added to the animator." << std::endl;
}

template <typename T>
void Animator<T>::update()
{
    mAnimations[mAnimIdx].update();
}

template <typename T>
void Animator<T>::stop()
{
    if(mAnimations.size() > 0) {
        mAnimations[mAnimIdx].stop();
    } else 
        std::cout << "Failed to stop animation. There are no animation added to the animator." << std::endl;
}

template <typename T>
bool Animator<T>::isPlaying()
{
    return mAnimations[mAnimIdx].mIsPlaying;
}