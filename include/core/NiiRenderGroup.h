/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-30

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

#ifndef _NII_RENDER_GROUP_H_
#define _NII_RENDER_GROUP_H_

#include "NiiPreInclude.h"
#include "NiiRenderSort.h"

namespace NII
{
    /** ��Ⱦ�ȼ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderLevelGroup : public RenderAlloc
    {
    public:
        RenderLevelGroup(RenderGroup * parent);

        virtual ~RenderLevelGroup();

        /** �������ģʽ
        @version NIIEngine 3.0.0
        */
        virtual void addSort(RenderSortMode rsm);

        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        virtual void removeSort(RenderSortMode rsm);

        /** ��������ģʽ
        @see RenderSortMode
        */
        virtual void resetSort();

        /** �����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void add(GeometryObj * obj, ShaderFusion * sf);

        /** ��ȥ��Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void remove(ShaderCh * ch);

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual void sort(const Camera * cam);

        /** ����������Ⱦ��Ⱥ��
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getBasic() const { return mBasic; }

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getAlphaBasic() const { return mAlphaBasic; }

        /** ��ȡ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getAlphaSortBasic() const { return mAlphaSortBasic; }

        /** �ϲ�����Ⱦ��Ⱥ��
        @version NIIEngine 3.0.0
        */
        virtual void merge(const RenderLevelGroup * o);
    protected:
        RenderGroup * mParent;
        RenderSort mBasic;
        RenderSort mAlphaBasic;
        RenderSort mAlphaSortBasic;
    };

    /** ��Ⱦ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroup : public RenderAlloc
    {
    public:
        typedef map<Nui16, RenderLevelGroup *, std::less<Nui16> >::type RenderList;
    public:
        RenderGroup(RenderQueue * parent);

        virtual ~RenderGroup();

        /** �����Ƿ�������Ӱ
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b) { mShadowsEnable = b; }

        /** ��ȡ�Ƿ�������Ӱ
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const { return mShadowsEnable; }

        /** �������ģʽ
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);

        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

        /** ��������ģʽ
        @version NIIEngine 3.0.0
        */
        void resetSort();

        /** �����Ⱦ
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, ShaderFusion * sf, Nui16 level);

        /** ��ȥ��Ⱦ
        @param[in] destroy
        @version NIIEngine 3.0.0
        */
        void clear(bool destroy = false);

        /** �ϲ���Ⱦ��
        @version NIIEngine 3.0.0
        */
        void merge(const RenderGroup * o);

        /** ��ȡ��Ⱦ�б�
        @version NIIEngine 3.0.0
        */
        inline const RenderList & getRenderList() const { return mRenderList; }
    protected:
        /** ������Ⱦ�ȼ���ʵ��
        @version NIIEngine 3.0.0
        */
        virtual void createImpl(RenderLevelGroup *& out);
    protected:
        RenderQueue * mParent;
        RenderList mRenderList;
        Nmark mSortMark;
        bool mShadowsEnable;
    };
}

#endif