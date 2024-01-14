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
#include "NiiVoiceBuffer.h"

namespace NII
{
    //------------------------------------------------------------------------
    VoiceBuffer::VoiceBuffer(BufferManager * mag, Nmark mode) :
        Buffer(mag, mode),
        mBuffer(0),
        mSize(0),
        mEnd(0),
        mCurrent(0)
    {
    }
    //------------------------------------------------------------------------
    VoiceBuffer::~VoiceBuffer()
    {
        dealloc();
    }
    //------------------------------------------------------------------------
    void VoiceBuffer::alloc(NCount size)
    {
        dealloc();
        mBuffer = (Nui8*)N_alloc_align(size, 16);
        mSize = size;
        mEnd = 0;
    }
    //------------------------------------------------------------------------
    void VoiceBuffer::realloc(NCount size)
    {
#if defined(N_PLAT_WIN32)
        mBuffer = (Nui8*)N_realloc_align(mBuffer, mSize, size, 16);
#else
        Nui8* tmp = (Nui8*)N_alloc_align(size, 16);
        if (mBuffer)
        {
            size_t copy = std::min(size, mSize);
            ::memcpy(tmp, mBuffer, copy);
            _aligned_free(mBuffer);
        }
        mBuffer = tmp;
#endif

        mSize = size;
        mEnd = std::min(mEnd, mSize);
    }
    //------------------------------------------------------------------------
    void VoiceBuffer::dealloc()
    {
        if (mBuffer)
        {
            N_free(mBuffer);
            mBuffer = 0;
        }
        mSize = 0;
        mEnd = 0;
    }
    //------------------------------------------------------------------------
    void VoiceBuffer::read(void * dest, NCount oft, NCount size)
    {

    }
    //------------------------------------------------------------------------
    void VoiceBuffer::write(const void * src, NCount oft, NCount size)
    {

    }
    //------------------------------------------------------------------------
}
