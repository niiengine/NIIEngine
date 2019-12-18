/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-10

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

#ifndef _NII_TEMPPRCBUFFER_H_
#define _NII_TEMPPRCBUFFER_H_

#include "NiiPreInclude.h"
#include "NiiTempBufferCtl.h"

namespace NII
{
    /** ��ʱ���㻺��
    @version NIIEngine 3.0.0
    */
    class _EngineInner AniTempBuffer : public TempBufferCtl, public BufferAlloc
    {
    public:
        AniTempBuffer(bool autoDsy = true);
        ~AniTempBuffer();

        /** ����ӳ��Դ
        @param[in] src Դ����
        @version NIIEngine 3.0.0
        */
        void map(const VertexData * src);

        /** ���丱��
        @version NIIEngine 3.0.0
        */
        void alloc(bool pos = true, bool normal = true);

        /** ������ʱ����
        @version NIIEngine 3.0.0
        */
        bool refill(bool pos = true, bool normal = true) const;

        /** �󶨸�����ָ������������
        @param[in] dst ָ����������
        @param[in] syn �Ƿ�ͬ��ǰ�󻺴�
        @version NIIEngine 3.0.0
        */
        void bind(VertexData * dst, bool syn);

        /// @copydetails TempBufferCtl::expire
        void expire(Buffer * buffer);
    private:
        VertexBuffer * mSrcPos;     ///< Դͷָ��(λ��)
        VertexBuffer * mSrcNrls;    ///< Դͷָ��(����)
        VertexBuffer * mDstPos;     ///< ����ָ��(λ��)
        VertexBuffer * mDstNrls;    ///< ����ָ��(����)
        Nui16 mPosIndex;
        Nui16 mNrlsIndex;
        Nui8 mMark;
    };
}

#endif