/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-30

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

#ifndef _NII_RENDER_GROUP_H_
#define _NII_RENDER_GROUP_H_

#include "NiiPreInclude.h"
#include "NiiRenderSort.h"

namespace NII
{
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
        inline void setShadowEnable(bool b) { mShadowsEnable = b; }

        /** 获取是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const { return mShadowsEnable; }

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

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