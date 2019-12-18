/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

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
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, 
			ScriptTypeID stid = N_CMD_HighLevelGpuProgram, LangID lid = N_PrimaryLang);

        ~HighLevelGpuProgram();

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
        const GpuParamDefs & getParamDef() const;

        /// @copydetails GpuProgram::getMain
        GpuProgram * getMain();
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
    protected:
        GpuProgram * mAssemblerCode;
        bool mGenParamDef;
        bool mSysCodeValid;
    };
}
#endif
