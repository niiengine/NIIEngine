/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-8

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

#ifndef _NII_MEM_DATA_STREAM_H_
#define _NII_MEM_DATA_STREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"

namespace NII
{
    /** ϵͳ�ڴ�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemDataStream : public DataStream
    {
    public:
        MemDataStream();

        /** ���ڴ湹��
        @remark ���������ݿ�
        @param[in] size �����ڴ��Ĵ�С(��λ:�ֽ�)
        @param[in] name ��������
        @param[in] readonly ֻ��
        */
        MemDataStream(NCount size, const String & name = N_StrBlank, bool readonly = false);

        /** �ڴ湹��
        @remark ���������ݸ���
        @param[in] data �ڴ� ʹ�� N_alloc_t ����
        @param[in] size �ڴ���С(��λ:�ֽ�)
        @param[in] name ������
        @param[in] autofree �Զ��ͷ��ڴ�
        @param[in] readonly ֻ��
        @version NIIEngine 3.0.0
        */
        MemDataStream(Nui8 * data, NCount size, const String & name = N_StrBlank,
            bool autofree = false, bool readonly = false);

        /** ������
        @remark �������ݸ���
        @param[in] name ��������
        @param[in] r ֻ��
        */
        MemDataStream(DataStream * src, const String & name = N_StrBlank, bool readonly = false);

        ~MemDataStream();

        /** ��ǰ�ڴ�λ��
        @version NIIEngine 3.0.0
        */
        Nui8 * getPos();

        /** �Ƿ��Զ��ͷ��ڴ�
        @version NIIEngine 3.0.0
        */
        void setAutoFree(bool b);

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        Nui8 * getData() const;

        /// @copydetails DataStream::read
        NCount read(void * out, NCount count)const;

        /// @copydetails DataStream::write
        NCount write(const void * in, NCount count);

        /// @copydetails DataStream::readLine
        NCount readLine(Ntchar * out, NCount count, const String & delim = _T("\n")) const;

        /// @copydetails DataStream::skipLine
        NCount skipLine(const String & delim = _T("\n")) const;

        /// @copydetails DataStream::cloneData
        NCount cloneData(Nui8 *& out, NCount oft = 0);
        
        /// @copydetails DataStream::skip
        void skip(NIIl count);

        /// @copydetails DataStream::seek
        void seek(NCount pos);

        /// @copydetails DataStream::tell
        NCount tell() const;

        /// @copydetails DataStream::end
        bool end() const;

        /// @copydetails DataStream::close
        void close();
        
        /// @copydetails DataStream:isSysMemory
        bool isSysMemory() const;
    protected:
        Nui8 * mData;       ///< ��������ʼ��ָ��
        mutable Nui8 * mPos;///< ��ǰ���ڴ��е�ָ��
        Nui8 * mEnd;        ///< �ڴ��β��ָ��
    };
}

#endif