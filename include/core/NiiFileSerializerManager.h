/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_FILE_SERIALIZER_MANAGER_H_
#define _NII_FILE_SERIALIZER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiFileSerializer.h"

namespace NII
{
    /** 文件序列管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileSerializerManager : public Singleton<FileSerializerManager>, public ManagerAlloc
    {
    public:
        FileSerializerManager();
        ~FileSerializerManager();

        /** 读取指定单元
        @version NIIEngine 3.0.0
        */
        void read(DataStream * src, FactoryID fid, Nui32 objid, void * out);

        /** 读取下一个单元
        @version NIIEngine 3.0.0
        */
        void readUnit(DataStream * src, FactoryID & fid, Nui32 & objid, void * out);

        /** 写入下一个单元
        @version NIIEngine 3.0.0
        */
        void write(DataStream * dst, FactoryID fid, Nui32 objid, const void * in,
            SerialVer ver = 30000, SerialFormat sf = FileSerializer::F_EC_None, 
                Endian emode = E_Native);

        /** 添加工厂类
        @param[in] fat
        @version NIIEngine 3.0.0
        */
        bool addFactory(FileSerializerFactory * fat);

        /** 移去工厂类
        @param[in] fid
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** 获取工厂类
        @param[in] fid
        @version NIIEngine 3.0.0
        */
        FileSerializerFactory * getFactory(FactoryID fid) const;

        /** 创建文件序列器
        @version NIIEngine 3.0.0
        */
        FileSerializer * create(FactoryID fid, DataStream * src);

        /** 删除文件序列器
        @version NIIEngine 3.0.0
        */
        void destroy(FileSerializer * obj);

        /// @copydetails Singleton::getOnly
        static FileSerializerManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static FileSerializerManager * getOnlyPtr();
    protected:
        typedef map<FactoryID, FileSerializerFactory *>::type FileSerializerFactoryList;
        FileSerializerFactoryList mFileSerializers;
    };
}

#endif