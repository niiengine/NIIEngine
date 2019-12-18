/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_FUNCTOR_H_
#define _NII_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 事件回调执行的函子类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Functor : public EventAlloc
    {
    public:
        virtual ~Functor();

        /** 运行函子
        @param[in] args 因子
        @version NIIEngine 3.0.0
        */
        inline bool operator()(const EventArgs * args)
        {
            return execute(args);
        }

        /** 运行函子
        @param[in] args 因子
        @note 子类需要重写这个函数
        @version NIIEngine 3.0.0
        */
        virtual bool execute(const EventArgs * args);
        
        /** 产生副本对象
        @remark 
            会产生一个副本,是使用 N_new 创建出来的,所以使用完后需要用 N_delete 销毁
        @note 子类需要重写这个函数
        @version NIIEngine 3.0.0
        */
        virtual Functor * clone() const;
    };

    /// 傀儡
    class _EngineInner DummyFunctor : public Functor
    {
    public:
        DummyFunctor();
        ~DummyFunctor();
        
        ///@copydetails Functor()
        bool execute(const EventArgs * args);
    };
}
}

#endif