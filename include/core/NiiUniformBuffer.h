/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2017-1-23

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
#ifndef NII_UNIFORMBuffer
#define NII_UNIFORMBuffer

#include "NiiPreInclude.h"
#include "NiiBuffer.h"

namespace NII 
{
    /** Ƭ�黺��
    @version NIIEngine 4.0.0
    */
    class _EngineAPI UniformBuffer : public Buffer
    {
    public:
        UniformBuffer(BufferManager * mag, NCount size, Nmark mode);
        ~UniformBuffer();

        /// @copydetails Buffer::clone
        Buffer * clone(Nmark m = Buffer::M_WRITE | Buffer::M_BLOCK | Buffer::M_CPU) const;
    };
}
#endif
