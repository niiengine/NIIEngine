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

#ifndef _NII_RENDER_GROUP_H_
#define _NII_RENDER_GROUP_H_

#include "NiiPreInclude.h"
#include "NiiShaderCh.h"

namespace NII
{
    /** ��Ⱦ������
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI RenderFilter : public RenderAlloc
    {
    public:
        RenderFilter() {}
        virtual ~RenderFilter() {}

        /** ִ����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, GeometryObj * obj) = 0;

        /** ִ����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, const GeometryObjList & obj) = 0;
    };
    
    /** ��Ⱦ����ģʽ
    @ersion NIIEngine 3.0.0
    */
    enum RenderSortMode
    {
        RSM_View_Descending = 0x01, ///< ���ӿھ��뽵��
        RSM_View_Ascending  = 0x02, ///< ���ӿھ�������
        RSM_Ch_Index        = 0x04, ///< ����Ⱦͨ·���Ⱥ��������
        RSM_All             = 0x07
    };

    /** ��Ⱦ����
    @version NIIEngine 3.0.0
    */
    struct RenderCh
    {
        RenderCh(GeometryObj * geo, ShaderCh * shader) :
            mGeo(geo),
            mCh(shader){}
            
        ShaderCh * mCh;
        GeometryObj * mGeo;
    };
    
    /** ��Ⱦ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSort : public RenderAlloc
    {
    public:
        typedef vector<RenderCh>::type RenderList;
    public:
        RenderSort();
        ~RenderSort();

        /** ���
        @version NIIEngine 3.0.0
        */
        void add(ShaderCh * ch, GeometryObj * obj);

        /** ��ȥ
        @version NIIEngine 3.0.0
        */
        void remove(ShaderCh * ch);

        /** �������ģʽ
        @version NIIEngine 3.0.0
        */
        void addSort(RenderSortMode om)         {mSortMark |= om;}

        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om)      {mSortMark &= ~om;}

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void clear();

        /** ����ָ�������������Ⱦ����
        @version NIIEngine 3.0.0
        */
        void sort(const Camera * cam);

        /** ִ����Ⱦ��������Ⱦ
        @version NIIEngine 3.0.0
        */
        void render(RenderFilter * filter, RenderSortMode rsm) const;

        /** �ϲ���Ⱦ����
        @version NIIEngine 3.0.0
        */
        void merge(const RenderSort & o);
    protected:
        typedef map<ShaderCh *, GeometryObjList *, ShaderCh::IndexLess>::type ChRenderList;
    protected:
        RenderList mRenderList;
        ChRenderList mChRenderList;
        Nmark mSortMark;
    };
    
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
        DrawCallGroup * mDrawCallGroup;
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
        inline void setShadowEnable(bool b)         { mShadowsEnable = b; }

        /** ��ȡ�Ƿ�������Ӱ
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const          { return mShadowsEnable; }

        /** �������ģʽ
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);
        
        /** �������ģʽ
        @see RenderSortMode
        */
        void addSort(Nui16 level, RenderSortMode om);

        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);
        
        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        void removeSort(Nui16 level, RenderSortMode om);

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