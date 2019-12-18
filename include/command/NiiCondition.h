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

#ifndef _NII_CONDITION_H_
#define _NII_CONDITION_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

namespace NII
{
namespace NII_COMMAND
{
    /** �����ж���
    @remark
        ����ͨ��ʵ�־����reach()���������������Ƿ����
    @par
        �����Ĳ��Ӳ���ǰ��,���ͬ�����ӱ���Ӷ��,ֻ��ѡ�����һ��
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

        /** ���һ������
        @remark
            Ϊ��ȷ���߼���ȷ,ʹ����������������compile,������һ����Ϸ���п�ʼ����
            ��һ���Ѿ����ڵ���������Ӹ���Ĳ�����һ�����������.��Ϊһ���Ѿ���ɵ�
            �����ǲ�����ص�,����32���жϲ���,����Ľ��ᱻ����
        @param[in] id ���ݴ������ID
        @param[in] op �Ƚϲ�������
        @param[in] ex ��Ҫ�Ƚϵ�����,����������ڴ潫������������
        @par ���������Ӻ����Ҫ����reset����
        @version NIIEngine 3.0.0
        */
        void add(EventID id, CmpMode op, EventArgs * ex);

        /** ��ȥһ������
        @remark
            Ϊ��ȷ���߼���ȷ,ʹ����������������compile,������һ����Ϸ���п�ʼ����
            ��һ���Ѿ����ڵ���������Ӹ���Ĳ�����һ�����������.��Ϊһ���Ѿ���ɵ�
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
		Command * mExec;			///< ����������ִ�е�����
        ConditionParams mParams;	///< �����Ĳ���
        bool mAutoReset;			///< ��������������Ƿ��Զ�ɾ��
    };
}
}
#endif