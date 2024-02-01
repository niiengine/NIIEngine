/*
Copyright (c) "2018-2019", Shenzhen Mindeng Technology Co., Ltd(www.niiengine.com),
        Mindeng Base Communication Application Framework
All rights reserved.
    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.
    Neither the name of the "ORGANIZATION" nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior written
permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MDF_RingBuffer_H_
#define _MDF_RingBuffer_H_

#include "NiiNetPreInclude.h"

namespace Mdf
{
    /** ���λ���
    @version 0.9.1
    */
    class MdfNetAPI RingBuffer
    {
    public:
        RingBuffer();
        ~RingBuffer();

        /** ��ȡ����
        @version 0.9.1
        */
        inline Mui8 * getBuffer() const { return mData; }

        /** ��ȡ�����С
        @version 0.9.1
        */
        inline MCount getAllocSize() const { return mAllocSize; }

        /** ��ȡд���С
        @version 0.9.1
        */
        inline MCount getWriteSize() const { return mWriteSize; }

        /** ��ȡָ����ת
        @version 0.9.1
        */
        void readSkip(MCount cnt);
        
        /** д��ָ����ת
        @version 0.9.1
        */
        void writeSkip(MCount cnt);
        
        /** ���������ڴ�
        @version 0.9.1
        */
        void enlarge(MCount cnt);
        
        /** д��
        @version 0.9.1
        */
        Mui32 write(const void * in, MCount cnt);

        /** ��ȡ
        @version 0.9.1
        */
        Mui32 read(void * out, MCount cnt) const;
    private:
        Mui8 * mData;
        Mui32 mAllocSize;
        mutable Mui32 mWriteSize;
    };
    
    /** ����ѭ������
    @version 0.9.1
    */
    class RingLoopBuffer
    {
    public:
        RingLoopBuffer();
        ~RingLoopBuffer();

        /** ��ȡ�����С
        @version 0.9.1
        */
        inline Mui32 getAllocSize() const { return mAllocSize; }

        /** ��ȡд���С
        @version 0.9.1
        */
        inline Mui32 getWriteSize() const { return mWriteSize; }

        /** ��ȡ��Ч��ȡ��С
        @version 0.9.1
        */
        inline Mui32 getRemainSize() const { return mAllocSize - mWriteSize; }

        /** �����С
        @version 0.9.1
        */
        bool alloc(Mui32 nsize);

        /** �ͷŷ���
        @version 0.9.1
        */
        void free();

        /** д��
        @version 0.9.1
        */
        Mui32 write(const void * in, int size);

        /** ��ȡ(�ƶ���ȡָ��)
        @version 0.9.1
        */
        Mui32 read(void * out, int size) const;

        /** ��ȡ(���ƶ���ȡָ��)
        @version 0.9.1
        */
        Mui32 peek(void * out, int size) const;
    private:
        Mui8 * mData;
        Mui32 mAllocSize;
        mutable Mui32 mWriteSize;
        mutable Mui32 mHead;
        Mui32 mTail;
    };
}
#endif