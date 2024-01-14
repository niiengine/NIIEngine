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

#ifndef _NII_EVENT_ARGS_H_
#define _NII_EVENT_ARGS_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ִ�к���,�ຯ��,��������,��Ա����ʱ��Ҫ�Ĳ�����
    @remark
        ���������������Ϊ�������,����ı�һ��������,Ϊ��Ч�ʺܴ���ռ����ĵ�����,��
        Ҫ������ʹ��const &�����γ�Ա����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventArgs : public EventAlloc
    {
    public:
        EventArgs();
        virtual ~EventArgs();
    };

    template <typename NT> class ValueEventArgs : public EventArgs
    {
    public:
        ValueEventArgs(const NT & value) : mValue(value) {}
        ~ValueEventArgs() {}

        const NT & mValue;
    };

    /// �����¼��б�ṹ
    class _EngineAPI MechanismArgs : public EventArgs
    {
    public:
        MechanismArgs(const NII_COMMAND::CommandObj & co, const NII_COMMAND::EventObj & eo);
        virtual ~MechanismArgs();

        const NII_COMMAND::EventObj & mReceiver;     ///< ��������Ķ���
        const NII_COMMAND::CommandObj & mSender;     ///< ��������Ķ����Ŀ���������
    };

    /** ����һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��
    */
    class _EngineAPI GenerateObjEventArgs : public MechanismArgs
    {
    public:
        GenerateObjEventArgs(const NII_COMMAND::CommandObj & co, const NII_COMMAND::EventObj & eo,
            void * data, TimeDurMS vtime, bool audestroy) :
            MechanismArgs(co, eo),
            mOperatorData(data),
            mTime(vtime),
            mAutoDestroy(audestroy) {}
        ~GenerateObjEventArgs() {}

        TimeDurMS mTime;                ///< ���ɶ������������(��λ:��,0.0Ϊ���ö���)
        bool mAutoDestroy;              ///< �Ƿ��Զ�ɾ������Ҫ�����ö���
        const void * mOperatorData;     ///< ����������Ҫ�Ĳ���
    };

    /** ɾ��һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��
    */
    class _EngineAPI DeleteObjEventArgs : public MechanismArgs
    {
    public:
        DeleteObjEventArgs(const NII_COMMAND::CommandObj & co, const NII_COMMAND::EventObj & eo) :
            MechanismArgs(co, eo) {}
        ~DeleteObjEventArgs() {}
    };

    /** ����һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��.
    @par �����Ĳ������붼��ͬһ�������
    */
    class _EngineAPI AdjustObjEventArgs : public MechanismArgs
    {
    public:
        /// ��������
        enum AttributeOperator
        {
            AO_Original,                ///< �����κ����ݲ���
            AO_Replace,                 ///< ����ԭ����(����)
            AO_Plus,                    ///< ��ԭ�������
            AO_Minus,                   ///< ��ԭ�������
            AO_Multiply,                ///< ��ԭ�������
            AO_Divide,                  ///< ��ԭ�������
            AO_Average                  ///< ��ԭ����ľ���
            //any most
        };

        /// �����¼�����ִ����Ҫ�Ĳ���
        class AdjustObjEventExec
        {
        public:
            AdjustObjEventExec(EventID id, AttributeOperator op, void * po) :
                mMID(id),
                mOperator(op),
                mSrc(po) {}

            EventID mMID;               ///< ���������ID,��֪����δ�������
            AttributeOperator mOperator;///< ��������
            void * mSrc;             ///< ������
        };

        typedef vector<AdjustObjEventExec>::type Operators;
    public:
        AdjustObjEventArgs(const NII_COMMAND::CommandObj & co, const NII_COMMAND::EventObj & eo,
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

        AttributeOperator mGOperator;   ///< ͬ����Ԫ�����ظ�ʱ�Ĳ���
        Operators mDest;                ///< ��Ҫ���õĲ����б�
    };
}
#endif