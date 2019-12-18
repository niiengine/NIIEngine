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

#ifndef _NII_MEM_DATA_STREAM_H_
#define _NII_MEM_DATA_STREAM_H_

#include "NiiPreInclude.h"
#include "NiiDataStream.h"

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
}

#endif