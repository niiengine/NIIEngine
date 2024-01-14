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

#ifndef _NII_BUFFER_MANAGER_H_
#define _NII_BUFFER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiBuffer.h"

namespace NII
{
    class TempBufferCtlPrc;
    
    /** ��ʱ����
    @remark һ�����ڷ�CPU�洢����ڴ�,��CPU�洢�ڴ���/�ͷ�ռ�õ�ϵͳʱ���
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI TempBufferCtl
    {
    public:
        /** ����Ƶ������
        @version NIIEngine 3.0.0 �߼�api
        */
        enum OpType
        {
            T_ReadLess,     ///< ��������ȡ
            T_ReadMore,     ///< ������ȡ
            T_WriteLess,    ///< ������д��
            T_WriteMore     ///< ����д��
        };
    public:
        TempBufferCtl(bool autoDsy = true);
        virtual ~TempBufferCtl();

        /** �����Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool b)             { mAutoDestroy = b; }

        /** ��ȡ�Ƿ��Զ�ɾ��
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const              { return mAutoDestroy; }

        /** �����Ƶ������
        @note ���ڵ�������洢�ṹ
        @param[in] mark OpType�Ķ���򵥸�ö��
        */
        virtual void task(Nmark mark);

        /** ִ�й���
        @version NIIEngine 3.0.0
        */
        virtual void expire(Buffer * src);
    protected:
        bool mAutoDestroy;
    };
    
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