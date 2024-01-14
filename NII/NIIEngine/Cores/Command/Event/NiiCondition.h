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

#ifndef _NII_CONDITION_H_
#define _NII_CONDITION_H_

#include "NiiPreInclude.h"
#include "NiiEvent.h"
#include "NiiEventArgs.h"
#include "NiiCommon.h"

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

    /** (可接受命令的对象)的创建命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CreateCommand : public Command
    {
    public:
        CreateCommand();
        ~CreateCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当创建命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onCreated(const GenerateObjEventArgs & arg);
    private:
        GenerateObjEventArgs * mArg;
    };

    /** (可接受命令的对象)的删除命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DeleteCommand : public Command
    {
    public:
        DeleteCommand();
        ~DeleteCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当删除命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onDeleted(const DeleteObjEventArgs & arg);
    private:
        DeleteObjEventArgs * mArg;
    };

    /** (可接受命令的对象)的调整参数的命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ModifyCommand : public Command
    {
    public:
        ModifyCommand();
        ~ModifyCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当调整参数命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onSet(const AdjustObjEventArgs & arg);
    protected:
        AdjustObjEventArgs * mArg;
    };

    class _EngineAPI ConditionReachArgs : public EventArgs
    {
    public:
        ConditionReachArgs(const Condition * co) : mCondition(co) {}
        ~ConditionReachArgs() {}

        const Condition * mCondition;
    };

    /** 条件判断器
    @remark
        可以通过实现具体的reach()操作来决定条件是否成立
    @par
        条件的参子不分前后,如果同个参子被添加多次,只会选择最后一次
    @par
        可以通过继承这个类,实现具体的reached函数,而并非强制需要达成所有的参子,同时可
        以通过实现具体的reach和nonreach函数,去控制这个局部性的参子达成,但有一点相同
        的是条件是已经附加到EventObj对象中,而且都需要EventObj::receive这个函数去监
        听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Condition : public EventAlloc
    {
        friend class MemberFunctor;
        friend class EventObj;
    public:
        /** 调整事件具体执行需要的参数
        @version NIIEngine 3.0.0
        */
        class ConditionParam : public EventAlloc
        {
        public:
            /** 构造函数
            @remark
                为了确保正确性,第二个参数使用手动动态分配,但删除时由引擎动态完成
            @param[in] op 比较类型
            @param[in] ex 请手动分配动态内存,随后的删除由引擎完成
            */
            ConditionParam(Condition * own, CmpMode op, EventArgs * ex);

            Condition * mOwn;   ///< 所属的条件
            CmpMode mOperator;  ///< 比较类型类型
            EventArgs * mExpect;///< 期望数值(手动分配空间,回收则是自动释放)
            Nui8 mIndex;        ///< 内部索引,唯一,无空缺
        };

        typedef map<EventID, ConditionParam *>::type ConditionParams;
    public:
        Condition();
        Condition(Command * command);
        virtual ~Condition();

        /** 设置条件达成后执行的命令
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题,NII引擎设计一个Condition执行一个Command,但Command设置是可以
            存在多个函子的,所以如果想同时执行多个Command,请把多个Command合成到一个
            Command中.一个Condition执行一个Command理论上是效率问题的考虑
        @param[in] co 命令对象
        @par 这个对象不管理这个参数的内存
        @version NIIEngine 3.0.0
        */
        void setExec(Command * co);

        /** 添加一个参子
        @remark
            为了确保逻辑正确,使用这个方法后请调用compile,所以在一个游戏运行开始后再
            向一个已经存在的条件中添加更多的参子是一个错误的做法.因为一个已经达成的
            条件是不可逆回的,而且32组判断参子,多出的将会被忽略
        @param[in] id 数据处理入口ID
        @param[in] op 比较操作类型
        @param[in] ex 需要比较的数据,这个参数的内存将由这个对象管理
        @par 参子完成添加后必须要调用reset函数
        @version NIIEngine 3.0.0
        */
        void add(EventID id, CmpMode op, EventArgs * ex);

        /** 移去一个参子
        @remark
            为了确保逻辑正确,使用这个方法后请调用compile,所以在一个游戏运行开始后再
            向一个已经存在的条件中添加更多的参子是一个错误的做法.因为一个已经达成的
            条件是不可逆回的,而且32组判断参子,多出的将会被忽略
        @version NIIEngine 3.0.0
        */
        void remove(EventID);

        /** 找到一个参子
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题
        @param[in] mid 能改变这个参子的函数入口ID
        @version NIIEngine 3.0.0
        */
        const ConditionParam * find(EventID mid) const;

        /** 执行条件成立后的事务
        @version NIIEngine 3.0.0
        */
        void exec(const EventArgs * args);

        /** 编译所有成立的参子
        @remark 调用这个方法后,将会丢失所有在机制中成立的参子
        @version NIIEngine 30.0
        */
        virtual void compile() = 0;
    protected:
        /** 设置给定的参子为成立状态
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题
        @param[in] cp 需要去操作的参子
        @version NIIEngine 3.0.0
        */
        virtual void reach(const ConditionParam * cp) = 0;

        /** 设置所有参数为成立状态
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题
        */
        virtual void reach() = 0;

        /** 设置给定的参子为不成立状态
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题
        @param[in] id 需要去操作的参子
        */
        virtual void fail(const ConditionParam * cp) = 0;

        /** 设置所有参子为不成立状态
        @remark
            非线程安全,而且推荐所有的事件机制在同一个线程中执行,这样可以保证同步和
            效率问题
        */
        virtual void fail() = 0;

        /** 是否已经达成引发条件
        @remark 每个有效的参子变化时候将会调用一次这个函数
        */
        virtual bool reached() = 0;
    protected:
        Command * mExec;            ///< 条件成立后执行的命令
        ConditionParams mParams;    ///< 条件的参子
        bool mAutoReset;            ///< 完成条件驱动后是否自动删除
    };

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