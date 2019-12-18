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

#ifndef _NII_DEFAULT_CONDITION_H_
#define _NII_DEFAULT_CONDITION_H_

#include "NiiPreInclude.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 条件判断器的默认实现
    @remark 默认的实现是,假设条件的所有参子都达成,则触发调用命令,为了实际产生的效率,
        系统限制一个条件仅仅能有32组判断参子
    @version NIIEngine 3.0.0 内部类
    */
    class _EngineAPI DefaultCondition : public Condition
    {
    public:
		DefaultCondition();
        DefaultCondition(Command * command);
        ~DefaultCondition();

        ///@copydetails Condition::reset
        void compile();
    protected:
        ///@copydetails Condition::reach
        void reach(const ConditionParam * cp);

        ///@copydetails Condition::reach
        void reach();

        ///@copydetails Condition::fail
        void fail(const ConditionParam * cp);

        ///@copydetails Condition::fail
        void fail();

        ///@copydetails Condition::reached
        bool reached();
    private:
        Nui32 mComparesMark;
    };
}
}
#endif