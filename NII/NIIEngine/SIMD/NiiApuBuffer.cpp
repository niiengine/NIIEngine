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
#include "NiiApuBuffer.h"
#include "NiiSimdManager.h"
#include "NiiException.h"
#include "NiiStrConv.h"
#include "NiiEngine.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ApuBuffer
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ApuBuffer::ApuBuffer(BufferManager * mag, NCount unitSize, NCount unitCnt, Nmark mode, ApuLanguage type) :
        Buffer(mag, unitSize, unitCnt, mode),
        mType(type)
    {
        mSize = mUnitSize * mUnitCount;
        //if (mMark & M_HOST)
        //{
        //    mShadow = N_new MemIndexBuffer(mag, size, count, Buffer::M_READ | Buffer::M_WRITE);
        //}
    }
    //------------------------------------------------------------------------
    ApuBuffer::~ApuBuffer()
    {
        if (mMag)
        {
            static_cast<SimdManager *>(mMag)->onDestroy(this);
        }

        if (mShadow)
        {
            N_delete mShadow;
            mShadow = 0;
        }
    }
    //------------------------------------------------------------------------
}