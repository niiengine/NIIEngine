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
    protected:
        typedef map<FactoryID, FileSerializerFactory *>::type FileSerializerFactoryList;
        FileSerializerFactoryList mFileSerializers;
    };
}

#endif