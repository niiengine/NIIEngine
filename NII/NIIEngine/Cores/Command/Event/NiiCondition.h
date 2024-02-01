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
    /** ����
    @remark
        ���Ӳ۴�����,���԰����������,�������ӿ������ڴ���ִ��������Ӱ��,���ھ�
        �����������ھ������,�������Ҫ������Ϣ���Ǳ˶���������������ͨ�����Ӵ�
        �����Ӳ���
    @par �������Event��
    @vesion NIIEngine 3.0.0
    */
    class _EngineAPI Command : public EventAlloc
    {
    public:
        typedef vector<EventFunctor *>::type Funcs;
    public:
        Command();
        virtual ~Command();

        /** ����һ��ִ�к���,�������ظ���������
        @remark ������ִ�е�ʱ�򽫻�ͬʱִ����Щ����
        @param[in] func ��Ҫִ�еĺ��Ӳ�
        @version NIIEngine 3.0.0
        */
        void add(EventFunctor * func);

        /** ��ȥһ��ִ�к���,�������ظ���������
        @remark
        @param[in] func ��Ҫɾ���ĺ��Ӳ�
        @version NIIEngine 3.0.0
        */
        void remove(EventFunctor * func);

        /** ִ���������(����)
        @remark ������ִ�е�ʱ�򽫻�ͬʱִ����Щ����
        @version NIIEngine 3.0.0
        */
        void operator()(const EventArgs * args);

        /** ����ĺ���
        @remark ��Ҫִ�е���Ҫ����
        @version NIIEngine 3.0.0
        */
        virtual void main();
    protected:
        /** ������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void onDone(const EventArgs * arg);
    protected:
        Funcs mExecutes;    ///< ��Ҫִ�е����������б�
    };

    /** (�ɽ�������Ķ���)�Ĵ�������
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
        /** ����������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onCreated(const GenerateObjEventArgs & arg);
    private:
        GenerateObjEventArgs * mArg;
    };

    /** (�ɽ�������Ķ���)��ɾ������
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
        /** ��ɾ������ִ�к��ͬʱ�����������
        @version NIIEngine 3.0.0
        */
        void onDeleted(const DeleteObjEventArgs & arg);
    private:
        DeleteObjEventArgs * mArg;
    };

    /** (�ɽ�������Ķ���)�ĵ�������������
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
        /** ��������������ִ�к��ͬʱ�����������
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

    /** �����ж���
    @remark
        ����ͨ��ʵ�־����reach()���������������Ƿ����
    @par
        �����Ĳ��Ӳ���ǰ��,���ͬ�����ӱ����Ӷ��,ֻ��ѡ�����һ��
    @par
        ����ͨ���̳������,ʵ�־����reached����,������ǿ����Ҫ������еĲ���,ͬʱ��
        ��ͨ��ʵ�־����reach��nonreach����,ȥ��������ֲ��ԵĲ��Ӵ��,����һ����ͬ
        �����������Ѿ����ӵ�EventObj������,���Ҷ���ҪEventObj::receive�������ȥ��
        ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Condition : public EventAlloc
    {
        friend class MemberFunctor;
        friend class EventObj;
    public:
        /** �����¼�����ִ����Ҫ�Ĳ���
        @version NIIEngine 3.0.0
        */
        class ConditionParam : public EventAlloc
        {
        public:
            /** ���캯��
            @remark
                Ϊ��ȷ����ȷ��,�ڶ�������ʹ���ֶ���̬����,��ɾ��ʱ�����涯̬���
            @param[in] op �Ƚ�����
            @param[in] ex ���ֶ����䶯̬�ڴ�,����ɾ�����������
            */
            ConditionParam(Condition * own, CmpMode op, EventArgs * ex);

            Condition * mOwn;   ///< ����������
            CmpMode mOperator;  ///< �Ƚ���������
            EventArgs * mExpect;///< ������ֵ(�ֶ�����ռ�,���������Զ��ͷ�)
            Nui8 mIndex;        ///< �ڲ�����,Ψһ,�޿�ȱ
        };

        typedef map<EventID, ConditionParam *>::type ConditionParams;
    public:
        Condition();
        Condition(Command * command);
        virtual ~Condition();

        /** ����������ɺ�ִ�е�����
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������,NII�������һ��Conditionִ��һ��Command,��Command�����ǿ���
            ���ڶ�����ӵ�,���������ͬʱִ�ж��Command,��Ѷ��Command�ϳɵ�һ��
            Command��.һ��Conditionִ��һ��Command��������Ч������Ŀ���
        @param[in] co �������
        @par ������󲻹�������������ڴ�
        @version NIIEngine 3.0.0
        */
        void setExec(Command * co);

        /** ����һ������
        @remark
            Ϊ��ȷ���߼���ȷ,ʹ����������������compile,������һ����Ϸ���п�ʼ����
            ��һ���Ѿ����ڵ����������Ӹ���Ĳ�����һ�����������.��Ϊһ���Ѿ���ɵ�
            �����ǲ�����ص�,����32���жϲ���,����Ľ��ᱻ����
        @param[in] id ���ݴ������ID
        @param[in] op �Ƚϲ�������
        @param[in] ex ��Ҫ�Ƚϵ�����,����������ڴ潫������������
        @par ����������Ӻ����Ҫ����reset����
        @version NIIEngine 3.0.0
        */
        void add(EventID id, CmpMode op, EventArgs * ex);

        /** ��ȥһ������
        @remark
            Ϊ��ȷ���߼���ȷ,ʹ����������������compile,������һ����Ϸ���п�ʼ����
            ��һ���Ѿ����ڵ����������Ӹ���Ĳ�����һ�����������.��Ϊһ���Ѿ���ɵ�
            �����ǲ�����ص�,����32���жϲ���,����Ľ��ᱻ����
        @version NIIEngine 3.0.0
        */
        void remove(EventID);

        /** �ҵ�һ������
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������
        @param[in] mid �ܸı�������ӵĺ������ID
        @version NIIEngine 3.0.0
        */
        const ConditionParam * find(EventID mid) const;

        /** ִ�����������������
        @version NIIEngine 3.0.0
        */
        void exec(const EventArgs * args);

        /** �������г����Ĳ���
        @remark �������������,���ᶪʧ�����ڻ����г����Ĳ���
        @version NIIEngine 30.0
        */
        virtual void compile() = 0;
    protected:
        /** ���ø����Ĳ���Ϊ����״̬
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������
        @param[in] cp ��Ҫȥ�����Ĳ���
        @version NIIEngine 3.0.0
        */
        virtual void reach(const ConditionParam * cp) = 0;

        /** �������в���Ϊ����״̬
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������
        */
        virtual void reach() = 0;

        /** ���ø����Ĳ���Ϊ������״̬
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������
        @param[in] id ��Ҫȥ�����Ĳ���
        */
        virtual void fail(const ConditionParam * cp) = 0;

        /** �������в���Ϊ������״̬
        @remark
            ���̰߳�ȫ,�����Ƽ����е��¼�������ͬһ���߳���ִ��,�������Ա�֤ͬ����
            Ч������
        */
        virtual void fail() = 0;

        /** �Ƿ��Ѿ������������
        @remark ÿ����Ч�Ĳ��ӱ仯ʱ�򽫻����һ���������
        */
        virtual bool reached() = 0;
    protected:
        Command * mExec;            ///< ����������ִ�е�����
        ConditionParams mParams;    ///< �����Ĳ���
        bool mAutoReset;            ///< ��������������Ƿ��Զ�ɾ��
    };

    /** �����ж�����Ĭ��ʵ��
    @remark Ĭ�ϵ�ʵ����,�������������в��Ӷ����,�򴥷���������,Ϊ��ʵ�ʲ�����Ч��,
        ϵͳ����һ��������������32���жϲ���
    @version NIIEngine 3.0.0 �ڲ���
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