/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-8

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_FILE_STREAM_DATASTREAM_H_
#define _NII_FILE_STREAM_DATASTREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"
#include <istream>

namespace NII
{
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
}
#endif