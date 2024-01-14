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

#ifndef _NII_VOICE_BUFFER_H_
#define _NII_VOICE_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiBuffer.h"
#include "NiiAudioFormat.h"

namespace NII
{
    /** 这个类是描述一个音频文件缓存
    @remark
        大多数声卡都是集成的,没有独立内存而使用系统缓存,所以设计起来并不像显卡资源
        那样.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VoiceBuffer : public Buffer
    {
    public :
        VoiceBuffer(BufferManager * mag, Nmark mode);
        ~VoiceBuffer();

        void alloc(NCount size);

        void realloc(NCount size);

        void dealloc();

        /// @copydetails Buffer::read
        virtual void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        virtual void write(const void * src, NCount oft, NCount size);

        inline NCount size()
        {
            return mSize;
        }

        inline NCount offset()
        {
            return mEnd ;
        }

        inline NCount remain()
        {
            return mSize - mEnd;
        }

        inline void empty()
        {
            mEnd = 0;
        }

        inline void write(const Nui8 * src, NCount size)
        {
            ::memcpy(mBuffer, src, size);
            mEnd = 0;
        }

        inline void push(const Nui8 * src, NCount size)
        {
            ::memcpy(mBuffer + mEnd, src, size);
            mEnd += size;
        }

        inline void UnShift(const Nui8 * src, NCount size)
        {
            memmove(mBuffer + size, mBuffer, mSize - size);
            ::memcpy (mBuffer, src, size);
            mEnd += size;
        }

        inline void * take(NCount size)
        {
            void * ret = mBuffer + mEnd;
            mEnd += size;
            return ret;
        }

        inline void * raw(NCount size)
        {
            return mBuffer;
        }

        inline void read(void * dst, NCount size)
        {
            ::memcpy(dst, mBuffer, size);
        }

        inline void pop(void * dst, NCount size)
        {
            mEnd -= size;
            if (dst)
                ::memcpy(dst, mBuffer + mEnd, size);
        }

        inline void shift(void * dst, NCount size)
        {
            if (dst)
                ::memcpy(dst, mBuffer, size);
            if ((mEnd != size) && size <= mEnd)
                memmove(mBuffer, mBuffer + size, mSize - size);
            if (size <= mEnd)
                mEnd -= size;
        }

        inline void reset()
        {
            mCurrent = 0;
        }

        inline NCount getOffset()
        {
            return mCurrent;
        }

        inline bool isEnd()
        {
            return mCurrent == mEnd;
        }

        inline void seek(NCount pos)
        {
            mCurrent = pos;
        }

        inline void * read(NCount size)
        {
            Nui8* out = mBuffer + mCurrent;
            mCurrent += size;
            return out;
        }
    private:
        Nui8 * mBuffer;
        NCount mSize;
        NCount mEnd;
        NCount mCurrent;
    };
}
#endif
