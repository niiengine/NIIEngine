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

        /** 是否支持指定语法
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
        HighLevelGpuProgram * create(ResourceID rid, GroupID gid, ShaderLanguage lang, GpuProgram::ShaderType type);

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
            ResLoadScheme * ls, ResResultScheme * rs,
                const PropertyData * params);
    protected:
        typedef map<ShaderLanguage, HighLevelGpuProgramFactory *>::type FactoryMap;
    protected:
        FactoryMap mFactList;
    };
}
#endif