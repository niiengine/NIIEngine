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

#ifndef _NII_SCRIPT_COMPILER_MANAGER_H_
#define _NII_SCRIPT_COMPILER_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiScriptCompiler.h"
#include "NiiScriptSys.h"

namespace NII
{
    /** 脚本编译管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptCompilerManager : public Singleton<ScriptCompilerManager>,
        public PropertyScriptSys, public ScriptAlloc
    {
    public:
        ScriptCompilerManager();
        virtual ~ScriptCompilerManager();

        /** 添加工厂类
        @version NIIEngine 3.0.0
        */
        bool addFactory(ScriptAnalyzeFactory * obj);

        /** 移去工厂类
        @version NIIEngine 3.0.0
        */
        void removeFactory(ScriptAnalyzeFactory * obj);

        /** 获取工厂类
        @version NIIEngine 3.0.0
        */
        ScriptAnalyzeFactory * getFactory(FactoryID fid) const;

        /** 创建分析器
        @version NIIEngine 3.0.0
        */
        ScriptAnalyze * create(ScriptCompiler * cpl, FactoryID fid, LangID lid) const;

        /** 删除分析器
        @version NIIEngine 3.0.0
        */
        void destroy(ScriptAnalyze * obj);

        /// @copydetails PropertyScriptSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        /// @copydetails PropertyScriptSys::getScriptLevel
        NIIf getScriptLevel() const;
    protected:
        typedef map<FactoryID, ScriptAnalyzeFactory *>::type FactoryList;
    protected:
        FactoryList mFactory;
        ScriptCompiler * mScriptCompiler;
    };
}

#endif