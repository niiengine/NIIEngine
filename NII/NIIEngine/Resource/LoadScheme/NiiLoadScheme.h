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

#ifndef _NII_LOAD_SCHEME_H_
#define _NII_LOAD_SCHEME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ���ز���
    @remark
        ��Ҫ��Ϊ��Э����Դ�������ϵ,��������Ͷ��̵߳Ĺ�ϵ,��ͨ�Ŀ�������߳�ȥ�Ⱥ�
        ���ض��в�����һ���ܺõİ취,ÿ����Դ���Ҳֻ�ܱ�һ���̲߳���,
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI LoadScheme : public PatternAlloc
    {
    public:
        LoadScheme(SchemeID sid);
        virtual ~LoadScheme();

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        void setPendingMode(bool s);

        /**
        @remark
        @version NIIEngine 3.0.0
        */
        bool isPendingMode() const;

        /** ������ز��Դ��ָ��״̬
        @remark
        @return �����Ƿ�õ�����Ӧ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual bool request(LoadingState ls) = 0;

        /** �ڿ�ʼ������ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onStart(LoadingState ls);

        /** ������������ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onRestart(LoadingState ls);

        /** �ڴ���ÿһ�����ڵ�ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onProcess(LoadingState ls);

        /** ���������ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onRequest(LoadingState ls);

        /** ������ֹͣ��ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onStop(LoadingState ls);

        /** ������ֹͣ��ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onPause(LoadingState ls);

        /** ���쳣ֹͣ��ʱ�򴥷�
        @remark
        @param[in] ls ����ʱ��״̬
        @version NIIEngine 3.0.0
        */
        virtual void onSuspend(LoadingState ls);

    protected:
        SchemeID mSID;
    };
}
#endif