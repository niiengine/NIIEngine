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

#ifndef _NII_BUILTIN_SCRIPT_ANALYZE_H_
#define _NII_BUILTIN_SCRIPT_ANALYZE_H_

#include "NiiPreInclude.h"
#include "NiiScriptAnalyze.h"
#include "NiiMaterialProperty.h"

namespace NII
{
    /** 材质脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MaterialScriptAnalyze : public MaterialProperty, public ScriptAnalyze
    {
    public:
        MaterialScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::analyzeCom
        bool analyzeCom(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();

        using ScriptAnalyze::getValue;

        /** 获取颜色因子
        @version NIIEngine 3.0.0
        */
        bool getValue(const ScriptNode * node, ColourFactor & out) const;

        /** 获取比较模式
        @version NIIEngine 3.0.0
        */
        bool getValue(const ScriptNode * node, CmpMode & out) const;
    protected:
        void analyzeShaderFusion(const ScriptNode * node);
        void analyzeShaderCh(const ScriptNode * node);
        void analyzeTextureUnit(const ScriptNode * node);
        void analyzeTextureSrc(const ScriptNode * node);

        void analyzeVPSet(ObjectScriptNode * node);
        void analyzeFPSet(ObjectScriptNode * node);
        void analyzeGPSet(ObjectScriptNode * node);
        void analyzeShadowCastVPSet(ObjectScriptNode * node);
        void analyzeShadowCastFPSet(ObjectScriptNode * node);
        void analyzeShadowReceiveVPSet(ObjectScriptNode * node);
        void analyzeShadowReceiveFPSet(ObjectScriptNode * node);
    protected:
        Material * mMaterial;
        ShaderFusion * mFusion;
        ShaderCh * mPass;
        ShaderChTextureUnit * mUnit;
    };

    /** 着色程序脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramScriptAnalyze : public ScriptAnalyze
    {
    public:
        GpuProgramScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();

        void analyzeParam(GpuProgramParam * params, ObjectScriptNode * obj);
    protected:
        void analyzeASM(const ObjectScriptNode * obj);
        void analyzeAlter(const ObjectScriptNode * obj);
        void analyzeHighLevel(const ObjectScriptNode * obj);
    };

    /** 着色程序预定义脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GpuProgramParamScriptAnalyze : public ScriptAnalyze
    {
    public:
        GpuProgramParamScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /** 获取值
        @version NIIEngine 3.0.0
        */
        static bool getConstantType(const ScriptNode * node, GpuDataType & op);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        using ScriptAnalyze::getValue;

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();
    };

    /** 粒子系统脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleSpaceScriptAnalyze : public ScriptAnalyze
    {
    public:
        ParticleSpaceScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();
    protected:
        ParticleSpace * mSystem;
    };

    /** 字体脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontScriptAnalyze : public ScriptAnalyze
    {
    public:
        FontScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();
    protected:
        Font * mFont;
    };

    /** 粒子发射器脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpreadParticleScriptAnalyze : public ScriptAnalyze
    {
    public:
        SpreadParticleScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();
    protected:
        SpreadParticle * mEmitter;
    };

    /** 粒子效果脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleEffectScriptAnalyze : public ScriptAnalyze
    {
    public:
        ParticleEffectScriptAnalyze(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();
    protected:
        ParticleEffect * mAffector;
    };

    /** 帧合成脚本分析
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CompositorTranslator : public ScriptAnalyze
    {
    public:
        CompositorTranslator(ScriptCompiler * cpl, LangID lid);

        /// @copydetails ScriptAnalyze::analyze
        void analyze(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::analyzeCom
        bool analyzeCom(const ScriptNode * node);

        /// @copydetails ScriptAnalyze::getUnit
        ScriptUnitID getUnit();

        /// @copydetails ScriptAnalyze::getGroup
        GroupID getGroup();

        /** 获取一个蒙板操作类型
        @version NIIEngine 3.0.0
        */
        bool getStencilOp(const ScriptNode * node, StencilOpType * op);
    public:
        static const PropertyID ID_COMPOSITOR;
        static const PropertyID MaterialProperty;
        static const PropertyID ID_TECHNIQUE;
        static const PropertyID ID_PASS;
        static const PropertyID ID_SCHEME;
        static const PropertyID ID_TEXTURE;
        static const PropertyID ID_GAMMA;
        static const PropertyID ID_TARGET;
        static const PropertyID ID_TARGET_OUTPUT;
        static const PropertyID ID_INPUT;
        static const PropertyID ID_PREVIOUS;
        static const PropertyID ID_TARGET_WIDTH;
        static const PropertyID ID_TARGET_HEIGHT;
        static const PropertyID ID_TARGET_WIDTH_SCALED;
        static const PropertyID ID_TARGET_HEIGHT_SCALED;
        static const PropertyID ID_COMPOSITOR_LOGIC;
        static const PropertyID ID_TEXTURE_REF;
        static const PropertyID ID_SCOPE_LOCAL;
        static const PropertyID ID_SCOPE_CHAIN;
        static const PropertyID ID_SCOPE_GLOBAL;
        static const PropertyID ID_POOLED;
        static const PropertyID ID_NO_FSAA;
        static const PropertyID ID_DEPTH_POOL;
        static const PropertyID ID_ONLY_INITIAL;
        static const PropertyID ID_VISIBILITY_MASK;
        static const PropertyID ID_LOD_BIAS;
        static const PropertyID ID_MATERIAL_SCHEME;
        static const PropertyID ID_SHADOWS_ENABLED;
        static const PropertyID ID_CLEAR;
        static const PropertyID ID_STENCIL;
        static const PropertyID ID_RENDER_SCENE;
        static const PropertyID ID_RENDER_QUAD;
        static const PropertyID ID_IDENTIFIER;
        static const PropertyID ID_FIRST_RENDER_QUEUE;
        static const PropertyID ID_LAST_RENDER_QUEUE;
        static const PropertyID ID_QUAD_NORMALS;
        static const PropertyID ID_CAMERA_FAR_CORNERS_VIEW_SPACE;
        static const PropertyID ID_CAMERA_FAR_CORNERS_WORLD_SPACE;
        static const PropertyID ID_BUFFERS;
        static const PropertyID ID_COLOUR;
        static const PropertyID ID_DEPTH;
        static const PropertyID ID_COLOUR_VALUE;
        static const PropertyID ID_DEPTH_VALUE;
        static const PropertyID ID_STENCIL_VALUE;
        static const PropertyID ID_CHECK;
        static const PropertyID ID_COMP_FUNC;
        static const PropertyID ID_REF_VALUE;
        static const PropertyID ID_MASK;
        static const PropertyID ID_FAIL_OP;
        static const PropertyID ID_KEEP;
        static const PropertyID ID_INCREMENT;
        static const PropertyID ID_DECREMENT;
        static const PropertyID ID_INCREMENT_WRAP;
        static const PropertyID ID_DECREMENT_WRAP;
        static const PropertyID ID_INVERT;
        static const PropertyID ID_DEPTH_FAIL_OP;
        static const PropertyID ID_PASS_OP;
        static const PropertyID ID_TWO_SIDED;
        static const PropertyID ColourFactorZeroProperty;
        static const PropertyID ReplaceMathProperty;
        static const PropertyID ID_COMPOSITOR_LOGI;
        static const PropertyID NoneProperty;
        static const PropertyID PropertyCount;
    protected:
        void analyzeFrameShaderFusion(const ScriptNode * node);
        void analyzeFrameShaderCh(const ScriptNode * node);
        void analyzeFrameShaderOp(const ScriptNode * node);
    protected:
        Fusion * mCompositor;
        FrameShader * mTechnique;
        FusionShader * mTarget;
        FusionShaderOp * mPass;
    };
}
#endif