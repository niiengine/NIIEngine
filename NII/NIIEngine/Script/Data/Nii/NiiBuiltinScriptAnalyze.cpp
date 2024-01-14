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

#include "NiiPreProcess.h"
#include "NiiBuiltinScriptAnalyze.h"
#include "NiiScriptCompiler.h"
#include "NiiLogManager.h"
#include "NiiMaterialManager.h"
#include "NiiShaderCh.h"
#include "NiiGpuProgramManager.h"
#include "NiiParticleSpaceManager.h"
#include "NiiParticleGeo.h"
#include "NiiSpreadParticle.h"
#include "NiiParticleEffect.h"
#include "NiiFusionManager.h"
#include "NiiFusion.h"
#include "NiiTextureManager.h"
#include "NiiLodSchemeManager.h"
#include "NiiMaterialLodScheme.h"
#include "NiiMeshLodScheme.h"
#include "NiiExtFrameBuffer.h"
#include "NiiScriptAnalyzeList.h"
#include "NiiLodUnitList.h"
#include "NiiShaderChTexture.h"
#include "NiiPredefinedControllers.h"
#include "NiiShaderCh.h"
#include "NiiGpuProgram.h"
#include "NiiFontManager.h"
#include "NiiViewport.h"

namespace NII
{
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COMPOSITOR,                0);
    N_PROPERTY(ScriptProperty, CompositorTranslator, MaterialProperty,            1);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TECHNIQUE,                2);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_PASS,                    3);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_SCHEME,                    4);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TEXTURE,                5);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_GAMMA,                    6);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET,                    7);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET_OUTPUT,            8);

    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_INPUT,                    9);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_PREVIOUS,                10);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET_WIDTH,            11);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET_HEIGHT,            12);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET_WIDTH_SCALED,    13);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TARGET_HEIGHT_SCALED,    14);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COMPOSITOR_LOGIC,        15);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TEXTURE_REF,            16);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_SCOPE_LOCAL,            17);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_SCOPE_CHAIN,            18);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_SCOPE_GLOBAL,            19);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_POOLED,                    20);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_NO_FSAA,                21);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DEPTH_POOL,                22);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_ONLY_INITIAL,            23);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_VISIBILITY_MASK,        24);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_LOD_BIAS,                25);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_MATERIAL_SCHEME,        26);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_SHADOWS_ENABLED,        27);

    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_CLEAR,                28);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_STENCIL,            29);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_RENDER_SCENE,        30);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_RENDER_QUAD,        31);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_IDENTIFIER,            32);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_FIRST_RENDER_QUEUE, 33);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_LAST_RENDER_QUEUE,    34);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_QUAD_NORMALS,        35);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_CAMERA_FAR_CORNERS_VIEW_SPACE, 36);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_CAMERA_FAR_CORNERS_WORLD_SPACE, 37);

    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_BUFFERS,            38);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COLOUR,                39);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DEPTH,                40);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COLOUR_VALUE,        41);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DEPTH_VALUE,        42);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_STENCIL_VALUE,        43);

    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_CHECK,                44);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COMP_FUNC,            45);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_REF_VALUE,            46);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_MASK,                47);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_FAIL_OP,            48);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_KEEP,                49);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_INCREMENT,            50);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DECREMENT,            51);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_INCREMENT_WRAP,        52);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DECREMENT_WRAP,        53);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_INVERT,                54);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_DEPTH_FAIL_OP,        55);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_PASS_OP,            56);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_TWO_SIDED,            57);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ColourFactorZeroProperty, 58);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ReplaceMathProperty,    59);
    N_PROPERTY(ScriptProperty, CompositorTranslator, ID_COMPOSITOR_LOGI,    60);
    N_PROPERTY(ScriptProperty, CompositorTranslator, NoneProperty,            61);
    N_PROPERTY(ScriptProperty, CompositorTranslator, PropertyCount,            62);
    //-------------------------------------------------------------------------
    GpuProgram::ShaderType __ToGpuProgramType(Nui32 id)
    {
        if(id == N_Script_Unit_GpuProgramVP)
            return GpuProgram::ST_VS;
        else if(id == N_Script_Unit_GpuProgramTP)
            return GpuProgram::ST_TS;
        else if(id == N_Script_Unit_GpuProgramDP)
            return GpuProgram::ST_DS
        else if(id == N_Script_Unit_GpuProgramGP)
            return GpuProgram::ST_GS;
        else if(id == N_Script_Unit_GpuProgramFP)
            return GpuProgram::ST_FS;
        else if(id == N_Script_Unit_GpuProgramCP)
            return GpuProgram::ST_CS;

        return GpuProgram::ST_VS;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // MaterialScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    MaterialScriptAnalyze::MaterialScriptAnalyze(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_Material, lid),
        mMaterial(0),
        mFusion(0),
        mPass(0),
        mUnit(0)
    {
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        if(obj->mName.empty())
            mCompiler->error(*obj, ScriptError::ET_ObjIDError);
        Nui32 ttid;
        StrConv::conv(obj->mName, ttid);
        // Create a material with the given name
        mMaterial = reinterpret_cast<Material *>(N_Only(Material).create(ttid, mCompiler->getResourceGroup()));
        mMaterial->destroyAll();
        mMaterial->setSrc(obj->mFile);

        obj->mData = mMaterial;

        ScriptNodeList::const_iterator i, iend = obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = getProperty(prop->mPropertyName);
                if(LodSchemeValueProperty == tempid)
                {
                    MaterialLodScheme * scheme = static_cast<MaterialLodScheme*>(mMaterial->getLodScheme());
                    scheme->removeAll();
                    ScriptNodeList::iterator j, jend = prop->mValueDataList.end();
                    for(j = prop->mValueDataList.begin(); j != jend; ++j)
                    {
                        NIIf v = 0;
                        if(getValue(*j, v))
                            scheme->add(v);
                        else
                            mCompiler->error(*prop, ScriptError::ET_NumberError, _T("lod_values expects only numbers as arguments"));
                    }
                }
                else if (LodSchemeCameraProperty == tempid)
                {
                    // Set strategy to distance strategy
                    MaterialLodScheme * scheme = static_cast<MaterialLodScheme *>(N_Only(LodScheme).create(N_LOD_Material, 1));
                    mMaterial->setLodScheme(scheme);
                    ScriptNodeList::iterator j, jend = prop->mValueDataList.end();
                    for(j = prop->mValueDataList.begin(); j != jend; ++j)
                    {
                        NIIf v = 0;
                        if(getValue(*j, v))
                            scheme->add(v);
                        else
                            mCompiler->error(*prop, ScriptError::ET_NumberError, _T("lod_values expects only numbers as arguments"));
                    }
                }
                else if (LodSchemeProperty == tempid)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError, _T("lod_strategy only supports 1 argument"));
                    }
                    else
                    {
                        Nui32 schemeid;
                        if (getValue(prop->mValueDataList.front(), schemeid))
                        {
                            MaterialLodScheme * scheme = 
                                static_cast<MaterialLodScheme *>(N_Only(LodScheme).create(N_LOD_Material, schemeid));

                            if (scheme != 0)
                                mMaterial->setLodScheme(scheme);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("lod_strategy argument must be a valid lod scheme"));
                        }
                    }
                }
                else if (ReceiveShadowProperty == tempid)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError, _T("receive_shadows only supports 1 argument"));
                    }
                    else
                    {
                        bool val = true;
                        if (getValue(prop->mValueDataList.front(), val))
                            mMaterial->setReceiveShadow(val);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("receive_shadows argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\""));
                    }
                }
                else if (AlphaCastShadowProperty == tempid)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("transparency_casts_shadows only supports 1 argument"));
                    }
                    else
                    {
                        bool val = true;
                        if (getValue(prop->mValueDataList.front(), val))
                            mMaterial->setAlphaShadowCast(val);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("transparency_casts_shadows argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\""));
                    }
                }
                else
                {
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
                }
            }
            else if((*i)->mType == SNT_Object)
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    bool MaterialScriptAnalyze::analyzeCom(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        ObjectScriptNode * parent = obj->mParent ? reinterpret_cast<ObjectScriptNode *>(obj->mParent) : 0;

        if(getProperty(obj->mClass) == ShaderFusionProperty && parent && getProperty(parent->mClass) == N_Script_Unit_Material)
        {
            analyzeShaderFusion(node);
            return true;
        }
        else if(getProperty(obj->mClass) == ShaderChProperty && parent && getProperty(parent->mClass) == ShaderFusionProperty)
        {
            analyzeShaderCh(node);
            return true;
        }
        else if(getProperty(obj->mClass) == ShaderChTextureUnitProperty && parent && getProperty(parent->mClass) == ShaderChProperty)
        {
            analyzeTextureUnit(node);
            return true;
        }
        else if(getProperty(obj->mClass) == ShaderChTextureSrcProperty && parent && getProperty(parent->mClass) == ShaderChTextureUnitProperty)
        {
            analyzeTextureSrc(node);
            return true;
        }

        return false;
    }
    //-------------------------------------------------------------------------
    ScriptUnitID MaterialScriptAnalyze::getUnit()
    {
        return N_Script_Unit_Material;
    }
    //-------------------------------------------------------------------------
    GroupID MaterialScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShaderFusion(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // Create the technique from the material
        Material * material = static_cast<Material *>(obj->mParent->mData);
        mFusion = material->create();
        obj->mData = mFusion;

        // Get the name of the technique
        if(!obj->mName.empty())
            mFusion->setName(obj->mName);

        // Set the properties for the material
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = getProperty(prop->mPropertyName);

                if(ShaderFusionSchemeProperty == tempid)
                {
                    Nui32 scheme;
                    if(getValue(prop->mValueDataList, 0, scheme))
                        mFusion->setSchemeID(scheme);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError, _T("scheme must have 1 id argument"));
                }
                else if (LodSchemeIndexProperty == tempid)
                {
                    Nui32 v = 0;
                    if(getValue(prop->mValueDataList, 0, v))
                        mFusion->setLodIndex((Nui16)v);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError, _T("lod_index cannot accept argument \"") + 
                            getNode(prop->mValueDataList, 0)->getValue() + _T("\""));
                }
                else if (ShadowCastMaterialProperty == tempid)
                {
                    Nui32 matid;
                    if(getValue(prop->mValueDataList, 0, matid))
                    {
                        mFusion->setShadowCaster(static_cast<Material *>(N_Only(Material).get(matid))); // Use the processed name
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError, _T("shadow_caster_material cannot accept argument \"") + 
                            getNode(prop->mValueDataList, 0)->getValue() + _T("\""));
                }
                else if (ShadowReceiveMaterialProperty == tempid)
                {
                    Nui32 matid;
                    if(getValue(prop->mValueDataList, 0, matid))
                    {
                        mFusion->setShadowReceiver(static_cast<Material *>(N_Only(Material).get(matid)));
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("shadow_receiver_material_name cannot accept argument \"") + getNode(prop->mValueDataList, 0)->getValue() + _T("\""));
                }
                else if (GpuVendorProperty == tempid)
                {
                    if (prop->getCount() < 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError, _T("gpu_vendor_rule must have 2 arguments"));
                    }
                    else if (prop->getCount() > 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError, _T("gpu_vendor_rule must have 2 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0);

                        GpuVendorDetail rule;
                        if (i0->mType == SNT_Value)
                        {
                            String vendor;
                            if (!getValue(prop->mValueDataList, 1, vendor))
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("gpu_vendor_rule cannot accept \"") + getNode(prop->mValueDataList, 1)->getValue() + _T("\" as second argument"));
                            String ser;
                            if (!getValue(prop->mValueDataList, 2, ser))
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("gpu_vendor_rule cannot accept \"") + getNode(prop->mValueDataList, 2)->getValue() + _T("\" as second argument"));
                            Nui32 num;
                            if (!getValue(prop->mValueDataList, 3, num))
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("gpu_vendor_rule cannot accept \"") + getNode(prop->mValueDataList, 3)->getValue() + _T("\" as second argument"));

                            rule.mVendor = GpuVendorDetail::toVendor(vendor);
                            rule.mSer = ser;
                            rule.mNum = num;
                            if (rule.mVendor != GT_Unknow)
                            {
                                mFusion->add(rule);
                            }
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("gpu_vendor_rule cannot accept \"") + i0->getValue() + _T("\" as first argument"));
                        }

                    }
                }
                else
                {
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
                }
            }
            else if((*i)->mType == SNT_Object)
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    bool MaterialScriptAnalyze::getValue(const ScriptNode * node, CmpMode & out)const
    {
        if(node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        PropertyID tempid = ScriptProperty::getProperty(atom->mValueData);

        if(tempid == CmpModeAlwaysFailProperty)
            out = CPM_ALWAYS_FAIL;
        else if(tempid == CmpModeAlwaysPassProperty)
            out = CPM_ALWAYS_PASS;
        else if(tempid ==  CmpModeLessProperty)
            out = CPM_LESS;
        else if(tempid == CmpModeLessEqualProperty)
            out = CPM_LESS_EQUAL;
        else if(tempid == CmpModeEqualProperty)
            out = CPM_EQUAL;
        else if(tempid == CmpModeNotEqualProperty)
            out = CPM_NOT_EQUAL;
        else if(tempid == CmpModeGreaterEqualProperty)
            out = CPM_GREATER_EQUAL;
        else if(tempid == CmpModeGreaterProperty)
            out = CPM_GREATER;
        else
        {
            out = CPM_LESS_EQUAL;
            return false;
        }

        return true;
    }
    //-------------------------------------------------------------------------
    bool MaterialScriptAnalyze::getValue(const ScriptNode * node, ColourFactor & out)const
    {
        if (node->mType != SNT_Value)
            return false;

        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        PropertyID tempid = ScriptProperty::getProperty(atom->mValueData);

        if (tempid == ColourFactorOneProperty)
            out = CF_ONE;
        else if (tempid == ColourFactorZeroProperty)
            out = CF_ZERO;
        else if (tempid == ColourFactorDestProperty)
            out = CF_DST;
        else if (tempid == ColourFactorDestAlphaProperty)
            out = CF_DST_ALPHA;
        else if (tempid == ColourFactorSrcAlphaProperty)
            out = CF_SRC_ALPHA;
        else if (tempid == ColourFactorSrcProperty)
            out = CF_SRC;
        else if (tempid == ColourFactorOneMinusDestProperty)
            out = CF_INV_DST;
        else if (tempid == ColourFactorOneMinusSrcProperty)
            out = CF_INV_SRC;
        else if (tempid == ColourFactorOneMinusDestAlphaProperty)
            out = CF_INV_DST_ALPHA;
        else if (tempid == ColourFactorOneMinusSrcAlphaProperty)
            out = CF_INV_SRC_ALPHA;
        else
        {
            out = CF_ONE;
            return false;
        }
        return true;
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShaderCh(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        ShaderFusion * technique = static_cast<ShaderFusion *>(obj->mParent->mData);
        mPass = technique->create();
        obj->mData = mPass;

        // Get the name of the technique
        if(!obj->mName.empty())
            mPass->setName(obj->mName);

        // Set the properties for the material
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = getProperty(prop->mPropertyName);
                if(tempid == AmbientColourProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value &&
                        getProperty(((ValueScriptNode *)prop->mValueDataList.front())->mValueData) == VertexColourProperty)
                    {
                        mPass->setColour(ShaderCh::SOT_Set)->addColourTracking(CTM_T_AMBIENT);
                    }
                    else
                    {
                        Colour val = Colour::White;
                        if(getValue(prop->mValueDataList, 0, val))
                            mPass->setColour(ShaderCh::SOT_Set)->setAmbient(val);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("ambient requires 3 or 4 colour arguments, or a \"vertexcolour\" directive"));
                    }
                }
                else if(tempid == DiffuseColourProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value &&
                        getProperty(((ValueScriptNode *)prop->mValueDataList.front())->mValueData) == VertexColourProperty)
                    {
                        mPass->setColour(ShaderCh::SOT_Set)->addColourTracking(CTM_T_DIFFUSE);
                    }
                    else
                    {
                        Colour val = Colour::White;
                        if(getValue(prop->mValueDataList, 0, val))
                            mPass->setColour(ShaderCh::SOT_Set)->setDiffuse(val);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("diffuse requires 3 or 4 colour arguments, or a \"vertexcolour\" directive"));
                    }
                }
                else if (tempid == SpecularColourProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                    }
                    else if (prop->getCount() > 5)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("specular must have at most 5 arguments"));
                    }
                    else
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value &&
                            getProperty(((ValueScriptNode *)prop->mValueDataList.front())->mValueData) == VertexColourProperty)
                        {
                            mPass->setColour(ShaderCh::SOT_Set)->addColourTracking(CTM_T_SPECULAR);

                            if (prop->getCount() >= 2)
                            {
                                NIIf val = 0;
                                if (getValue(prop->mValueDataList.back(), val))
                                    mPass->setColour(ShaderCh::SOT_Set)->setShininess(val);
                                else
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("specular does not support \"") + prop->mValueDataList.back()->getValue() + _T("\" as its second argument"));
                            }
                        }
                        else
                        {
                            if (prop->getCount() < 4)
                            {
                                mCompiler->error(*prop, ScriptError::ET_NumberError, _T("specular expects at least 4 arguments"));
                            }
                            else
                            {
                                Colour val(0.0f, 0.0f, 0.0f, 1.0f);
                                if (getValue(prop->mValueDataList, 0, val.r) && getValue(prop->mValueDataList, 1, val.g) &&
                                    getValue(prop->mValueDataList, 2, val.b))
                                {
                                    if (prop->getCount() == 4)
                                    {
                                        mPass->setColour(ShaderCh::SOT_Set)->setSpecular(val);

                                        NIIf shininess = 0.0f;
                                        if (getValue(prop->mValueDataList, 3, shininess))
                                            mPass->setColour(ShaderCh::SOT_Set)->setShininess(shininess);
                                        else
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("specular fourth argument must be a valid number for shininess attribute"));
                                    }
                                    else
                                    {
                                        if (!getValue(prop->mValueDataList, 3, val.a))
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("specular fourth argument must be a valid color component value"));
                                        else
                                            mPass->setColour(ShaderCh::SOT_Set)->setSpecular(val);

                                        NIIf shininess = 0.0f;
                                        if (getValue(prop->mValueDataList, 4, shininess))
                                            mPass->setColour(ShaderCh::SOT_Set)->setShininess(shininess);
                                        else
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("specular fourth argument must be a valid number for shininess attribute"));
                                    }
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("specular must have first 3 arguments be a valid colour"));
                                }
                            }

                        }
                    }
                }
                else if (tempid == EmissiveColourProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value &&
                        getProperty(((ValueScriptNode *)prop->mValueDataList.front())->mValueData) == VertexColourProperty)
                    {
                        mPass->setColour(ShaderCh::SOT_Set)->addColourTracking(CTM_T_EMISSIVE);
                    }
                    else
                    {
                        Colour val(0.0f, 0.0f, 0.0f, 1.0f);
                        if(getValue(prop->mValueDataList, 0, val))
                            mPass->setColour(ShaderCh::SOT_Set)->setEmissive(val);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("emissive requires 3 or 4 colour arguments, or a \"vertexcolour\" directive"));
                    }
                }
                else if (tempid == FrameBlendProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("scene_blend supports at most 2 arguments"));
                    }
                    else if (prop->getCount() == 1)
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                            PropertyID tempid2 = getProperty(atom->mValueData);

                            if(tempid2 == AddMathProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_ADD);
                               
                            else if (tempid2 == ModulateMathProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_MODULATE);
                               
                            else if (tempid2 == FrameColourBlendProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_COLOUR);
                                
                            else if (tempid2 == AlphaMathProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_ALPHA);
                           
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("scene_blend does not support \"") + prop->mValueDataList.front()->getValue() + _T("\" for argument 1"));
           
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("scene_blend does not support \"") + prop->mValueDataList.front()->getValue() + _T("\" for argument 1"));
                        }
                    }
                    else
                    {
                        String sbf0t, sbf1t;
                        if (getValue(prop->mValueDataList, 0, sbf0t) && getValue(prop->mValueDataList, 1, sbf1t))
                        {
                            ColourFactor sbf0, sbf1;
                            StrConv::conv(sbf0t, sbf0);
                            StrConv::conv(sbf1t, sbf1);
                            mPass->setBlend(ShaderCh::SOT_Set)->setBlend(sbf0, sbf1);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("scene_blend does not support \"") + getNode(prop->mValueDataList, 0)->getValue() + 
                                _T("\" and \"") + getNode(prop->mValueDataList, 1)->getValue() + _T("\" as arguments"));
                        }
                    }
                }
                else if (tempid == FrameBlend2Property)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() == 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("separate_scene_blend must have 2 or 4 arguments"));
                    }
                    else if (prop->getCount() > 4)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("separate_scene_blend must have 2 or 4 arguments"));
                    }
                    else if (prop->getCount() == 2)
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value)
                        {
                            ValueScriptNode * atom0 = (ValueScriptNode *)(i0),
                                *atom1 = (ValueScriptNode *)(i1);
                            FrameBlendMode sbt0, sbt1;
                            PropertyID tempid2 = getProperty(atom0->mValueData);
                            if(tempid2 == AddMathProperty)
                                sbt0 = FBM_ADD;
                             
                            else if (tempid2 == ModulateMathProperty)
                                sbt0 = FBM_MODULATE;
                              
                            else if (tempid2 == FrameColourBlendProperty)
                                sbt0 = FBM_INV_COLOUR;
                               
                            else if (tempid2 == AlphaMathProperty)
                                sbt0 = FBM_INV_ALPHA;
                            
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("separate_scene_blend does not support \"") + atom0->mValueData + _T("\" as argument 1"));
                                return;
                            }
                            tempid2 = getProperty(atom1->mValueData);

                            if (tempid2 == AddMathProperty)
                                sbt1 = FBM_ADD;
                            
                            else if(tempid2 == ModulateMathProperty)
                                sbt1 = FBM_MODULATE;
                             
                            else if (tempid2 == FrameColourBlendProperty)
                                sbt1 = FBM_INV_COLOUR;
                            
                            else if (tempid2 == AlphaMathProperty)
                                sbt1 = FBM_INV_ALPHA;
                            
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("separate_scene_blend does not support \"") + atom1->mValueData + _T("\" as argument 2"));
                                return;
                            }
                            mPass->setBlend(ShaderCh::SOT_Set)->setBlend(sbt0, sbt1);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("separate_scene_blend does not support \"") + i0->getValue() + _T("\" as argument 1"));
                        }
                    }
                    else
                    {
                        String sbf0t, sbf1t, sbf2t, sbf3t;
                        if (getValue(prop->mValueDataList, 0, sbf0t) && getValue(prop->mValueDataList, 1, sbf1t) &&
                            getValue(prop->mValueDataList, 2, sbf2t) && getValue(prop->mValueDataList, 3, sbf3t))
                        {
                            ColourFactor sbf0, sbf1, sbf2, sbf3;
                            StrConv::conv(sbf0t, sbf0);
                            StrConv::conv(sbf1t, sbf1);
                            StrConv::conv(sbf2t, sbf2);
                            StrConv::conv(sbf3t, sbf3);
                            mPass->setBlend(ShaderCh::SOT_Set)->setBlend(sbf0, sbf1, sbf2, sbf3);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("one of the arguments to separate_scene_blend is not a valid scene blend factor directive"));
                        }
                    }
                }
                else if (tempid == FrameBlendOpProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("scene_blend_op must have 1 argument"));
                    }
                    else
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = reinterpret_cast<ValueScriptNode *>(prop->mValueDataList.front());
                            PropertyID tempid2 = getProperty(atom->mValueData);

                            if(tempid2 == AddMathProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(CBM_ADD);
                               
                            else if (tempid2 == SubtractMathProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(CBM_SUBTRACT);
                               
                            else if (tempid2 == FrameOpRevSubtractProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(CBM_REV_SUBTRACT);
                              
                            else if (tempid2 == FrameOpMinProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(CBM_MIN);
                             
                            else if (tempid2 == FrameOpMaxProperty)
                                mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(CBM_MAX);
                         
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom->mValueData + _T(": unrecognized argument"));
                         
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(": unrecognized argument"));
                        }
                    }
                }
                else if (tempid == FrameBlend2OpProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() != 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("separate_scene_blend_op must have 2 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value)
                        {
                            const ValueScriptNode * atom0 = reinterpret_cast<const ValueScriptNode *>(i0),
                                *atom1 = reinterpret_cast<const ValueScriptNode *>(i1);
                            ColourBlendMode op = CBM_ADD, alphaOp = CBM_ADD;
                            PropertyID tempid2 = getProperty(atom0->mValueData);

                            if(tempid2 == AddMathProperty)
                                op = CBM_ADD;
                               
                            else if (tempid2 == SubtractMathProperty)
                                op = CBM_SUBTRACT;
                             
                            else if (tempid2 == FrameOpRevSubtractProperty)
                                op = CBM_REV_SUBTRACT;
                               
                            else if (tempid2 == FrameOpMinProperty)
                                op = CBM_MIN;
                           
                            else if (tempid2 == FrameOpMaxProperty)
                                op = CBM_MAX;
                          
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom0->mValueData + _T(": unrecognized first argument"));
                         
                            tempid2 = getProperty(atom1->mValueData);

                            if (tempid2 == AddMathProperty)
                                alphaOp = CBM_ADD;
                         
                            else if (tempid2 == SubtractMathProperty)
                                alphaOp = CBM_SUBTRACT;
                     
                            else if (tempid2 == FrameOpRevSubtractProperty)
                                alphaOp = CBM_REV_SUBTRACT;
                           
                            else if (tempid2 == FrameOpMinProperty)
                                alphaOp = CBM_MIN;
                              
                            else if (tempid2 == FrameOpMaxProperty)
                                alphaOp = CBM_MAX;
                                
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom1->mValueData + _T(": unrecognized second argument"));
                           

                            mPass->setBlend(ShaderCh::SOT_Set)->setBlendMode(op, alphaOp);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(": unrecognized argument"));
                        }
                    }
                }
                else if (tempid == DepthCheckProperty)
                {
                    bool val = true;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("depth_check third argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\""));
                }
                else if (tempid == DepthWriteProperty)
                {
                    bool val = true;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("depth_write third argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\""));
                }
                else if (tempid == DepthBiasProperty)
                {
                    NIIf val0, val1 = 0.0f;
                    if(getValue(prop->mValueDataList, 0, val0))
                    {
                        getValue(prop->mValueDataList, 1, val1);
                        mPass->setStencil(ShaderCh::SOT_Set)->setBiasConstant(val0);

                        mPass->setStencil(ShaderCh::SOT_Set)->setBiasSlopeScale(val1);
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("depth_bias does not support \"") + getNode(prop->mValueDataList, 0)->getValue() + _T("\" for argument 1"));
                    }
                }
                else if (tempid == DepthFuncProperty)
                {
                    String funct;
                    if(getValue(prop->mValueDataList.front(), funct))
                    {
                        CmpMode func;
                        StrConv::conv(funct, func);
                        mPass->setStencil(ShaderCh::SOT_Set)->setDepthCompareFunc(func);
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid CmpMode"));
                }
                else if (tempid == DepthMultiBiasProperty)
                {
                    NIIf val = 0.0f;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencil(ShaderCh::SOT_Set)->setBiasConstantFactor(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid NIIf value"));
                }
                else if (tempid == AlphaRejectProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("alpha_rejection must have at most 2 arguments"));
                    }
                    else
                    {
                        String funct;
                        if (getValue(prop->mValueDataList, 0, funct))
                        {
                            CmpMode func;
                            StrConv::conv(funct, func);
                            if (getNode(prop->mValueDataList, 1) != 0)
                            {
                                Nui32 val = 0;
                                if (getValue(prop->mValueDataList, 1, val))
                                {
                                    mPass->setColour(ShaderCh::SOT_Set)->setAlphaReject(func);
                                    mPass->setColour(ShaderCh::SOT_Set)->setValue(val);
                                    mPass->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
                                }
                                else
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        getNode(prop->mValueDataList, 1)->getValue() + _T(" is not a valid integer"));
                            }
                            else
                                mPass->setColour(ShaderCh::SOT_Set)->setAlphaReject(func);
                        }
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                getNode(prop->mValueDataList, 0)->getValue() + _T(" is not a valid CmpMode"));
                    }
                }
                else if (tempid == AlphaCoverageProperty)
                {
                    bool val = true;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("alpha_to_coverage argument must be \"true\", \"false\", \"yes\", \"no\", \"on\", or \"off\""));
                }
                else if (tempid == LightClipProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLightClip(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == LightClipPlaneProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLightClipPlane(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == ForceDepthSortProperty)
                {
                    bool val = true;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        mPass->setDepthSort(val);
                        mPass->setForceDepthSort(false);
                    }
                    else
                    {
                        String val2;
                        if(getValue(prop->mValueDataList.front(), val2) && val2 == _T("force"))
                        {
                            mPass->setDepthSort(true);
                            mPass->setForceDepthSort(true);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" must be boolean or force"));
                        }
                    }
                }
                else if (tempid == ShaderStepProperty)
                {
                    if (prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                        PropertyID tempid2 = getProperty(atom->mValueData);

                        if(tempid2 == ShaderStepGlobalProperty)
                            mPass->setShaderStep(SS_Global);
                        else if (tempid2 == ShaderStepDetailLightProperty)
                            mPass->setShaderStep(SS_Detail_Light);
                        else if (tempid2 == ShaderStepDecalProperty)
                            mPass->setShaderStep(SS_Decal);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid ShaderStep"));
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid ShaderStep"));
                    }
                }
                else if (tempid == CullingModeProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                        PropertyID tempid2 = getProperty(atom->mValueData);

                        if(tempid2 ==  CullingModeClockwiseProperty)
                            mPass->setCullingMode(CM_Clockwise);
                        else if(tempid2 == CullingModeAntClockwiseProperty)
                            mPass->setCullingMode(CM_Anticlockwise);
                        else if (tempid2 == NoneProperty)
                            mPass->setCullingMode(CM_None);
                        else
                            mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid CullingMode"));
          
                    }
                    else
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid CullingMode"));
                    }
                }
                else if (tempid == SysCullingModeProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode*)prop->mValueDataList.front();
                        PropertyID tempid2 = getProperty(atom->mValueData);

                        if(tempid2 == SysCullingFrontProperty)
                            mPass->setSysCullingMode(SCM_FRONT);

                        else if (tempid2 == SysCullingBackProperty)
                            mPass->setSysCullingMode(SCM_BACK);
                     
                        else if (tempid2 == NoneProperty)
                            mPass->setSysCullingMode(SCM_NONE);
                        
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid SysCullingMode"));
                    
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid SysCullingMode"));
                    }
                }
                else if (tempid == UnitNormalsProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setUnitNormals(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == LightingProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLight(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == ShadeModeProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode*)prop->mValueDataList.front();
                        PropertyID tempid2 = getProperty(atom->mValueData);

                        if(tempid2 == ShadeModeFlatProperty)
                            mPass->setShadeMode(SM_FLAT);
                        else if(tempid2 == ShadeModeGouraudProperty)
                            mPass->setShadeMode(SM_GOURAUD);
                        else if(tempid2 == ShadeModePhongProperty)
                            mPass->setShadeMode(SM_PHONG);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid shading mode (flat, gouraud, or phong)"));
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid shading mode (flat, gouraud, or phong)"));
                    }
                }
                else if (tempid == PolygonModeProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                        PropertyID tempid2 = getProperty(atom->mValueData);

                        if(tempid2 == PolygonModeSolidProperty)
                            mPass->setPolygonMode(PM_SOLID);
                        else if (tempid2 == PolygonModePointsProperty)
                            mPass->setPolygonMode(PM_POINTS);
                        else if (tempid2 == PolygonModeWireFrameProperty)
                            mPass->setPolygonMode(PM_LINE);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid polygon mode (solid, points, or wireframe)"));
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid polygon mode (solid, points, or wireframe)"));
                    }
                }
                else if (tempid == AutoReduceProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setAutoReduce(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == FogProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 8)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("fog_override must have at most 8 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i1 = getNode(prop->mValueDataList, 1);
                        ScriptNodeList::const_iterator i2 = getIterator(prop->mValueDataList, 2);
                        bool val = false;
                        if (getValue(prop->mValueDataList, 0, val))
                        {
                            if (!val)
                                return;
                            FogMode mode = FM_NONE;
                            Colour clr = Colour::White;
                            NIIf dens = 0.001f, start = 0.0f, end = 1.0f;

                            if (i1 != 0)
                            {
                                if (i1->mType == SNT_Value)
                                {
                                    ValueScriptNode * atom = (ValueScriptNode*)(i1);
                                    PropertyID tempid2 = getProperty(atom->mValueData);

                                    if(tempid2 == NoneProperty)
                                        mode = FM_NONE;
                                    else if (tempid2 == LinearMathProperty)
                                        mode = FM_LINEAR;
                                    else if (tempid2 == FogExpProperty)
                                        mode = FM_EXP;
                                    else if (tempid2 == FogExp2Property)
                                        mode = FM_EXP2;
                                    else
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            i1->getValue() + _T(" is not a valid FogMode"));
                                   
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        i1->getValue() + _T(" is not a valid FogMode"));
                                    break;
                                }
                            }

                            if (i2 != prop->mValueDataList.end())
                            {
                                if (!getValue(prop->mValueDataList, 2, clr))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*i2)->getValue() + _T(" is not a valid colour"));
                                    break;
                                }

                                i2 = getIterator(prop->mValueDataList, 6);
                            }

                            if (i2 != prop->mValueDataList.end())
                            {
                                if (!getValue(*i2, dens))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*i2)->getValue() + _T(" is not a valid number"));
                                    break;
                                }
                                ++i2;
                            }

                            if (i2 != prop->mValueDataList.end())
                            {
                                if (!getValue(*i2, start))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*i2)->getValue() + _T(" is not a valid number"));
                                    return;
                                }
                                ++i2;
                            }

                            if (i2 != prop->mValueDataList.end())
                            {
                                if (!getValue(*i2, end))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*i2)->getValue() + _T(" is not a valid number"));
                                    return;
                                }
                                ++i2;
                            }

                            *mPass->setFog(ShaderCh::SOT_Set) = ShaderChFog(mode, clr, dens, start, end);
                        }
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                    }
                }
                else if (tempid == ColourWriteProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        if(val)
                            mPass->setColourWrite(ColourMark::CM_RGBA);
                        else
                            mPass->setColourWrite(ColourMark::CM_ZERO);
                    }
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == MaxLightsProperty)
                {
                    Nui32 val = 0;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLightMax((NCountb)val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid integer"));
                }
                else if (tempid == StartLightProperty)
                {
                    Nui32 val = 0;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLightBegin((NCountb)val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid integer"));
                }
                else if (tempid == LightMaskProperty)
                {
                    Nui32 val = 0;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setLightMark((Nmark)val);
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid integer"));
                }
                else if (tempid == MultiChProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0);
                        if (i0->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode *)(i0);
                            if (getProperty(atom->mValueData) == NoEachLightProperty)
                            {
                                mPass->setEachLight(false);
                                mPass->setLightTypeLimit(LT_POINT);
                            }
                            else if (getProperty(atom->mValueData) == EachLightProperty)
                            {
                                const ScriptNode * i1 = getNode(prop->mValueDataList, 1);
                                if (i1 != 0 && i1->mType == SNT_Value)
                                {
                                    atom = (ValueScriptNode *)i1;
                                    PropertyID tempid2 = getProperty(atom->mValueData);
                                    if (tempid2 == PointLightProperty)
                                    {
                                        mPass->setEachLight(true);
                                        mPass->setLightTypeLimit(LT_POINT);
                                    }
                                    else if (tempid2 == DirLightProperty)
                                    {
                                        mPass->setEachLight(true);
                                        mPass->setLightTypeLimit(LT_DIRECTIONAL);
                                    }
                                    else if (tempid2 == SpotLightProperty)
                                    {
                                        mPass->setEachLight(true);
                                        mPass->setLightTypeLimit(LT_SPOT);
                                    }
                                    else
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            prop->mValueDataList.front()->getValue() + _T(" is not a valid light type (point, directional, or spot)"));
                                }
                                else
                                {
                                    mPass->setEachLight(true);
                                    mPass->setLightTypeLimit(LT_NONE);
                                }
                            }
                            else if (StrConv::isNumber(atom->mValueData))
                            {
                                Nui32 temp;
                                StrConv::conv(atom->mValueData, temp);
                                mPass->setRenderCount(temp);

                                const ScriptNode * i1 = getNode(prop->mValueDataList, 1);
                                if (i1 != 0 && i1->mType == SNT_Value)
                                {
                                    atom = (ValueScriptNode *)(i1);
                                    if (getProperty(atom->mValueData) == ShaderStepDetailLightProperty)
                                    {
                                        const ScriptNode * i2 = getNode(prop->mValueDataList, 2);
                                        if (i2 != 0 && i2->mType == SNT_Value)
                                        {
                                            atom = (ValueScriptNode*)(i2);
                                            PropertyID tempid2 = getProperty(atom->mValueData);

                                            if (tempid2 == PointLightProperty)
                                            {
                                                mPass->setEachLight(true);
                                                mPass->setLightTypeLimit(LT_POINT);
                                            }
                                            else if (tempid2 == DirLightProperty)
                                            {
                                                mPass->setEachLight(true);
                                                mPass->setLightTypeLimit(LT_DIRECTIONAL);
                                            }
                                            else if (tempid2 == SpotLightProperty)
                                            {
                                                mPass->setEachLight(true);
                                                mPass->setLightTypeLimit(LT_SPOT);
                                            }
                                            else
                                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                    i2->getValue() + _T(" is not a valid light type (point, directional, or spot)"));
                                            
                                        }
                                        else
                                        {
                                            mPass->setEachLight(true);
                                            mPass->setLightTypeLimit(LT_NONE);
                                        }
                                    }
                                    else if (EachLightLimitCountProperty)
                                    {
                                        const ScriptNode * i2 = getNode(prop->mValueDataList, 2);
                                        if (i2 != 0 && i2->mType == SNT_Value)
                                        {
                                            atom = (ValueScriptNode *)i2;
                                            if (StrConv::isNumber(atom->mValueData))
                                            {
                                                Nui32 temp2;
                                                StrConv::conv(atom->mValueData, temp2);
                                                mPass->setLightRenderLimit((NCountb)temp2);

                                                const ScriptNode * i3 = getNode(prop->mValueDataList, 3);
                                                if (i3 != 0 && i3->mType == SNT_Value)
                                                {
                                                    atom = (ValueScriptNode *)i3;
                                                    PropertyID tempid2 = getProperty(atom->mValueData);
                                                    if(tempid2 == PointLightProperty)
                                                    {
                                                        mPass->setEachLight(true);
                                                        mPass->setLightTypeLimit(LT_POINT);
                                                    }
                                                    else if (tempid2 == DirLightProperty)
                                                    {
                                                        mPass->setEachLight(true);
                                                        mPass->setLightTypeLimit(LT_DIRECTIONAL);
                                                    }
                                                    else if (tempid2 == SpotLightProperty)
                                                    {
                                                        mPass->setEachLight(true);
                                                        mPass->setLightTypeLimit(LT_SPOT);
                                                    }
                                                    else
                                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                            i3->getValue() + _T(" is not a valid light type (point, directional, or spot)"));
                                                }
                                                else
                                                {
                                                    mPass->setEachLight(true);
                                                    mPass->setLightTypeLimit(LT_NONE);

                                                }
                                            }
                                            else
                                            {
                                                mCompiler->error(*prop, ScriptError::ET_NumberError,
                                                    i2->getValue() + _T(" is not a valid number"));
                                            }
                                        }
                                        else
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_NumberError,
                                                prop->mValueDataList.front()->getValue() + _T(" is not a valid number"));
                                        }
                                    }
                                }
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            }
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                else if (tempid == PointSizeProperty)
                {
                    NIIf val = 0.0f;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setPoint(ShaderCh::SOT_Set)->setSize(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid number"));
                }
                else if (tempid == PointSpritesProperty)
                {
                    bool val = false;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setPoint(ShaderCh::SOT_Set)->setSpritesEnable(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                }
                else if (tempid == PointSizeAttenuationProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 4)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("point_size_attenuation must have at most 4 arguments"));
                    }
                    else
                    {
                        bool val = false;
                        ShaderChPoint * scp = mPass->setPoint(ShaderCh::SOT_Set);
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            if (val)
                            {
                                if (prop->getCount() > 1)
                                {
                                    NIIf constant = 0.0f, linear = 1.0f, quadratic = 0.0f;

                                    if (!getValue(prop->mValueDataList, 1, constant))
                                    {
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            getNode(prop->mValueDataList, 1)->getValue() + _T(" is not a valid number"));
                                    }

                                    if (!getValue(prop->mValueDataList, 2, linear))
                                    {
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            getNode(prop->mValueDataList, 2)->getValue() + _T(" is not a valid number"));
                                    }

                                    if (!getValue(prop->mValueDataList, 3, quadratic))
                                    {
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            getNode(prop->mValueDataList, 3)->getValue() + _T(" is not a valid number"));
                                    }

                                    scp->setAttenuateEnable(true);
                                    scp->setConstant(constant);
                                    scp->setLinear(linear);
                                    scp->setQuadratic(quadratic);
                                }
                                else
                                {
                                    scp->setAttenuateEnable(true);
                                }
                            }
                            else
                            {
                                scp->setAttenuateEnable(false);
                            }
                        }
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid boolean"));
                    }
                }
                else if (tempid == PointSizeMinProperty)
                {
                    NIIf val = 0.0f;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setPoint(ShaderCh::SOT_Set)->setMinSize(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid number"));
                }
                else if (tempid == PointSizeMaxProperty)
                {
                    NIIf val = 0.0f;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setPoint(ShaderCh::SOT_Set)->setMaxSize(val);
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid number"));
                }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                    _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));

            }
            else if((*i)->mType == SNT_Object)
            {
                ObjectScriptNode * child = reinterpret_cast<ObjectScriptNode *>(*i);
                PropertyID tempid2 = getProperty(child->mClass);

                if(tempid2 == FragmentProgramProperty)
                    analyzeFPSet(child);
                else if (tempid2 == VertexProgramProperty)
                    analyzeVPSet(child);
                else if (tempid2 == GeoProgramProperty)
                    analyzeGPSet(child);
                else if (tempid2 == ShadowCastVertexProgramProperty)
                    analyzeShadowCastVPSet(child);
                else if (tempid2 == ShadowCastFragmentProgramProperty)
                    analyzeShadowCastFPSet(child);
                else if (tempid2 == ShadowReceiveVertexProgramProperty)
                    analyzeShadowReceiveVPSet(child);
                else if (tempid2 == ShadowReceiveFragmentProgramProperty)
                    analyzeShadowReceiveFPSet(child);
                else
                    ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeFPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, rid);
        pass->getProgram().get(ShaderChProgram::ST_FS, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_FS, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeVPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * ch = static_cast<ShaderCh *>(node->mParent->mData);
        ch->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, rid);
        ch->getProgram().get(ShaderChProgram::ST_VS, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            ch->getProgram().getParam(ShaderChProgram::ST_VS, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeGPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_GS, rid);
        pass->getProgram().get(ShaderChProgram::ST_GS, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_GS, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShadowCastVPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS_SC, rid);
        pass->getProgram().get(ShaderChProgram::ST_VS_SC, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_VS_SC, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShadowCastFPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS_SC, rid);
        pass->getProgram().get(ShaderChProgram::ST_FS_SC, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_FS_SC, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShadowReceiveVPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS_SR, rid);
        pass->getProgram().get(ShaderChProgram::ST_VS_SR, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_VS_SR, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeShadowReceiveFPSet(ObjectScriptNode * node)
    {
        if(node->mName.empty())
        {
            mCompiler->error(*node, ScriptError::ET_ObjIDError);
            return;
        }

        Nui32 rid;
        StrConv::conv(node->mName, rid);
        if(N_Only(GpuProgram).get((ResourceID)rid) == 0)
        {
            mCompiler->error(*node, ScriptError::ET_ObjExistError);
            return;
        }
        GpuProgram * prog;
        ShaderCh * pass = static_cast<ShaderCh *>(node->mParent->mData);
        pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS_SR, rid);
        pass->getProgram().get(ShaderChProgram::ST_FS_SR, prog);
        if(prog->isValid())
        {
            GpuProgramParam * params;
            pass->getProgram().getParam(ShaderChProgram::ST_FS_SR, params);
            GpuProgramScriptAnalyze * analy = N_new GpuProgramScriptAnalyze(mCompiler, mLangMap->getLangID());
            analy->analyzeParam(params, node);
            N_delete analy;
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeTextureUnit(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        ShaderCh * pass = static_cast<ShaderCh *>(obj->mParent->mData);
        mUnit = pass->setTexture(ShaderCh::SOT_Set)->create();
        obj->mData = mUnit;

        // Get the name of the technique
        if(!obj->mName.empty())
            mUnit->setName(obj->mName);

        // Set the properties for the material
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if ((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid2 = getProperty(prop->mPropertyName);
                if (tempid2 == TextureProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() > 5)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("texture must have at most 5 arguments"));
                    }
                    else
                    {
                        ScriptNodeList::const_iterator j = prop->mValueDataList.begin();
                        Nui32 val;
                        ResourceIDList rlist;
                        if (getValue(*j, val))
                        {
                            Texture::Type texType = Texture::T_2D;
                            bool isAlpha = false;
                            bool sRGBRead = false;
                            PixelFormat format = PF_UNKNOWN;
                            NIIi mipmaps = -1;

                            ++j;
                            while (j != prop->mValueDataList.end())
                            {
                                if ((*j)->mType == SNT_Value)
                                {
                                    ValueScriptNode * atom = (ValueScriptNode*)(*j);
                                    PropertyID tempid3 = getProperty(atom->mValueData);

                                    if(tempid3 == Texture1DProperty)
                                        texType = Texture::T_1D;
              
                                    else if (tempid3 == Texture2DProperty)
                                        texType = Texture::T_2D;
                                 
                                    else if (tempid3 == Texture3DProperty)
                                        texType = Texture::T_3D;
                              
                                    else if (tempid3 == TextureCubicProperty)
                                        texType = Texture::T_CUBE;
                                 
                                    else if (tempid3 == TextureAutoMipMapProperty)
                                        mipmaps = N_TEX_MIPMAP;
                                    
                                    else if (tempid3 == TextureAlphaProperty)
                                        isAlpha = true;
                                      
                                    else if (tempid3 == TextureGammaProperty)
                                        sRGBRead = true;
                                    
                                    else
                                        if (StrConv::isNumber(atom->mValueData))
                                            StrConv::conv(atom->mValueData, mipmaps);
                                        else
                                            format = PixelUtil::getFormat(atom->mValueData, true);
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*j)->getValue() + _T(" is not a supported argument to the texture property"));
                                }
                                ++j;
                            }
                            rlist.push_back(val);
                            mUnit->setSrc(rlist, texType);
                            mUnit->setFormat(format);
                            mUnit->setIsAlpha(isAlpha);
                            mUnit->setMipmapCount(mipmaps);
                            mUnit->setHardwareSRGB(sRGBRead);
                        }
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError, (*j)->getValue() + _T(" is not a valid texture name"));
                    }
                }
                else if (tempid2 == TextureAnimProperty)
                {
                    if (prop->getCount() < 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else
                    {
                        ResourceIDList rlist;
                        if (prop->getCount() == 3)
                        {
                            Nui32 rid, val1;
                            TimeDurMS val2;
                            
                            if (getValue(prop->mValueDataList, 0, rid) && getValue(prop->mValueDataList, 1, val1) && 
                                getValue(prop->mValueDataList, 2, val2))
                            {
                                for (NCount kc = 0; kc < val1; ++ kc)
                                {
                                    rlist.push_back(rid + kc);
                                }
                                mUnit->setAni(rlist, val2);
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_NumberError,
                                    _T("anim_texture NIIs form requires a texture name, number of frames, and animation duration"));
                            }
                        }
                        else
                        {
                            // Long form has n number of frames
                            TimeDurMS duration = 0;
                            ScriptNodeList::const_iterator in = getIterator(prop->mValueDataList, prop->getCount() - 1);
                            if (getValue(*in, duration))
                            {
                                //Nui32 * names = N_new_array_t(Nui32, prop->getCount() - 1);
                                ScriptNodeList::iterator j = prop->mValueDataList.begin();
                                while (j != in)
                                {
                                    if ((*j)->mType == SNT_Value)
                                    {
                                        Nui32 trid;
                                        StrConv::conv(((ValueScriptNode*)(*j))->mValueData, trid);
                                        rlist.push_back(trid);
                                    }
                                    else
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        (*j)->getValue() + _T(" is not supported as a texture name"));
                                    ++j;
                                }
                                
                                mUnit->setAni(rlist, duration);
                                //N_delete_array_t(names, Nui32, prop->getCount() - 1);
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_NumberError,
                                    (*in)->getValue() + _T(" is not supported for the duration argument"));
                            }
                        }
                    }
                }
                else if (tempid2 == TextureCubic6Property)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() == 2)
                    {
                        Nui32 rid;
                        const ScriptNode * i1 = getNode(prop->mValueDataList, 1);
                        if (getValue(prop->mValueDataList, 0, rid) && i1->mType == SNT_Value)
                        {
                            ValueScriptNode * atom1 = (ValueScriptNode *)i1;
                            ResourceIDList rlist;
                            rlist.push_back(rid);
                            mUnit->setSrc(rlist, getProperty(atom1->mValueData) == TextureUVWProperty ? Texture::T_CUBE : Texture::T_CUBE_2D);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                    else if (prop->getCount() == 6)
                    {
                        Nui32 names[6];
                        ResourceIDList rlist;
                        //const ScriptNode * i6 = getNode(prop->mValueDataList, 6);
                        if (getValue(prop->mValueDataList, 0, 6, names) /*&& i6->mType == SNT_Value*/)
                        {
                            //ValueScriptNode * atom6 = (ValueScriptNode*)i6;
                            ResourceIDList rlist;
                            rlist.push_back(names[0]);
                            rlist.push_back(names[1]);
                            rlist.push_back(names[2]);
                            rlist.push_back(names[3]);
                            rlist.push_back(names[4]);
                            rlist.push_back(names[5]);
                            mUnit->setSrc(rlist, Texture::T_CUBE_2D);
                        }
                    }
                    else if (prop->getCount() == 7)
                    {
                        StringList reslist;
                        Nui32 rid;
                        ResourceIDList ridlist;
                        const ScriptNode * i1 = getNode(prop->mValueDataList, 1);
                        if (getValue(prop->mValueDataList, 0, 6, reslist) && getValue(prop->mValueDataList, 0, rid))
                        {
                            ValueScriptNode * atom1 = (ValueScriptNode*)i1;
                            mUnit->setSrc(rid, reslist, Texture::T_CUBE);
                        }
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError, _T("cubic_texture must have at most 7 arguments"));
                    }
                }
                else if (tempid2 == TextureCoordSetProperty)
                {
                    Nui32 val = 0;
                    if(getValue(prop->mValueDataList, 0, val))
                        mUnit->setCoordSet((Nidx16)val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not supported as an integer argument"));
                }
                else if (tempid2 == TextureAddressModeProperty)
                {
                    if(prop->isEmpty())
                    {
                            mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1),
                            *i2 = getNode(prop->mValueDataList, 2);
                        TextureAddressing mode;
                        mode.mS_U = mode.mT_V = mode.mR_W = TAM_REPEAT;

                        if(i0 != 0 && i0->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode *)i0;
                            PropertyID tempid3 = getProperty(atom->mValueData);

                            if(tempid3 == TextureAddressWrapProperty)
                                mode.mS_U = TAM_REPEAT;
                            else if (tempid3 == TextureAddressClampProperty)
                                mode.mS_U = TAM_CLAMP;
                            else if (tempid3 == TextureAddressMirrorProperty)
                                mode.mS_U = TAM_MIRROR;
                            else if (tempid3 == TextureAddressBorderProperty)
                                mode.mS_U = TAM_BORDER;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as first argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")"));
                        }
                        mode.mT_V = mode.mS_U;
                        mode.mR_W = mode.mS_U;

                        if(i1 != 0 && i1->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode*)i1;
                            PropertyID tempid3 = getProperty(atom->mValueData);

                            if(tempid3 == TextureAddressWrapProperty)
                                mode.mT_V = TAM_REPEAT;
                            else if (tempid3 == TextureAddressClampProperty)
                                mode.mT_V = TAM_CLAMP;
                            else if (tempid3 == TextureAddressMirrorProperty)
                                mode.mT_V = TAM_MIRROR;
                            else if (tempid3 == TextureAddressBorderProperty)
                                mode.mT_V = TAM_BORDER;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as second argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")"));
                        }

                        if(i2 != 0 && i2->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode*)i2;
                            PropertyID tempid3 = getProperty(atom->mValueData);

                            if(tempid3 == TextureAddressWrapProperty)
                                mode.mR_W = TAM_REPEAT;
                            else if(tempid3 == TextureAddressClampProperty)
                                mode.mR_W = TAM_CLAMP;
                            else if (TextureAddressMirrorProperty)
                                mode.mR_W = TAM_MIRROR;
                            else if (TextureAddressBorderProperty)
                                mode.mR_W = TAM_BORDER;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as third argument (must be \"wrap\", \"clamp\", \"mirror\", or \"border\")"));
                        }

                        mUnit->getSample()->setMode(mode);
                    }
                }
                else if (tempid2 == TextureBorderColourProperty)
                {
                    Colour val;
                    if(getValue(prop->mValueDataList, 0, val))
                        mUnit->getSample()->setBorder(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError, _T("tex_border_colour only accepts a colour argument"));
                }
                else if (tempid2 == TextureFilteringProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if (prop->getCount() == 1)
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                            PropertyID temp3 = getProperty(atom->mValueData);
                            if(temp3 == NoneProperty)
                                mUnit->getSample()->setFiltering(TFM_NONE);
                            else if (temp3 == TextureBilinearProperty)
                                mUnit->getSample()->setFiltering(TFM_BILINEAR);
                            else if (temp3 == TextureTrilinearProperty)
                                mUnit->getSample()->setFiltering(TFM_TRILINEAR);
                            else if (temp3 == AnisoMathProperty)
                                mUnit->getSample()->setFiltering(TFM_ANISOTROPIC);
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    prop->mValueDataList.front()->getValue() + _T(" not supported as first argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")"));
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" not supported as first argument (must be \"none\", \"bilinear\", \"trilinear\", or \"anisotropic\")"));
                        }
                    }
                    else if (prop->getCount() == 3)
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1),
                            *i2 = getNode(prop->mValueDataList, 2);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value && i2->mType == SNT_Value)
                        {
                            ValueScriptNode *atom0 = (ValueScriptNode*)i0,
                                *atom1 = (ValueScriptNode*)i1,
                                *atom2 = (ValueScriptNode*)i2;
                            TextureFilterOP tmin = TFOP_NONE, tmax = TFOP_NONE, tmip = TFOP_NONE;
                            PropertyID temp3 = getProperty(atom0->mValueData);

                            if(temp3 == NoneProperty)
                                tmin = TFOP_NONE;
                            else if(temp3 == PointMathProperty)
                                tmin = TFOP_POINT;
                            else if(temp3 == LinearMathProperty)
                                tmin = TFOP_LINEAR;
                            else if(temp3 == AnisoMathProperty)
                                tmin = TFOP_ANISOTROPIC;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as first argument (must be \"none\", \"point\", \"linear\", or \"anisotropic\")"));
                          
                            temp3 = getProperty(atom1->mValueData);

                            if(temp3 == NoneProperty)
                                tmax = TFOP_NONE;
                            else if (temp3 == PointMathProperty)
                                tmax = TFOP_POINT;
                            else if (temp3 == LinearMathProperty)
                                tmax = TFOP_LINEAR;
                            else if (temp3 == AnisoMathProperty)
                                tmax = TFOP_ANISOTROPIC;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as second argument (must be \"none\", \"point\", \"linear\", or \"anisotropic\")"));

                            temp3 = getProperty(atom2->mValueData);

                            if (temp3 == NoneProperty)
                                tmip = TFOP_NONE;
                        
                            else if (temp3 == PointMathProperty)
                                tmip = TFOP_POINT;
                        
                            else if (temp3 == LinearMathProperty)
                                tmip = TFOP_LINEAR;
                   
                            else if (temp3 == AnisoMathProperty)
                                tmip = TFOP_ANISOTROPIC;
                          
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" not supported as third argument (must be \"none\", \"point\", \"linear\", or \"anisotropic\")"));
                           

                            mUnit->getSample()->setFiltering(tmin, tmax, tmip);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError, _T("filtering must have either 1 or 3 arguments"));
                    }
                }
                else if (tempid2 == TextureAnisotropyProperty)
                {
                    Nui32 val = 0;
                    if(getValue(prop->mValueDataList.front(), val))
                        mUnit->getSample()->setAnisotropy(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid integer argument"));
                }
                else if (tempid2 == TextureMipmapBiasProperty)
                {
                    NIIf val = 0.0f;
                    if(getValue(prop->mValueDataList.front(), val))
                        mUnit->getSample()->setMipmapBias(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid number argument"));
                }
                else if (tempid2 == TextureColourBlendProperty)
                {
                    if(prop->mValueDataList.front()->mType == SNT_Value)
                    {
                        ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                        PropertyID tempid3 = getProperty(atom->mValueData);

                        if(tempid3 == ReplaceMathProperty)
                            mUnit->getColourBlend()->setColourBlend(TBM_Src_Replace_Dst);
                       
                        else if (tempid3 == AddMathProperty)
                            mUnit->getColourBlend()->setColourBlend(TBM_Src_Add_Dst);
                        
                        else if (tempid3 == ModulateMathProperty)
                            mUnit->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
                         
                        else if (tempid3 == AlphaMathProperty)
                            mUnit->getColourBlend()->setColourBlend(TBM_Src_Alpha_Dst);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid argument (must be \"replace\", \"add\", \"modulate\", or \"alpha_blend\")"));
                    }
                    else
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mValueDataList.front()->getValue() + _T(" is not a valid argument (must be \"replace\", \"add\", \"modulate\", or \"alpha_blend\")"));
                    }
                }
                else if (tempid2 == TextureColourBlend2Property)
                {
                    if (prop->getCount() < 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError,
                            _T("colour_op_ex must have at least 3 arguments"));
                    }
                    else if (prop->getCount() > 10)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("colour_op_ex must have at most 10 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1),
                            *i2 = getNode(prop->mValueDataList, 2);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value && i2->mType == SNT_Value)
                        {
                            ValueScriptNode * atom0 = (ValueScriptNode*)i0,
                                *atom1 = (ValueScriptNode*)i1,
                                *atom2 = (ValueScriptNode*)i2;
                            TextureBlendOp op = TBO_ADD;
                            TextureBlendSource source1 = TBS_PRE_TEXTURE, source2 = TBS_TEXTURE;
                            Colour arg1 = Colour::White, arg2 = Colour::White;
                            NIIf manualBlend = 0.0f;

                            PropertyID tempid3 = getProperty(atom0->mValueData);

                            if(tempid3 == TextureBlendSource1Property)
                                op = TBO_OPERAND1;
                            else if (tempid3 == TextureBlendSource2Property)
                                op = TBO_OPERAND2;
                            else if (tempid3 == ModulateMathProperty)
                                op = TBO_MODULATE;
                            else if (tempid3 == TextureBlendModulateX2Property)
                                op = TBO_MODULATE_2X;
                            else if (tempid3 == TextureBlendModulateX4Property)
                                op = TBO_MODULATE_4X;
                            else if (tempid3 == AddMathProperty)
                                op = TBO_ADD;
                            else if (tempid3 == TextureBlendSignedProperty)
                                op = TBO_ADD_SIGNED;
                            else if (tempid3 == TextureBlendSmoothProperty)
                                op = TBO_ADD_SMOOTH;
                            else if (tempid3 == SubtractMathProperty)
                                op = TBO_SUB;
                            else if (tempid3 == TextureDiffuseBlendProperty)
                                op = TBO_ADD_DIFFUSE;
                            else if (tempid3 == TextureCurrentTextureBlendProperty)
                                op = TBO_ADD_TEXTURE;
                            else if (tempid3 == TexturePreTextureBlendProperty)
                                op = TBO_ADD_PRE_TEXTURE;
                            else if (tempid3 == TextureBlendConstantFactorProperty)
                                op = TBO_CONSTANT_FACTOR;
                            else if (tempid3 == TextureBlendDotProductProperty)
                                op = TBO_DOT_PRODUCT;
                            else if (tempid3 == TextureSpecularBlendProperty)
                                op = TBO_ADD_SPECULAR;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" is not a valid first argument (must be \"source1\", \"source2\", \"modulate\", \"modulate_x2\", \"modulate_x4\", \"add\", \"add_signed\", \"add_smooth\", \"subtract\", \"blend_diffuse_alpha\", \"blend_texture_alpha\", \"blend_current_alpha\", \"blend_manual\", \"dot_product\", or \"blend_diffuse_colour\")"));
           
                            tempid3 = getProperty(atom1->mValueData);

                            if(tempid3 == TextureBlendPreTextureProperty)
                                source1 = TBS_PRE_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentTextureProperty)
                                source1 = TBS_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentDiffuseProperty)
                                source1 = TBS_DIFFUSE;
                            else if (tempid3 == TextureBlendCurrentSpecularProperty)
                                source1 = TBS_SPECULAR;
                            else if (tempid3 == TextureBlendConstantProperty)
                                source1 = TBS_CONSTANT;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i1->getValue() + _T(" is not a valid second argument (must be \"src_current\", \"src_texture\", \"src_diffuse\", \"src_specular\", or \"src_manual\")"));
                          
                            tempid3 = getProperty(atom2->mValueData);

                            if(tempid3 == TextureBlendPreTextureProperty)
                                source2 = TBS_PRE_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentTextureProperty)
                                source2 = TBS_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentDiffuseProperty)
                                source2 = TBS_DIFFUSE;
                            else if (tempid3 == TextureBlendCurrentSpecularProperty)
                                source2 = TBS_SPECULAR;
                            else if (tempid3 == TextureBlendConstantProperty)
                                source2 = TBS_CONSTANT;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i2->getValue() + _T(" is not a valid third argument (must be \"src_current\", \"src_texture\", \"src_diffuse\", \"src_specular\", or \"src_manual\")"));
                          

                            if (op == TBO_CONSTANT_FACTOR)
                            {
                                if (!getValue(prop->mValueDataList, 3, manualBlend))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        getNode(prop->mValueDataList, 3)->getValue() + _T(" is not a valid number argument"));
                                }
                            }

                            ScriptNodeList::const_iterator j = getIterator(prop->mValueDataList, 3);
                            if (op == TBO_CONSTANT_FACTOR)
                                ++j;
                            if (source1 == TBS_CONSTANT)
                            {
                                if (j != prop->mValueDataList.end())
                                {
                                    if (!getValue(prop->mValueDataList, 3, arg1))
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            _T("valid colour expected when src_manual is used"));
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        _T("valid colour expected when src_manual is used"));
                                }
                            }
                            if (source2 == TBS_CONSTANT)
                            {
                                if (j != prop->mValueDataList.end())
                                {
                                    if (!getValue(prop->mValueDataList, 3, arg2))
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            _T("valid colour expected when src_manual is used"));
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        _T("valid colour expected when src_manual is used"));
                                }
                            }

                            TextureBlend * tb = mUnit->getColourBlend();
                            tb->setBlendType(TCT_RGB);
                            tb->setBlend(op, source1, source2);
                            tb->setConstant1(arg1);
                            tb->setConstant2(arg2);
                            tb->setConstantFactor(manualBlend);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                else if (tempid2 == TextureAlphaBlendProperty)
                {
                    if (prop->getCount() < 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError,
                            _T("alpha_op_ex must have at least 3 arguments"));
                    }
                    else if (prop->getCount() > 6)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("alpha_op_ex must have at most 6 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1),
                            *i2 = getNode(prop->mValueDataList, 2);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value && i2->mType == SNT_Value)
                        {
                            ValueScriptNode *atom0 = (ValueScriptNode*)i0,
                                *atom1 = (ValueScriptNode*)i1,
                                *atom2 = (ValueScriptNode*)i2;
                            TextureBlendOp op = TBO_ADD;
                            TextureBlendSource source1 = TBS_PRE_TEXTURE, source2 = TBS_TEXTURE;
                            NIIf arg1 = 0.0f, arg2 = 0.0f;
                            NIIf manualBlend = 0.0f;
                            PropertyID tempid3 = getProperty(atom0->mValueData);

                            if(tempid3 == TextureBlendSource1Property)
                                op = TBO_OPERAND1;
                              
                            else if (tempid3 == TextureBlendSource2Property)
                                op = TBO_OPERAND2;
                               
                            else if (tempid3 == ModulateMathProperty)
                                op = TBO_MODULATE;
                              
                            else if (tempid3 == TextureBlendModulateX2Property)
                                op = TBO_MODULATE_2X;
                         
                            else if (tempid3 == TextureBlendModulateX4Property)
                                op = TBO_MODULATE_4X;
                          
                            else if (tempid3 == AddMathProperty)
                                op = TBO_ADD;
                         
                            else if (tempid3 == TextureBlendSignedProperty)
                                op = TBO_ADD_SIGNED;
                         
                            else if (tempid3 == TextureBlendSmoothProperty)
                                op = TBO_ADD_SMOOTH;
                        
                            else if (tempid3 == SubtractMathProperty)
                                op = TBO_SUB;
                          
                            else if (tempid3 == TextureDiffuseBlendProperty)
                                op = TBO_ADD_DIFFUSE;
                           
                            else if (tempid3 == TextureCurrentTextureBlendProperty)
                                op = TBO_ADD_TEXTURE;
                          
                            else if (tempid3 == TexturePreTextureBlendProperty)
                                op = TBO_ADD_PRE_TEXTURE;
                        
                            else if (tempid3 == TextureBlendConstantFactorProperty)
                                op = TBO_CONSTANT_FACTOR;
                         
                            else if (tempid3 == TextureBlendDotProductProperty)
                                op = TBO_DOT_PRODUCT;
                           
                            else if (tempid3 == TextureSpecularBlendProperty)
                                op = TBO_ADD_SPECULAR;
                          
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i0->getValue() + _T(" is not a valid first argument (must be \"source1\", \"source2\", \"modulate\", \"modulate_x2\", \"modulate_x4\", \"add\", \"add_signed\", \"add_smooth\", \"subtract\", \"blend_diffuse_alpha\", \"blend_texture_alpha\", \"blend_current_alpha\", \"blend_manual\", \"dot_product\", or \"blend_diffuse_colour\")"));
                         
                            tempid3 = getProperty(atom1->mValueData);

                            if(tempid3 == TextureBlendPreTextureProperty)
                                source1 = TBS_PRE_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentTextureProperty)
                                source1 = TBS_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentDiffuseProperty)
                                source1 = TBS_DIFFUSE;
                            else if (tempid3 == TextureBlendCurrentSpecularProperty)
                                source1 = TBS_SPECULAR;
                            else if (tempid3 == TextureBlendConstantProperty)
                                source1 = TBS_CONSTANT;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i1->getValue() + _T(" is not a valid second argument (must be \"src_current\", \"src_texture\", \"src_diffuse\", \"src_specular\", or \"src_manual\")"));

                            tempid3 = getProperty(atom2->mValueData);

                            if (tempid3 == TextureBlendPreTextureProperty)
                                source2 = TBS_PRE_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentTextureProperty)
                                source2 = TBS_TEXTURE;
                            else if (tempid3 == TextureBlendCurrentDiffuseProperty)
                                source2 = TBS_DIFFUSE;
                            else if (tempid3 == TextureBlendCurrentSpecularProperty)
                                source2 = TBS_SPECULAR;
                            else if (tempid3 == TextureBlendConstantProperty)
                                source2 = TBS_CONSTANT;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    i2->getValue() + _T(" is not a valid third argument (must be \"src_current\", \"src_texture\", \"src_diffuse\", \"src_specular\", or \"src_manual\")"));
       

                            if (op == TBO_CONSTANT_FACTOR)
                            {
                                if (!getValue(prop->mValueDataList, 3, manualBlend))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("valid number expected when blend_manual is used"));
                                }
                            }

                            ScriptNodeList::const_iterator j = getIterator(prop->mValueDataList, 3);
                            if (op == TBO_CONSTANT_FACTOR)
                                ++j;
                            if (source1 == TBS_CONSTANT)
                            {
                                if (j != prop->mValueDataList.end())
                                {
                                    if (!getValue(*j, arg1))
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            _T("valid colour expected when src_manual is used"));
                                    else
                                        ++j;
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        _T("valid colour expected when src_manual is used"));
                                }
                            }
                            if (source2 == TBS_CONSTANT)
                            {
                                if (j != prop->mValueDataList.end())
                                {
                                    if (!getValue(*j, arg2))
                                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                            _T("valid colour expected when src_manual is used"));
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        _T("valid colour expected when src_manual is used"));
                                }
                            }

                            TextureBlend * tb = mUnit->getAlphaBlend();
                            tb->setBlendType(TCT_AAA);
                            tb->setBlend(op, source1, source2);
                            tb->setConstant1(arg1);
                            tb->setConstant2(arg2);
                            tb->setConstantFactor(manualBlend);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                else if (tempid2 == TextureEnvMapProperty)
                {
                    String pvalue;
                    if (getValue(prop->mValueDataList, 0, pvalue))
                    {
                        PropertyID tempid3 = getProperty(pvalue);

                        if(tempid3 == FalseProperty)
                            mUnit->setEnvMap(false);
                        else if (tempid3 == TextureSphericalProperty)
                        {
                            mUnit->setEnvMap(true);
                            mUnit->setEnvMapType(ShaderChTextureUnit::ENM_Sphere);
                        }
                        else if (tempid3 == TexturePlanarProperty)
                        {
                            mUnit->setEnvMap(true);
                            mUnit->setEnvMapType(ShaderChTextureUnit::ENM_Planar);
                        }
                        else if (tempid3 == TextureReflectionProperty)
                        {
                            mUnit->setEnvMap(true);
                            mUnit->setEnvMapType(ShaderChTextureUnit::ENM_Reflection);
                        }
                        else if (tempid3 == TextureNormalMapProperty)
                        {
                            mUnit->setEnvMap(true);
                            mUnit->setEnvMapType(ShaderChTextureUnit::ENM_Normal);
                        }
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mPropertyName + _T(" is not a valid argument (must be \"off\", \"spherical\", \"planar\", \"cubic_reflection\", or \"cubic_normal\")"));
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is not a valid argument (must be \"off\", \"spherical\", \"planar\", \"cubic_reflection\", or \"cubic_normal\")"));
                    }
                }
                else if (tempid2 == TextureScrollProperty)
                {
                    NIIf x, y;
                    if(getValue(prop->mValueDataList, 0, x) && getValue(prop->mValueDataList, 1, y))
                    {
                        mUnit->setUOft(x);
                        mUnit->setVOft(y);
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is invalid; both must be numbers"));
                }
                else if (tempid2 == TextureScrollAnimProperty)
                {
                    NIIf x, y;
                    if(getValue(prop->mValueDataList, 0, x) && getValue(prop->mValueDataList, 1, y))
                        mUnit->setScrollAnimation(x, y);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is invalid; both must be numbers"));
                }
                else if (tempid2 == TextureRotateProperty)
                {
                    NIIf angle;
                    if (getValue(prop->mValueDataList, 0, angle))
                    {
                        Angle tempangle(angle);
                        mUnit->setRotate(tempangle);
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is not a valid number value"));
                }
                else if (tempid2 == TextureRotateAnimProperty)
                {
                    NIIf angle;
                    if(getValue(prop->mValueDataList, 0, angle))
                        mUnit->setRotateAnimation(angle);
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is not a valid number value"));
                }
                else if (tempid2 == TextureScaleProperty)
                {
                    NIIf x, y;
                    if(getValue(prop->mValueDataList, 0, x) && getValue(prop->mValueDataList, 1, y))
                    {
                        mUnit->setUScale(x);
                        mUnit->setVScale(y);
                    }
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("first and second arguments must both be valid number mValueDataList (received ") +
                                prop->mPropertyName + _T(")"));
                }
                else if (tempid2 == TextureWaveXFormProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                    }
                    else if (prop->getCount() > 6)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("wave_xform must have at most 6 arguments"));
                    }
                    else
                    {
                        const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                            *i1 = getNode(prop->mValueDataList, 1),
                            *i2 = getNode(prop->mValueDataList, 2),
                            *i3 = getNode(prop->mValueDataList, 3),
                            *i4 = getNode(prop->mValueDataList, 4),
                            *i5 = getNode(prop->mValueDataList, 5);
                        if (i0->mType == SNT_Value && i1->mType == SNT_Value && i2->mType == SNT_Value &&
                            i3->mType == SNT_Value && i4->mType == SNT_Value && i5->mType == SNT_Value)
                        {
                            ValueScriptNode * atom0 = (ValueScriptNode *)i0,
                                *atom1 = (ValueScriptNode*)i1;
                            ShaderChTextureUnit::TextureTransformType type = ShaderChTextureUnit::TTT_ROTATE;
                            WaveformType wave = WFT_SINE;
                            NIIf base = 0.0f, freq = 0.0f, phase = 0.0f, amp = 0.0f;
                            DataValue<NIIf> * val = 0;
                            PropertyID tempid3 = getProperty(atom0->mValueData);

                            if (tempid3 == TextureScrollXProperty)
                            {
                                type = ShaderChTextureUnit::TTT_U_OFT;
                                val = N_new TexCoordModifierControllerValue(true, mUnit, true);
                            }
                            else if (tempid3 == TextureScrollYProperty)
                            {
                                type = ShaderChTextureUnit::TTT_V_OFT;
                                val = N_new TexCoordModifierControllerValue(true, mUnit, false, true);
                            }
                            else if (tempid3 == TextureScaleXProperty)
                            {
                                type = ShaderChTextureUnit::TTT_U_SCALE;
                                val = N_new TexCoordModifierControllerValue(true, mUnit, false, false, true);
                            }
                            else if (tempid3 == TextureScaleYProperty)
                            {
                                type = ShaderChTextureUnit::TTT_V_SCALE;
                                val = N_new TexCoordModifierControllerValue(true, mUnit, false, false, false, true);
                            }
                            else if (tempid3 == TextureRotateProperty)
                            {
                                type = ShaderChTextureUnit::TTT_ROTATE;
                                val = N_new TexCoordModifierControllerValue(true, mUnit, false, false, false, false, true);
                            }
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom0->mValueData + _T(" is not a valid transform type (must be \"scroll_x\", \"scroll_y\", \"scale_x\", \"scale_y\", or \"rotate\")"));
                     
                            tempid3 = getProperty(atom1->mValueData);

                            if (tempid3 == TextureWaveSineProperty)
                                wave = WFT_SINE;
                            else if (tempid3 == TextureWaveTriangleProperty)
                                wave = WFT_TRIANGLE;
                            else if (tempid3 == TextureWaveSquareProperty)
                                wave = WFT_SQUARE;
                            else if (tempid3 == TextureWaveSawtoothProperty)
                                wave = WFT_SAWTOOTH;
                            else if (tempid3 == TextureWaveInvSawtoothProperty)
                                wave = WFT_INVERSE_SAWTOOTH;
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom1->mValueData + _T(" is not a valid waveform type (must be \"sine\", \"triangle\", \"square\", \"sawtooth\", or \"inverse_sawtooth\")"));
                         

                            if (!getValue(i2, base) || !getValue(i3, freq) || !getValue(i4, phase) || !getValue(i5, amp))
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    _T("arguments 3, 4, 5, and 6 must be valid numbers; received ") +
                                    i2->getValue() + _T(", ") + i3->getValue() + _T(", ") + i4->getValue() + _T(", ") + i5->getValue());

                            DataFunc<TimeDurMS, NIIf> * func = N_new WaveformControllerFunction(true, wave, base, freq, phase, amp, true);
                            mUnit->addAnimation(type, val, func);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                else if (tempid2 == TextureMatrixProperty)
                {
                    Matrix4f m;
                    if(getValue(prop->mValueDataList, 0, m))
                        mUnit->setMatrix(m);
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError);
                }
                else if (tempid2 == TextureFetchTypeProperty)
                {
                    String pvalue;
                    if(getValue(prop->mValueDataList, 0, pvalue))
                    {
                        PropertyID tempid3 = getProperty(pvalue);
                        if(tempid3 == TextureFetchVertexProperty)
                            mUnit->setFetchType(TDF_VS);
                        else if (tempid3 == TextureFetchFragmentProperty)
                            mUnit->setFetchType(TDF_FS);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mPropertyName + _T(" is not a valid binding type (must be \"vertex\" or \"fragment\")"));
                    }
                    else
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            prop->mPropertyName + _T(" is not a valid binding type"));
                    }
                }
                else if (tempid2 == TextureContentTypeProperty)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                    }
                    else if (prop->getCount() > 4)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("content_type must have at most 4 arguments"));
                    }
                    else
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode *)prop->mValueDataList.front();
                            PropertyID tempid3 = getProperty(atom->mValueData);

                            if(tempid3 == TextureNormalContentTypeProperty)
                                mUnit->setContentType(ShaderChTexture::T_NORMAL);
                            else if (tempid3 == TextureShadowContentTypeProperty)
                                mUnit->setContentType(ShaderChTexture::T_SHADOW);
                            else if (tempid3 == TextureFusionContentTypeProperty)
                            {
                                mUnit->setContentType(ShaderChTexture::T_FUSION);
                                if (prop->getCount() >= 3)
                                {
                                    Nui32 compositorName;
                                    String textureName;
                                    getValue(prop->mValueDataList, 1, compositorName);
                                    getValue(prop->mValueDataList, 2, textureName);

                                    if (prop->getCount() == 4)
                                    {
                                        Nui32 mrtIndex;
                                        getValue(prop->mValueDataList, 3, mrtIndex);
                                        mUnit->setFusion(compositorName, textureName);
                                        mUnit->setMultiIndex(mrtIndex);
                                    }
                                    else
                                    {
                                        mUnit->setFusion(compositorName, textureName);
                                    }
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("content_type compositor must have an additional 2 or 3 parameters"));
                                }

                            }
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                    atom->mValueData + _T(" is not a valid content type (must be \"named\" or \"shadow\" or \"compositor\")"));
                         
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                prop->mValueDataList.front()->getValue() + _T(" is not a valid content type"));
                        }
                    }
                }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
            }
            else if((*i)->mType == SNT_Object)
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    void MaterialScriptAnalyze::analyzeTextureSrc(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // It has to have one value identifying the texture source name
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*node, ScriptError::ET_StringError,
                _T("texture_source requires a type value"));
            return;
        }

        // Set the value of the source
        N_Only(Texture).setPlugin(obj->mParamValue.front()->getValue());

        // Set up the fusion, ch, and texunit levels
        if(N_Only(Texture).getPlugin() != 0)
        {
            ShaderChTextureUnit * texunit = static_cast<ShaderChTextureUnit *>(obj->mParent->mData);
            ShaderCh * ch = texunit->getParent();
            ShaderFusion * fusion = ch->getParent();
            Material * material = fusion->getParent();

            Nidx techniqueIndex = 0;
            Nidx passIndex = 0;
            Nidx texUnitIndex = 0;

            for(Nidx i = 0; i < material->getCount(); ++i)
            {
                if(material->get(i) == fusion)
                {
                    techniqueIndex = i;
                    break;
                }
            }
            for(Nidx i = 0; i < fusion->getCount(); ++i)
            {
                if(fusion->get(i) == ch)
                {
                    passIndex = i;
                    break;
                }
            }
            for(Nidx i = 0; i < ch->getTexture().getCount(); ++i)
            {
                if(ch->getTexture().get(i) == texunit)
                {
                    texUnitIndex = i;
                    break;
                }
            }

            //String tps;
            //tps = StrConv::conv(techniqueIndex) + _T(" ") + StrConv::conv(passIndex) + _T(" ") + StrConv::conv(texUnitIndex);

            N_Only(Texture).getPlugin()->setShaderFusion(techniqueIndex);
            N_Only(Texture).getPlugin()->setShaderCh(passIndex);
            N_Only(Texture).getPlugin()->setRenderLevel(texUnitIndex);
            for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
            {
                if((*i)->mType == SNT_Property)
                {
                    PropertyScriptNode * prop = (PropertyScriptNode*)(*i);
                    // Glob the property mValueDataList all together
                    String str = _T("");
                    ScriptNodeList::iterator j, jend = prop->mValueDataList.end();
                    for(j = prop->mValueDataList.begin(); j != jend; ++j)
                    {
                        if(j == prop->mValueDataList.begin())
                            str = str + (*j)->getValue();
                        else
                            str = str + _T(" ") + (*j)->getValue();
                    }
                    N_Only(Texture).getPlugin()->set(getProperty(prop->mPropertyName), str);
                }
                else if((*i)->mType == SNT_Object)
                {
                    ScriptAnalyze::analyze(*i);
                }
            }

            N_Only(Texture).getPlugin()->createTexture(material->getOriginID(), material->getGroup());
        }
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // GpuProgramScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    GpuProgramScriptAnalyze::GpuProgramScriptAnalyze(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_GpuProgram, lid)
    {
    }
    //-------------------------------------------------------------------------
    void GpuProgramScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // Must have a name
        if(obj->mName.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_ObjIDError,
                _T("gpu program object must have names"));
            return;
        }

        // Must have a language type
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj,ScriptError::ET_StringError,
                _T("gpu program object require language declarations"));
            return;
        }

        // Get the language
        String language;
        if(!getValue(obj->mParamValue.front(), language))
        {
            mCompiler->error(*obj,ScriptError::ET_InvalidParamError);
            return;
        }

        if(language == _T("asm"))
            analyzeASM(obj);
        else if(language == _T("alter"))
            analyzeAlter(obj);
        else
            analyzeHighLevel(obj);
    }
    //-------------------------------------------------------------------------
    ScriptUnitID GpuProgramScriptAnalyze::getUnit()
    {
        return N_Script_Unit_GpuProgram;
    }
    //-------------------------------------------------------------------------
    GroupID GpuProgramScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    void GpuProgramScriptAnalyze::analyzeASM(const ObjectScriptNode * obj)
    {
        list<std::pair<PropertyID, String> >::type customParameters;
        String syntaxt, source;
        ScriptNode * params = 0;
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = (PropertyScriptNode *)(*i);
                PropertyID tempid2 = getProperty(prop->mPropertyName);
                if(tempid2 == N_PropertyGpuProgram_Src)
                {
                    if(!getValue(prop->mValueDataList, 0, source))
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            _T("source mFile expected"));
                    }
                }
                else if(tempid2 == N_PropertyGpuProgram_Syntax)
                {
                    if(!getValue(prop->mValueDataList, 0, syntaxt))
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            _T("syntax string expected"));
                    }
                }
                else
                {
                    String value;
                    bool first = true;
                    PropertyID name = getProperty(prop->mPropertyName);
                    ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                    for(it = prop->mValueDataList.begin(); it != itend; ++it)
                    {
                        if((*it)->mType == SNT_Value)
                        {
                            if(!first)
                                value += _T(" ");
                            else
                                first = false;
                            value += ((ValueScriptNode *)(*it))->mValueData;
                        }
                    }
                    customParameters.push_back(Npair(name, value));
                }
            }
            else if((*i)->mType == SNT_Object)
            {
                if(getProperty(((ObjectScriptNode *)(*i))->mClass) == N_PropertyGpuProgram_Param)
                    params = *i;
                else
                    ScriptAnalyze::analyze(*i);
            }
        }

        ShaderLanguage syntax;
        Nui32 tempid;
        StrConv::conv(syntaxt, syntax);
        if(!N_Only(GpuProgram).isSupport(syntax))
        {
            //Register the unsupported program so that materials that use it know that
            //it exists but is unsupported
            StrConv::conv(obj->mName, tempid);
            GpuProgram * unsupportedProg = N_Only(GpuProgram).create(tempid,
                mCompiler->getResourceGroup(), __ToGpuProgramType(getProperty(obj->mClass)), syntax);
            return;
        }

        // Allocate the program
        StrConv::conv(obj->mName, tempid);
        GpuProgram * prog = reinterpret_cast<GpuProgram *>(N_Only(GpuProgram).create(tempid,
            mCompiler->getResourceGroup(), source, __ToGpuProgramType(getProperty(obj->mClass)), syntax));

        prog->setVertexInterpolation(false);
        prog->setVertexOffsetCount(0);
        prog->setVertexMatrixFusion(false);
        prog->setVertexTextureFetch(false);
        prog->setSrc(obj->mFile);

        obj->mData = prog;

        // Set the custom parameters
        for(list<std::pair<PropertyID, String> >::type::iterator i = customParameters.begin(); i != customParameters.end(); ++i)
            prog->set(i->first, i->second);

        // Set up default parameters
        if(prog->isValid() && params != 0)
        {
            GpuProgramParam * ptr = prog->createDefaultParam();
            GpuProgramScriptAnalyze::analyzeParam(ptr, reinterpret_cast<ObjectScriptNode *>(params));
        }
    }
    //-------------------------------------------------------------------------
    void GpuProgramScriptAnalyze::analyzeAlter(const ObjectScriptNode * obj)
    {
        list<std::pair<PropertyID, String> >::type customParameters;
        ScriptNode * params = 0;
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = (PropertyScriptNode*)(*i);
                if(getProperty(prop->mPropertyName) == N_PropertyAlterGpuProgram_Alter)
                {
                    String value;
                    getValue(prop->mValueDataList, 0, value);
                    customParameters.push_back(Npair(N_PropertyAlterGpuProgram_Alter, value));
                }
                else
                {

                    PropertyID name = getProperty(prop->mPropertyName);
                    String value;
                    ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                    for(it = prop->mValueDataList.begin(); it != itend; ++it)
                    {
                        if((*it)->mType == SNT_Value)
                        {
                            if(it == prop->mValueDataList.begin())
                                value = ((ValueScriptNode*)(*it))->mValueData;
                            else
                                value = value + _T(" ") + ((ValueScriptNode*)(*it))->mValueData;
                        }
                    }
                    customParameters.push_back(Npair(name, value));
                }
            }
            else if((*i)->mType == SNT_Object)
            {
                if(getProperty(((ObjectScriptNode *)(*i))->mClass) == N_PropertyGpuProgram_Param)
                    params = *i;
                else
                    ScriptAnalyze::analyze(*i);
            }
        }

        // Allocate the program
        Nui32 tempid;
        StrConv::conv(obj->mName, tempid);
        HighLevelGpuProgram * prog = reinterpret_cast<HighLevelGpuProgram *>(
            N_Only(HighLevelGpuProgram).create(tempid, mCompiler->getResourceGroup(),
                SL_ALTER, __ToGpuProgramType(getProperty(obj->mClass))));

        prog->setVertexInterpolation(false);
        prog->setVertexOffsetCount(0);
        prog->setVertexMatrixFusion(false);
        prog->setVertexTextureFetch(false);
        prog->setSrc(obj->mFile);

        obj->mData = prog;

        // Set the custom parameters
        for(list<std::pair<PropertyID, String> >::type::iterator i = customParameters.begin(); i != customParameters.end(); ++i)
            prog->set(i->first, i->second);

        // Set up default parameters
        if(prog->isValid() && params != 0)
        {
            GpuProgramParam * ptr = prog->createDefaultParam();
            GpuProgramScriptAnalyze::analyzeParam(ptr, reinterpret_cast<ObjectScriptNode *>(params));
        }
    }
    //-------------------------------------------------------------------------
    void GpuProgramScriptAnalyze::analyzeHighLevel(const ObjectScriptNode * obj)
    {
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_StringError);
            return;
        }
        String languaget;
        if(!getValue(obj->mParamValue.front(), languaget))
        {
            mCompiler->error(*obj, ScriptError::ET_InvalidParamError);
            return;
        }

        list<std::pair<PropertyID, String> >::type customParameters;
        String source;
        ScriptNode * params = 0;
        ScriptNodeList::const_iterator i, iend= obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = (PropertyScriptNode*)(*i);
                if(getProperty(prop->mPropertyName) == N_PropertyGpuProgram_Src)
                {
                    if(!getValue(prop->mValueDataList, 0, source))
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                            _T("source file expected"));
                    }
                }
                else
                {
                    PropertyID name = getProperty(prop->mPropertyName);
                    String value;
                    ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                    for(it = prop->mValueDataList.begin(); it != itend; ++it)
                    {
                        if((*it)->mType == SNT_Value)
                        {
                            if(it == prop->mValueDataList.begin())
                                value = ((ValueScriptNode *)(*it))->mValueData;
                            else
                                value = value + _T(" ") + ((ValueScriptNode*)(*it))->mValueData;
                        }
                    }
                    customParameters.push_back(Npair(name, value));
                }
            }
            else if((*i)->mType == SNT_Object)
            {
                if(getProperty(((ObjectScriptNode *)(*i))->mClass) == N_PropertyGpuProgram_Param)
                    params = *i;
                else
                    ScriptAnalyze::analyze(*i);
            }
        }
        ShaderLanguage language;
        StrConv::conv(languaget, language);
        Nui32 tempid;
        StrConv::conv(obj->mName, tempid);
        // Allocate the program
        HighLevelGpuProgram * prog = reinterpret_cast<HighLevelGpuProgram *>(
            N_Only(HighLevelGpuProgram).create(tempid, mCompiler->getResourceGroup(), 
                language, __ToGpuProgramType(getProperty(obj->mClass))));

        prog->setProgramSrc(source);
        prog->setVertexInterpolation(false);
        prog->setVertexOffsetCount(0);
        prog->setVertexMatrixFusion(false);
        prog->setVertexTextureFetch(false);
        prog->setSrc(obj->mFile);

        obj->mData = prog;

        // Set the custom parameters
        for(list<std::pair<PropertyID, String> >::type::iterator i = customParameters.begin(); i != customParameters.end(); ++i)
            prog->set(i->first, i->second);

        // Set up default parameters
        if(prog->isValid() && params != 0)
        {
            GpuProgramParam * ptr = prog->createDefaultParam();
            analyzeParam(ptr, reinterpret_cast<ObjectScriptNode*>(params));
        }
    }
    //-------------------------------------------------------------------------
    void GpuProgramScriptAnalyze::analyzeParam(GpuProgramParam * params, ObjectScriptNode * obj)
    {
        NCount animParametricsCount = 0;

        for(ScriptNodeList::iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = getProperty(prop->mPropertyName);
                if(tempid == N_PropertyGpuProgram_ParamValue)
                {
                    if(prop->getCount() != 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("shared_params_ref requires a single parameter"));
                        continue;
                    }

                    ValueScriptNode * atom0 = (ValueScriptNode*)getNode(prop->mValueDataList, 0);
                    if(atom0->mType != SNT_Value)
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("shared parameter set name expected"));
                        continue;
                    }

                    try
                    {
                        params->addValue(atom0->mValueData);
                    }
                    catch(Exception & e)
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError, e.getVerbose());
                    }
                }
                else if(tempid == N_PropertyGpuProgram_ParamIndex || tempid == N_PropertyGpuProgram_ParamName)
                {
                    if(prop->getCount() >= 3)
                    {
                        bool named = (getProperty(prop->mPropertyName) == N_PropertyGpuProgram_ParamName);
                        const ScriptNode *i0 = getNode(prop->mValueDataList, 0);
                        const ScriptNode *i1 = getNode(prop->mValueDataList, 1);

                        if(i0->mType != SNT_Value || i1->mType != SNT_Value)
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("name or index and parameter type expected"));
                            return;
                        }

                        ValueScriptNode * atom0 = (ValueScriptNode*)i0, *atom1 = (ValueScriptNode*)i1;
                        if(!named && !StrConv::isNumber(atom0->mValueData))
                        {
                            mCompiler->error(*prop, ScriptError::ET_NumberError, _T("parameter index expected"));
                            return;
                        }

                        String name;
                        NCount index = 0;
                        // Assign the name/index
                        if(named)
                            name = atom0->mValueData;
                        else
                            StrConv::conv(atom0->mValueData, index);

                            // Determine the type
                        if(atom1->mValueData == _T("matrix4x4"))
                        {
                            Matrix4f m;
                            if(getValue(prop->mValueDataList, 2, m))
                            {
                                try
                                {
                                    if (named)
                                    {
                                        VString tempstr;
                                        StrConv::conv(name, tempstr);
                                        params->set(tempstr, 0, m);
                                    }
                                    else
                                        params->set(index, 0, m);
                                }
                                catch(...)
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("setting matrix4x4 parameter failed"));
                                }
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_NumberError,
                                    _T("incorrect matrix4x4 declaration"));
                            }
                        }
                        else
                        {
                                // Find the number of parameters
                                bool isValid = true;
                                bool type = true;
                                NIIi count;
                                if(atom1->mValueData.find(_T("float")) != String::npos)
                                {
                                    type = true;
                                    if(atom1->mValueData.size() >= 6)
                                        StrConv::conv(atom1->mValueData.substr(5), count);
                                    else
                                    {
                                        count = 1;
                                    }
                                }
                                else if(atom1->mValueData.find(_T("int")) != String::npos)
                                {
                                    type = false;
                                    if(atom1->mValueData.size() >= 4)
                                        StrConv::conv(atom1->mValueData.substr(3), count);
                                    else
                                    {
                                        count = 1;
                                    }
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("incorrect type specified; only variants of NIIi and NIIf allowed"));
                                    isValid = false;
                                }

                            if(isValid)
                            {
                                    // First, clear out any offending auto constants
                                if (named)
                                {
                                    VString tempstr;
                                    StrConv::conv(name, tempstr);
                                    params->removeSyncParam(tempstr);
                                }
                                else
                                    params->removeSyncParam(index);

                                NIIi roundedCount = count % 4 != 0 ? count + 4 - (count % 4) : count;
                                if(type == false)
                                {
                                    NIIi * vals = N_alloc_t(NIIi, roundedCount);
                                    if(getValue(prop->mValueDataList, 2, roundedCount, vals))
                                    {
                                        try
                                        {
                                            if (named)
                                            {
                                                VString tempstr;
                                                StrConv::conv(name, tempstr);
                                                params->set(tempstr, 0, vals, count, 1);
                                            }
                                            else
                                                params->set(index, 0, vals, roundedCount/4);
                                        }
                                        catch(...)
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("setting of constant failed"));
                                        }
                                    }
                                    else
                                    {
                                        mCompiler->error(*prop, ScriptError::ET_NumberError,
                                            _T("incorrect integer constant declaration"));
                                    }
                                    N_free(vals);
                                }
                                else
                                {
                                    NIIf * vals = N_alloc_t(NIIf, roundedCount);
                                    if(getValue(prop->mValueDataList, 2, roundedCount, vals))
                                    {
                                        try
                                        {
                                            if (named)
                                            {
                                                VString tempstr;
                                                StrConv::conv(name, tempstr);
                                                params->set(tempstr, 0, vals, count, 1);
                                            }
                                            else
                                                params->set(index, 0, vals, roundedCount / 4);
                                        }
                                        catch(...)
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("setting of constant failed"));
                                        }
                                    }
                                    else
                                    {
                                        mCompiler->error(*prop, ScriptError::ET_NumberError,
                                            _T("incorrect NIIf constant declaration"));
                                    }
                                    N_free(vals);
                                }
                            }
                        }
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("param_named and param_indexed properties requires at least 3 arguments"));
                    }
                }
                if(tempid == N_PropertyGpuProgram_SParamIndex || N_PropertyGpuProgram_SParamName)
                {
                        bool named = (getProperty(prop->mPropertyName) == N_PropertyGpuProgram_SParamName);
                        String name;

                        if(prop->getCount() >= 2)
                        {
                            NCount index;
                            const ScriptNode * i0 = getNode(prop->mValueDataList, 0),
                                *i1 = getNode(prop->mValueDataList, 1),
                                *i2 = getNode(prop->mValueDataList, 2),
                                *i3 = getNode(prop->mValueDataList, 3);
                            if(i0->mType != SNT_Value || i1->mType != SNT_Value)
                            {
                                mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                                    _T("name or index and auto constant type expected"));
                                return;
                            }
                            ValueScriptNode *atom0 = (ValueScriptNode*)i0, *atom1 = (ValueScriptNode*)i1;
                            if(!named && !StrConv::isNumber(atom0->mValueData))
                            {
                                mCompiler->error(*prop, ScriptError::ET_NumberError, _T("parameter index expected"));
                                return;
                            }

                            if(named)
                            {
                                name = atom0->mValueData;
                                index = 0;
                            }
                            else
                                StrConv::conv(atom0->mValueData, index);

                            // Look up the auto constant
                            StrUtil::toLower(atom1->mValueData);
                            const GpuParamUnit * def = GpuProgramManager::getSyncParam(atom1->mValueData);
                            if(def)
                            {
                                switch(def->mInputType)
                                {
                                case SPI_None:
                                    // Set the auto constant
                                    try
                                    {
                                        if (named)
                                        {
                                            VString tempstr;
                                            StrConv::conv(name, tempstr);
                                            params->set(tempstr, def->mSyncParam);
                                        }
                                        else
                                            params->set(index, def->mSyncParam);
                                    }
                                    catch(...)
                                    {
                                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                                            _T("setting of constant failed"));
                                    }
                                    break;
                                case SPI_Int:
                                    if(def->mSyncParam == GSP_Ch_AniParam)
                                    {
                                        try
                                        {
                                            if (named)
                                            {
                                                VString tempstr;
                                                StrConv::conv(name, tempstr);
                                                params->set(tempstr, def->mSyncParam, (NIIi)animParametricsCount++);
                                            }
                                            else
                                                params->set(index, def->mSyncParam, (NIIi)animParametricsCount++);
                                        }
                                        catch(...)
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("setting of constant failed"));
                                        }
                                    }
                                    else
                                    {
                                        // Only certain texture projection auto params will assume 0
                                        // Otherwise we will expect that 3rd parameter
                                        if(i2 == 0)
                                        {
                                            if(def->mSyncParam == GSP_Texture_Matrix_Proj_X_View_ParamIndex ||
                                                def->mSyncParam == GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex ||
                                                def->mSyncParam == GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex ||
                                                def->mSyncParam == GSP_Light_Spot_Matrix_Proj_X_View_X_Space
                                                )
                                            {
                                                try
                                                {
                                                    if (named)
                                                    {
                                                        VString tempstr;
                                                        StrConv::conv(name, tempstr);
                                                        params->set(tempstr, def->mSyncParam, (NIIi)0);
                                                    }
                                                    else
                                                        params->set(index, def->mSyncParam, (NIIi)0);
                                                }
                                                catch(...)
                                                {
                                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                        _T("setting of constant failed"));
                                                }
                                            }
                                            else
                                            {
                                                mCompiler->error(*prop, ScriptError::ET_NumberError,
                                                    _T("extra parameters required by constant definition ") + atom1->mValueData);
                                            }
                                        }
                                        else
                                        {
                                            bool success = false;
                                            Nui32 extraInfo = 0;
                                            if(i3 == 0)
                                            { // Handle only one extra value
                                                if(getValue(i2, extraInfo))
                                                {
                                                    success = true;
                                                }
                                            }
                                            else
                                            { // Handle two extra mValueDataList
                                                Nui32 extraInfo1 = 0, extraInfo2 = 0;
                                                if(getValue(i2, extraInfo1) && getValue(i3, extraInfo2))
                                                {
                                                    extraInfo = extraInfo1 | (extraInfo2 << 16);
                                                    success = true;
                                                }
                                            }

                                            if(success)
                                            {
                                                try
                                                {
                                                    if (named)
                                                    {
                                                        VString tempstr;
                                                        StrConv::conv(name, tempstr);
                                                        params->set(tempstr, def->mSyncParam, (NIIi)extraInfo);
                                                    }
                                                    else
                                                        params->set(index, def->mSyncParam, (NIIi)extraInfo);
                                                }
                                                catch(...)
                                                {
                                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                        _T("setting of constant failed"));
                                                }
                                            }
                                            else
                                            {
                                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                    _T("invalid auto constant extra info parameter"));
                                            }
                                        }
                                    }
                                    break;
                                case SPI_Float:
                                    if(def->mSyncParam == GSP_Sys_CostTime ||
                                        def->mSyncParam == GSP_Frame_CostTime)
                                    {
                                        NIIf f = 1.0f;
                                        if(i2 != 0)
                                            getValue(i2, f);

                                        try
                                        {
                                            if (named)
                                            {
                                                VString tempstr;
                                                StrConv::conv(name, tempstr);
                                                params->set(tempstr, def->mSyncParam, f);
                                            }
                                            else
                                                params->set(index, def->mSyncParam, f);
                                        }
                                        catch(...)
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                _T("setting of constant failed"));
                                        }
                                    }
                                    else
                                    {
                                        if(i2 != 0)
                                        {
                                            NIIf extraInfo = 0.0f;
                                            if(getValue(i2, extraInfo))
                                            {
                                                try
                                                {
                                                    if (named)
                                                    {
                                                        VString tempstr;
                                                        StrConv::conv(name, tempstr);
                                                        params->set(tempstr, def->mSyncParam, extraInfo);
                                                    }
                                                    else
                                                        params->set(index, def->mSyncParam, extraInfo);
                                                }
                                                catch(...)
                                                {
                                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                        _T("setting of constant failed"));
                                                }
                                            }
                                            else
                                            {
                                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                                    _T("incorrect NIIf argument definition in extra parameters"));
                                            }
                                        }
                                        else
                                        {
                                            mCompiler->error(*prop, ScriptError::ET_NumberError,
                                                _T("extra parameters required by constant definition ") + atom1->mValueData);
                                        }
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            }
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
            }
        }
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // GpuProgramParamScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    GpuProgramParamScriptAnalyze::GpuProgramParamScriptAnalyze(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_GpuProgramParam, lid)
    {
    }
    //-------------------------------------------------------------------------
    void GpuProgramParamScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // Must have a name
        if(obj->mName.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_ObjIDError, _T("shared_params must be given a name"));
            return;
        }

        GpuParamValue * sharedParams = N_Only(GpuProgram).createShare(obj->mName);
        ScriptNodeList::const_iterator i, iend= obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = ScriptProperty::getProperty(prop->mPropertyName);

                if(tempid == N_PropertyGpuProgramParam_ParamName)
                {
                        if(prop->getCount() < 2)
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("shared_param_named - expected 2 or more arguments"));
                            continue;
                        }

                        String pName;
                        if(!getValue(prop->mValueDataList, 0, pName))
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("invalid parameter type"));
                            continue;
                        }
                        GpuDataType dtype = GDT_Unknow;
                        NCount arraySz = 1;
                        if(!getConstantType(getNode(prop->mValueDataList, 1), dtype))
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("invalid parameter type"));
                            continue;
                        }

                        bool _isFloat = isFloat(dtype);
                        bool _isDouble = isDouble(dtype);
                        FloatArray mFloats;
                        IntArray mInts;
                        DoubleArray mDoubles;
                        ScriptNodeList::const_iterator otherValsi = prop->mValueDataList.begin();
                        std::advance(otherValsi, 2);

                        for(; otherValsi != prop->mValueDataList.end(); ++otherValsi)
                        {
                            if((*otherValsi)->mType != SNT_Value)
                                continue;

                            ValueScriptNode * atom = (ValueScriptNode *)(*otherValsi);

                            if(atom->mValueData.at(0) == '[' && atom->mValueData.at(atom->mValueData.size() - 1) == ']')
                            {
                                String arrayStr = atom->mValueData.substr(1, atom->mValueData.size() - 2);
                                if(!StrConv::isNumber(arrayStr))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        _T("invalid array size"));
                                    continue;
                                }
                                StrConv::conv(arrayStr, arraySz);
                            }
                            else
                            {
                                if(!StrConv::isNumber(atom->mValueData))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError,
                                        atom->mValueData + _T(" invalid - extra parameters to shared_param_named must be numbers"));
                                    continue;
                                }
                                if (_isFloat)
                                {
                                    NIIf temp;
                                    StrConv::conv(atom->mValueData, temp);
                                    mFloats.push_back((NIIf)temp);
                                }
                                else if(_isDouble)
                                {
                                    NIId temp;
                                    StrConv::conv(atom->mValueData, temp);
                                    mDoubles.push_back(temp);
                                }
                                else
                                {
                                    NIIi temp;
                                    StrConv::conv(atom->mValueData, temp);
                                    mInts.push_back(temp);
                                }
                            }

                        } // each extra param

                        // define constant entry
                        try
                        {
                            VString tempstr;
                            StrConv::conv(pName, tempstr);
                            sharedParams->add(tempstr, dtype, arraySz);
                        }
                        catch(Exception & e)
                        {
                            mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                                e.getVerbose());
                            continue;
                        }

                        // initial mValueDataList
                        NCount elemsExpected = GpuProgramParam::getTypeCount(dtype, false) * arraySz;
                        NCount elemsFound = _isDouble ? mDoubles.size() : (_isFloat ? mFloats.size() : mInts.size());
                        if(elemsFound)
                        {
                            if(elemsExpected != elemsFound)
                            {
                                mCompiler->error(*prop,ScriptError::ET_InvalidParamError,
                                    _T("Wrong number of mValueDataList supplied for parameter type"));
                                continue;
                            }

                            if (_isFloat)
                            {
                                VString tempstr;
                                StrConv::conv(pName, tempstr);
                                sharedParams->set(tempstr, 0, &mFloats[0], elemsFound);
                            }
                            else if(_isDouble)
                            {
                                VString tempstr;
                                StrConv::conv(pName, tempstr);
                                sharedParams->set(tempstr, 0, &mDoubles[0], elemsFound);
                            }
                            else
                            {
                                VString tempstr;
                                StrConv::conv(pName, tempstr);
                                sharedParams->set(tempstr, 0, &mInts[0], elemsFound);
                            }
                        }
                }
            }
        }
    }
    //---------------------------------------------------------------------
    ScriptUnitID GpuProgramParamScriptAnalyze::getUnit()
    {
        return N_Script_Unit_GpuProgramParam;
    }
    //---------------------------------------------------------------------
    GroupID GpuProgramParamScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //---------------------------------------------------------------------
    bool GpuProgramParamScriptAnalyze::getConstantType(const ScriptNode * node, GpuDataType & op)
    {
        if(node->mType != SNT_Value)
            return false;

        String val;
        getValue(node, val);
        if(val.find(_T("float")) != String::npos)
        {
            NIIi count;
            if(val.size() == 6)
                StrConv::conv(val.substr(5), count, 1);
            else if (val.size() > 6)
                return false;

            if(count > 4 || count == 0)
                return false;

            op = (GpuDataType)(GDT_Float + count - 1);
        }
        else if(val.find(_T("int")) != String::npos)
        {
            NIIi count;
            if(val.size() == 4)
                StrConv::conv(val.substr(3), count, 1);
            else if (val.size() > 4)
                return false;

            if(count > 4 || count == 0)
                return false;

            op = (GpuDataType)(GDT_Int + count - 1);
        }
        else if(val.find(_T("matrix")) != String::npos)
        {
            NIIi count1, count2;

            if(val.size() == 9)
            {
                StrConv::conv(val.substr(6, 1), count1);
                StrConv::conv(val.substr(8, 1), count2);
            }
            else
                return false;

            if(count1 > 4 || count1 < 2 || count2 > 4 || count2 < 2)
                return false;

            switch(count1)
            {
            case 2:
                op = (GpuDataType)(GDT_Matrix2X2 + count2 - 2);
                break;
            case 3:
                op = (GpuDataType)(GDT_Matrix3X2 + count2 - 2);
                break;
            case 4:
                op = (GpuDataType)(GDT_Matrix4X2 + count2 - 2);
                break;
            }
        }
        return true;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // ParticleSpaceScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    ParticleSpaceScriptAnalyze::ParticleSpaceScriptAnalyze(ScriptCompiler * cpl, LangID lid) :
        ScriptAnalyze(cpl, N_Script_Unit_ParticleSpace, lid),
        mSystem(0)
    {
    }
    //-------------------------------------------------------------------------
    void ParticleSpaceScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        if(obj->mName.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_ObjIDError);
            return;
        }

        // Allocate the particle system
        Nui32 tempid;
        StrConv::conv(obj->mName, tempid);
        mSystem = N_Only(ParticleSpace).createTemplate(tempid, mCompiler->getResourceGroup());

        mSystem->setSrc(obj->mFile);
        mSystem->removeAllSpread();
        mSystem->removeAllEffect();

        obj->mData = mSystem;

        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid2 = getProperty(prop->mPropertyName);
                if(tempid2 == N_PropertyParticleSpace_Material)
                {
                    String name;
                    if (getValue(prop->mValueDataList, 0, name))
                    {
                        if (!mSystem->set(N_PropertyParticleSpace_Material, name))
                        {
                            if (mSystem->getGeo())
                            {
                                if (!mSystem->getGeo()->set(N_PropertyParticleSpace_Material, name))
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                        _T("material property could not be set with material \"") + name + _T("\""));
                            }
                        }
                    }
                }
                else
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else
                    {
                        PropertyID pid = getProperty(prop->mPropertyName);

                        String value;
                        // Glob the value together
                        ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                        for(it = prop->mValueDataList.begin(); it != itend; ++it)
                        {
                            if((*it)->mType == SNT_Value)
                            {
                                if(it == prop->mValueDataList.begin())
                                    value = ((ValueScriptNode*)(*it))->mValueData;
                                else
                                    value = value + _T(" ") + ((ValueScriptNode*)(*it))->mValueData;
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                                return;
                            }
                        }

                        if(!mSystem->set(pid, value))
                        {
                            if(mSystem->getGeo())
                            {
                                if(!mSystem->getGeo()->set(pid, value))
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            }
                        }
                    }
                }
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    ScriptUnitID ParticleSpaceScriptAnalyze::getUnit()
    {
        return N_Script_Unit_ParticleSpace;
    }
    //-------------------------------------------------------------------------
    GroupID ParticleSpaceScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // FontScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    FontScriptAnalyze::FontScriptAnalyze(ScriptCompiler * cpl, LangID lid) :
        ScriptAnalyze(cpl, N_Script_Unit_Font, lid),
        mFont(0)
    {
    }
    //-------------------------------------------------------------------------
    void FontScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        if(obj->mName.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_ObjIDError);
            return;
        }

        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_StringError);
            return;
        }
        String fonttype;
        if(!getValue(obj->mParamValue.front(), fonttype))
        {
            mCompiler->error(*obj, ScriptError::ET_InvalidParamError);
            return;
        }
        Nui32 fid, pdi = 300;
        NIIf size, lspace = 1.0f;
        String fsrc;
        bool antialias = true;
        ScaleMode smode = SM_Unknow;
        NIIf nativeRes[2];
        nativeRes[0] = 640.0f;
        nativeRes[1] = 480.0f;
        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid = getProperty(prop->mPropertyName);

                if (tempid == N_PropertyFont_ID)
                {
                    if (!getValue(prop->mValueDataList, 0, fid))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_Src)
                {
                    if (!getValue(prop->mValueDataList, 0, fsrc))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_Size)
                {
                    if (!getValue(prop->mValueDataList, 0, size))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_AutoRes)
                {
                    String smodet;
                    if (!getValue(prop->mValueDataList, 0, smodet))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    StrConv::conv(smodet, smode);
                }
                else if (tempid == N_PropertyFont_NativeRes)
                {
                    if (!getValue(prop->mValueDataList, 0, 2, nativeRes))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_LineSpace)
                {
                    if (!getValue(prop->mValueDataList, 0, antialias))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_AntiAlias)
                {
                    if (!getValue(prop->mValueDataList, 0, antialias))
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                }
                else if (tempid == N_PropertyFont_CharDefine)
                {
                    if (!mFont)
                    {
                        if (fonttype == _T("pixmap"))
                        {
                            mFont = N_Only(Font).createPixmap(fid, fsrc, GroupUnknow, smode,
                                PlaneSizef(nativeRes[0], nativeRes[1]));
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_StringError);
                            mFont = 0;
                            return;
                        }
                    }

                    if (prop->getCount() < 9)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }

                    // Glob the value together
                    String value;
                    ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                    for (it = prop->mValueDataList.begin(); it != itend; ++it)
                    {
                        if ((*it)->mType == SNT_Value)
                        {
                            if (it == prop->mValueDataList.begin())
                                value = ((ValueScriptNode*)(*it))->mValueData;
                            else
                                value = value + _T(" ") + ((ValueScriptNode*)(*it))->mValueData;
                        }
                    }

                    if (!mFont->set(N_PropertyFont_CharDefine, value))
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                    }
                }
                else
                    break;
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
        if(!mFont)
        {
            if(fonttype == _T("freetype"))
            {
                mFont = N_Only(Font).createFreeType(fid, size, pdi, antialias, fsrc,
                    GroupUnknow, smode, PlaneSizef(nativeRes[0], nativeRes[1]));
            }
            else if(fonttype == _T("pixmap"))
            {
                mFont = N_Only(Font).createPixmap(fid, fsrc, GroupUnknow, smode,
                    PlaneSizef(nativeRes[0], nativeRes[1]));
            }
            else
            {
                mFont = 0;
            }
        }
    }
    //-------------------------------------------------------------------------
    ScriptUnitID FontScriptAnalyze::getUnit()
    {
        return N_Script_Unit_Font;
    }
    //-------------------------------------------------------------------------
    GroupID FontScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // SpreadParticleScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    SpreadParticleScriptAnalyze::SpreadParticleScriptAnalyze(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_SpreadParticle, lid),
        mEmitter(0){}
    //-------------------------------------------------------------------------
    void SpreadParticleScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // Must have a type as the first value
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_StringError);
            return;
        }

        Nui32 type;
        if(!getValue(obj->mParamValue.front(), type))
        {
            mCompiler->error(*obj,ScriptError::ET_InvalidParamError);
            return;
        }

        ParticleSpace * system = static_cast<ParticleSpace *>(obj->mParent->mData);
        mEmitter = system->addSpread(type);

        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);

                String value;
                // Glob the mValueDataList together
                ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                for(it = prop->mValueDataList.begin(); it != itend; ++it)
                {
                    if((*it)->mType == SNT_Value)
                    {
                        if(it == prop->mValueDataList.begin())
                            value = ((ValueScriptNode *)(*it))->mValueData;
                        else
                            value = value + _T(" ") + ((ValueScriptNode *)(*it))->mValueData;
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        break;
                    }
                }

                if(!mEmitter->set(getProperty(prop->mPropertyName), value))
                {
                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    ScriptUnitID SpreadParticleScriptAnalyze::getUnit()
    {
        return N_Script_Unit_SpreadParticle;
    }
    //-------------------------------------------------------------------------
    GroupID SpreadParticleScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // ParticleEffectScriptAnalyze
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    ParticleEffectScriptAnalyze::ParticleEffectScriptAnalyze(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_ParticleEffect, lid),
        mAffector(0)
    {
    }
    //-------------------------------------------------------------------------
    void ParticleEffectScriptAnalyze::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        // Must have a type as the first value
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_StringError);
            return;
        }

        Nui32 type;
        if(!getValue(obj->mParamValue.front(), type))
        {
            mCompiler->error(*obj, ScriptError::ET_InvalidParamError);
            return;
        }

        ParticleSpace * system = static_cast<ParticleSpace *>(obj->mParent->mData);
        mAffector = system->addEffect(type);

        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>((*i));

                String value;
                // Glob the mValueDataList together
                ScriptNodeList::iterator it, itend = prop->mValueDataList.end();
                for(it = prop->mValueDataList.begin(); it != itend; ++it)
                {
                    if((*it)->mType == SNT_Value)
                    {
                        if(it == prop->mValueDataList.begin())
                            value = ((ValueScriptNode *)(*it))->mValueData;
                        else
                            value = value + _T(" ") + ((ValueScriptNode *)(*it))->mValueData;
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        break;
                    }
                }

                if(!mAffector->set(getProperty(prop->mPropertyName), value))
                {
                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    ScriptUnitID ParticleEffectScriptAnalyze::getUnit()
    {
        return N_Script_Unit_ParticleEffect;
    }
    //-------------------------------------------------------------------------
    GroupID ParticleEffectScriptAnalyze::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    // CompositorTranslator
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    CompositorTranslator::CompositorTranslator(ScriptCompiler * compiler, LangID lid) :
        ScriptAnalyze(compiler, N_Script_Unit_FrameFusion, lid),
            mCompositor(0),
            mTechnique(0),
            mTarget(0),
            mPass(0)
    {
    }
    //-------------------------------------------------------------------------
    void CompositorTranslator::analyze(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        if(obj->mName.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_ObjIDError);
            return;
        }

        // Create the compositor
        Nui32 id;
        StrConv::conv(obj->mName, id);

        mCompositor = reinterpret_cast<Fusion *>(N_Only(Fusion).create(id,
            mCompiler->getResourceGroup()));

        mCompositor->destroyAll();
        mCompositor->setSrc(obj->mFile);
        obj->mData = mCompositor;

        ScriptNodeList::const_iterator i, iend = obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Object)
            {
                ScriptAnalyze::analyze(*i);
            }
            else
            {
                mCompiler->error(**i, ScriptError::ET_ScriptError, _T("token not recognized"));
            }
        }
    }
    //-------------------------------------------------------------------------
    bool CompositorTranslator::analyzeCom(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);
        ObjectScriptNode * parent = obj->mParent ? reinterpret_cast<ObjectScriptNode *>(obj->mParent) : 0;

        if(getProperty(obj->mClass) == ID_TECHNIQUE && parent && getProperty(parent->mClass) == ID_COMPOSITOR)
        {
            analyzeFrameShaderFusion(node);
            return true;
        }
        else if((getProperty(obj->mClass) == ID_TARGET || getProperty(obj->mClass) == ID_TARGET_OUTPUT) && parent && getProperty(parent->mClass) == ID_TECHNIQUE)
        {
            analyzeFrameShaderCh(node);
            return true;
        }
        else if(getProperty(obj->mClass) == ID_PASS && parent && (getProperty(parent->mClass) == ID_TARGET || getProperty(parent->mClass) == ID_TARGET_OUTPUT))
        {
            analyzeFrameShaderOp(node);
            return true;
        }

        return false;
    }
    //-------------------------------------------------------------------------
    ScriptUnitID CompositorTranslator::getUnit()
    {
        return N_Script_Unit_Compositor;
    }
    //-------------------------------------------------------------------------
    GroupID CompositorTranslator::getGroup()
    {
        return GroupUnknow;
    }
    //-------------------------------------------------------------------------
    void CompositorTranslator::analyzeFrameShaderFusion(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        Fusion * compositor = static_cast<Fusion *>(obj->mParent->mData);
        mTechnique = compositor->create();
        obj->mData = mTechnique;

        for(ScriptNodeList::const_iterator i = obj->mChildren.begin(); i != obj->mChildren.end(); ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
                PropertyID tempid3 = getProperty(prop->mPropertyName);

                if(tempid3 == ID_TEXTURE)
                {
                    NCount atomIndex = 1;

                    const ScriptNode * it = getNode(prop->mValueDataList, 0);

                    if(it->mType != SNT_Value)
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        return;
                    }
                    // Save the first atom, should be name
                    ValueScriptNode * atom0 = (ValueScriptNode *)it;

                    NCount width = 0, height = 0;
                    NIIf widthFactor = 1.0f, heightFactor = 1.0f;
                    bool widthSet = false, heightSet = false, formatSet = false;
                    bool pooled = false;
                    bool hwGammaWrite = false;
                    bool fsaa = true;
                    Nui32 depthBufferId = GroupInner;
                    FrameShader::SourceType scope = FrameShader::ST_Temp;
                    PixelFormatList formats;

                    while(atomIndex < prop->getCount())
                    {
                        it = getNode(prop->mValueDataList, atomIndex++);
                        if(it->mType != SNT_Value)
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            return;
                        }
                        ValueScriptNode * atom = (ValueScriptNode*)it;
                        PropertyID tempid3 = getProperty(atom->mValueData);
                        if (tempid3 == ID_TARGET_WIDTH)
                        {
                            width = 0;
                            widthSet = true;
                        }
                        else if (tempid3 == ID_TARGET_HEIGHT)
                        {
                            height = 0;
                            heightSet = true;
                        }
                        else if (tempid3 == ID_TARGET_WIDTH_SCALED || tempid3 == ID_TARGET_HEIGHT_SCALED)
                        {
                            bool *pSetFlag;
                            NCount *pSize;
                            NIIf *pFactor;

                            if(getProperty(atom->mValueData) == ID_TARGET_WIDTH_SCALED)
                            {
                                pSetFlag = &widthSet;
                                pSize = &width;
                                pFactor = &widthFactor;
                            }
                            else
                            {
                                pSetFlag = &heightSet;
                                pSize = &height;
                                pFactor = &heightFactor;
                            }
                            // advance to next to get scaling
                            if(!getValue(prop->mValueDataList, atomIndex++, *pFactor))
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                                return;
                            }
                            *pSize = 0;
                            *pSetFlag = true;
                        }
                        else if (tempid3 == ID_POOLED)
                            pooled = true;
                        else if (tempid3 == ID_SCOPE_LOCAL)
                            scope = FrameShader::ST_Temp;
                        else if (tempid3 == ID_SCOPE_CHAIN)
                            scope = FrameShader::ST_Fusion;
                        else if (tempid3 == ID_SCOPE_GLOBAL)
                            scope = FrameShader::ST_Pool;
                        else if (tempid3 == ID_GAMMA)
                            hwGammaWrite = true;
                        else if (tempid3 == ID_NO_FSAA)
                            fsaa = false;
                        else if (tempid3 == ID_DEPTH_POOL)
                        {
                            // advance to next to get the ID
                            if(!getValue(prop->mValueDataList, atomIndex++, depthBufferId))
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                                return;
                            }
                        }
                        else
                        {
                            if(StrConv::isNumber(atom->mValueData))
                            {
                                if(atomIndex == 2)
                                {
                                    StrConv::conv(atom->mValueData, width);
                                    widthSet = true;
                                }
                                else if(atomIndex == 3)
                                {
                                    StrConv::conv(atom->mValueData, height);
                                    heightSet = true;
                                }
                                else
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                                    return;
                                }
                            }
                            else
                            {
                                PixelFormat format = PixelUtil::getFormat(atom->mValueData, true);
                                if(format == PF_UNKNOWN)
                                {
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                                    return;
                                }
                                formats.push_back(format);
                                formatSet = true;
                            }
                        }
                    }
                    if(!widthSet || !heightSet || !formatSet)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }

                    // No errors, create
                    FrameShader::FrameDefine * def = mTechnique->createDefine(atom0->mValueData);
                    def->mWidth = width;
                    def->mHeight = height;
                    def->mWidthFactor = widthFactor;
                    def->mHeightFactor = heightFactor;
                    def->mFormatList = formats;
                    def->mFSAA = fsaa;
                    def->mHWGamma = hwGammaWrite;
                    def->mRasterGroup = depthBufferId;
                    def->mPool = pooled;
                    def->mSourceType = scope;
                }
                else if (tempid3 == ID_TEXTURE_REF)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if(prop->getCount() != 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("texture_ref only supports 3 argument"));
                    }
                    else
                    {
                        String texName, refTexName;
                        Nui32 refCompName;
                        if(!getValue(prop->mValueDataList, 0, texName))
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("texture_ref must have 3 string arguments"));

                        if(!getValue(prop->mValueDataList, 1, refCompName))
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("texture_ref must have 3 string arguments"));

                        if(!getValue(prop->mValueDataList, 2, refTexName))
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("texture_ref must have 3 string arguments"));

                        FrameShader::FrameDefine * refTexDef = mTechnique->createDefine(texName);

                        refTexDef->mFusionSource = refCompName;
                        refTexDef->mTextureSource = refTexName;
                    }
                }

                else if (tempid3 == ID_SCHEME)
                {
                    Nui32 scheme;
                    if(getValue(prop->mValueDataList, 0, scheme))
                        mTechnique->setSchemeID(scheme);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                            _T("scheme must have 1 string argument"));
                }

                else if (tempid3 == ID_COMPOSITOR_LOGI)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                    }
                    else if(prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError,
                            _T("compositor logic only supports 1 argument"));
                    }
                    else
                    {
                        String logicName;
                        if(getValue(prop->mValueDataList, 0, logicName))
                            mTechnique->setName(logicName);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError,
                                _T("compositor logic must have 1 string argument"));
                    }
                }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                    _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    void CompositorTranslator::analyzeFrameShaderCh(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        FrameShader * technique = static_cast<FrameShader *>(obj->mParent->mData);
        if(getProperty(obj->mClass) == ID_TARGET)
        {
            mTarget = technique->createCh();
            if(!obj->mName.empty())
            {
                mTarget->setOutputName(obj->mName);
            }
        }
        else if(getProperty(obj->mClass) == ID_TARGET_OUTPUT)
        {
            mTarget = technique->getOutputCh();
        }
        obj->mData = mTarget;

        ScriptNodeList::const_iterator i, iend = obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode*>(*i);
                PropertyID tempid2 = getProperty(prop->mPropertyName);

                if (tempid2 == ID_INPUT)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        if (prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = (ValueScriptNode*)prop->mValueDataList.front();
                            PropertyID tempid3 = getProperty(atom->mValueData);

                            if(tempid3 == NoneProperty)
                                mTarget->setUsePreResult(false);
                            if (tempid3 == ID_PREVIOUS)
                                mTarget->setUsePreResult(true);
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                if (tempid2 == ID_ONLY_INITIAL)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        bool val;
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            mTarget->setOnce(val);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                if (tempid2 == ID_VISIBILITY_MASK)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        Nui32 val;
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            mTarget->setVisibleMask((Nmark)val);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                if (tempid2 == ID_LOD_BIAS)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        NIIf val;
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            mTarget->setLodBias(val);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                if (tempid2 == ID_MATERIAL_SCHEME)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        Nui32 val;
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            mTarget->setMaterial((SchemeID)val);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                if (tempid2 == ID_SHADOWS_ENABLED)
                {
                    if (prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        bool val;
                        if (getValue(prop->mValueDataList.front(), val))
                        {
                            mTarget->setShadowEnable(val);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));

            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //------------------------------------------------------------------------
    void CompositorTranslator::analyzeFrameShaderOp(const ScriptNode * node)
    {
        const ObjectScriptNode * obj = reinterpret_cast<const ObjectScriptNode *>(node);

        FusionShader * target = static_cast<FusionShader *>(obj->mParent->mData);
        mPass = target->create();
        obj->mData = mPass;

        // The name is the type of the pass
        if(obj->mParamValue.empty())
        {
            mCompiler->error(*obj, ScriptError::ET_StringError);
            return;
        }
        String type;
        if(!getValue(obj->mParamValue.front(), type))
        {
            mCompiler->error(*obj, ScriptError::ET_InvalidParamError);
            return;
        }

        if(type == _T("clear"))
            mPass->setType(FusionShaderOp::OT_Clear);
        else if(type == _T("stencil"))
            mPass->setType(FusionShaderOp::OT_StencilTest);
        else if(type == _T("render_quad"))
            mPass->setType(FusionShaderOp::OT_RenderQuad);
        else if(type == _T("render_scene"))
            mPass->setType(FusionShaderOp::OT_RenderScene);
        else if(type == _T("render_custom"))
        {
            mPass->setType(FusionShaderOp::OT_RenderCustom);
            String customType;
            //This is the ugly one liner for safe access to the second parameter.
            if (obj->mParamValue.size() < 2 || !getValue(*(++(obj->mParamValue.begin())), customType))
            {
                mCompiler->error(*obj,ScriptError::ET_StringError);
                return;
            }
            mPass->setCustomType(customType);
        }
        else
        {
            mCompiler->error(*obj, ScriptError::ET_InvalidParamError,
                _T("pass types must be \"clear\", \"stencil\", \"render_quad\", \"render_scene\" or \"render_custom\"."));
            return;
        }

        ScriptNodeList::const_iterator i, iend = obj->mChildren.end();
        for(i = obj->mChildren.begin(); i != iend; ++i)
        {
            if((*i)->mType == SNT_Property)
            {
                PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode*>(*i);
                PropertyID tempid3 = getProperty(prop->mPropertyName);

                if (tempid3 == ID_CHECK)
                {
                    bool val;
                    if(getValue(prop->mValueDataList, 0, val))
                        mPass->setStencilCheck(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                }
                else if (tempid3 == ID_COMP_FUNC)
                    {
                        if(prop->isEmpty())
                        {
                            mCompiler->error(*prop, ScriptError::ET_StringError);
                            return;
                        }
                        CmpMode func;
                        MaterialScriptAnalyze * analy = N_new MaterialScriptAnalyze(mCompiler, mLangMap->getLangID());
                        if(analy->getValue(prop->mValueDataList.front(), func))
                            mPass->setStencilFunc(func);
                        else
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        N_delete analy;
                    }
            
                else if (tempid3 == ID_REF_VALUE)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                        return;
                    }
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencilValue(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                else if (tempid3 == ID_MASK)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                        return;
                    }
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencilMask(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
              
                else if (tempid3 == ID_FAIL_OP)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop,ScriptError::ET_StringError);
                        return;
                    }
                    StencilOpType val;
                    if(getStencilOp(prop->mValueDataList.front(), &val))
                        mPass->setStencilFailOp(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                else if (tempid3 == ID_DEPTH_FAIL_OP)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop,ScriptError::ET_StringError);
                        return;
                    }
                    StencilOpType val;
                    if(getStencilOp(prop->mValueDataList.front(), &val))
                        mPass->setStencilDepthFailOp(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                  
                else if (tempid3 == ID_PASS_OP)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    StencilOpType val;
                    if(getStencilOp(prop->mValueDataList.front(), &val))
                        mPass->setStencilPassOp(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                  
                else if (tempid3 == ID_TWO_SIDED)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    bool val;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setStencilTwoSided(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                   
                else if (tempid3 == ID_BUFFERS)
                    {
                        Nui32 buffers = 0;
                        ScriptNodeList::iterator k, kend = prop->mValueDataList.end();
                        for(k = prop->mValueDataList.begin(); k != kend; ++k)
                        {
                            if((*k)->mType == SNT_Value)
                            {
                                PropertyID tempid3 = getProperty(((ValueScriptNode*)(*k))->mValueData);

                                if(tempid3 == ID_COLOUR)
                                    buffers |= FBT_COLOUR;
                                else if (tempid3 == ID_DEPTH)
                                    buffers |= FBT_DEPTH;
                                else if (tempid3 == ID_STENCIL)
                                    buffers |= FBT_STENCIL;
                                else
                                    mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            }
                            else
                            {
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                            }
                        }
                        mPass->setClearBuffers(buffers);
                    }
               
                else if (tempid3 == ID_COLOUR_VALUE)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                        return;
                    }
                    Colour val;
                    if(getValue(prop->mValueDataList, 0, val))
                        mPass->setClearColour(val);
                    else
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError);
                }
                
                else if (tempid3 == ID_DEPTH_VALUE)
                {
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_NumberError);
                        return;
                    }
                    NIIf val;
                    if(getValue(prop->mValueDataList.front(), val))
                        mPass->setClearDepth(val);
                    else
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                }
                
                else if (tempid3 == ID_STENCIL_VALUE)
                {
                        Nui32 val;
                        if(getValue(prop->mValueDataList, 0, val))
                            mPass->setClearStencil(val);
                        else
                            mCompiler->error(*prop,ScriptError::ET_InvalidParamError);
                }
            
                else if (tempid3 == MaterialProperty)
                {
                    Nui32 rid;
                    if(getValue(prop->mValueDataList, 0, rid))
                    {
                        mPass->setMaterial((ResourceID)rid);
                    }
                    else
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError);
                    }
                }
              
                else if (tempid3 == ID_INPUT)
                    if(prop->getCount() < 2)
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 3)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        Nui32 id;
                        String name;
                        if(getValue(prop->mValueDataList, 0, id) && getValue(prop->mValueDataList, 1, name))
                        {
                            Nui32 index = 0;
                            if(getNode(prop->mValueDataList, 2) != 0)
                            {
                                if(!getValue(prop->mValueDataList, 2, index))
                                {
                                    mCompiler->error(*prop, ScriptError::ET_NumberError);
                                    return;
                                }
                            }

                            mPass->setInput(id, name, index);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
               
                else if (tempid3 == ID_IDENTIFIER)
                {
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        mPass->setID(val);
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                    }
                }
           
                else if (tempid3 == ID_FIRST_RENDER_QUEUE)
                {
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        mPass->setFirstRenderQueue((Nui8)val);
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                    }
                }
            
                else if (tempid3 == ID_LAST_RENDER_QUEUE)
                {
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        mPass->setLastRenderQueue((Nui8)val);
                    }
                    else
                    {
                        mCompiler->error(*prop,ScriptError::ET_InvalidParamError);
                    }
                }
            
                else if (tempid3 == ID_MATERIAL_SCHEME)
                {
                    Nui32 val;
                    if(getValue(prop->mValueDataList.front(), val))
                    {
                        mPass->setMaterial(val);
                    }
                    else
                    {
                        mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                    }
                }
               
                else if (tempid3 == ID_QUAD_NORMALS)
                    if(prop->isEmpty())
                    {
                        mCompiler->error(*prop, ScriptError::ET_StringError);
                        return;
                    }
                    else if (prop->getCount() > 1)
                    {
                        mCompiler->error(*prop, ScriptError::ET_ParamCountError);
                        return;
                    }
                    else
                    {
                        if(prop->mValueDataList.front()->mType == SNT_Value)
                        {
                            ValueScriptNode * atom = reinterpret_cast<ValueScriptNode *>(prop->mValueDataList.front());
                            if(getProperty(atom->mValueData) == ID_CAMERA_FAR_CORNERS_VIEW_SPACE)
                                mPass->setQuadFarCorners(true, true);
                            else if(getProperty(atom->mValueData) == ID_CAMERA_FAR_CORNERS_WORLD_SPACE)
                                mPass->setQuadFarCorners(true, false);
                            else
                                mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                        else
                        {
                            mCompiler->error(*prop, ScriptError::ET_InvalidParamError);
                        }
                    }
                else
                    mCompiler->error(*prop, ScriptError::ET_ScriptError,
                        _T("token \"") + prop->mPropertyName + _T("\" is not recognized"));
            }
            else
            {
                ScriptAnalyze::analyze(*i);
            }
        }
    }
    //-------------------------------------------------------------------------
    bool CompositorTranslator::getStencilOp(const ScriptNode * node, StencilOpType * op)
    {
        if(node->mType != SNT_Value)
            return false;
        const ValueScriptNode * atom = static_cast<const ValueScriptNode *>(node);
        PropertyID tempid = getProperty(atom->mValueData);
        if(tempid == ID_KEEP)
            *op = SOT_KEEP;
        else if (tempid == ColourFactorZeroProperty)
            *op = SOT_ZERO;
        else if (tempid == ReplaceMathProperty)
            *op = SOT_REPLACE;
        else if (tempid == ID_INCREMENT)
            *op = SOT_INCR;
        else if (tempid == ID_DECREMENT)
            *op = SOT_DECR;
        else if (tempid == ID_INCREMENT_WRAP)
            *op = SOT_INCR_WRAP;
        else if (tempid == ID_DECREMENT_WRAP)
            *op = SOT_DECR_WRAP;
        else if (tempid == ID_INVERT)
            *op = SOT_INVERT;
        else
            return false;

        return true;
    }
    //------------------------------------------------------------------------
}