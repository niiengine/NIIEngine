/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#include "NiiSound.h"

namespace NII
{
    //------------------------------------------------------------------------
    void Sound::Sound(ResourceID rid, GroupID gid, ResLoadScheme * ls, ResResultScheme * rs):
        Resource(rid, gid, pid, ls, rs),
         // 初始化默认,可在load()前重新设置
        mGain(1.0f),
        mBalance(0.5f),
        mVelocity(Vector3f::ZERO),
        mMinDist(1.0f),
        mMaxDist(sqrtf(NII_NUM_MAX(NIIf)),
        mLength(0),
        mLoop(0),
        mFade(1.0),
        mPitch(1.0f),
        mInner(0.0f),
        mOuter(0.0f),
        mOutGain(0.1f),
        mDeferred(true),
        mNorModel(true),
        mPos(Vector3f::ZERO),
        mDir(Vector3f::ZERO),
        mBuffer(0),
    {
        mTime = N_Engine().getTimer();
    }
    //------------------------------------------------------------------------
    void Sound::load(const Audio & audio)
    {

    }
    //------------------------------------------------------------------------
    void Sound:: convert(Audio & ptr)
    {

    }
    //------------------------------------------------------------------------
    void Sound::setGain(NIIf volume)
    {
        Math<NIIf>::Clamp(volume, 0.00001f, 1.0f);
        mGain = volume;
    }
    //------------------------------------------------------------------------
    NIIf Sound::getGain() const
    {
        return mGain;
    }
    //------------------------------------------------------------------------
    void Sound::setVel(Vector3f vel = Vector3f::ZERO,bool deferred = true)
    {
        mVelocity = vel;
    }
    //------------------------------------------------------------------------
    const Vector3f & Sound::getVel()const
    {
        return mVelocity;
    }
    //------------------------------------------------------------------------
    void Sound::setMin(NIIf dist)
    {
        mMinDist = dist;
    }
    //------------------------------------------------------------------------
    NIIf Sound::getMix()const
    {
        return mMinDist;
    }
    //------------------------------------------------------------------------
    void Sound::setMax(NIIf dist)
    {
        mMaxDist = dist;
    }
    //------------------------------------------------------------------------
    NIIf Sound::getMax()const
    {
        return mMaxDist;
    }
    //------------------------------------------------------------------------
    void Sound::setBal(NIIf bal)
    {
        mBalance = bal;
    }
    //------------------------------------------------------------------------
    NIIf Sound::getBal()const
    {
        return mBalance;
    }
    //------------------------------------------------------------------------
    void Sound::setLength(NIId len)
    {
        mLength = len;
    }
    //------------------------------------------------------------------------
    NIId Sound::getLength()const
    {
        return mLength;
    }
    //------------------------------------------------------------------------
    void Sound::setPos(const Vector3f & pos)
    {
        mPos = pos;
    }
    //------------------------------------------------------------------------
    const Vector3f & Sound::getPos()
    {
        return mPos;
    }
    //------------------------------------------------------------------------
    void Sound::setDir(const Vector3f & dir)
    {
        mDir= dir;
    }
    //------------------------------------------------------------------------
    const Vector3f & Sound::getDir()
    {
        return mDir;
    }
    //------------------------------------------------------------------------
    void Sound::setCone(NIIf innerAngle, NIIf outerAngle, NIIf outerGain)
    {
        mInner = innerAngle;
        mOuter = outerAngle;
        mOutGain = outerGain;
    }
    //------------------------------------------------------------------------
    NIIf getInnerCone() const
    {
        return mInner;
    }
    //------------------------------------------------------------------------
    NIIf getOuterCone() const
    {
        return mOuter;
    }
    //------------------------------------------------------------------------
    NIIf getConeGain() const
    {
        return mOutGain;
    }
    //------------------------------------------------------------------------
    void Sound::setPitch(NIIf pitch)
    {
        mPitch = pitch;
    }
    //------------------------------------------------------------------------
    NIIf Sound::getPitch()
    {
        return mPitch;
    }
    //------------------------------------------------------------------------
    void Sound::setFade(NIId f)
    {
        mFade = a;
    }
    //------------------------------------------------------------------------
    Ni32 Sound::computeRollOff(Ni32 vol, Ni32 min, Ni32 max, NIIf fade, NIIf dist,
        NIIf dmin, NIIf dmax)
    {
        if (dist < dmin * dmin)
        {
            // no attenuation
            return vol;
        }
        else if (dist > dmax * dmax)
        {
            // full attenuation
            return min;
        }
        else
        {
            NIIf _dist = sqrtf(dist);

            // linearly descending volume on a dB scale
            NIIf db1 = min * (_dist - dmin) / (dmax - dmin);

            if (fade == 0.0)
            {
                vol += (Ni32) db1;

            }
            else if (fade > 0.0)
            {
                // linear amp between 0.00001 and 1.0
                NIIf amp2 = 0.0001 + 0.9999 * (dmax - _dist) / (dmax - dmin);
                // convert to 1/100th decibels
                NIIf db2 = 2000.0 * log10f(amp2);
                vol += (Ni32) ((1.0 - fade) * db1 + fade * db2);

            }
            else if (fade < 0.0)
            {
                // linear amplitude is 1/distance
                NIIf amp3 = dmin / _dist;
                // convert to 1/100th decibels
                NIIf db3 = 2000.0 * log10f(amp3);
                vol += (Ni32) ((1.0 + fade) * db1 - fade * db3);
            }

            Math::Clamp(vol, min, max);
            return vol;
        }
    }
    //------------------------------------------------------------------------
    NIIf Sound::computeRolloff(NIIf fade, NIIf dist, NIIf dmin, NIIf dmax)
    {
        static const NIIf min = -10000;
        static const NIIf max = 0;

        if (dist < dmin * dmin)
        {
            // no attenuation
            return 1.0f;
        }
        else
        {
            if (fade < 0.0f)
            {
                NIIf dist = sqrtf(dist);
                // inverse distance rolloff
                NIIf rolloff = dmin / dist;
                if (fade <= -1.0f)
                    return rolloff;
                if (dist > dmax)
                {
                    // full attenuation of mbrolloff
                    return (-fade * rolloff);
                }
                else
                {
                    NIIf mb = min * (dist - dmin) / (dmax - dmin);
                    NIIf mbrolloff = powf(10.0, mb / 2000.0);
                    return ((1.0 + fade) * mbrolloff - fade * rolloff);
                }
            }
            else
            {
                if (dist > dmax * dmax)
                {
                    // full attenuation
                    return 0.0f;
                }

                NIIf dist = sqrtf(dist);

                if (fade == 0.0f)
                {
                    // linearly descending volume on a dB scale
                    NIIf mb = min * (dist - dmin) / (dmax - dmin);
                    return powf(10.0, mb / 2000.0);
                }
                else // if (alpha > 0.0f)
                {
                    // linear distance rolloff
                    NIIf rolloff = (dmax - dist) / (dmax - dmin);
                    if (fade >= 1.0f) return rolloff;

                    NIIf mb = min * (dist - dmin) / (dmax - dmin);
                    NIIf mbrolloff = powf(10.0, mb / 2000.0);
                    return ((1.0 - fade) * mbrolloff + fade * rolloff);
                }
            }
        }
    }
    //------------------------------------------------------------------------
}