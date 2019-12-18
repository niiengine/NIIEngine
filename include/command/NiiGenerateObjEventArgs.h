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

#ifndef _NII_GENERATE_OBJ_EVENT_ARGS_H_
#define _NII_GENERATE_OBJ_EVENT_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiMechanismArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 生成一个游戏对象需要的参数
    @remark 具体的统筹,不存在虚函数
    */
    class _EngineAPI GenerateObjEventArgs : public MechanismArgs
    {
    public:
		GenerateObjEventArgs(const CommandObj & co, const EventObj & eo,
            void * data, TimeDurMS vtime, bool audestroy) :
			MechanismArgs(co, eo),
			mOperatorData(data),
			mTime(vtime),
			mAutoDestroy(audestroy) {}
        ~GenerateObjEventArgs() {}

        TimeDurMS mTime;                ///< 生成对象的生命周期(单位:秒,0.0为永久对象)
        bool mAutoDestroy;              ///< 是否自动删除不需要的永久对象
        const void * mOperatorData;     ///< 创建对象需要的参数
    };
}
}
#endif