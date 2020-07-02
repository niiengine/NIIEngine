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

#ifndef _NII_RenderFrameListener_H_
#define _NII_RenderFrameListener_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
    /** 帧参数
    @version NIIEngine 3.0.0
    */
    class RenderFrameArgs : public EventArgs
    {
    public:
        RenderFrameArgs();
        virtual ~RenderFrameArgs();

        /** 帧标准
        @note 一般是 14 毫秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS mStandard;

        /** 一帧需要的时间
        @version NIIEngine 3.0.0
        */
        TimeDurMS mActual;

        /** 当前和上次需要的时间
        @note 应该不大于 mActual
        @version NIIEngine 3.0.0
        */
        TimeDurMS mCurrent;
    };

    /** 定义一个可以接收帧事件通知的监听.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderFrameListener
    {
        friend class Engine;
    public:
        RenderFrameListener();
        virtual ~RenderFrameListener();

        /** 当一帧开始渲染时调用.
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onPreRender(const RenderFrameArgs * e);

        /** 渲染主模型区域时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderCore(const RenderFrameArgs * e);

        /** 渲染天空时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderSky(const RenderFrameArgs * e);

        /** 渲染地形时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderTerrain(const RenderFrameArgs * e);

        /** 渲染图形界面时
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onRenderUI(const RenderFrameArgs * e);

        /** 当一帧已经渲染时调用.
        @remark
        @return 返回false 将会终止当前渲染循环机制
        @version NIIEngine 3.0.0
        */
        virtual bool onEndRender(const RenderFrameArgs * e);
    protected:
        bool mEnable;
    };
}
#endif