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

#ifndef _NII_HighLevelGpuProgram_H_
#define _NII_HighLevelGpuProgram_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"

namespace NII
{
    /** 高层着色程序
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HighLevelGpuProgram : public GpuProgram
    {
    public:
        HighLevelGpuProgram(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, 
            ScriptTypeID stid = N_CmdObj_HighLevelGpuProgram, LangID lid = N_PrimaryLang);

        ~HighLevelGpuProgram();

        /**
        @version NIIEngine 3.0.0
        */
        inline void setPreprocessorDefines(const VString & defines) { mPreprocessorDefines = defines; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline const VString& getPreprocessorDefines() const { return mPreprocessorDefines; }

        /** 加载系统级代码
        @version NIIEngine 3.0.0
        */
        virtual void loadSysCode();

        /** 卸载系统级代码
        @version NIIEngine 3.0.0
        */
        virtual void unloadSysCode();

        /// @copydetails GpuProgram::createParam
        GpuProgramParam * createParam();

        /// @copydetails GpuProgram::getParamDef
        const GpuParamDefine & getParamDef() const;

        /// @copydetails GpuProgram::getMain
        GpuProgram * getMain();

        /// Scan the source for \#include and replace with contents from OGRE resources
        static VString _resolveIncludes(const VString& source, Resource* res, const String& fileName);
    protected:
        /** 加载系统级代码实现
        @version NIIEngine 3.0.0
        */
        virtual void loadSysCodeImpl();

        /** 卸载系统级代码实现
        @version NIIEngine 3.0.0
        */
        virtual void unloadSysCodeImpl() = 0;

        /** 生成汇编编码实现
        @version NIIEngine 3.0.0
        */
        virtual void genAssemblerImpl() = 0;

        /** 配置参数
        @version NIIEngine 3.0.0
        */
        virtual void configParamImpl(GpuProgramParam * params);

        /** 生成常量定义/映射
        @version NIIEngine 3.0.0
        */
        virtual void genParamDefImpl() = 0;

        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// in-situ parsing of defines
        static std::vector<std::pair<const char*, const char*>> parseDefines(VString & defines);
    protected:
        GpuProgram * mAssemblerCode;
        VString mPreprocessorDefines;
        bool mGenParamDef;
        bool mSysCodeValid;
    };
}
#endif
