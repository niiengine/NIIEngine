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

#ifndef _NII_VFS_MANAGER_H_
#define _NII_VFS_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiVFS.h"

namespace NII
{
    /** 虚拟文件系统管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VFSManager : public Singleton<VFSManager>, public VFSAlloc
    {
    public:
        typedef map<VFSType, VFSFactory *>::type VFSList;
    public:
        VFSManager();
        virtual ~VFSManager();

        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(VFSFactory * factory);

        /** 移去工厂
        @version NIIEngine 3.0.0
        */
        void remove(VFSFactory * factory);

        /** 加载文件/文件集
        @param[in] protocol 文件协议(实例)
        @param[in] type 档案类型
        @version NIIEngine 3.0.0
        */
        VFS * load(const String & protocol, VFSType type);

        /** 卸载文件/文件集
        @param[in] protocol 文件协议(实例)
        @note 你必须确保在移去它的时候没有正在使用
        @version NIIEngine 3.0.0
        */
        void unload(const String & protocol);

        /// @copydetails Singleton::getOnly
        static VFSManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static VFSManager * getOnlyPtr();
    protected:
        typedef map<String, VFS *>::type VFSInsList;

        VFSList mVFSList;
        VFSInsList mVFSInsList;
    };
}
#endif