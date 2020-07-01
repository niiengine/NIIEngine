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

#ifndef _NII_CODEC_JOB_H_
#define _NII_CODEC_JOB_H_

#include "NiiPreInclude.h"
#include "NiiJob.h"
#include "NiiAny.h"
#include "NiiDataType.h"

namespace NII
{
    /** ����/ѹ�� ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CodecJob : public Job
    {
        friend Codec;
    public:
        enum DataType
        {
            /** �޲���,δ��ʼ��
            @par û�����ݸ�ʽ
            @version NIIEngine 3.0.0
            */
            DT_None = 0,
            /** д�뵽�ļ�
            @par String���ݸ�ʽ
            @version NIIEngine 3.0.0
            */
            DT_File = 0x01,
            /** д�뵽�ڴ�
            @par MemDataStream ** ���ݸ�ʽ
            @version NIIEngine 3.0.0
            */
            DT_Memory = 0x02,
            /** д�뵽�豸
            @version NIIEngine 3.0.0
            */
            DT_Dev = 0x08,
            /** �ϴ���������
            @par String���ݸ�ʽ
            @version NIIEngine 3.0.0
            */
            DT_Net = 0x10
        };
    public:
        CodecJob(NCount total, DataType type, const Any & any);
        virtual ~CodecJob();

        /** ���ü����Ŀ��
        @remark ע������ setDestType �����������
        */
        void setData(const Any & data);

        /** ��ȡ�����Ŀ��
        @remark ע������ setDestType �����������
        */
        const Any & getData() const;

        /** ���ü����Ŀ������
        @remark ע������ setDest �����������
        */
        void setDataType(DataType type);

        /** ��ȡ�����Ŀ������
        @remark ע������ setDest �����������
        */
        DataType getDataType() const;

        /** ִ�е�ǰ����
        @remark
        */
        virtual void go(Nui32 group) = 0;

        /// @copydetails Job::run
        bool run();
    protected:
        /** �����¼�����ʱ
        @param[in] count
        */
        void onRequest(NCount count);
    protected:
        NCount mTotal;                  ///< ����������
        NCount mComplete;               ///< �Ѿ���ɶ�
        DataType mDataType;             ///< Ŀ������        
        Any mData;                      ///< Ŀ��λ��
    };
}
#endif