/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-1

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_MEM_VERTEX_BUFFER_H_
#define _NII_MEM_VERTEX_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiVertexBuffer.h"

namespace NII
{
    /// SYS / IndexBuffer
    class _EngineAPI MemIndexBuffer : public IndexBuffer
    {
    public:
        MemIndexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);

        ~MemIndexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };

    /// SYS / VertexBuffer
    class _EngineAPI MemVertexBuffer : public VertexBuffer
    {
    public:
        MemVertexBuffer(BufferManager * mag, NCount size, NCount count, Nmark mode);

        ~MemVertexBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount size);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount size);

        /// @copydetails Buffer::lock
        void * lock(Nmark m, NCount oft, NCount size);

        /// @copydetails Buffer::unlock
        void unlock();
    protected:
        /// @copydetails Buffer::lockImpl
        void * lockImpl(Nmark lm, NCount oft, NCount size);

        /// @copydetails Buffer::unlockImpl
        void unlockImpl();
    protected:
        Nui8 * mData;
    };
}

#endif