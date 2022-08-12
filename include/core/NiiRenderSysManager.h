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

#ifndef _NII_RenderSysManager_H_
#define _NII_RenderSysManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiRenderSys.h"

namespace NII
{
    /** 渲染系统管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSysManager : public Singleton<RenderSysManager>, public ManagerAlloc
    {
    public:
        typedef map<String, RenderFeature *>::type FeatureList;
    public:
        RenderSysManager();
        virtual ~RenderSysManager();

        /** 添加特性
        @note 参数2内存交由本类管理
        @version NIIEngine 3.0.0
        */
        void add(const String & name, RenderFeature * spec);

        /** 移去特性
        @version NIIEngine 3.0.0
        */
        void remove(const String & name);

        /** 从文件协议种加载
        @version NIIEngine 3.0.0
        */
        void load(const String & vfs, VFSType type = VFST_Local, bool r = true);
        
        /** 从文件种加载特性
        @version NIIEngine 3.0.0
        */
        RenderFeature * getFeature(const String & name);

        /** 获取特性列表 
        @version NIIEngine 3.0.0
        */
        const FeatureList & getFeatureList() const;
    protected:
        RenderFeatureSerializer * mSerializer;
        FeatureList mFeatureList;
    };
}
#endif