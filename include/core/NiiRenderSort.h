/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_RENDER_SORT_H_
#define _NII_RENDER_SORT_H_

#include "NiiPreInclude.h"
#include "NiiShaderCh.h"
#include "NiiRenderFilter.h"

namespace NII
{
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
        void addSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

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
}
#endif