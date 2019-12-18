/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-7-22

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_FILE_SERIALIZER_H_
#define _NII_FILE_SERIALIZER_H_

#include "NiiPreInclude.h"
#include "NiiSerializer.h"

namespace NII
{
    // 12字节       + 2字节  + 4字节  + 4字节    + 2字节       + 4字节		    + 2字节    + 4字节
    // NIIEngine300 + 字节序 + UnitID + 对象的ID + 加密压缩成分 + 文件中的成分大小 + 序列版本 + 下一个资源的开始位置(保留)
    #define NII_SERIAL_FILE_COM_SIZE 31

    /** 单一或复合文件成分序列器
    @remarks 导入/导出数据二进制文件(.niiengine文件专用).
    @verion NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializer : public Serializer
    {
        friend class FileSerializerManager;
    public:
        /** 监听器
        @version NIIEngine 3.0.0
        */
        class _EngineAPI Listener : public SerializerAlloc
        {
        public:
            Listener(){}
            virtual ~Listener(){}
        };

        /** 加密与通用格式(前八位是格式描述,后八位是压缩方式）
        @version NIIEngine 3.0.0
        */
        enum Format
        {
            F_EC_None    = 0x0,         ///< 不使用格式和包装

            F_E_AES128   = 0x01,        ///< 128位的AES(非压缩包)
            F_E_AES256   = 0x02,        ///< 256位的AES(7z或zip包)
            F_E_ZIP      = 0x03,        ///< zip格式包装专用
            F_E_LAMZ     = 0x04,        ///< lamz编码
            F_E_INNER    = 0xFF,        ///< 内部加密

            F_C_ZIP      = 0x100,       ///< zip格式包装
            F_C_7Z       = 0x200,       ///< 7z格式包装
            F_C_XZ       = 0x300,       ///< Xz格式包装
            F_C_GZIP     = 0x400,       ///< Gzip格式包装
            F_C_BZ2      = 0x500,       ///< Bz2格式包装
            F_C_INNER    = 0xFF00,      ///< 内部编码格式包装
        };
    public:
        FileSerializer(const DataStream * src, bool flip = false);
        FileSerializer(const FileSerializer & obj);
        virtual ~FileSerializer();

        using Serializer::read;
        using Serializer::write;

        /** 设置监听器
        @version NIIEngine 3.0.0
        */
        void setListener(Listener * obj);

        /** 获取监听器
        @version NIIEngine 3.0.0
        */
        Listener * getListener() const;

        /** 写入文件头部
        @remark .niiengine 文件头
        @version NIIEngine 3.0.0
        */
        virtual void writeFileHeader();

        /** 读取文件头部
        @remark .niiengine 文件头
        @version NIIEngine 3.0.0
        */
		virtual void readFileHeader();

        /** 写入单元头
        @version NIIEngine 3.0.0 高级api
        */
        void writeHeader();

        /** 读取单元头
        @version NIIEngine 3.0.0 高级api
        */
        NCount readHeader();

        /** 扫描下个单元头
        @version NIIEngine 3.0.0
        */
        bool searchHeader();
        
        /** 跳转到下一个单元头
        @note 读取下一个单元头
        @version NIIEngine 3.0.0 高级api
        */
        bool skipHeader();
        
        /** 跳转到下个单元头
        @version NIIEngine 3.0.0
        */
        bool skipToNextUnit();

        /** 设置文件类型
        @note Format 组成,仅在序列写入时使用
        @version NIIEngine 3.0.0
        */
        void setFormat(SerialFormat m);

        /** 获取文件类型
        @note Format 组成,仅在序列读取时使用
        @version NIIEngine 3.0.0
        */
        SerialFormat getFormat() const;

        /** 检测是否支持当前版本
        @version NIIEngine 3.0.0
        */
        virtual bool isVersion(SerialVer ver) const;

        /** 获取Xml成分ID
        @note 子类必须实现
        @version NIIEngine 3.0.0
        */
        virtual SerializerID getUnit() const = 0;

        /** 获取群组ID
        @note 子类必须实现
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const = 0;
    protected:
        FileSerializer();

        /** 写入部件
        @param[in] out 输出对象指针
        @param[in] src 输出流
        @version NIIEngine 3.0.0 高级api
        */
        virtual void readUnit(void * out) = 0;

        /** 读取部件
        @param[in] in 输入对象指针
        @param[in] dst 输出流
        @version NIIEngine 3.0.0 高级api
        */
        virtual void writeUnit(const void * in, SerialVer ver = 30000,
            SerialFormat sf = F_EC_None, Endian emode = E_Native) = 0;

        /** 完成单元写入时调用
        @note 写入完部件后调用
        @version NIIEngine 3.0.0 高级api
        */
        void reviseWrite();
    protected:
        Nui32 mUnitID;          ///< 对应所解析的单元类型(读写)
        Nui32 mObjectID;        ///< 当前部件对象的ID(读写)
        Nui32 mComSize;         ///< 当前部件的总长度(就是序列文件中的大小,不包含压缩未压缩概念)(读写)
        Nui32 mFileSize;        ///< 当前部件的总大小(读写)
        Nui32 mNextPos;
        Nui16 mVersion;         ///< 当前部件序列的版本(读写)
        Nui16 mFormat;          ///< 当前部件的加密压缩(2个成分)(读写)
		Endian mEndian;
        Nui32 mComHeaderPos;    ///< 单元头位置
        Nui32 mChunkSize;       ///< 当前部件成分的长度(读写)
        Listener * mListener;
    };
}
#endif