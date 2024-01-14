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

#include "NiiPreProcess.h"
#include "NiiFrameObjListener.h"
#include "NiiTimer.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FramesPerSecond
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FramesPerSecond::FramesPerSecond(Timer * time)
    {
        mTimer = time;
    }
    //------------------------------------------------------------------------
    void FramesPerSecond::reset()
    {
        mAvg = 0.0;
        mBest = 0.0;
        mLast = 0.0;
        mWorst = 999.0;
        mSecFrame = 0;
        mBestFrame = 999999;
        mWorstFrame = 0;
        mLastTime = mTimer->getMS();
        mTempTime = mLastTime;
    }
    //------------------------------------------------------------------------
    void FramesPerSecond::cost()
    {
        // Update statistics (always on top)
        ++mSecFrame;
        Nul thisTime = mTimer->getMS();

        // check frame time
        Nul frameTime = thisTime - mLastTime ;
        mLastTime = thisTime ;

        mBestFrame = N_MIN(TimeDurMS, mBestFrame, frameTime);
        mWorstFrame = N_MAX(TimeDurMS, mWorstFrame, frameTime);

        // check if new second (update only once per second)
        if(thisTime - mTempTime > 1000)
        {
            // new second - not 100% precise
            mLast = (NIIf)mSecFrame / (NIIf)(thisTime - mTempTime) * 1000.0f;

            if(mAvg == 0)
                mAvg = mLast;
            else
                mAvg = (mAvg + mLast) / 2; // not strictly correct, but good enough

            mBest = std::max(mBest, mLast);
            mWorst = std::min(mWorst, mLast);

            mTempTime = thisTime ;
            mSecFrame  = 0;
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FrameObjListener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FrameObjListener::~FrameObjListener()
    {
    }
    //------------------------------------------------------------------------
    void FrameObjListener::onPreFlush(const FrameObjArgs *)
    {
    }
    //------------------------------------------------------------------------
    void FrameObjListener::onEndFlush(const FrameObjArgs *)
    {
    }
    //------------------------------------------------------------------------
    void FrameObjListener::onPreViewFlush(const FrameViewArgs *)
    {
    }
    //------------------------------------------------------------------------
    void FrameObjListener::onEndViewFlush(const FrameViewArgs *)
    {
    }
    //------------------------------------------------------------------------
}