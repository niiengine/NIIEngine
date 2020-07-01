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

#ifndef _NII_COMMAND_H_
#define _NII_COMMAND_H_

#include "NiiPreInclude.h"
#include "NiiEvent.h"

namespace NII
{
namespace NII_COMMAND
{
    /** ����
    @remark
        ���Ӳ۴���,���԰����������,�����ӿ������ڴ���ִ��������Ӱ��,���ھ�
        �����������ھ������,�������Ҫ������Ϣ���Ǳ˶���������������ͨ����Ӵ�
        ���Ӳ���
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

        /** ���һ��ִ�к���,�������ظ��������
        @remark ������ִ�е�ʱ�򽫻�ͬʱִ����Щ����
        @param[in] func ��Ҫִ�еĺ��Ӳ�
        @version NIIEngine 3.0.0
        */
        void add(EventFunctor * func);

        /** ��ȥһ��ִ�к���,�������ظ��������
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
}
}
#endif