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

#ifndef _NII_FrameShaderOP_H_
#define _NII_FrameShaderOP_H_

#include "NiiPreInclude.h"
#include "NiiMaterial.h"
#include "NiiRenderSys.h"
#include "NiiRenderQueue.h"

namespace NII
{
    /** 帧通路
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FrameShaderOp : public FrameAlloc
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
        FrameShaderOp(FrameShaderCh * ch);
        ~FrameShaderOp();

        /** 获取渲染通路
        @version NIIEngine 3.0.0
        */
        inline FrameShaderCh * getCh() const { return mCh; }

        /** 获取当前渲染器是否支持
        @version NIIEngine 3.0.0
        */
        bool isValid();

        /** 设置组成通道的类型
        @version NIIEngine 3.0.0
        */
        void setType(OpType type);

        /** 获取组成通道的类型
        @version NIIEngine 3.0.0
        */
        OpType getType() const;

        /** 设置ID
        @version NIIEngine 3.0.0
        */
        void setID(Nui32 id);

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nui32 getID() const;

        /** 设置这条通道的材质方案.(OT_RenderScene|OT_RenderQuad|OT_RenderCustom)
        @see ShaderFusion::setScheme.
        */
        void setMaterialScheme(SchemeID sid);

        /** 获取这条通道的材质方案.(OT_RenderScene|OT_RenderQuad|OT_RenderCustom)
        @see ShaderFusion::setScheme.
        */
        SchemeID getMaterialScheme() const;

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
        Material * getMaterial() const;

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
        bool getQuadFarCorners() const;

        /** 获取四方面是否使用无限远截面视图空间.(OT_RenderQuad)
        @version NIIEngine 3.0.0
        */
        bool getQuadFarCornersViewSpace() const;

        /** 设置这个通道首先渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        void setFirstRenderQueue(Nui8 id);

        /** 设置这个通道首先渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        Nui8 getFirstRenderQueue() const;

        /** 设置这个通道最后渲染的队列 (OT_RenderScene)
        @note applies when OpType is OT_RenderScene
        */
        void setLastRenderQueue(Nui8 id);

        /** 获取这个通道最后渲染的队列(OT_RenderScene)
        @version NIIEngine 3.0.0
        */
        Nui8 getLastRenderQueue() const;

        /** 设置是否清空缓存.(FBT_COLOUR|FBT_DEPTH)(OT_Clear)
        @param[in] mark FBT_COLOUR|FBT_DEPTH|FBT_STENCIL组合
        @version NIIEngine 3.0.0
        */
        void setClearBuffers(Nui32 mark);

        /** 获取是否清空缓存.(FBT_COLOUR|FBT_DEPTH)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        Nui32 getClearBuffers() const;

        /** 设置清空后的颜色(0,0,0,0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearColour(Colour val);

        /** 获取清空后的颜色(0,0,0,0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        const Colour & getClearColour() const;

        /** 设置清理后的深度 (1.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearDepth(NIIf depth);

        /** 获取清理后的深度 (1.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        NIIf getClearDepth() const;

        /** 设置清理后的模板值 (0.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        void setClearStencil(Nui32 value);

        /** 获取清理后的模板值(0.0)(OT_Clear)
        @version NIIEngine 3.0.0
        */
        Nui32 getClearStencil() const;

        /** 设置是否开启模板测试(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilCheck(bool b);

        /** 获取是否开启模板测试(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        bool getStencilCheck() const;

        /** 设置模板测试函数.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilFunc(CmpMode value);

        /** 获取模板测试函数.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        CmpMode getStencilFunc() const;

        /** 设置模板测试值.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilValue(Nui32 value);

        /** 获取模板测试值(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        Nui32 getStencilValue() const;

        /** 设置模板测试掩码 (OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilMask(Nui32 value);

        /** 获取模板测试掩码.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        Nui32 getStencilMask() const;

        /** 设置模版失败操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilFailOp(StencilOpType value);

        /** 获取模版失败操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilFailOp() const;

        /** 设置模板测试失败深度操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilDepthFailOp(StencilOpType value);

        /** 获取模板测试失败深度操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilDepthFailOp() const;

        /** 设置模版通道操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilPassOp(StencilOpType value);

        /** 获取模版通道操作.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        StencilOpType getStencilPassOp() const;

        /** 设置是否双面模板.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        void setStencilTwoSided(bool value);

        /** 获取是否双面模板.(OT_StencilTest)
        @version NIIEngine 3.0.0
        */
        bool getStencilTwoSided() const;

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
        void setCustomType(const String & customType);

        /** 获取自定义合成通道的类型名字(OT_RenderCustom)
        @version NIIEngine 3.0.0
        */
        const String & getCustomType() const;
    private:
        Nui32 mID;
        FrameShaderCh * mCh;
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
}
#endif