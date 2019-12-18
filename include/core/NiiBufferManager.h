/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-2

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

#ifndef _NII_BUFFER_MANAGER_H_
#define _NII_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiTempBufferCtl.h"
#include "NiiBuffer.h"

namespace NII
{
    class TempBufferCtlPrc;
    /** �������������
    @note ֡�������ʱ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BufferManager : public BufferAlloc
    {
    public:
        BufferManager();
        virtual ~BufferManager();

        /** ������ʱ����
        @param[in] type ����,��Ӧ��ʵ�ʵ� BufferType
        @param[in] src Դ����ָ��
        @param[in] ctl ������
        @return ��ʱ����
        @version NIIEngine 3.0.0
        */
        virtual Buffer * allocTemp(const Buffer * src, TempBufferCtl * ctl);

        /** ������ʱ����,����ɾ����ʱ����
        @param[in] temp ��ʱ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual void freeTemp(Buffer * temp);

        /** ������ʱ����,����ɾ����ʱ����
        @param[in] src Դ����ָ��
        @version NIIEngine 3.0.0
        */
        virtual void recoverTemp(const Buffer * src);

        /** ��ʱ��������
        @note ���ض�֡����ȥ��,��������ڽ��Զ��ͷ���ʱ����,����ɾ����ʱ����
        @param[in] temp ��Դ���ݷ����������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void refillTemp(Buffer * temp);

        /** �ͷ���ʱ����
        @param[in] cost �ϴε��ú��Ŵε���ʱ��
        @param[in] recover �Ƿ�ִ�л��ջ���
        */
        virtual void recoverTemp(TimeDurMS cost, bool recover = false);
        
        /** ���ջ���
        @note ����Ҫɾ���Ļ���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void recover(Buffer * obj);
        
        /** ���ջ���
        @remark ɾ��δʹ����ʱ����,ɾ�����л��ջ���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void recover();
    private:
        N_mutex(mBufferRecoverMutex)
        N_mutex(mTempBuffersMutex)
    private:
        typedef multimap<const Buffer *, Buffer *>::type CopyBuffers;
        typedef map<Buffer *, TempBufferCtlPrc *>::type TempBuffers;
    private:
        BufferList mRecoverList;
        TempBuffers mTempBuffers;
        CopyBuffers mCopyBuffers;
        NCount mFrameMark;
    };
}

#endif