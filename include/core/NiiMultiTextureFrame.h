/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-8

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_MultiTextureFrame_H_
#define _NII_MultiTextureFrame_H_

#include "NiiPreInclude.h"
#include "NiiFrameObj.h"

namespace NII
{
    /** ֡������������
    @note ���������ʽ������ͬ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiTextureFrame : public FrameObj
    {
    public:
        typedef vector<TextureFrame *>::type AttachList;
    public:
        MultiTextureFrame(const String & name);

        /** ����
        @param[in] index ����
        @param[in] obj ����
        @version NIIEngine 3.0.0
        */
        virtual void attach(Nidx index, TextureFrame * obj);

        /** ���
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        virtual void detach(Nidx index);

        /** ��ȡ
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        TextureFrame * getAttach(NCount index);

        /** ��ȡ�б�
        @version NIIEngine 3.0.0
        */
        const AttachList & getAttachList() const;

        /// @copydetails FrameObj::fill
        virtual void fill(FrameObj::FaceType buffer, PixelFormat pf, PixelBlock & dst);

        /// @copydetails FrameObj::getFormat
        PixelFormat getFormat() const;
    protected:
        /** ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void attachImpl(Nidx index, TextureFrame * obj) = 0;

        /** ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void detachImpl(Nidx index) = 0;
    protected:
        AttachList mAttachList;
    };
}

#endif