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
    /** 渲染过滤器
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderFilter : public RenderAlloc
    {
    public:
        RenderFilter() {}
        virtual ~RenderFilter() {}

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, GeometryObj * obj) = 0;

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(const ShaderCh * ch, const GeometryObjList & obj) = 0;
    };
    
    /** 渲染排序模式
    @ersion NIIEngine 3.0.0
    */
    enum RenderSortMode
    {
        RSM_View_Descending = 0x01, ///< 由视口距离降序
        RSM_View_Ascending  = 0x02, ///< 由视口距离升序
        RSM_Ch_Index        = 0x04, ///< 由渲染通路的先后绘制排序
        RSM_All             = 0x07
    };

    /** 渲染几何
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
    
    /** 渲染排序
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSort : public RenderAlloc
    {
    public:
        typedef vector<RenderCh>::type RenderList;
    public:
        RenderSort();
        ~RenderSort();

        /** 添加
        @version NIIEngine 3.0.0
        */
        void add(ShaderCh * ch, GeometryObj * obj);

        /** 移去
        @version NIIEngine 3.0.0
        */
        void remove(ShaderCh * ch);

        /** 添加排序模式
        @version NIIEngine 3.0.0
        */
        void addSort(RenderSortMode om)         {mSortMark |= om;}

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om)      {mSortMark &= ~om;}

        /** 移去所有
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 依照指定摄象机排序渲染对象
        @version NIIEngine 3.0.0
        */
        void sort(const Camera * cam);

        /** 执行渲染过滤器渲染
        @version NIIEngine 3.0.0
        */
        void render(RenderFilter * filter, RenderSortMode rsm) const;

        /** 合并渲染排序
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
    
    /** 渲染等级组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderLevelGroup : public RenderAlloc
    {
    public:
        RenderLevelGroup(RenderGroup * parent);

        virtual ~RenderLevelGroup();

        /** 添加排序模式
        @version NIIEngine 3.0.0
        */
        virtual void addSort(RenderSortMode rsm);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        virtual void removeSort(RenderSortMode rsm);

        /** 重至排序模式
        @see RenderSortMode
        */
        virtual void resetSort();

        /** 添加渲染
        @version NIIEngine 3.0.0
        */
        virtual void add(GeometryObj * obj, ShaderFusion * sf);

        /** 移去渲染
        @version NIIEngine 3.0.0
        */
        virtual void remove(ShaderCh * ch);

        /** 排序
        @version NIIEngine 3.0.0
        */
        virtual void sort(const Camera * cam);

        /** 清除这个可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** 获取渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getBasic() const { return mBasic; }

        /** 获取渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getAlphaBasic() const { return mAlphaBasic; }

        /** 获取渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderSort & getAlphaSortBasic() const { return mAlphaSortBasic; }

        /** 合并可渲染物群组
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

    /** 渲染组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroup : public RenderAlloc
    {
    public:
        typedef map<Nui16, RenderLevelGroup *, std::less<Nui16> >::type RenderList;
    public:
        RenderGroup(RenderQueue * parent);

        virtual ~RenderGroup();

        /** 设置是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b)         { mShadowsEnable = b; }

        /** 获取是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const          { return mShadowsEnable; }

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);
        
        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(Nui16 level, RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);
        
        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(Nui16 level, RenderSortMode om);

        /** 重至排序模式
        @version NIIEngine 3.0.0
        */
        void resetSort();

        /** 添加渲染
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, ShaderFusion * sf, Nui16 level);

        /** 移去渲染
        @param[in] destroy
        @version NIIEngine 3.0.0
        */
        void clear(bool destroy = false);

        /** 合并渲染组
        @version NIIEngine 3.0.0
        */
        void merge(const RenderGroup * o);

        /** 获取渲染列表
        @version NIIEngine 3.0.0
        */
        inline const RenderList & getRenderList() const { return mRenderList; }
    protected:
        /** 创建渲染等级组实现
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