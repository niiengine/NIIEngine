/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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