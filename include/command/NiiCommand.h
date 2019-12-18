/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_COMMAND_H_
#define _NII_COMMAND_H_

#include "NiiPreInclude.h"
#include "NiiEventFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 命令
    @remark
        函子槽处理集,可以包含多个函子,处理函子可以用于传播执行命令后的影响,由于具
        体命令是属于具体对象,如果想需要传播信息给非彼对象的其他对象可以通过添加处
        理函子操作
    @par 这个类像Event类
    @vesion NIIEngine 3.0.0
    */
    class _EngineAPI Command : public EventAlloc
    {
    public:
        typedef vector<EventFunctor *>::type Funcs;
    public:
        Command();
        virtual ~Command();

        /** 添加一个执行函子,不会检测重复添加问题
        @remark 在命令执行的时候将会同时执行这些函子
        @param[in] func 需要执行的函子槽
        @version NIIEngine 3.0.0
        */
        void add(EventFunctor * func);

        /** 移去一个执行函子,不会检测重复添加问题
        @remark
        @param[in] func 需要删除的函子槽
        @version NIIEngine 3.0.0
        */
        void remove(EventFunctor * func);

        /** 执行这个对象(命令)
        @remark 在命令执行的时候将会同时执行这些函子
        @version NIIEngine 3.0.0
        */
        void operator()(const EventArgs * args);

        /** 命令的核心
        @remark 需要执行的主要部件
        @version NIIEngine 3.0.0
        */
        virtual void main();
    protected:
        /** 当命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onDone(const EventArgs * arg);
    protected:
        Funcs mExecutes;    ///< 需要执行的衍生函子列表
    };
}
}
#endif