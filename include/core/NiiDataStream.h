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

#ifndef _NII_DATASTREAM_H_
#define _NII_DATASTREAM_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 读取和写入数据流.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DataStream : public StreamAlloc
    {
    public:
        enum AccessMode
        {
            AM_READ  = 0x01,    ///< 读取
            AM_WRITE = 0x02,    ///< 写入
            AM_LOCK  = 0x04,    ///< 单向锁定只读/只写
            AM_EVENT = 0x08,    ///< 事件应用中
            AM_PIPE  = 0x10,    ///< 管道
            AM_CLOSE = 0x20,    ///< 自动释放内存
            AM_EXT1  = 0x40,
            AM_EXT2  = 0x80,
            AM_EXT3  = 0x100,
            AM_EXT4  = 0x200,
            AM_EXT5  = 0x400,
            AM_EXT6  = 0x800
        };
    public:
        DataStream(Nmark mode = AM_READ);
        DataStream(const String & name, Nmark mode = AM_READ);
        virtual ~DataStream();

        /** 名字(辅助)
        @version NIIEngine 3.0.0
        */
        const String & getName() const      { return mName; }

        /** 访问模式
        @version NIIEngine 3.0.0
        */
        Nmark getAccessMode() const         { return mMark; }

        /** 跳转
        @version NIIEngine 3.0.0
        */
        virtual void skip(NIIl count) = 0;

        /** 重新定位
        @version NIIEngine 3.0.0
        */
        virtual void seek(NCount pos) = 0;

        /** 当前位置
        @version NIIEngine 3.0.0
        */
        virtual NCount tell() const = 0;

        /** 是否末尾
        @version NIIEngine 3.0.0
        */
        virtual bool end() const = 0;

        /** 数据的总大小(字节)
        @version NIIEngine 3.0.0
        */
        NCount size() const             { return mSize; }

        /** 读取数据
        @param[in] count 字节数量
        @return 读取的字节数量
        */
        virtual NCount read(void * out, NCount count) const;

        /** 写入数据
        @param[in] count 字节数量
        @return 已写入的字节数量
        */
        virtual NCount write(const void * in, NCount count);

        /** 返回字符串
        @remark 返回数据末尾有个0,不返回分割符号
        @param[in] count 最大长度, 不包括终止符
        @param[in] delim 分隔符
        @note 非ascii字符的数据流必须是utf8编码
        @verison NIIEngine 3.0.0
        */
        virtual NCount readLine(Ntchar * out, NCount count, const String & delim = _T("\n")) const;

        /** 返回字符串
        @remark 以换行标志为分割符
        @param[in] trim 是否删除前后空格
        @note 非ascii字符的数据流必须是utf8编码
        @version NIIEngine 3.0.0
        */
        virtual String readLine(bool trim = true) const;

        /** 跳过一行
        @param[in] delim 分隔符
        @return 跳过的字节数量
        @note 非ascii字符的数据流必须是utf8编码
        @version NIIEngine 3.0.0
        */
        virtual NCount skipLine(const String & delim = _T("\n")) const;

        /** 跳过下一个开始框号
        @version NIIEngine 3.0.0
        */
        void skipOpenBrace();

        /** 跳过下一个关闭框号
        @version NIIEngine 3.0.0
        */
        void skipCloseBrace();
        
        /** 获取数据(整体)
        @param[in] out 输出
        @param[in] oft 内存开始
        @note 对调用的线程会产生阻塞,内存使用 N_alloc_t(Nui8, mSize)创建,使用完需要调用 N_free
        @version NIIEngine 3.0.0 高级api
        */
        virtual NCount cloneData(Nui8 *& out, NCount oft = 0);

        /** 关闭流
        @version NIIEngine 3.0.0
        */
        virtual void close() = 0;

        /** 是否可读
        @version NIIEngine 3.0.0
        */
        virtual bool isReadable() const;

        /** 是否可写
        @version NIIEngine 3.0.0
        */
        virtual bool isWriteable() const;
        
        /** 是否系统内存
        @version NIIEngine 3.0.0
        */
        virtual bool isSysMemory() const;

        /** 多字节转单字节
        @version NIIEngine 3.0.0
        */
        static void ws2s(const WString & in, VString & out);

        /** 单字节转多字节
        @version NIIEngine 3.0.0
        */
        static void s2ws(const VString & in, WString & out);

        /** 多字节转单字节
        @version NIIEngine 3.0.0 adv
        */
        static void ws2sByte(const Nwchar * in, NCount incnt, Nchar * out, NCount outcnt);

        /** 多字节转单字节
        @version NIIEngine 3.0.0
        */
        static void s2wsByte(const Nchar * in, NCount incnt, Nwchar * out, NCount outcnt);

        /** 多字节转单字节
        @version NIIEngine 3.0.0
        */
        static void ws2sAppend(const WString & in, VString & out);

        /** 单字节转多字节
        @version NIIEngine 3.0.0
        */
        static void s2wsAppend(const VString & in, WString & out);
    protected:
        String mName;   ///< 名字
        NCount mSize;   ///< 数据大小
        Nui8 * mTemp;   ///< 辅助数据
        Nmark mMark;    ///< 数据类型
    };

    template <typename T> const DataStream & operator >>(const DataStream & stream, T & out)
    {
        stream.read(static_cast<void *>(&out), sizeof(T));
        return stream;
    }

    template <typename T> DataStream & operator <<(DataStream & stream, const T & in)
    {
        stream.write(static_cast<const void *>(&in), sizeof(T));
        return stream;
    }

    typedef vector<DataStream *>::type DataStreamList;
}
#endif