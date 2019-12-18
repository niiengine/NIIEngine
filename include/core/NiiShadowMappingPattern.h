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

#ifndef NII_SHADOWMAPPING_RENDERPATTERN_H
#define NII_SHADOWMAPPING_RENDERPATTERN_H

#include "NiiPreInclude.h"
#include "NiiRenderPattern.h"
#include "NiiShadowGenCamera.h"
#include "NiiPixelFormat.h"
#include "NiiRenderSort.h"

namespace NII
{
    class ShadowMappingData;
    /** 纹理配置
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowTextureConfig : public ShadowAlloc
    {
    public:
        ShadowTextureConfig();

        bool operator== (const ShadowTextureConfig & o2);
        bool operator!= (const ShadowTextureConfig & o2);
    public:
        Nui32 mWidth;           ///< 宽度
        Nui32 mHeight;          ///< 高度
        Nui32 mFSAA;            ///< 抗锯齿等级
        PixelFormat mFormat;    ///< 格式
        GroupID mGroup;         ///< 群组id
    };

    typedef vector<ShadowTextureConfig>::type ShadowTexCfgList;

    /** 阴影映射渲染处理器
    @version NIIEngine 3.0.0
    */
    class ShadowMappingPattern : public RenderPattern
    {
    public:
        class _EngineAPI Listener
        {
        public:
            void onCast(Light * light, Camera * camera) {}
        };

        /** 纹理阶段
        @version NIIEngine 3.0.0
        */
        enum StageType
        {
            ST_Normal,
            ST_Texture,
            ST_Receive
        };

        /** 光影排序
        @version NIIEngine 3.0.0
        */
        struct LightShadowLess
        {
            _EngineAPI bool operator()(const Light * l1, const Light * l2) const;
        };

        typedef map<const Camera *, const Light *>::type CameraLightList;
    public:
        ShadowMappingPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~ShadowMappingPattern();
    public:
        /// @copydetails RenderPattern::init
        void init();

        /// @copydetails RenderPattern::isValid
        bool isValid();

        /// @copydetails RenderPattern::createQueue
        RenderQueue * createQueue() const;

        /// @copydetails RenderPattern::setCullingMode
        CullingMode setCullingMode(CullingMode mode);

        /// @copydetails RenderPattern::applyTexture
        void applyTexture(const ShaderCh * ch);

        /// @copydetails RenderPattern::test
        virtual bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /// @copydetails RenderPattern::test
        virtual bool testRender(const ShaderCh * ch);

        /// @copydetails RenderPattern::renderShadow
        void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /// @copydetails RenderPattern::apply
        void applyLight(const ShaderCh * ch, const LightList & src, NCount & oft, LightList & app);

        /// @copydetails RenderPattern::sort
        void sortLight(LightList & dest);

        /// @copydetails RenderPattern::find
        void findLight(LightList & dest);

        /// @copydetails RenderPattern::derive
        void deriveShadow(const ShaderCh *& out, const ShaderCh * src);

        /// @copydetails RenderPattern::isRenderShadow
        bool isRenderShadow() const;

        /// @copydetails RenderPattern::isTextureShadow
        bool isTextureShadow() const;
    public:
        /** 设置投射材质
        @version NIIEngine 3.0.0
        */
        void setTextureCasterMaterial(ResourceID rid);

        /** 设置接收材质
        @version NIIEngine 3.0.0
        */
        void setTextureReceiverMaterial(ResourceID rid);

        /** 设置纹理大小
        @param[in] w power of 2.
        @param[in] h power of 2.
        @version NIIEngine 3.0.0
        */
        void setTextureSize(NCount w, NCount h);

        /** 设置阴影纹理格式
        @version NIIEngine 3.0.0
        */
        void setTexturePixelFormat(PixelFormat pf);

        /** 设置阴影纹理各异向性
        @version NIIEngine 3.0.0
        */
        void setTextureFSAA(Nui16 fsaa);

        /** 摄象机与纹理阴影距离的比例.
        @remark the default is 0.6.
        */
        void setExtentOffsetFactor(NIIf oft);

        /** 摄象机与纹理阴影距离的比例
        @version NIIEngine 3.0.0
        */
        NIIf getExtentOffsetFactor() const;

        /** 设置阴影开始渐弱比例距离.
        @remark The default is 0.7
        */
        void setFadeBegin(NIIf start);

        /** 设置阴影结束渐弱的比例距离
        @remark The default is 0.9.
        */
        void setFadeEnd(NIIf end);

        /** 设置是否投射阴影到自己
        @version NIIEngine 3.0.0
        */
        void setSelfShadow(bool b);

        /** 获取是否投射阴影到自己
        @version NIIEngine 3.0.0
        */
        bool isSelfShadow();

        /** 设置纹理属性
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void setTexture(Nidx idx, NCount w, NCount h, PixelFormat pf, Nui16 fsaa = 0,
            GroupID rasterGroup = 1);

        /** 设置纹理属性.
        @param[in] idx
        @version NIIEngine 3.0.0
        */
        void setTexture(Nidx idx, const ShadowTextureConfig & config);

        /** 统筹纹理属性
        @version NIIEngine 3.0.0
        */
        void setTextureConfig(NCount w, NCount h, NCount count, PixelFormat fmt = PF_X8R8G8B8,
            Nui16 fsaa = 0, GroupID rasterGroup = 1);

        /** 给顶索引获得当前关联的阴影纹理.
        @version NIIEngine 3.0.0
        */
        Texture * getTexture(Nidx index) const;

        /** 设置纹理数量
        @version NIIEngine 3.0.0
        */
        void setTextureCount(NCount count);

        /** 设置阴影纹理数量.
        @version NIIEngine 3.0.0 高级api
        */
        void setTextureCount(LightType type, NCount count);

        /** 获取阴影纹理数量
        @version NIIEngine 3.0.0 高级api
        */
        NCount getTextureCount(LightType type) const;

        /** 阴影呈现摄象机
        @version NIIEngine 3.0.0
        */
        void setShadowGenCamera(ShadowGenCamera * obj);

        /** 阴影呈现摄象机
        @version NIIEngine 3.0.0
        */
        const ShadowGenCamera * getShadowGenCamera() const;

        /** 设置监听
        @version NIIEngine
        */
        void setListener(Listener * lis);
    protected:
        /** 创建阴影摄象机
        @version NIIEngine 3.0.0 高级api
        */
        Camera * createCamera(SpaceID sid);

        /** 创建阴影纹理
        @version NIIEngine 3.0.0
        */
        void createTexture();

        /** 删除阴影纹理
        @version NIIEngine 3.0.0
        */
        void destroyTexture();

        /** 渲染阴影纹理队列
        @version NIIEngine 3.0.0 高级api
        */
        void renderTexture(RenderGroup * rg, RenderSortMode om);

        /** 推导接收
        @version NIIEngine 3.0.0 高级api
        */
        virtual ShaderCh * deriveReceiver(const ShaderCh * src);

        /** 推导投射
        @version NIIEngine 3.0.0 高级api
        */
        virtual ShaderCh * deriveCaster(const ShaderCh * src);

        /** 在渲染到纹理时的特殊处理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setTextureFade(NIIf start, NIIf end);

        /** 在渲染到纹理时的特殊处理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setTextureAmbient();

        /** 在渲染到纹理时的特殊处理
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setColourBlend(ShaderChTextureUnit * src);
    private:
        ShadowMappingData * mShaderData;
    protected:
        ShaderCh * mSTCCP;
        ShaderCh * mSTCRP;
        Listener * mListener;
        Texture * mCurrentTexture;
        StageType mRenderStage;
        TextureList mTextures;
        CameraList mCameras;
        MaterialList mMaterials;
        LightList mShadowLights;
        CameraLightList mCamLightList;
        ShadowTexCfgList mTexConfList;
        vector<NCount>::type mTextureIndexList;
        NIIf mFadeBegin;
        NIIf mFadeEnd;
        NIIf mExtentOffsetFactor;
        NCount mTextureCount[3];
        bool mSelfShadow;
        bool mShadowTexValid;
    };
}
#endif