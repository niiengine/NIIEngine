/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-29

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_JOB_H_
#define _NII_JOB_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"

namespace NII
{
    /** ������,�������Ϊһ����������Ĺ����߳�
    @remark
        ������������ʹ�õ� Pattern ����,�� Pattern ��������ѯ����,�������������
        �������������Ľ�����������׼ȷ��ĳ�������ٻ���˵�ǳ���������Ŀ��Ʒ�Χ,������
        ʵ���Ե�����.
    @note
        ����һ���������಻Ӧ������ ThreadMain ������,����������� ThreadMain ��
        ����һ��������߳�����,�������������ͬʱ�ɶ����ͬ��ͬ�ľ����߳̿���
    */
    class _EngineAPI Job : public JobAlloc
    {
        friend class ThreadManager;
    public:
        class Identifier
        {
        public:
            virtual ~Identifier(){}
            
            virtual bool isOnly() const { return true; }
        };
        
        enum ProcessStage
        {
            PS_Request,
            PS_Process,
            PS_Response
        };
    public:
        Job(NCount retry);
        virtual ~Job();

        /** ִ������
        @remark ���������ִ�к���
        @version NIIEngine 3.0.0
        */
        virtual void process();

        /** ��ͣ����
        @version NIIEngine 3.0.0
        */
        virtual void pause();

        /** ֹͣ����
        @version NIIEngine 3.0.0
        */
        virtual void stop();

        /** �����Ƿ�ֹͣ
        @version NIIEngine 3.0.0
        */
        bool isStop() const;

        /** ��ȡID
        @note �������0���������û������
        @version NIIEngine 3.0.0
        */
        inline RequestID getID() const { return mID; }

        /** �������Դ���
        @version NIIEngine 3.0.0
        */
        void setRetryCount(NCount c);

        /** ��ȡ���Դ���
        @version NIIEngine 3.0.0
        */
        NCount getRetryCount() const;

        /** ��������ʱ
        @remark ����ӿ�ʼ������ʱ�䳬�������Ľ��Զ���������
        @param[in] ms Ĭ��Ϊ0,��ʾû��ʱ������
        @version nIIEngine 3.0.0
        */
        void setTimeOut(TimeDurMS ms);

        /** ��ȡ����ʱ
        @remark ����ӿ�ʼ������ʱ�䳬�������Ľ��Զ���������
        @return Ĭ��Ϊ0,��ʾû��ʱ������.��λ:����
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeOut() const;

        /** �����Ƿ�ʹ�ö������߳�
        @note ��������δ����ǰ������Ч
        @version NIIEngine 3.0.0
        */
        virtual void setAlone(bool s);

        /** �����Ƿ�ʹ�ö������߳�
        @note ��������δ����ǰ������Ч
        @version NIIEngine 3.0.0
        */
        virtual bool isAlone() const;

        /** ������������ȼ�
        @version NIIEngine 3.0.0
        */
        virtual void setLevel(PrcLevel l);

        /** ��ȡ��������ȼ�
        @version NIIEngine 3.0.0
        */
        virtual PrcLevel getLevel() const;

        /** ��ȡ���ʹ����
        @version NIIEngine 3.0.0
        */
        virtual NIIf getRelUsage() const;

        /** ��ȡ����ʹ����
        @version NIIEngine 3.0.0
        */
        virtual Nui64 getAbsUsage() const;
        
        /** �Ƿ���ȸ���
        @version NIIEngine 4.0.0
        */
        virtual bool equal(const Identifier * id);
    protected:
        Job();
        
        /** ���ڶϵ㵽��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onProcess();

        /** ����/״̬�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onNotify();
        
        /** ���ʱ����
        @version NIIEngine 4.0.0
        */
        virtual void onComplete();
        
        /** ����
        @version NIIEngine 4.0.0
        */
        virtual void onFail();
        
        /** ����ʱ����
        @version NIIEngine 4.0.0
        */
        virtual void onRetry();
    protected:
        RequestID mID;
        TimeDurMS mTimeOut;
        PrcLevel mLevel;
        NCount mRetryCount;
        volatile ProcessStage mStage;
        bool mAborted;
        bool mAlone;
    };
    
    /// ���������ṹ.
    class _EngineAPI JobResult : public JobAlloc
    {
        friend class ThreadManager;
    public:
        JobResult(Job * src, bool complete = true, const String & msg = StrUtil::WBLANK);
        virtual ~JobResult();

        /** �Ƿ��������
        @version NIIEngine 3.0.0
        */
        inline bool isComplete() const { return mSuccess; }

        /** �����������
        @version NIIEngine 3.0.0
        */
        inline const Job * getJob() const { return mJob; }

        /** ��ȡ������Ϣ
        @version NIIEngine 3.0.0
        */
        inline const String & getMessages() const { return mMessages; }

        /** ��ֹ����
        @version NIIEngine 3.0.0
        */
        void abort();

        /** �����ٴ�ִ������
        @version NIIEngine 3.0.0
        */
        void retry(JobPrc * prc);
    private:
        Job * mJob;
        String mMessages;
        bool mSuccess;
        bool mAbort;
        bool mRetry;
    };
    
    /** ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JobPrc
    {
        friend class ThreadManager;
    public:
        JobPrc(bool autodestroy = true) : mAutoDestroy(autodestroy) {}
        virtual ~JobPrc() {}
        
        /** �Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDestroy() const { return mAutoDestroy; }
    protected:
        JobPrc() : mAutoDestroy(true) {}
        
        /** ��������
        @return ���û����Ҫ���̴߳���������򷵻�0
        @version NIIEngine 3.0.0
        */
        virtual JobResult * handle(Job * jop) = 0;

        /** �������
        @version NIIEngine 3.0.0
        */
        virtual void handle(JobResult * result){}
        
        /** ��������ǰ
        @version NIIEngine 3.0.0
        */
        virtual void onPrcBegin(Job * jop){}
        
        /** ���������
        @version NIIEngine 3.0.0
        */
        virtual void onPrcEnd(Job * jop){}
    protected:
        bool mAutoDestroy;
    };
}
#endif