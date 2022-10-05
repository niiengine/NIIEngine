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

#ifndef _NII_FRAMEFUSION_H_
#define _NII_FRAMEFUSION_H_

#include "NiiPreInclude.h"
#include "NiiMaterialManager.h"
#include "NiiFusion.h"
#include "NiiTexture.h"
#include "NiiRenderQueue.h"
#include "NiiFrameObjListener.h"
#include "NiiViewport.h"

namespace NII
{
    const NCount RENDER_QUEUE_COUNT = RQG_Count + 1;

    /** FrameFusion监听器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameFusionListener : public FrameAlloc
    {
    public:
        virtual ~FrameFusionListener() {}

        /** 创建时触发.
        @version NIIEngine 3.0.0
        */
        virtual void onCreate(FrameFusion * obj) = 0;

        /** 删除时触发
        @version NIIEngine 3.0.0
        */
        virtual void onDestroy(FrameFusion * obj) = 0;
    };

    /** FrameFusion渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameFusionRender : public FrameAlloc
    {
    public:
        virtual ~FrameFusionRender();

        /** 执行渲染操作
        @vesion NIIEngine 3.0.0
        */
        virtual void execute(SpaceManager * sm, RenderSys * rsys) = 0;

        /** 创建一个自定义合成操作.
        @param[in] fusion
        @param[in] pass
        @version NIIEngine 3.0.0
        */
        virtual FrameFusionRender * createInstance(FrameFusion * fusion, const FusionShaderOp * op);
    };

    /** 合成实例
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameFusion : public FrameAlloc
    {
        friend class FrameResult;
        friend class RectFusionRender;
    public:
        /** 监听器
        @version NIIEngine 3.0.0
        */
        class _EngineAPI Listener
        {
        public:
            virtual ~Listener();

            /** 
            @param[in] shaderchid
            @param[in] obj 材质
            @version NIIEngine 3.0.0
            */
            virtual void notifyMaterialSetup(Nui32 shaderchid, Material * obj);

            /** 
            @param[in] shaderchid
            @param[in] obj 材质
            @version NIIEngine 3.0.0
            */
            virtual void notifyMaterialRender(Nui32 shaderchid, Material * obj);
        };
    public:
        FrameFusion(FrameShader * shader, FrameResult * result);
        virtual ~FrameFusion();

        /** 设置使用的渲染
        @param[in] reuseTextures
        */
        void setShader(FrameShader * shader, bool reuseTextures = true);

        /** 获取合成器混合
        @version NIIEngine 3.0.0
        */
        inline FrameShader * getShader() const    { return mFusionShader; }

        /** 获取合成结果
        @version NIIEngine 3.0.0
        */
        inline FrameResult * getResult() const          { return mResult; }

        /** 获取合成器
        @version NIIEngine 3.0.0
        */
        inline Fusion * getFusion() const               { return mFusion; }

        /** 设置材质策略ID
        @version NIIEngine 3.0.0
        */
        void setScheme(SchemeID sid, bool reusetex = true);

        /** 获取材质策略ID
        @version NIIEngine 3.0.0
        */
        SchemeID getSchemeID() const;

        /** 设置是否启动
        @version NIIEngine 3.0.0
        */
        void setEnable(bool set);

        /** 获取是否启动
        @version NIIEngine 3.0.0
        */
        bool isEnable() const                           { return mEnabled; }

        /** 设置是否活动
        @version NIIEngine 3.0.0
        */
        void setAlive(bool set);

        /** 获取是否活动
        @version NIIEngine 3.0.0
        */
        bool isAlive() const                            { return mAlive; }

        /** 获取纹理实例.
        @version NIIEngine 3.0.0
        */
        Texture * getTexture(const String & name, NCount multiIdx);

        /** 获取纹理ID.
        @version NIIEngine 3.0.0
        */
        ResourceID getTextureID(const String & name, NCount multiIdx = 0);

        /** 获取
        @version NIIEngine 3.0.0
        */
        FrameObj * getFrameObj(const String & name);

        /** 添加监听器
        @version NIIEngine 3.0.0
        */
        void add(Listener * obj);

        /** 移去监听器
        @version NIIEngine 3.0.0
        */
        void remove(Listener * obj);
    private:
        /** 摄像机改变时
        @version NIIEngine 3.0.0
        */
        void onCameraChange(Camera * camera);

        /** 创建纹理
        @version NIIEngine 3.0.0
        */
        void createTexture(bool resize);

        /** 删除纹理
        @version NIIEngine 3.0.0
        */
        void destroyTexture(bool resize, bool clearRef);

        /** 获取
        @version NIIEngine 3.0.0
        */
        void deriveOptions(const FrameShader::FrameDefine * def, bool & sRGB, Nui32 & fsaa, String & fsaaHint);
    private:
        typedef vector<Listener *>::type Listeners;
        typedef map<String, Texture *>::type TextureList;
        typedef map<String, MultiTextureFrame *>::type MultiFrameObjList;
        typedef map<FrameShader::FrameDefine *, Texture *>::type RefTextureList;
    protected:
        Fusion * mFusion;
        FrameResult * mResult;
        FrameShader * mFusionShader;
        Listeners mListeners;
        TextureList mTextureList;
        MultiFrameObjList mMultiFrameObjList;
        RefTextureList mRefTextureList;
        bool mEnabled;
        bool mAlive;
    };
    
    class FrameResultRestore;
    /** 合成结果.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameResult : public FrameObjListener, public ViewportListener, public FrameAlloc
    {
    public:
        typedef vector<FrameFusion *>::type FusionList;
    public:
        FrameResult(Viewport * vp);
        virtual ~FrameResult();

        /** 获取这个链的目标视口
        @version NIIEngine 3.0.0
        */
        Viewport * getViewport()                            { return mViewport; }

        /** 构建合成
        @version NIIEngine 3.0.0
        */
        void build();

        /** 刷新内容
        @version NIIEngine 3.0.0
        */
        void refresh();

        /** 启用合成器
        @version NIIEngine 3.0.0
        */
        void setEnable(Nidx pos, bool set);

        /** 添加合成器
        @version NIIEngine 3.0.0
        */
        FrameFusion * create(Fusion * fusion, Nidx pos = -1, SchemeID sid = 0);

        /** 移去合成器
        @param[in] pos 位置
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx pos = -1);

        /** 移去合成器
        @param[in] fusion 对象
        @version NIIEngine 3.0.0
        */
        void destroy(FrameFusion * fusion);

        /** 获取合成器的数量.
        @version NIIEngine 3.0.0
        */
        NCount getCount()                                   { return mFusionList.size(); }

        /** 移去所有合成器.
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 通过位置获取一个合成器实例.
        @version NIIEngine 3.0.0
        */
        FrameFusion * get(Nidx index);

        /** 通过名字获取一个合成器,返回空如果没找到
        @version NIIEngine 3.0.0
        */
        FrameFusion * get(ResourceID name);

        /** 获取合成器的迭代 在列表先的合成器先应用 后的后应用.
        @version NIIEngine 3.0.0
        */
        const FusionList & getList() const                  { return mFusionList; }

        /** 创建渲染步骤
        @version NIIEngine 3.0.0
        */
        void setupRender(FrameFusionRender * render);

        /** 删除渲染步骤
        @version NIIEngine 3.0.0
        */
        void destroyRender();

        /** 获取前混合
        @version NIIEngine 3.0.0
        */
        FrameFusion * getPre(FrameFusion * fusion, bool activeOnly = true);

        /** 获取后混合
        @version NIIEngine 3.0.0
        */
        FrameFusion * getNext(FrameFusion * fusion, bool activeOnly = true);

        /// @copydetails FrameObjListener::onPreFlush
        virtual void onPreFlush(const FrameObjArgs * args);

        /// @copydetails FrameObjListener::onEndFlush
        virtual void onEndFlush(const FrameObjArgs * args);

        /// @copydetails FrameObjListener::onPreViewFlush
        virtual void onPreViewFlush(const FrameViewArgs * args);

        /// @copydetails FrameObjListener::onEndViewFlush
        virtual void onEndViewFlush(const FrameViewArgs * args);

        /// @copydetails ViewportListener::onDestroy
        virtual void onDestroy(Viewport * obj);

        /// @copydetails ViewportListener::onCameraChange
        virtual void onCameraChange(Viewport * obj);

        /// @copydetails ViewportListener::onAreaChange
        virtual void onAreaChange(Viewport * obj);
    protected:
        void createResource();
        void destroyResource();
        typedef vector<FrameFusionRender *>::type RenderList;
    protected:
        Viewport * mViewport;
        FrameFusion * mOriginal;
        FrameResultRestore * mFrameResultRestore;
        RenderList mRenderList;
        ResourceID mResourceID;
        FusionList mFusionList;
        SchemeID mSchemeID;
        bool mEnable;
        bool mRefesh;
    };
}
#endif