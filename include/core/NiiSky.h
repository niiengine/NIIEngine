/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-1

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

#ifndef _NII_SKY_H_
#define _NII_SKY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Sky : public SpaceAlloc
    {
    public:
        enum PlaneType
        {
            PT_Front = 0,
            PT_Back = 1,
            PT_Left = 2,
            PT_Right = 3,
            PT_Up = 4,
            PT_Down = 5
        };
    public:
        Sky();
        Sky(const SpaceManager * sm);
        virtual ~Sky();

        /** �������Ⱦ���������
        @param[in] obj �����
        @version NIIEngine 3.0.0
        */
        virtual void render(Camera * obj);

        /** ���õ�ǰ�ռ���
        @version NIIEngine 3.0.0
        */
        void setSpace(SpaceManager * sm);
        
        /** ������Ⱦ��
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(GroupID gid);

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup()const;

        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        void setResourceGroup(GroupID gid);

        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;

        /** �����Ƿ�����
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** ��ȡ�Ƿ�����
        @version NIIEngine 3.0.0
        */
        bool isEnable() const;
    protected:
        /** ��Ⱦ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void renderImpl(RenderQueue * rq) = 0;
    protected:
        SpaceManager * mParent;
        GroupID mRenderGroup;
        GroupID mResourceGroup;
        SpaceNode * mNode;
        bool mEnable;
    };
}

#endif