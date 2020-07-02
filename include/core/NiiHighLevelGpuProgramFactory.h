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

#ifndef _NII_HighLevelGpuProgramFactory_H
#define _NII_HighLevelGpuProgramFactory_H

#include "NiiPreInclude.h"
#include "NiiFactoryObj.h"

namespace NII
{
    /** 高层着色程序工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HighLevelGpuProgramFactory : public FactoryObj
    {
        friend class HighLevelGpuProgramManager;
    public:
        HighLevelGpuProgramFactory(HighLevelGpuProgramManager * mag);
        virtual ~HighLevelGpuProgramFactory();

        /// @copydetails FactoryObj::getVFSType
        virtual Nmark getVFSType() const;

        /** 语言类型
        @version NIIEngine 3.0.0
        */
        virtual ShaderLanguage getLanguage() const = 0;

        /** 创建着色程序
        @version NIIEngine 3.0.0
        */
        virtual HighLevelGpuProgram * create(ResourceID rid, GroupID gid) = 0;
    protected:
        HighLevelGpuProgramManager * mManager;
    };
}
#endif