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