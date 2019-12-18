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

#ifndef _NII_COMMAND_OBJ_FUNCTOR_H_
#define _NII_COMMAND_OBJ_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"

namespace NII
{
namespace NII_COMMAND
{
	typedef void(CommandObj::*CmdObjMemberFunc)(const EventArgs *);

    /** 命令对象的成员函数
    @remark
        引擎事件机制中的命令对象独有的成员函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CommandObjFunctor : public Functor
    {
    public:
        CommandObjFunctor(CommandObj * obj, CmdObjMemberFunc mf);

        /// @copydetails Functor::execute
        bool execute(const EventArgs * args);
        
        /// @copydetails Functor::clone
        Functor * clone() const;
    private:
        CommandObj * mObj;
		CmdObjMemberFunc mMethod;
    };
}
}

#endif