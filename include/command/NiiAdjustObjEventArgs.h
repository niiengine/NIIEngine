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

#ifndef _NII_ADJUST_OBJ_EVENTARGS_H_
#define _NII_ADJUST_OBJ_EVENTARGS_H_

#include "NiiPreInclude.h"
#include "NiiMechanismArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 调整一个游戏对象需要的参数
    @remark 具体的统筹,不存在虚函数.
    @par 调整的参数必须都是同一个对象的
    */
    class _EngineAPI AdjustObjEventArgs : public MechanismArgs
    {
    public:
        /// 操作类型
        enum AttributeOperator
        {
            AO_Original,                ///< 不做任何数据操作
            AO_Replace,                 ///< 覆盖原数据(常用)
            AO_Plus,                    ///< 与原数据相加
            AO_Minus,                   ///< 与原数据相减
            AO_Multiply,                ///< 与原数据相乘
            AO_Divide,                  ///< 与原数据相乘
            AO_Average                  ///< 与原数间的均数
            //any most
        };

        /// 调整事件具体执行需要的参数
        class AdjustObjEventExec
        {
        public:
			AdjustObjEventExec(EventID id, AttributeOperator op, void * po) :
                mMID(id),
                mOperator(op),
                mSrc(po){}

            EventID mMID;               ///< 处理方法入口ID,能知道如何处理数据
            AttributeOperator mOperator;///< 操作类型
            void * mSrc;             ///< 操作数
        };

        typedef vector<AdjustObjEventExec>::type Operators;
    public:
        AdjustObjEventArgs(const CommandObj & co, const EventObj & eo,
            AttributeOperator Go) :
			MechanismArgs(co, eo),
			mGOperator(Go) {}
        ~AdjustObjEventArgs() 
		{
			mDest.clear();
		}

        void add(const AdjustObjEventExec & exec) 
		{
			mDest.push_back(exec);
		}

        AttributeOperator mGOperator;   ///< 同个单元操作重复时的操作
        Operators mDest;                ///< 需要设置的参数列表
    };

}
}

#endif