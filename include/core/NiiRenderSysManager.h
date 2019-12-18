/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_RenderSysManager_H_
#define _NII_RenderSysManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiString.h"
#include "NiiRenderFeature.h"

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

        /// @copydetails Singleton::getOnly
        static RenderSysManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static RenderSysManager * getOnlyPtr();
    protected:
        RenderFeatureSerializer * mSerializer;
        FeatureList mFeatureList;
    };
}
#endif