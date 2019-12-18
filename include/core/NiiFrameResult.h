/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-25

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

#ifndef _NII_FRAMERESULT_H_
#define _NII_FRAMERESULT_H_

#include "NiiPreInclude.h"
#include "NiiFrameObjListener.h"
#include "NiiRenderListener.h"
#include "NiiFrameFusion.h"
#include "NiiFusion.h"
#include "NiiViewport.h"

namespace NII
{
    class FrameResultRestore;
    /** 合成结果.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameResult : public FrameObjListener, public ViewportListener, public FrameAlloc
    {
    public:
        typedef vector<FrameFusion *>::type FusionList;
    public:
        FrameResult(Viewport * vp);
        virtual ~FrameResult();

        /** 获取这个链的目标视口
        @version NIIEngine 3.0.0
        */
        Viewport * getViewport();

        /** 构建合成
        @version NIIEngine 3.0.0
        */
        void build();

        /** 刷新内容
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** 启用合成器
        @version NIIEngine 3.0.0
        */
        void setEnable(Nidx pos, bool set);

        /** 添加合成器
        @version NIIEngine 3.0.0
        */
        FrameFusion * create(Fusion * fusion, Nidx pos = -1, SchemeID sid = 0);

        /** 移去合成器
        @param[in] pos 位置
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx pos = -1);

        /** 移去合成器
        @param[in] fusion 对象
        @version NIIEngine 3.0.0
        */
        void destroy(FrameFusion * fusion);

        /** 获取合成器的数量.
        @version NIIEngine 3.0.0
        */
        NCount getCount();

        /** 移去所有合成器.
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 通过位置获取一个合成器实例.
        @version NIIEngine 3.0.0
        */
        FrameFusion * get(Nidx index);

        /** 通过名字获取一个合成器,返回空如果没找到
        @version NIIEngine 3.0.0
        */
        FrameFusion * get(ResourceID name);

        /** 获取合成器的迭代 在列表先的合成器先应用 后的后应用.
        @version NIIEngine 3.0.0
        */
        const FusionList & getList() const;

        /** 创建渲染步骤
        @version NIIEngine 3.0.0
        */
        void setupRender(FrameFusionRender * render);

        /** 删除渲染步骤
        @version NIIEngine 3.0.0
        */
        void destroyRender();

        /** 获取前混合
        @version NIIEngine 3.0.0
        */
        FrameFusion * getPre(FrameFusion * fusion, bool activeOnly = true);

        /** 获取后混合
        @version NIIEngine 3.0.0
        */
        FrameFusion * getNext(FrameFusion * fusion, bool activeOnly = true);

        /// @copydetails FrameObjListener::onPreFlush
        virtual void onPreFlush(const FrameObjArgs * args);

        /// @copydetails FrameObjListener::onEndFlush
        virtual void onEndFlush(const FrameObjArgs * args);

        /// @copydetails FrameObjListener::onPreViewFlush
        virtual void onPreViewFlush(const FrameViewArgs * args);

        /// @copydetails FrameObjListener::onEndViewFlush
        virtual void onEndViewFlush(const FrameViewArgs * args);

        /// @copydetails ViewportListener::onDestroy
        virtual void onDestroy(Viewport * obj);

        /// @copydetails ViewportListener::onCameraChange
        virtual void onCameraChange(Viewport * obj);

        /// @copydetails ViewportListener::onAreaChange
        virtual void onAreaChange(Viewport * obj);
    protected:
        void createResource();
        void destroyResource();
        typedef vector<FrameFusionRender *>::type RenderList;
    protected:
        Viewport * mViewport;
        FrameFusion * mOriginal;
        FrameResultRestore * mFrameResultRestore;
        RenderList mRenderList;
        ResourceID mResourceID;
        FusionList mFusionList;
        SchemeID mSchemeID;
        bool mEnable;
        bool mRefesh;
    };
}
#endif