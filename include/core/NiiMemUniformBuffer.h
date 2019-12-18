/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-15

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

#ifndef _NII_MEMUniform_BUFFER_H_
#define _NII_MEMUniform_BUFFER_H_

#include "NiiPreInclude.h"
#include "NiiUniformBuffer.h"

namespace NII
{
    /// CPU UniformBuffer
    class _EngineAPI MemUniformBuffer : public UniformBuffer
    {
    public:
        MemUniformBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~MemUniformBuffer();

        /// @copydetails Buffer::read
        void read(void * dest, NCount oft, NCount length);

        /// @copydetails Buffer::write
        void write(const void * src, NCount oft, NCount length);

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