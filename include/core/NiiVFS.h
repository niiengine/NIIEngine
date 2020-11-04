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
        @version NIIEngine 3.0.0
        */
        virtual DataStream * open(const String & file, bool r = true) = 0;

        /** 创建新文件(或者覆盖现存在的).
        @param[in] file 文件完全名
        @return DataStream 操作指针
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
        @param[in] dirs 目录?还是文件
        @version NIIEngine 3.0.0
        */
        virtual void find(StringList & out, const String & pattern, bool r = true,
            bool dirs = false)const = 0;

        /** 查找文件信息
        @param[in] pattern 文件后缀,可为通配符(*)
        @param[in] r 递归所有路径
        @param[in] dirs 目录?还是文件
        @version NIIEngine 3.0.0
        */
        virtual void find(FileInfoList & out, const String & pattern, bool r = true,
            bool dirs = false) const = 0;

        /** 列出文件
        @return 返回文件完全名
        @param[in] r 递归所有路径
        @param[in] dirs 目录?还是文件
        @version NIIEngine 3.0.0
        */
        virtual void list(StringList & out, bool r = true, bool dirs = false) const = 0;

        /** 列出文件信息
        @param[in] r 递归所有路径
        @param[in] dirs 目录?还是文件
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
}
#endif