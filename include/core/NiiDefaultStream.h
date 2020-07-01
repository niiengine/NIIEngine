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
    /** 系统内存数据流
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MemDataStream : public DataStream
    {
    public:
        MemDataStream();

        /** 新内存构建
        @remark 产生新数据块
        @param[in] size 创建内存块的大小(单位:字节)
        @param[in] name 流的名字
        @param[in] readonly 只读
        */
        MemDataStream(NCount size, const String & name = N_StrBlank, bool readonly = false);

        /** 内存构建
        @remark 不产生数据副本
        @param[in] data 内存 使用 N_alloc_t 分配
        @param[in] size 内存块大小(单位:字节)
        @param[in] name 流名字
        @param[in] autofree 自动释放内存
        @param[in] readonly 只读
        @version NIIEngine 3.0.0
        */
        MemDataStream(Nui8 * data, NCount size, const String & name = N_StrBlank,
            bool autofree = false, bool readonly = false);

        /** 流构建
        @remark 产生数据副本
        @param[in] name 流的名字
        @param[in] r 只读
        */
        MemDataStream(DataStream * src, const String & name = N_StrBlank, bool readonly = false);

        ~MemDataStream();

        /** 当前内存位置
        @version NIIEngine 3.0.0
        */
        Nui8 * getPos();

        /** 是否自动释放内存
        @version NIIEngine 3.0.0
        */
        void setAutoFree(bool b);

        /** 获取缓存数据
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
        Nui8 * mData;       ///< 数据区域开始的指针
        mutable Nui8 * mPos;///< 当前在内存中的指针
        Nui8 * mEnd;        ///< 内存结尾的指针
    };
    
    /** 文件流
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileStreamDataStream : public DataStream
    {
    public:
        /** 只读流构建
        @param[in] s 只读流
        @param[in] close 自动释放s指针
        */
        FileStreamDataStream(std::ifstream * s, const String & name = N_StrBlank,
            bool close = true);

        /** 读写流构建
        @param[in] s 读写流
        @param[in] close 自动释放s指针
        */
        FileStreamDataStream(std::fstream * s, const String & name= N_StrBlank,
            bool close = true);

        /** 只读流构建
        @param[in] s 只读流
        @param[in] size 流内容大小(单位字节)
        @param[in] close 自动释放s指针
        */
        FileStreamDataStream(std::ifstream * s, NCount size, const String & name = N_StrBlank,
            bool close = true);

        /** 读写流
        @param[in] s 读写流
        @param[in] size 流内容大小(单位 字节)
        @param[in] close 自动释放s指针
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
        std::fstream * mStream;     ///< 文件流引用(读写)
        std::istream * mIStream;    ///< 读取流引用(读取)
    };
    
    /** 文件句柄数据流
    @remark 最普通的文件数据流
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
    
    /** Socket数据流
    @note getData 函数会产生严重阻塞
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SocketDataStream : public DataStream
    {
    public:
        /// 从一个C文件句柄中创建命名流
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
    
    /** 这个类的实际效果其实和StreamSerialiser是一样的,但具体的安全性比
        StreamSerialiser要高,我们可以通过这个类做到输出文件的加密,在文件
        的基本头中输出了基本文本,这些文本是以 wchar 读取的,包含对这个文件
        的基本加密情况,和文件的基本介绍
    @remark 通过这个类可以保证这个文件属个人所有
    @par 
    Nui32(字节序的标识码) Nui32(有效数据的具体大小) Nui32(加密算法标识码) 
    Nui32(文件开始到文件介绍结束的偏移量) Nui32(文件开始到文件说明的偏移量)
    1.引擎号的掩码(int类型)
    2.这里是文件的介绍
    3.所有者名字(unsinged int类型,一般和引擎号相同,如果是0代表无版权问题)
    4.授权者名字(unsinged int类型,如果存在多个,每个之间使用空格格开)
    5.文件格式说明
    6.(有效的数据)
    */
    class EncryptDataStream
    {
    public:
        EncryptDataStream();
        ~EncryptDataStream();
    };
}

#endif