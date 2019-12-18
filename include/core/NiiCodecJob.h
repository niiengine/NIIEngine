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
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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