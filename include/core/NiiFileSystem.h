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

#ifndef _NII_FileSystem_H_
#define _NII_FileSystem_H_

#include "NiiPreInclude.h"
#include "NiiVFSFactory.h"

namespace NII
{
    /** 本地文件系统
    @par ID 0x03
    @version NIIEngine
    */
    class _EngineAPI FileSystem : public VFS
    {
    public:
        FileSystem(const String & name);
        ~FileSystem();

        /// @copydetails VFS::exists
        bool isExist(const String & file) const;

        /// @copydetails VFS::peek
        void peek();

        /// @copydetails VFS::open
        DataStream * open(const String & file, bool r = true);

        /// @copydetails VFS::create
        DataStream * create(const String & file);

        /// @copydetails VFS::delete
        void remove(const String & filename);

        /// @copydetails VFS::close
        void close();

        /// @copydetails VFS::list
        void list(StringList & out, bool r = true, bool dirs = false) const;

        /// @copydetails VFS::list
        void list(FileInfoList & out, bool r = true, bool dirs = false) const;

        /// @copydetails VFS::find
        void find(StringList & out, const String & pattern, bool r = true, bool dirs = false) const;

        /// @copydetails VFS::find
        void find(FileInfoList & out, const String & pattern, bool r = true, bool dirs = false) const;

        /// @copydetails VFS::isMatchCase
        bool isMatchCase() const;

        /// @copydetails VFS::getModifiedTime
        time_t getModifiedTime(const String & filename);
        
        /** 写入文件
        @version NIIEngine 4.0.0
        */
        static bool wirteFile(const String & path, Nui8 *data, NCount size);
        
        /** 路径斜杠
        @version NIIEngine 4.0.0
        */
        static bool isPathSlash(Nui8 c);
        
        /** 路径存在
        @version NIIEngine 4.0.0
        */
        static bool isPathExist(const String & path);
        
        /** 获取文件大小
        @version NIIEngine 4.0.0
        */
        static Nui32 getFileSize(const String & path);

        /** 设置列举是否忽略隐藏文件.
        @remark 初始化文件资源定位前调用,默认是(忽略隐藏文件)
        @version NIIEngine 3.0.0
        */
        static void setIgnoreHidden(bool set = true);

        /** 获取列举是否忽略隐藏文件.
        @version NIIEngine 3.0.0
        */
        static bool getIgnoreHidden();
    protected:
        /** 实用方法,按匹配样板,在一个路径中检索所有文件
        @param[in] pattern 文件样板(如 *.jpg *.png)
        @param[in] r 是否向下级联目录
        @param[in] dirs 设置为真,如果你需要列出目录而不是文件
        @param[in] slist 检索一个简单的列表
        @param[in] dlist 检索一个细节的列表
        */
        void findFiles(const String & pattern, bool r, bool dirs, StringList * slist, FileInfoList * dlist) const;

        static bool mHiddenFail;
        N_mutex1
    };

    /** 文件系统工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FileSystemArchiveFactory : public VFSFactory
    {
    public:
        FileSystemArchiveFactory();
        virtual ~FileSystemArchiveFactory();

        /// @copydetails FactoryObj::getType
        VFSType getType() const;

        /// @copydetails FactoryObj::create
        VFS * create(const String & prl);

        /// @copydetails FactoryObj::destroy
        void destroy(VFS * obj);
    };
}
#endif