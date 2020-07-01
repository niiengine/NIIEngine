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

#ifndef _NII_MEM_DATA_STREAM_H_
#define _NII_MEM_DATA_STREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include <istream>

#define Nsocket int
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
    
    /** Socket������
    @note getData �����������������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SocketDataStream : public DataStream
    {
    public:
        /// ��һ��C�ļ�����д���������
        SocketDataStream(Nsocket socket, const String & name = N_StrBlank, Nmark mode = AM_READ);

        ~SocketDataStream();

        /// @copydetails DataStream::read
        NCount read(void * buf, NCount count) const;

        /// @copydetails DataStream::write
        NCount write(const void * buf, NCount count);

        /// @copydetails DataStream::getData
        Nui8 * getData();

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
        Nsocket mSocket;
    };
    
    /** ������ʵ��Ч����ʵ��StreamSerialiser��һ����,������İ�ȫ�Ա�
        StreamSerialiserҪ��,���ǿ���ͨ���������������ļ��ļ���,���ļ�
        �Ļ���ͷ������˻����ı�,��Щ�ı����� wchar ��ȡ��,����������ļ�
        �Ļ����������,���ļ��Ļ�������
    @remark ͨ���������Ա�֤����ļ�����������
    @par 
    Nui32(�ֽ���ı�ʶ��) Nui32(��Ч���ݵľ����С) Nui32(�����㷨��ʶ��) 
    Nui32(�ļ���ʼ���ļ����ܽ�����ƫ����) Nui32(�ļ���ʼ���ļ�˵����ƫ����)
    1.����ŵ�����(int����)
    2.�������ļ��Ľ���
    3.����������(unsinged int����,һ����������ͬ,�����0�����ް�Ȩ����)
    4.��Ȩ������(unsinged int����,������ڶ��,ÿ��֮��ʹ�ÿո��)
    5.�ļ���ʽ˵��
    6.(��Ч������)
    */
    class EncryptDataStream
    {
    public:
        EncryptDataStream();
        ~EncryptDataStream();
    };
}

#endif