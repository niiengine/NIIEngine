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

#ifndef Nii_ApuOpenCLBuffer_H_
#define Nii_ApuOpenCLBuffer_H_

#include "NiiPreInclude.h"
#include "NiiApuBuffer.h"
#include "NiiApuOpenCLUtils.h"
#include "NiiSimdManager.h"

namespace NII
{
    /*class openCLBufferInfo
    {
        openCLBufferInfo(cl_mem data, bool readonly = false) : mData(data), mReadOnly(readonly) {}

        cl_mem mData;
        bool mReadOnly;
    };*/
    class OpenCLApuProgram;
    class OpenCLApuBuffer : public ApuBuffer
    {
    public:
        struct ConstData
        {
            union {
                Vector4i mIntData;
                Vector4f mFloatData;
                Vector4ui mUintData;
            };
        };
    public:
        OpenCLApuBuffer(void * src, NCount unitSize, NCount unitCnt, Nmark mode, cl_context ctx, cl_command_queue queue, size_t reserveSize = 0, bool growsize = true) :
            ApuBuffer(N_OnlyPtr(Simd), unitSize, unitCnt, mode, ApuL_OPENCL),
            mData(0),
            mSrc(src),
            mContext(ctx),
            mQueue(queue),
            mCopyData(true),
            mRunFinish(true)
        {
            if (reserveSize)
            {
                reserveImpl(reserveSize);
            }
            mGrowSize = growsize;
        }

        ~OpenCLApuBuffer();

        static void setup(const ApuContext * ctx);

        void setData(cl_mem data, size_t size);

        ///@copydetails Buffer::read
        virtual void read(void * out, NCount oft, NCount size);

        ///@copydetails Buffer::write
        virtual void write(const void * in, NCount oft, NCount size);

        ///@copydetails Buffer::write
        virtual void write(Buffer * src, NCount srcOft, NCount oft, NCount size) const;

        ///@copydetails Buffer::write
        virtual void write(Buffer * src) const;

        inline cl_mem getCLData() const         { return mData; }

        /*N_FORCEINLINE bool push_back(const T & _Val)
        {
            bool result = true;
            size_t sz = getUnitCount();
            if (sz == mUnitReserveCount)
            {
                result = reserve((sz ? sz * 2 : 1));
            }
            write(&_Val, sz, 1);
            mUnitCount++;
            return result;
        }*/
        /*
        N_FORCEINLINE T forcedAt(size_t n) const
        {
            N_assert(n >= 0);
            N_assert(n < mUnitReserveCount);
            T elem;
            read(&elem, n, 1);
            return elem;
        }*/

        /*N_FORCEINLINE T at(size_t n) const
        {
            N_assert(n >= 0 && n < getUnitCount(), "");
            T elem;
            read(&elem, n, 1);
            return elem;
        }
        */

        inline void setRunFinish(bool set)      { mRunFinish = set; }

        inline bool isRunFinish() const         { return mRunFinish; }

        void execute(Ni32 value, NCount n, NCount offset = 0);

        void execute(Nui32 value, NCount n, NCount offset = 0);

        void execute(NIIf value, NCount n, NCount offset = 0);

        void execute(const Vector2i & value, NCount n, NCount offset = 0);

        /// @copydetails Buffer::clone()
        Buffer * clone(Nmark m = M_WRITE | M_WHOLE | M_HOST) const;
    protected:
        OpenCLApuBuffer & operator=(const OpenCLApuBuffer & src);

        ///@copydetails Buffer::lockImpl
        virtual void * lockImpl(Nmark m, NCount oft, NCount size);

        ///@copydetails Buffer::unlockImpl
        virtual void unlockImpl();

        ///@copydetails Buffer::reserveImpl
        virtual bool reserveImpl(NCount newSize, Nmark newMode = -1, bool oldData = true);
    private:
        void * mSrc;
        cl_mem mData;
        cl_context mContext;
        cl_command_queue mQueue;

        bool mCopyData;
        bool mGrowSize;
        bool mRunFinish;

        static OpenCLApuProgram * mFillProgram;
    };
}
#endif
