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

#ifndef _NII_EVENT_H_
#define _NII_EVENT_H_

#include "NiiPreInclude.h"
#include "NiiFunctor.h"
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
        EventFunctor(Event * obj);
		EventFunctor(Functor * func);

        EventFunctor(const EventFunctor & o);
        EventFunctor(const Event * dst, const EventFunctor & o);
        EventFunctor(Functor & func);
        EventFunctor(const Functor & func);

        EventFunctor(Event * obj, EventMethod m);
        EventFunctor(CommandObj * obj, CommandObjFunctor mf);

        ~EventFunctor();

        /**
        @version NIIEngine 3.0.0
        */
        bool operator ==(const EventFunctor & o) const;

        /**
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
    
namespace NII_COMMAND
{
    /** �����¼�(����һ����������)
    @version NIIEngine 3.0.0
    */
    class Event : public EventAlloc
    {
    public:
        Event(EventID id);
        virtual ~Event();

        /** ����¼�����
        @param[in] ef ���Ӵ����
        @return �źŴ���
        @version NIIEngine 3.0.0
        */
        SignalPtr add(EventFunctor * ef);

        /** ��ȥ�¼�����
        @param[in] ef �¼������ʵ��
        @version NIIEngine 3.0.0
        */
        void remove(EventFunctor * ef);

        /** ִ������¼�
        @version NIIEngine 3.0.0
        */
        void operator()(const EventArgs * args);
        
        /** �¼��Ƿ�������
        @version NIIEngine 3.0.0
        */
        bool isInvalid() const;

        /** ִ������¼�
        @version NIIEngine 3.0.0
        */
        virtual void execute(const EventArgs * args);
    protected:
        EventID mID;            ///< �¼�ID
        SignalPtrs mSignalList; ///< ���Ӵ�����б�
    };
}
}
#endif