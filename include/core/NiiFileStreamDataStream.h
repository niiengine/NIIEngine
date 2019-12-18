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

#ifndef _NII_FILE_STREAM_DATASTREAM_H_
#define _NII_FILE_STREAM_DATASTREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include <istream>

namespace NII
{
    /** �ļ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileStreamDataStream : public DataStream
    {
    public:
        /** ֻ��������
        @param[in] s ֻ����
        @param[in] close �Զ��ͷ�sָ��
        */
        FileStreamDataStream(std::ifstream * s, const String & name = N_StrBlank,
            bool close = true);

        /** ��д������
        @param[in] s ��д��
        @param[in] close �Զ��ͷ�sָ��
        */
        FileStreamDataStream(std::fstream * s, const String & name= N_StrBlank,
            bool close = true);

        /** ֻ��������
        @param[in] s ֻ����
        @param[in] size �����ݴ�С(��λ�ֽ�)
        @param[in] close �Զ��ͷ�sָ��
        */
        FileStreamDataStream(std::ifstream * s, NCount size, const String & name = N_StrBlank,
            bool close = true);

        /** ��д��
        @param[in] s ��д��
        @param[in] size �����ݴ�С(��λ �ֽ�)
        @param[in] close �Զ��ͷ�sָ��
        */
        FileStreamDataStream(std::fstream * s, NCount size, const String & name = N_StrBlank,
            bool close = true);

        ~FileStreamDataStream();

        /// @copydetails DataStream::read
        NCount read(void * out, NCount count) const;

        /// @copydetails DataStream::write
        NCount write(const void * in, NCount count);

        /// @copydetails DataStream::readLine
        NCount readLine(Ntchar * out, NCount count, const String & delim = _T("\n")) const;;

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
    protected:
        std::fstream * mStream;     ///< �ļ�������(��д)
        std::istream * mIStream;    ///< ��ȡ������(��ȡ)
    };
}
#endif