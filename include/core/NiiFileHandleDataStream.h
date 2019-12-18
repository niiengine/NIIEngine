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

#ifndef _NII_FILEHANDLE_DATASTREAM_H_
#define _NII_FILEHANDLE_DATASTREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"

namespace NII
{
    /** �ļ����������
    @remark ����ͨ���ļ�������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileHandleDataStream : public DataStream
    {
    public:
        FileHandleDataStream(FILE * file, const String & name = N_StrBlank,
            Nmark mode = AM_READ);

        ~FileHandleDataStream();

        /// @copydetails DataStream::read
        NCount read(void * out, NCount count) const;

        /// @copydetails DataStream::write
        NCount write(const void * in, NCount count);

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
        FILE * mHandle;
    };
}

#endif