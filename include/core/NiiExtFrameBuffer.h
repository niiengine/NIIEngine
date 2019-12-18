/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-2-8

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

#ifndef _NII_EXT_FRAMEBUFFER_H_
#define _NII_EXT_FRAMEBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ֡����չ����
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI ExtFrameBuffer : public BufferAlloc
    {
    public:
        ExtFrameBuffer(GroupID gid, NCount w, NCount h, NCount bit, Nui32 fsaa,
            const String & hint, bool manual);

        virtual ~ExtFrameBuffer();

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        virtual NCount getWidth() const;

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        virtual NCount getHeight() const;

        /** ��ȡ���
        @version NIIEngine 3.0.0
        */
        virtual NCount getBitDepth() const;

        /** ��ȡ����������
        @version NIIEngine 3.0.0
        */
        virtual NCount getFsaa() const;

        /** ��ȡ������������ʾ
        @version NIIEngine 3.0.0
        */
        virtual const String & getFsaaHint() const;

        /** �Ƿ�������֡�������
        @version NIIEngine 3.0.0
        */
        virtual bool isSuitable(FrameObj * fo) const;

        /** ���ӵ�֡���������
        @version NIIEngine 3.0.0
        */
        virtual void attach(FrameObj * fo);

        /** ��֡��������н��
        @version NIIEngine 3.0.0
        */
        virtual void detach(FrameObj * fo);
        
        /** ������
        @version NIIEngine 3.0.0 �߼�api
        */
        void setGroup(GroupID gid);

        /** ��ȡ��
        @version NIIEngine 3.0.0 �߼�api
        */
        GroupID getGroup() const;

        /** ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        void setManual(bool b);
        
        /** ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        bool isManual() const;
    protected:
        typedef set<FrameObj *>::type UserList;
    protected:
        GroupID mPoolId;
        NCount mWidth;
        NCount mHeight;
        NCount mBitDepth;
        NCount mFsaa;
        String mFsaaHint;
        UserList mUserList;
        bool mManual;
    };
    typedef vector<ExtFrameBuffer *>::type ExtFrameBufferList;
}
#endif
