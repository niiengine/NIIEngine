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

#ifndef _NII_VFS_H_
#define _NII_VFS_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiDataStream.h"
#include <ctime>

namespace NII
{
    /** 虚拟文件系统
    @version NIIEngine 3.0.0
    */
    enum VFSType
    {
        VFST_Unknow         = 0,
        VFST_SMB            = 1,
        VFST_MultiPath      = 2,
        VFST_UPnP           = 3,
        VFST_SFTP           = 4,
        VFST_Pipe           = 5,
        VFST_ISO9660        = 6,
        VFST_Http           = 7,
        VFST_Ftp            = 8,
        VFST_Local          = 9,    ///< 本地文件
        VFST_Rar            = 10,
        VFST_Zip            = 11,
        VFST_ComplexZip     = 12,
        VFST_Archive        = 13,   ///< libarchive
        VFST_UniqueID       = 14,
        VFST_EXT1           = 15,
        VFST_EXT2           = 16,
        VFST_EXT3           = 17,
        VFST_EXT4           = 18,
        VFST_EXT5           = 19,
        VFST_EXT6           = 20,
        VFST_EXT7           = 21,
        VFST_EXT8           = 22,
        VFST_EXT9           = 23,
        VFST_EXT10          = 24
    };
    
    /** 虚拟文件系统工厂类
    @note VFS 类是实例对象,所以需要这个类辅助
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VFSFactory : public VFSAlloc
    {
    public:
        VFSFactory() {}
        virtual ~VFSFactory() {}
        
        /** 类型
        @version NIIEngine 3.0.0
        */
        virtual VFSType getType() const = 0;

        /** 创建实例
        @version NIIEngine 3.0.0
        */
        virtual VFS * create(const String & prl) = 0;

        /** 删除实例
        @version NIIEngine 3.0.0
        */
        virtual void destroy(VFS * obj) = 0;
    };

    /** 虚拟文件系统
    @remark
        这可能是文件/文件夹,压缩归档,还可能远程位置共享在网络上
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VFS : public VFSAlloc
    {
    public:
        VFS(const String & name, VFSType type);
        virtual ~VFS();

        /** 返回这个档案代码类型
        @version NIIEngine 3.0.0
        */
        VFSType getType() const             {return mType;}

        /** 获取协议名字
        @remark 很可能就是个普通的文件名字/路径,也可能是个复杂的 niiengine:// 协议
        @version NIIEngine 3.0.0
        */
        const String & getName() const      {return mName;}

        /** 是否存在
        @param[in] 文件完全名
        @version NIIEngine 3.0.0
        */
        virtual bool isExist(const String & file) const = 0;

        /** 文件创建时间
        @param[in] file 文件完全名
        @version NIIEngine 3.0.0
        */
        virtual time_t getCreateTime(const String & file) const;

        /** 文件修改时间
        @param[in] file 文件完全名
        @version NIIEngine 3.0.0
        */
        virtual time_t getModifiedTime(const String & file) const;

        /** 文件访问时间
        @param[in] file 文件完全名
        @version NIIEngine 3.0.0
        */
        virtual time_t getAccessTime(const String & file) const;

        /** 探载文件
        @remark 优化文件寻找
        @version NIIEngine 3.0.0
        */
        virtual void peek() = 0;

        /** 打开文件流
        @param[in] file 文件完全名
        @param[in] r 只读
        @return 返回对象 DataStream 使用完后用 N_delete 删除
        @version NIIEngine 3.0.0
        */
        virtual DataStream * open(const String & file, bool r = true) = 0;

        /** 创建新文件(或者覆盖现存在的).
        @param[in] file 文件完全名
        @return 返回对象 DataStream 使用完后用 N_delete 删除
        @version NIIEngine 3.0.0
        */
        virtual DataStream * create(const String & file);

        /** 删除文件
        @param[in] file 文件完全名
        @version NIIEngine 3.0.0
        */
        virtual void remove(const String & file);

        /** 关闭文件系统
        @version NIEngine 3.0.0
        */
        virtual void close() = 0;

        /** 查找文件
        @param[in] out 返回文件完全名列表
        @param[in] pattern 文件后缀,可为通配符(*)
        @param[in] r 递归所有路径
        @param[in] dirs 目录路径或文件
        @version NIIEngine 3.0.0
        */
        virtual void find(StringList & out, const String & pattern, bool r = true,
            bool dirs = false)const = 0;

        /** 查找文件信息
        @param[in] pattern 文件后缀,可为通配符(*)
        @param[in] r 递归所有路径
        @param[in] dirs 目录路径或文件
        @version NIIEngine 3.0.0
        */
        virtual void find(FileInfoList & out, const String & pattern, bool r = true,
            bool dirs = false) const = 0;

        /** 列出文件
        @return 返回文件完全名
        @param[in] r 递归所有路径
        @param[in] dirs 目录路径或文件
        @version NIIEngine 3.0.0
        */
        virtual void list(StringList & out, bool r = true, bool dirs = false) const = 0;

        /** 列出文件信息
        @param[in] r 递归所有路径
        @param[in] dirs 目录路径或文件
        @version NIIEngine 3.0.0
        */
        virtual void list(FileInfoList & out, bool r = true, bool dirs = false) const = 0;

        /** 匹配文件名字大小写
        @version NIIEngine 3.0.0
        */
        virtual bool isMatchCase() const = 0;

        /** 是否只读
        @version NIIEngine 3.0.0
        */
        virtual bool isReadOnly() const;

        /** 设置群组
        @version NIIEngine 3.0.0
        */
        void setGroup(GroupID gid)              { mGroup = gid; }

        /** 获取群组
        @version NIIEngine 3.0.0
        */
        GroupID getGroup() const                { return mGroup; }
    protected:
        String mName;       ///< 
        VFSType mType;      ///< 文件系统类型
        GroupID mGroup;     ///< 文件系统所属的组
        bool mReadOnly;     ///< 只读标记
    };
    typedef vector<VFS *>::type VFSList;
    
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
        static bool isPathSlash(Ntchar c);
        
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