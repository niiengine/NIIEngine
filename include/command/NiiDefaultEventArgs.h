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

#ifndef _NII_ADJUST_OBJ_EVENTARGS_H_
#define _NII_ADJUST_OBJ_EVENTARGS_H_

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
    
    /** 删除一个游戏对象需要的参数
    @remark 具体的统筹,不存在虚函数
    */
    class _EngineAPI DeleteObjEventArgs : public MechanismArgs
    {
    public:
        DeleteObjEventArgs(const CommandObj & co, const EventObj & eo):
			MechanismArgs(co, eo) {}
        ~DeleteObjEventArgs() {}
    };
    
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