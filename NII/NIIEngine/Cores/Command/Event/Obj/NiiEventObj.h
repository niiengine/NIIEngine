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

#ifndef _NII_EVENT_OBJ_H_
#define _NII_EVENT_OBJ_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiEventManager.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    class Condition;
    class MemberFunctor;

    /** 可接受命令的对象
    @remark 自动类才使用
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventObj
    {
        friend class MemberFunctor;
    public:
        typedef vector<Condition *>::type Conditions;
        typedef vector<MemberFunctor *>::type Mids;
    public:
        EventObj();
        virtual ~EventObj();

        /** 这是驱动机制的方法
        @remark 参数mid所对应的函数必须和参数arg所配对
        @param[in] mid 所引发的处理函数
        @param[in] arg 传输到处理函数的参数
        @version NIIEngine 3.0.0
        */
        void receive(EventID id, const EventArgs * arg);

        /** 创建一个事件功能成员函数
        @remark 必须按指定事件需求重写这个函数
        @param[in] id 事件的ID
        @version NIIEngine 3.0.0
        */
        virtual MemberFunctor * create(EventID id) const = 0;

        /** 删除一个事件功能成员函数
        @remark 必须按指定事件需求重写这个函数
        @param[in] id 事件的ID
        @version NIIEngine 3.0.0
        */
        virtual void destroy(MemberFunctor * func) const = 0;

        /** 添加一个条件
        @remark
            这个条件的内存管理以后将由这个对象负责,而且确保调用这个函数时参数已经不
            需要添加更多的参子了
        @param[in] con 条件对象
        @version NIIEngine 3.0.0
        */
        bool add(Condition * con);

        /** 移去一个条件
        @remark 这个条件的内存管理以后将由这个对象负责
        @param[in] con 条件对象
        @version NIIEngine 3.0.0
        */
        bool remove(Condition * con);

        /** 激活信号
        @remark] 激活指定的对象事件信号
        @param[in] id 事件的句柄
        @version NIIEngine 3.0.0
        */
        bool connect(EventID id);

        /** 禁止信号
        @remark 禁止指定的对象事件信号
        @param[in] id 事件的句柄
        @version NIIEngine 3.0.0
        */
        bool disconnect(EventID id);

        /** 设置是否接收信号
        @remark 第二参数将使状态不可变
        @param[in] s 是否
        @param[in] forever 永久
        @version NIIEngine 3.0.0
        */
        void setReceiveEnable(bool s, bool forever);

        /** 返回是否接收信号
        @version NIIEngine 3.0.0
        */
        bool isReceiveEnable() const;

        /** 返回这个对象是否还有效
        @remark 当这个对象中没有任何函子的时候,就会无效
        @version NIIEngine 3.0.0
        */
        bool isInvalid() const;
    protected:
        /** 条件成立的时候触发
        @remark 这个方法会删除成立的条件占用的内存
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onReach(const ConditionReachArgs * arg);
    protected:
        /** 设置事件信号总个数
        @remark
            设置事件信号总个数,一般是所有继承级别的基类的EvenCount和自身的EventCount
            相加
        @param[in] count 事件数量
        @version NIIEngine 3.0.0 高级api
        */
        void notifyReceive(NCount count);
    protected:
        Conditions mConditions;     ///< 条件集
    private:
        Mids mMids;                 ///< 方法函数集
        BitSet * mEventMarks;        ///< 方法函数集掩码
        bool mEnable;               ///<
    };
}
}
#endif  //__NII_EVENTOBJECT_H__