/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-6-19

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

#ifndef _NII_EVENT_FUNCTOR_H_
#define _NII_EVENT_FUNCTOR_H_

#include "NiiPreInclude.h"
#include "NiiMethodFunctor.h"
#include "NiiCommandObjFunctor.h"
#include "NiiSharedPtr.h"

namespace NII
{
namespace NII_COMMAND
{
    /** ����ʵ��ר�õĴ����
    @remark ���Ǹ��ӿ�����,����û���麯��,���ڱ�ʶ�������������ϵ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventFunctor : public EventAlloc
    {
        friend class Event;
    public:
        EventFunctor();

		/**
		@remark
		@param[in] o
		@version NIIEngine 3.0.0
		*/
        EventFunctor(Event * obj);

        /**
        @remark
        @param[in] o
        @version NIIEngine 3.0.0
        */
        EventFunctor(const EventFunctor & o);

        /**
        @remark
        @param[in] o
        @version NIIEngine 3.0.0
        */
        EventFunctor(const Event * dst, const EventFunctor & o);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(Functor * func);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(Functor & func);

        /**
        @remark
        @param[in] func
        @version NIIEngine 3.0.0
        */
        EventFunctor(const Functor & func);

        /**
        @remark
        @param[in] obj
        @param[in] m
        @version NIIEngine 3.0.0
        */
        EventFunctor(Event * obj, EventMethod m);

        /**
        @remark
        @param[in] obj
        @param[in] mf
        @version NIIEngine 3.000
        */
        EventFunctor(CommandObj * obj, CmdObjMemberFunc mf);

        ~EventFunctor();

        /**
        @param[in] o
        @version NIIEngine 3.0.0
        */
        bool operator ==(const EventFunctor & o) const;

        /**
        @param[in] o
        @version NIIEngine 3.0.0
        */
        bool operator !=(const EventFunctor & o) const;

        /**
        @remark
        @param[in] e
        @version NIIEngine 3.0.0
        */
        inline bool operator()(const EventArgs * arg)
        {
            return mFunc->execute(arg);
        }

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        bool connected() const;

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void disconnect();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void cleanup();
    protected:
        EventFunctor & operator=(const EventFunctor & o);
    protected:
        Event * mObj;
        Functor * mFunc;
    };
}

    typedef SharedPtr<NII_COMMAND::EventFunctor> SignalPtr;
    typedef vector<SignalPtr>::type SignalPtrs;
}
#endif