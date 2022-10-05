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

#ifndef _NII_Fusion_H_
#define _NII_Fusion_H_

#include "NiiPreInclude.h"
#include "NiiTexture.h"
#include "NiiMaterial.h"
#include "NiiRenderSys.h"
#include "NiiRenderQueue.h"
#include "NiiPixelFormat.h"

namespace NII
{
    /** 帧通路
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionShaderOp : public FrameAlloc
    {
    public:
        /**
        @version NIIEngine 3.0.0
        */
        enum OpType
        {
            OT_Clear,           ///< 清理视口
            OT_StencilTest,     ///< 模板测试
            OT_AlphaTest,       ///< 透明度测试
            OT_RenderColour_R,
            OT_RenderColour_G,
            OT_RenderColour_B,
            OT_RenderStencil,   ///< 渲染模板缓存
            OT_RenderAlpha,     ///< 渲染透明度缓存
            OT_RenderScene,     ///< 渲染场景
            OT_RenderQuad,      ///< 渲染完整的场景
            OT_RenderCustom     ///< 渲染自定队列
        };

        typedef std::pair<String, Nui32> OpUnit;
    public:
        FusionShaderOp(FusionShader * ch);
        ~FusionShaderOp();

        /** 获取渲染通路
        @version NIIEngine 3.0.0
        */
        inline FusionShader * getCh() const    { return mCh; }

        /** 获取当前渲染器是否支持
        @version NIIEngine 3.0.0
        */
        bool isValid();

        /** 设置组成通道的类型
        @version NIIEngine 3.0.0
        */
        void setType(OpType type)               { mType = type;}

        /** 获取组成通道的类型
        @version NIIEngine 3.0.0
        */
        OpType getType() const                  { return mType;}

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        void setID(Nui32 id)                    { mID = id; }

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nui32 getID() const                     { return mID; }

        /** 设置这条通道的材质方案.(OT_RenderScene|OT_RenderQuad|OT_RenderCustom)
        @see ShaderFusion::setScheme.
        */
        void setMaterialScheme(SchemeID sid)    { mMaterialScheme = sid;}

        /** 获取这条通道的材质方案.(OT_RenderScene|OT_RenderQuad|OT_RenderCustom)
        @see ShaderFusion::setScheme.
        */
        SchemeID getMaterialScheme() const      { return mMaterialScheme; }

        /** 设置这个通道使用的材质(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 设置这个通道使用的材质(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void setMaterial(Material * mat);

        /** 获取这个通道使用的材质(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        Material * getMaterial() const          { return mMaterial; }

        /** 设置四方面单位化位置[-1;1]x[-1;1]. (OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void setQuadCorners(NIIf left, NIIf top, NIIf right, NIIf bottom);

        /** 获取四方面单位化位置[-1;1]x[-1;1]. (OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        bool getQuadCorners(NIIf & left, NIIf & top, NIIf & right, NIIf & bottom) const;

        /** 设置四方面是否使用无限远截面.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void setQuadFarCorners(bool farCorners, bool farCornersViewSpace);

        /** 获取四方面是否使用无限远截面.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        bool getQuadFarCorners() const          { return mQuadFarCorners;}

        /** 获取四方面是否使用无限远截面视图空间.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        bool getQuadFarCornersViewSpace() const { return mQuadFarCornersViewSpace;}

        /** 设置这个通道首先渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        void setFirstRenderQueue(Nui8 id)       { mFirstRenderQueue = id; }

        /** 设置这个通道首先渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        Nui8 getFirstRenderQueue() const        { return mFirstRenderQueue; }

        /** 设置这个通道最后渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        void setLastRenderQueue(Nui8 id)        { mLastRenderQueue = id;}

        /** 获取这个通道最后渲染的队列(OT_RenderScene)
        @version NIIEngine 3.0.0
        */
        Nui8 getLastRenderQueue() const         { return mLastRenderQueue; }

        /** 设置是否清空缓存.(FBT_COLOUR|FBT_DEPTH)(OT_Clear)
        @param[in] mark FBT_COLOUR|FBT_DEPTH|FBT_STENCIL组合
        @version NIIEngine 3.0.0
        */
        void setClearBuffers(Nui32 mark)        { mClearBuffers = val;}

        /** 获取是否清空缓存.(FBT_COLOUR|FBT_DEPTH)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        Nui32 getClearBuffers() const           { return mClearBuffers; }

        /** 设置清空后的颜色(0,0,0,0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearColour(Colour val)         { mClearColour = val;}

        /** 获取清空后的颜色(0,0,0,0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        const Colour & getClearColour() const   { return mClearColour; }

        /** 设置清理后的深度 (1.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearDepth(NIIf depth)          { mClearDepth = depth; }

        /** 获取清理后的深度 (1.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        NIIf getClearDepth() const              { return mClearDepth;}

        /** 设置清理后的模板值 (0.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearStencil(Nui32 value)       { mClearStencil = value; }

        /** 获取清理后的模板值(0.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        Nui32 getClearStencil() const           { return mClearStencil; }

        /** 设置是否开启模板测试(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilCheck(bool b)            { mStencilCheck = value;}

        /** 获取是否开启模板测试(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        bool getStencilCheck() const            { return mStencilCheck;}

        /** 设置模板测试函数.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilFunc(CmpMode value)      { mStencilFunc = value;}

        /** 获取模板测试函数.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        CmpMode getStencilFunc() const          { return mStencilFunc;}

        /** 设置模板测试值.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilValue(Nui32 value)       { mStencilValue = value;}

        /** 获取模板测试值(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        Nui32 getStencilValue() const           { return mStencilValue;}

        /** 设置模板测试掩码 (OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilMask(Nui32 value)        { mStencilMask = value;}

        /** 获取模板测试掩码.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        Nui32 getStencilMask() const            { return mStencilMask;}

        /** 设置模版失败操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilFailOp(StencilOpType value)  { mStencilFailOp = value; }

        /** 获取模版失败操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilFailOp() const      { return mStencilFailOp;}

        /** 设置模板测试失败深度操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilDepthFailOp(StencilOpType value){ mStencilDepthFailOp = value;}

        /** 获取模板测试失败深度操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilDepthFailOp() const { return mStencilDepthFailOp;}

        /** 设置模版通道操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilPassOp(StencilOpType value)  {mStencilPassOp = value;}

        /** 获取模版通道操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilPassOp() const      { return mStencilPassOp;}

        /** 设置是否双面模板.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilTwoSided(bool value)         { mStencilTwoSided = value;}

        /** 获取是否双面模板.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        bool getStencilTwoSided() const             { return mStencilTwoSided;}

        /** 设置输入纹理(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void setInput(Nidx id, const String & input = StrUtil::WBLANK, Nidx secondidx = 0);

        /** 获取输入值(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        const OpUnit & getInput(Nidx idx) const;

        /** 获取输入使用的数量.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        NCount getInputCount() const;

        /** 清空所有输入值.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        void clearAllInput();

        /** 设置自定义合成通道的类型名字.(OT_RenderCustom)
        @version NIIEngine 3.0.0
        */
        void setCustomType(const String & type) { mCustomType = customType; }

        /** 获取自定义合成通道的类型名字(OT_RenderCustom)
        @version NIIEngine 3.0.0
        */
        const String & getCustomType() const    { return mCustomType; }
    private:
        Nui32 mID;
        FusionShader * mCh;
        String mCustomType;
        OpType mType;
        Material * mMaterial;
        Nui8 mFirstRenderQueue;
        Nui8 mLastRenderQueue;
        SchemeID mMaterialScheme;
        CmpMode mStencilFunc;
        Nui32 mStencilValue;
        Nui32 mStencilMask;
        StencilOpType mStencilFailOp;
        StencilOpType mStencilDepthFailOp;
        StencilOpType mStencilPassOp;
        NIIf mQuadLeft;                 ///< [-1,1]
        NIIf mQuadTop;                  ///< [-1;1]
        NIIf mQuadRight;                ///< [-1;1]
        NIIf mQuadBottom;               ///< [-1;1]
        Nmark mClearBuffers;
        NIIf mClearDepth;
        Colour mClearColour;
        Nui32 mClearStencil;
        OpUnit * mInputs;

        bool mStencilCheck;
        bool mQuadCorner;
        bool mStencilTwoSided;
        bool mQuadFarCorners;
        bool mQuadFarCornersViewSpace;
    };
    
    /** 合成的FrameObj的渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FusionShader : public FrameAlloc
    {
    public:
        typedef vector<FusionShaderOp *>::type OpList;
    public:
        FusionShader(FrameShader * fusion);
        ~FusionShader();

        /** 获取父对象
        @version NIIEngine 3.0.0
        */
        FrameShader * getFusion() const   { return mFusion; }

        /** 获取渲染器是否支持.
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** 设置只绘制一次
        @version NIIEngine 3.0.0
        */
        void setOnce(bool value)            { mOnce = value; }

        /** 获取只绘制一次
        @version NIIEngine 3.0.0
        */
        bool isOnce() const                 { return mOnce;}

        /** 设置是否启用阴影.
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)        { mShadowsEnable = b;}

        /** 获取是否启用阴影.
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const         { return mShadowsEnable; }
        
        /** 设置可见掩码
        @version NIIEngine 3.0.0
        */
        void setVisibleMask(Nmark mark)     { mVisibleMask = mask;}

        /** 获取可见掩码
        @version NIIEngine 3.0.0
        */
        Nmark getVisibleMask()              { return mVisibleMask; }
        
        /** 设置是否使用前个结果
        @version NIEngine 3.0.0
        */
        void setUsePreResult(bool set)      { mInputMode = set; }

        /** 获取是否使用前个结果
        @version NIEngine 3.0.0
        */
        bool isUsePreResult() const         { return mInputMode;}

        /** 设置输出局部纹理名字
        @version NIIEngine 3.0.0
        */
        void setOutputName(const String & out)  { mOutputName = out;}

        /** 获取输出局部纹理名字
        @version NIIEngine 3.0.0
        */
        const String & getOutputName() const    { return mOutputName;}

        /** 设置目标通道的材质方案.
        @version NIIEngine 3.0.0
        */
        void setMaterial(SchemeID sid)      { mMaterialScheme = sid; }

        /** 获取目标通道的材质方案.
        @see ShaderFusion::setScheme.
        */
        SchemeID getMaterial() const        { return mMaterialScheme; }

        /** 设置使用场景LOD偏量
        @version NIIEngine 3.0.0
        */
        void setLodBias(NIIf bias)          { mLodBias = bias; }

        /** 获取使用场景LOD偏量
        @version NIIEngine 3.0.0
        */
        NIIf getLodBias() const             { return mLodBias; }

        /** 创建混合操作.
        @version NIIEngine 3.0.0
        */
        FusionShaderOp * create();

        /** 移去混合操作.
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx idx);

        /** 获取混合操作.
        @version NIIEngine 3.0.0
        */
        FusionShaderOp * get(Nidx idx);

        /** 获取混合操作数量.
        @version NIIEngine 3.0.0
        */
        NCount getCount()                       { return mPasses.size(); }

        /** 移去所有混合操作
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取操作列表
        @version NIIEngine 3.0.0
        */
        const OpList & getList() const          { return mPasses; }
    private:
        FrameShader * mFusion;
        OpList mPasses;
        String mOutputName;
        Nui32 mVisibleMask;
        NIIf mLodBias;
        SchemeID mMaterialScheme;
        bool mOnce;
        bool mInputMode;
        bool mShadowsEnable;
    };
    
    /** 基本组成技术
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameShader : public FrameAlloc
    {
    public:
        /** 合成器的纹理来源
        @version NIIEngine 3.0.0
        */
        enum SourceType
        {
            ST_Temp,
            ST_Fusion,
            ST_Pool
        };

        /** 缓存帧定义
        @version NIIEngine 3.0.0
        */
        class FrameDefine : public FrameAlloc
        {
        public:
            FrameDefine() :
                mWidth(0),
                mHeight(0),
                mWidthFactor(1.0f),
                mHeightFactor(1.0f),
                mFusionSource(0),
                mRasterGroup(1),
                mSourceType(ST_Temp),
                mPool(false),
                mFSAA(true),
                mHWGamma(false) {}
        public:
            String mName;
            SourceType mSourceType;
            ResourceID mFusionSource;
            String mTextureSource;
            NCount mWidth;
            NCount mHeight;
            NIIf mWidthFactor;
            NIIf mHeightFactor;
            PixelFormatList mFormatList;
            Nui16 mRasterGroup;
            bool mPool;
            bool mFSAA;
            bool mHWGamma;
        };
        typedef vector<FusionShader *>::type TargetList;
        typedef vector<FrameDefine *>::type DefineList;
    public:
        FrameShader(Fusion * parent);
        virtual ~FrameShader();

        /** 获取父对象
        @version NIIEngine 3.0.0
        */
        inline Fusion * getParent() const   { return mParent; }

        /** 获取渲染器是否支持
        @version NIIEngine 3.0.0
        */
        bool isSupport(bool degenerate) const;

        /** 设置名字
        @version NIIEngine 3.0.0
        */
        void setName(const String & name)   { mName = name; }

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const      { return mName; }

        /** 设置方案ID
        @version NIIEngine 3.0.0
        */
        void setSchemeID(SchemeID sid)      { mSchemeID = sid; }

        /** 获取方案ID
        @version NIIEngine 3.0.0
        */
        SchemeID getSchemeID() const        { return mSchemeID; }

        /** 创建帧定义
        @param[in] name 名字
        @version NIIEngine 3.0.0
        */
        FrameDefine * createDefine(const String & name);

        /** 移去帧定义
        @version NIIEngine 3.0.0
        */
        void removeDefine(Nidx idx);

        /** 获取帧定义
        @version NIIEngine 3.0.0
        */
        FrameDefine * getDefine(Nidx idx) const;

        /** 获取帧定义
        @version NIIEngine 3.0.0
        */
        FrameDefine * getDefine(const String & name) const;

        /** 获取帧定义数量
        @version NIIEngine 3.0.0
        */
        NCount getDefineCount() const               { return mDefineList.size(); }

        /** 移去所有帧定义
        @version NIIEngine 3.0.0
        */
        void removeAllDefine();

        /** 获取帧定义列表
        @version NIIEngine 3.0.0
        */
        const DefineList & getDefineList() const    { return mDefineList; }

        /** 创建合成通道
        @version NIIEngine 3.0.0
        */
        FusionShader * createCh();

        /** 移去合成通道
        @version NIIEngine 3.0.0
        */
        void removeCh(Nidx idx);

        /** 获取合成通道
        @version NIIEngine 3.0.0
        */
        FusionShader * getCh(Nidx idx) const;

        /** 获取合成通道数量
        @version NIIEngine 3.0.0
        */
        NCount getChCount() const                   { return mTargetList.size(); }

        /** 移去全部目标通道
        @version NIIEngine 3.0.0
        */
        void removeAllCh();

        /** 获取目标渲染列表
        @version NIIEngine 3.0.0
        */
        const TargetList & getTargetList() const    { return mOutputCh;return mTargetList;}

        /** 获取输出混合通道
        @version NIIEngine 3.0.0
        */
        FusionShader * getOutputCh() const         { return mOutputCh;}
    private:
        String mName;
        SchemeID mSchemeID;
        Fusion * mParent;
        FusionShader * mOutputCh;
        DefineList mDefineList;
        TargetList mTargetList;
    };
    
    /** 帧合成对象
    @version NIIEngine 3.0.0
     */
    class _EngineAPI Fusion : public Resource
    {
    public:
        typedef vector<FrameShader *>::type FrameList;
    public:
        Fusion(ResourceID rid, GroupID gid, 
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);
        ~Fusion();

        /** 创建合成帧
        @param[out] obj 创建合成帧
        @version NIIEngine 3.0.0
        */
        FrameShader * create();

        /** 删除指定索引合成帧
        @param[in] index 指定索引
        @version NIIEngine 3.0.0
        */
        void destroy(Nidx index);

        /** 删除所有合成帧
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取合成帧
        @version NIIEngine 3.0.0
        */
        FrameShader * get(Nidx index) const;

        /** 合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const             { return mFusions.size(); }

        /** 获取合成帧数量
        @version NIIEngine 3.0.0
        */
        const FrameList & getList() const   { return mFusions; }

        /** 获取支持的合成帧数量
        @version NIIEngine 3.0.0
        */
        NCount getValidCount() const        { return mValidFusions.size(); }

        /** 获取当前支持的混合
        @version NIIEngine 3.0.0
        */
        const FrameList & geValidList() const{ return mValidFusions; }

        /** 获取支持的合成帧.
        @version NIIEngine 3.0.0
        */
        FrameShader * getValid(Nidx idx) const;

        /** 获取支持合成帧
        @param[in] sid 方案ID
        @version NIIEngine 3.0.0
        */
        FrameShader * getValid(SchemeID sid = 0) const;

        /** 获取渲染纹理
        @param[in] name
        @param[in] multiIdx
        */
        Texture * getTexture(const String & name, Nidx multiIdx) const;

        /** 获取渲染对象
        @version NIIEngine 3.0.0
        */
        FrameObj * getFrameObj(const String & name) const;
    protected:
        /// @see Resource::loadImpl
        void loadImpl();

        /// @see Resource::unloadImpl
        void unloadImpl();

        /// @see Resource::calcSize
        NCount calcSize() const;
    private:
        typedef map<String, Texture *>::type TextureList;
        typedef map<String, MultiTextureFrame *>::type MultiTexFrameList;

        FrameList mFusions;
        FrameList mValidFusions;
        TextureList mTextureList;
        MultiTexFrameList mMultiTexFrameList;
        bool mCompilationRequired;
    };
}
#endif