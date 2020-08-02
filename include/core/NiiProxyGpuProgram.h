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

#ifndef _NII_AlterGpuProgram_H_
#define _NII_AlterGpuProgram_H_

#include "NiiPreInclude.h"
#include "NiiHighLevelGpuProgram.h"
#include "NiiHighLevelGpuProgramFactory.h"

namespace NII
{
    /** 转换着色程序
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProxyGpuProgram : public HighLevelGpuProgram
    {
    public:
        ProxyGpuProgram(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);

        ~ProxyGpuProgram();

        /** 添加备用着色程序
        @version NIIEngine 3.0.0
        */
        void addCandidate(ResourceID rid);

        /** 移去所有备用着色程序
        @version NIIEngine 3.0.0
        */
        void removeAllCandidate();

        /** 获取当前备用着色程序
        @version NIIEngine 3.0.0
        */
        const HighLevelGpuProgram * getCurrent() const;

        /// @copydetails ResourceID::load
        void load();

        /// @copydetails ResourceID::reload
        void reload();

        /// @copydetails ResourceID::unload
        void unload();

        /// @copydetails ResourceID::touch
        void touch(bool load = true);

        /// @copydetails ResourceID::untouch
        void untouch();

        /// @copydetails ResourceID::isLoading
        bool isLoading() const;

        /// @copydetails ResourceID::isReloadable
        bool isReloadable() const;

        /// @copydetails ResourceID::getState
        LoadingState getState() const;

        /// @copydetails ResourceID::getPrc
        PrcID getPrc() const;

        /// @copydetails ResourceID::getOriginID
        ResourceID getOriginID() const;
        
        /// @copydetails ResourceID::getSize
        NCount getSize() const;

        /// @copydetails ResourceID::getOriginSize
        NCount getOriginSize() const;

        /// @copydetails ResourceID::setLoadScheme
        void setLoadScheme(ResLoadScheme * rls);

        /// @copydetails ResourceID::getLoadScheme
        ResLoadScheme * getLoadScheme() const;

        /// @copydetails ResourceID::setResultScheme
        void setResultScheme(ResResultScheme * rrs);

        /// @copydetails ResourceID::getResultScheme
        ResResultScheme * getResultScheme() const;

        /// @copydetails ResourceID::add
        void add(ResourceListener * lis);

        /// @copydetails ResourceID::remove
        void remove(ResourceListener * lis);
        
        /// @copydetails GpuProgram::getLanguage
        ShaderLanguage getLanguage() const;

        /// @copydetails GpuProgram::createParam
        GpuProgramParam * createParam();

        /// @copydetails GpuProgram::getMain
        GpuProgram * getMain();

        /// @copydetails GpuProgram::isValid
        bool isValid() const;

        /// @copydetails GpuProgram::isVertexMatrixFusion
        bool isVertexMatrixFusion() const;

        /// @copydetails GpuProgram::isVertexInterpolation
        bool isVertexInterpolation() const;

        /// @copydetails GpuProgram::getVertexOffsetCount
        NCount getVertexOffsetCount() const;

        /// @copydetails GpuProgram::isVertexTextureFetch
        bool isVertexTextureFetch() const;

        /// @copydetails GpuProgram::createDefaultParam
        GpuProgramParam * createDefaultParam();

        /// @copydetails GpuProgram::getDefaultParam
        GpuProgramParam * getDefaultParam() const;

        /// @copydetails GpuProgram::isEnvColourEnable
        bool isEnvColourEnable() const;

        /// @copydetails GpuProgram::isEnvLightEnable
        bool isEnvLightEnable() const;

        /// @copydetails GpuProgram::isEnvFogEnable
        bool isEnvFogEnable() const;

        /// @copydetails GpuProgram::isEnvSpaceEnable
        bool isEnvSpaceEnable() const;

        /// @copydetails GpuProgram::getErrorCode
        Nui32 getErrorCode() const;
    protected:
        /// @copydetails GpuProgram::init
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails HighLevelGpuProgram::genAssemblerImpl
        void genAssemblerImpl();

        /// @copydetails HighLevelGpuProgram::unloadSysCodeImpl
        void unloadSysCodeImpl();

        /// @copydetails HighLevelGpuProgram::genParamDefImpl
        void genParamDefImpl();

        /// @copydetails GpuProgram::genParamDefImpl
        void loadCodeImpl(const VString & code);
    protected:
        ResourceIDList mCandidateList;
        HighLevelGpuProgram * mCurrent;
    };

    /** 转换着色程序工厂类
    @version NIIEngine 3.0.0
    */
    class AlterGpuProgramFactory : public HighLevelGpuProgramFactory
    {
    public:
        AlterGpuProgramFactory(HighLevelGpuProgramManager * mag);
        ~AlterGpuProgramFactory();

        /// @copydetails FactoryObj::getID
        FactoryID getID() const;

        /// @copydetails HighLevelGpuProgramFactory::getLanguage
        ShaderLanguage getLanguage() const;

        /// @copydetails HighLevelGpuProgramFactory::create
        HighLevelGpuProgram * create(ResourceID rid, GroupID gid);

        /// @copydetails FactoryObj::destroy
        void destroy(void * prog);
    };
}
#endif