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

#ifndef _NII_VFSFactory_H_
#define _NII_VFSFactory_H_

#include "NiiPreInclude.h"
#include "NiiVFS.h"

namespace NII
{
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
}
#endif