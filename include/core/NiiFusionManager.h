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

#ifndef _NII_FusionManager_H_
#define _NII_FusionManager_H_

#include "NiiPreInclude.h"
#include "NiiResourceManager.h"
#include "NiiFusion.h"
#include "NiiViewRectGeo.h"
#include "NiiFrameFusion.h"

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
        FusionInstance * getInstance(Viewport * vp);

        /** 获取视口是否存在合成帧
        @version NIIEngine 3.0.0
        */
        bool isInstanceExist(Viewport * vp) const;

        /** 移去视口的合成帧
        @version NIIEngine 3.0.0
        */
        void removeInstance(Viewport * vp);

        /** 启用合成帧
        @version NIIEngine 3.0.0
        */
        void setEnable(Viewport * vp, ResourceID rid, bool set);

        /** 重新构建合成帧
        @version NIIEngine 3.0.0
        */
        void rebuildInstance();

        /** 检获纹理
        @version NIIEngine 3.0.0
        */
        Texture * touchFusionTexture(FrameFusion * fusion, ResourceID rid, const String & localName,
            NCount w, NCount h, PixelFormat pf, Nui32 fsaa, const String & fsaaHint, bool sRGB,
                TextureSet & applyList, FrameShader::SourceType scope);

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
        bool addRender(const String & name, FusionRender * render);

        /** 移去合成渲染方式
        @version NIIEngine 3.0.0
        */
        void removeRender(const String & name);

        /** 获取合成渲染方式
        @version NIIEngine 3.0.0
        */
        FusionRender * getRender(const String & name);

        /// @copydetails Resource::removeAll
        void removeAll();

        /// @see ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);
    protected:
        /** 初始化
        @version NIIEngine 3.0.0
        */
        void init();

        /// @see ResourceManager
        Resource * createImpl(ResourceID rid, GroupID gid,
            ResLoadScheme * ls, ResResultScheme * rs,
                const PropertyData * params);
    private:
        typedef map<Nui64, Texture *>::type DefineList;
        typedef map<Viewport *, FusionInstance *>::type FusionResultList;
        typedef map<String, FrameFusionListener *>::type ListenerList;
        typedef map<String, FusionRender *>::type RenderList;
        typedef map<Nui64, TextureList *>::type TempTextureList;
        typedef map<std::pair<ResourceID, String>, DefineList>::type FusionTextureList;
    private:
        FusionResultList mFusionResultList;
        mutable RectGeo * mRect;
        ListenerList mListenerList;
        RenderList mRenderList;
        TempTextureList mTempTextureList;
        FusionTextureList mFusionTextureList;
    };
}
#endif