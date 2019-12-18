/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-31

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

#ifndef _NII_HighLevelGpuProgramManager_H_
#define _NII_HighLevelGpuProgramManager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiHighLevelGpuProgram.h"

namespace NII
{
    /** 高层着色程序管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HighLevelGpuProgramManager : public Singleton<HighLevelGpuProgramManager>, 
        public ResourceManager
    {
    public:
        HighLevelGpuProgramManager();
        ~HighLevelGpuProgramManager();

        /** 是否支持高层语法
        @version NIIEngine 3.0.0
        */
        bool isSupport(ShaderLanguage lang);
        
        /** 添加工厂类
        @version NIIEngine 3.0.0
        */
        void add(HighLevelGpuProgramFactory * factory);

        /** 移去工厂类
        @version NIIEngine 3.0.0
        */
        void remove(HighLevelGpuProgramFactory * factory);

        /** 创建高层着色程序
        @note 不加载
        @param[in] rid 标识着色程序的名字
        @param[in] gid 着色程序的资源组名字
        @param[in] lang 代码语言
        @param[in] type 创建的着色程序的类型
        @version NIIEngine 3.0.0
        */
        HighLevelGpuProgram * create(ResourceID rid, GroupID gid,
            ShaderLanguage lang, GpuProgram::ShaderType type);

        /// @copydetails Singleton::getOnly
        static HighLevelGpuProgramManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static HighLevelGpuProgramManager * getOnlyPtr();
    protected:
		/// @copydetails ResourceManager::init
		void init();

        /** 获取
        @version NIIEngine 3.0.0 内部api
        */
        HighLevelGpuProgramFactory * getFactory(ShaderLanguage lang);

        /// @copydetails ResourceManager::createImpl
        Resource * createImpl(ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls, ResourceResultScheme * rs,
                const PropertyData * params);
    protected:
        typedef map<ShaderLanguage, HighLevelGpuProgramFactory *>::type FactoryMap;
    protected:
        FactoryMap mFactList;
    };
}
#endif