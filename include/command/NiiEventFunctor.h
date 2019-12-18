/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-6-19

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

#ifndef _NII_EVENT_FUNCTOR_H_
#define _NII_EVENT_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiMethodFunctor.h"
#include "NiiCommandObjFunctor.h"
#include "NiiSharedPtr.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 函子实例专用的处理槽
    @remark 这是个接口套类,所以没有虚函数,用于标识这个函数所在体系
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventFunctor : public EventAlloc
    {
        friend class Event;
    public:
        EventFunctor();

		/**
		@remark
		@param[in] o
		@version NIIEngine 3.0.0
		*/
        EventFunctor(Event * obj);

        /**
        @remark
        @param[in] o
        @version NIIEngine 3.0.0
        */
        EventFunctor(const EventFunctor & o);

        /**
        @remark
        @param[in] o
        @version NIIEngine 3.0.0
        */
        EventFunctor(const Event * dst, const EventFunctor & o);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(Functor * func);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(Functor & func);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(const Functor & func);

        /**
        @remark
        @param[in] obj
        @param[in] m
        @version NIIEngine 3.0.0
        */
        EventFunctor(Event * obj, EventMethod m);

        /**
        @remark
        @param[in] obj
        @param[in] mf
        @version NIIEngine 3.000
        */
        EventFunctor(CommandObj * obj, CmdObjMemberFunc mf);

        ~EventFunctor();

        /**
        @param[in] o
        @version NIIEngine 3.0.0
        */
        bool operator ==(const EventFunctor & o) const;

        /**
        @param[in] o
        @version NIIEngine 3.0.0
        */
        bool operator !=(const EventFunctor & o) const;

        /**
        @remark
        @param[in] e
        @version NIIEngine 3.0.0
        */
        inline bool operator()(const EventArgs * arg)
        {
            return mFunc->execute(arg);
        }

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        bool connected() const;

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void disconnect();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void cleanup();
    protected:
        EventFunctor & operator=(const EventFunctor & o);
    protected:
        Event * mObj;
        Functor * mFunc;
    };
}

    typedef SharedPtr<NII_COMMAND::EventFunctor> SignalPtr;
    typedef vector<SignalPtr>::type SignalPtrs;
}
#endif