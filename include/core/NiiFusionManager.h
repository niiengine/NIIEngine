/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-7-16

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

#ifndef _NII_FusionManager_H_
#define _NII_FusionManager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiFusion.h"
#include "NiiViewRectGeo.h"
#include "NiiFrameFusion.h"
#include "NiiFrameShaderOp.h"

namespace NII
{
    /** 帧合成对象管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionManager : public ResourceManager, public Singleton<FusionManager>
    {
        friend class Engine;
    public:
        FusionManager();
        virtual ~FusionManager();

        /** 获取全屏几何
        @version NIIEngine 3.0.0
        */
        GeometryObj * getArea() const;

        /** 添加合成到视口中
        @param[in] vp 视口
        @param[in] rid 合成资源ID
        @param[in] pos 插入位置
        @version NIIEngine 3.0.0
        */
        FrameFusion * attach(Viewport * vp, ResourceID rid, NCount pos = -1);

        /** 从视口中移去合成
        @param[in] vp 视口
        @param[in] rid 合成资源ID
        @version NIIEngine 3.0.0
        */
        void detach(Viewport * vp, ResourceID rid);

        /** 获取视口的合成帧
        @version NIIEngine 3.0.0
        */
        FrameResult * getResult(Viewport * vp);

        /** 获取视口是否存在合成帧
        @version NIIEngine 3.0.0
        */
        bool isResultExist(Viewport * vp) const;

        /** 移去视口的合成帧
        @version NIIEngine 3.0.0
        */
        void removeResult(Viewport * vp);

        /** 启用合成帧
        @version NIIEngine 3.0.0
        */
        void setEnable(Viewport * vp, ResourceID rid, bool set);

        /** 重新构建合成帧
        @version NIIEngine 3.0.0
        */
        void rebuildResult();

        /** 检获纹理
        @version NIIEngine 3.0.0
        */
        Texture * touchFusionTexture(FrameFusion * fusion, ResourceID rid, const String & localName,
            NCount w, NCount h, PixelFormat pf, Nui32 fsaa, const String & fsaaHint, bool sRGB,
                TextureSet & applyList, FrameFusionShader::SourceType scope);

        /** 释放纹理(未引用)
        @version NIIEngine 3.0.0
        */
        void freeFusionTexture(bool all = false);

        /** 注册监听器
        @version NIIEngine 3.0.0
        */
        bool addListener(const String & name, FrameFusionListener * logic);

        /** 移去监听器
        @version NIIEngine 3.0.0
        */
        void removeListener(const String & name);

        /** 获取监听器
        @version NIIEngine 3.0.0
        */
        FrameFusionListener * getListener(const String & name);

        /** 添加合成渲染方式
        @version NIIEngine 3.0.0
        */
        bool addRender(const String & name, FrameFusionRender * render);

        /** 移去合成渲染方式
        @version NIIEngine 3.0.0
        */
        void removeRender(const String & name);

        /** 获取合成渲染方式
        @version NIIEngine 3.0.0
        */
        FrameFusionRender * getRender(const String & name);

        /// @copydetails Resource::removeAll
        void removeAll();

        /// @see ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        /// @copydetails Singleton::getOnly
        static FusionManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static FusionManager * getOnlyPtr();
    protected:
        /** 初始化
        @version NIIEngine 3.0.0
        */
        void init();

        /// @see ResourceManager
        Resource * createImpl(ResourceID rid, GroupID gid,
            ResourceLoadScheme * ls, ResourceResultScheme * rs,
                const PropertyData * params);
    private:
        typedef map<Nui64, Texture *>::type DefineList;
        typedef map<Viewport *, FrameResult *>::type FusionResultList;
        typedef map<String, FrameFusionListener *>::type ListenerList;
        typedef map<String, FrameFusionRender *>::type RenderList;
        typedef map<Nui64, TextureList *>::type TempTextureList;
        typedef map<std::pair<ResourceID, String>, DefineList>::type FusionTextureList;
    private:
        FusionResultList mFusionResultList;
        mutable ViewRectGeo * mRect;
        ListenerList mListenerList;
        RenderList mRenderList;
        TempTextureList mTempTextureList;
        FusionTextureList mFusionTextureList;
    };
}
#endif