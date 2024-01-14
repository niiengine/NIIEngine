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

#ifndef _NII_FACTORYOBJ_H_
#define _NII_FACTORYOBJ_H_

#include "NiiPreInclude.h"
#include "NiiString.h"

namespace NII
{
    /** 工场类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FactoryObj : public FactoryAlloc
    {
    public:
        virtual ~FactoryObj();

        /** 获取ID
        @version NIIIEngine 3.0.0
        */
        virtual FactoryID getID() const = 0;
        
        /** 获取虚拟文件类型
        @return VFSType 组合
        @version NIIEngine 3.0.0 高级api
        */
        virtual Nmark getVFSType() const;

        /** 从(虚拟)文件系统创建
        @note 如果子类不实现,将返回0
        @version NIIEngine 3.0.0
        */
        virtual void * create(const String & file);

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        virtual void destroy(void * obj) = 0;
    };
}
#endif