/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
    class _EngineAPI AlterGpuProgram : public HighLevelGpuProgram
    {
    public:
        AlterGpuProgram(ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);

        ~AlterGpuProgram();

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
        void setLoadScheme(ResourceLoadScheme * rls);

        /// @copydetails ResourceID::getLoadScheme
        ResourceLoadScheme * getLoadScheme() const;

        /// @copydetails ResourceID::setResultScheme
        void setResultScheme(ResourceResultScheme * rrs);

        /// @copydetails ResourceID::getResultScheme
        ResourceResultScheme * getResultScheme() const;

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