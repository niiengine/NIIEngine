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
    /** ����һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��
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
        DeleteObjEventArgs(const CommandObj & co, const EventObj & eo):
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
                mSrc(po){}

            EventID mMID;               ///< ���������ID,��֪����δ�������
            AttributeOperator mOperator;///< ��������
            void * mSrc;             ///< ������
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

        AttributeOperator mGOperator;   ///< ͬ����Ԫ�����ظ�ʱ�Ĳ���
        Operators mDest;                ///< ��Ҫ���õĲ����б�
    };
}
}

#endif