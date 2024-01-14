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
#include "NiiMaterialSerializer.h"
#include "NiiMaterialManager.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiGpuProgramManager.h"
#include "NiiTextureManager.h"
#include "NiiLodSchemeManager.h"
#include "NiiMaterialLodScheme.h"
#include "NiiPredefinedControllers.h"

namespace NII
{
    void logParseError(const String & error, const MaterialScriptContext & context)
    {
        // log material name only if filename not specified
        if(context.filename.empty() && context.material)
        {
            N_Only(Log).log(_I18n("Error in material ") +
                N_conv(context.material->getOriginID()) + _T(" : ") + error);
        }
        else
        {
            if (context.material)
            {
                N_Only(Log).log(_I18n("Error in material ") + 
                    N_conv(context.material->getOriginID()) +
                    _I18n(" at line ") + N_conv(context.lineNo) + _I18n(" of ") +
                    context.filename + _T(": ") + error);
            }
            else
            {
                N_Only(Log).log(_I18n("Error at line ") + 
                    N_conv(context.lineNo) + _I18n(" of ") + 
                    context.filename + _T(": ") + error);
            }
        }
    }
    //-----------------------------------------------------------------------
    Colour _parseColourValue(StringList & vecparams)
    {
        Colour temp;
        StrConv::conv(vecparams[0], temp.r);
        StrConv::conv(vecparams[1], temp.g);
        StrConv::conv(vecparams[2], temp.b);

        if(vecparams.size() == 4)
            StrConv::conv(vecparams[3], temp.a);
        else
            temp.a = 1.0f;

        return temp ;
    }
    //-----------------------------------------------------------------------
    TextureFilterOP convertFiltering(const String & s)
    {
        if (s == _T("none"))
        {
            return TFOP_NONE;
        }
        else if (s == _T("point"))
        {
            return TFOP_POINT;
        }
        else if (s == _T("linear"))
        {
            return TFOP_LINEAR;
        }
        else if (s == _T("anisotropic"))
        {
            return TFOP_ANISOTROPIC;
        }

        return TFOP_POINT;
    }
    //-----------------------------------------------------------------------
    bool parseAmbient(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 1, 3 or 4 parameters
        if (vecparams.size() == 1) 
        {
            if(vecparams[0] == _T("vertexcolour"))
            {
               context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() | CTM_T_AMBIENT);
            }
            else
            {
                logParseError(_I18n("Bad ambient attribute, single parameter flag must be 'vertexcolour'"), context);
            }
        }
        else if (vecparams.size() == 3 || vecparams.size() == 4)
        {
            context.pass->setColour(ShaderCh::SOT_Set)->setAmbient(_parseColourValue(vecparams));
            context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() & ~CTM_T_AMBIENT);
        }
        else
        {
            logParseError(_I18n("Bad ambient attribute, wrong number of parameters (expected 1, 3 or 4)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseDiffuse(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 1, 3 or 4 parameters
        if (vecparams.size() == 1)
        {
            if(vecparams[0] == _T("vertexcolour"))
            {
               context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() | CTM_T_DIFFUSE);
            }
            else
            {
                logParseError(_I18n("Bad diffuse attribute, single parameter flag must be 'vertexcolour'"), context);
            }
        }
        else if (vecparams.size() == 3 || vecparams.size() == 4)
        {
            context.pass->setColour(ShaderCh::SOT_Set)->setDiffuse(_parseColourValue(vecparams));
            context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() & ~CTM_T_DIFFUSE);
        }
        else
        {
            logParseError(_I18n("Bad diffuse attribute, wrong number of parameters (expected 1, 3 or 4)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseSpecular(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 2, 4 or 5 parameters
        if(vecparams.size() == 2)
        {
            if(vecparams[0] == _T("vertexcolour"))
            {
                NIIf temp;
                StrConv::conv(vecparams[1], temp);
                context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() | CTM_T_SPECULAR);
                context.pass->setColour(ShaderCh::SOT_Set)->setShininess(temp);
            }
            else
            {
                logParseError(_I18n("Bad specular attribute, NIId parameter statement must be 'vertexcolour <shininess>'"),
                    context);
            }
        }
        else if(vecparams.size() == 4 || vecparams.size() == 5)
        {
            NIIf t1, t2, t3, t4, t5;
            StrConv::conv(vecparams[0], t1);
            StrConv::conv(vecparams[1], t2);
            StrConv::conv(vecparams[2], t3);
            if(vecparams.size() == 5)
                StrConv::conv(vecparams[3], t4);
            else
                t4 = 1.0f;
            context.pass->setColour(ShaderCh::SOT_Set)->setSpecular(Colour(t1, t2, t3, t4));
            context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() & ~CTM_T_SPECULAR);
            StrConv::conv(vecparams[vecparams.size() - 1], t5);
            context.pass->setColour(ShaderCh::SOT_Set)->setShininess(t5);
        }
        else
        {
            logParseError(_I18n("Bad specular attribute, wrong number of parameters (expected 2, 4 or 5)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseEmissive(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 1, 3 or 4 parameters
        if (vecparams.size() == 1)
        {
            if(vecparams[0] == _T("vertexcolour"))
            {
               context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() | CTM_T_EMISSIVE);
            } else
            {
                logParseError(_I18n("Bad emissive attribute, single parameter flag must be 'vertexcolour'"), context);
            }
        }
        else if (vecparams.size() == 3 || vecparams.size() == 4)
        {
            context.pass->setColour(ShaderCh::SOT_Set)->setEmissive(_parseColourValue(vecparams));
            context.pass->setColour(ShaderCh::SOT_Set)->setColourTracking(context.pass->getColour().getColourTracking() & ~CTM_T_EMISSIVE);
        }
        else
        {
            logParseError(_I18n("Bad emissive attribute, wrong number of parameters (expected 1, 3 or 4)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    ColourFactor convertBlendFactor(const String & param)
    {
        if (param == _T("one"))
            return CF_ONE;
        else if (param == _T("zero"))
            return CF_ZERO;
        else if (param == _T("dest_colour"))
            return CF_DST;
        else if (param == _T("src_colour"))
            return CF_SRC;
        else if (param == _T("one_minus_dest_colour"))
            return CF_INV_DST;
        else if (param == _T("one_minus_src_colour"))
            return CF_INV_SRC;
        else if (param == _T("dest_alpha"))
            return CF_DST_ALPHA;
        else if (param == _T("src_alpha"))
            return CF_SRC_ALPHA;
        else if (param == _T("one_minus_dest_alpha"))
            return CF_INV_DST_ALPHA;
        else if (param == _T("one_minus_src_alpha"))
            return CF_INV_SRC_ALPHA;
        else
        {
            N_EXCEPT(InvalidParam, _I18n("Invalid blend factor."));
        }
    }
    //-----------------------------------------------------------------------
    bool parseSceneBlend(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Should be 1 or 2 params
        if (vecparams.size() == 1)
        {
            //simple
            FrameBlendMode stype;
            if(vecparams[0] == _T("add"))
                stype = FBM_ADD;
            else if(vecparams[0] == _T("modulate"))
                stype = FBM_MODULATE;
            else if(vecparams[0] == _T("colour_blend"))
                stype = FBM_INV_COLOUR;
            else if(vecparams[0] == _T("alpha_blend"))
                stype = FBM_INV_ALPHA;
            else
            {
                logParseError(_I18n("Bad scene_blend attribute, unrecognised parameter '") + vecparams[0] + _T("'"), context);
                return false;
            }
            context.pass->setBlend(ShaderCh::SOT_Set)->setBlend(stype);

        }
        else if(vecparams.size() == 2)
        {
            //src/dest
            ColourFactor src, dest;

            try
            {
                src = convertBlendFactor(vecparams[0]);
                dest = convertBlendFactor(vecparams[1]);
                context.pass->setBlend(ShaderCh::SOT_Set)->setBlend(src, dest);
            }
            catch (Exception & e)
            {
                logParseError(_I18n("Bad scene_blend attribute, ") + e.getVerbose(), context);
            }

        }
        else
        {
            logParseError(_I18n("Bad scene_blend attribute, wrong number of parameters (expected 1 or 2)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseSeparateSceneBlend(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Should be 2 or 4 params
        if (vecparams.size() == 2)
        {
            //simple
            FrameBlendMode stype;
            if (vecparams[0] == _T("add"))
                stype = FBM_ADD;
            else if (vecparams[0] == _T("modulate"))
                stype = FBM_MODULATE;
            else if (vecparams[0] == _T("colour_blend"))
                stype = FBM_INV_COLOUR;
            else if (vecparams[0] == _T("alpha_blend"))
                stype = FBM_INV_ALPHA;
            else
            {
                logParseError(_I18n("Bad separate_scene_blend attribute, unrecognised parameter '") + 
                    vecparams[0] + _T("'"), context);
                return false;
            }

            FrameBlendMode stypea;
            if (vecparams[0] == _T("add"))
                stypea = FBM_ADD;
            else if (vecparams[0] == _T("modulate"))
                stypea = FBM_MODULATE;
            else if (vecparams[0] == _T("colour_blend"))
                stypea = FBM_INV_COLOUR;
            else if (vecparams[0] == _T("alpha_blend"))
                stypea = FBM_INV_ALPHA;
            else
            {
                logParseError(_I18n("Bad separate_scene_blend attribute, unrecognised parameter '") + vecparams[1] + _T("'"), context);
                return false;
            }

            context.pass->setBlend(ShaderCh::SOT_Set)->setBlend(stype, stypea);
        }
        else if (vecparams.size() == 4)
        {
            //src/dest
            ColourFactor src, dest;
            ColourFactor srca, desta;

            try 
            {
                src = convertBlendFactor(vecparams[0]);
                dest = convertBlendFactor(vecparams[1]);
                srca = convertBlendFactor(vecparams[2]);
                desta = convertBlendFactor(vecparams[3]);
                context.pass->setBlend(ShaderCh::SOT_Set)->setBlend(src, dest, srca, desta);
            }
            catch (Exception & e)
            {
                logParseError(_I18n("Bad separate_scene_blend attribute, ") + e.getVerbose(), context);
            }

        }
        else
        {
            logParseError(_I18n("Bad separate_scene_blend attribute, wrong number of parameters (expected 2 or 4)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    CmpMode convertCompareFunction(const String & param)
    {
        if (param == _T("always_fail"))
            return CPM_ALWAYS_FAIL;
        else if (param == _T("always_pass"))
            return CPM_ALWAYS_PASS;
        else if (param == _T("less"))
            return CPM_LESS;
        else if (param == _T("less_equal"))
            return CPM_LESS_EQUAL;
        else if (param == _T("equal"))
            return CPM_EQUAL;
        else if (param == _T("not_equal"))
            return CPM_NOT_EQUAL;
        else if (param == _T("greater_equal"))
            return CPM_GREATER_EQUAL;
        else if (param == _T("greater"))
            return CPM_GREATER;
        else
            N_EXCEPT(InvalidParam, _I18n("Invalid compare function"));

    }
    //-----------------------------------------------------------------------
    bool parseDepthCheck(String& params, MaterialScriptContext& context)
    {
        StrUtil::toLower(params);
        if(params == _T("on"))
            context.pass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(true);
        else if(params == _T("off"))
            context.pass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);
        else
            logParseError(_I18n("Bad depth_check attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseDepthWrite(String& params, MaterialScriptContext& context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(true);
        else if (params == _T("off"))
            context.pass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
        else
            logParseError(_I18n("Bad depth_write attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLightScissor(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if(params == _T("on"))
            context.pass->setLightClip(true);
        else if(params == _T("off"))
            context.pass->setLightClip(false);
        else
            logParseError(_I18n("Bad light_scissor attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLightClip(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setLightClipPlane(true);
        else if (params == _T("off"))
            context.pass->setLightClipPlane(false);
        else
            logParseError(_I18n("Bad light_clip_planes attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseDepthFunc(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        try
        {
            CmpMode func = convertCompareFunction(params);
            context.pass->setStencil(ShaderCh::SOT_Set)->setDepthCompareFunc(func);
        }
        catch (...)
        {
            logParseError(_I18n("Bad depth_func attribute, invalid function parameter."), context);
        }

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseNormaliseNormals(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setUnitNormals(true);
        else if (params == _T("off"))
            context.pass->setUnitNormals(false);
        else
            logParseError(_I18n("Bad normalise_normals attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseColourWrite(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setColourWrite(ColourMark::CM_RGBA);
        else if (params == _T("off"))
            context.pass->setColourWrite(ColourMark::CM_ZERO);
        else
            logParseError(_I18n("Bad colour_write attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseCullHardware(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if(params== _T("none"))
            context.pass->setCullingMode(CM_None);
        else if(params== _T("anticlockwise"))
            context.pass->setCullingMode(CM_Anticlockwise);
        else if(params== _T("clockwise"))
            context.pass->setCullingMode(CM_Clockwise);
        else
            logParseError(_I18n("Bad cull_hardware attribute, valid parameters are 'none', 'clockwise' or 'anticlockwise'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseCullSoftware(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if(params== _T("none"))
            context.pass->setSysCullingMode(SCM_NONE);
        else if(params== _T("back"))
            context.pass->setSysCullingMode(SCM_BACK);
        else if(params== _T("front"))
            context.pass->setSysCullingMode(SCM_FRONT);
        else
            logParseError(_I18n("Bad cull_software attribute, valid parameters are 'none', 'front' or 'back'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLighting(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params== _T("on"))
            context.pass->setLight(true);
        else if (params== _T("off"))
            context.pass->setLight(false);
        else
            logParseError(_I18n("Bad lighting attribute, valid parameters are 'on' or 'off'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseMaxLights(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        context.pass->setLightMax((Nui16)temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseStartLight(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        context.pass->setLightBegin(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    void parseIterationLightTypes(String & params, MaterialScriptContext & context)
    {
        // Parse light type
        if(params == _T("directional"))
        {
            context.pass->setEachLight(true);
            context.pass->setLightTypeLimit(LT_DIRECTIONAL);
        }
        else if(params == _T("point"))
        {
            context.pass->setEachLight(true);
            context.pass->setLightTypeLimit(LT_POINT);
        }
        else if(params == _T("spot"))
        {
            context.pass->setEachLight(true);
            context.pass->setLightTypeLimit(LT_SPOT);
        }
        else
        {
            logParseError(_I18n("Bad iteration attribute, valid values for light type parameter are 'point' or 'directional' or 'spot'."), context);
        }
    }
    //-----------------------------------------------------------------------
    bool parseIteration(String& params, MaterialScriptContext& context)
    {
        // we could have more than one parameter
        /* combinations could be:
            iteration once
            iteration once_per_light [light type]
            iteration <number>
            iteration <number> [per_light] [light type]
            iteration <number> [per_n_lights] <num_lights> [light type]
        */
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() < 1 || vecparams.size() > 4)
        {
            logParseError(_I18n("Bad iteration attribute, expected 1 to 3 parameters."), context);
            return false;
        }

        if(vecparams[0]== _T("once"))
        {
            context.pass->setEachLight(false);
            context.pass->setLightTypeLimit(LT_NONE);
        }
        else if (vecparams[0]== _T("once_per_light"))
        {
            if (vecparams.size() == 2)
            {
                parseIterationLightTypes(vecparams[1], context);
            }
            else
            {
                context.pass->setEachLight(true);
                context.pass->setLightTypeLimit(LT_NONE);
            }
        }
        else // could be using form: <number> [per_light] [light type]
        {
            NIIi passIterationCount;
            StrConv::conv(vecparams[0], passIterationCount);
            if(passIterationCount > 0)
            {
                context.pass->setRenderCount(passIterationCount);
                if(vecparams.size() > 1)
                {
                    if(vecparams[1] == _T("per_light"))
                    {
                        if(vecparams.size() == 3)
                        {
                            parseIterationLightTypes(vecparams[2], context);
                        }
                        else
                        {
                            context.pass->setEachLight(true);
                            context.pass->setLightTypeLimit(LT_NONE);
                        }
                    }
                    else if (vecparams[1] == _T("per_n_lights"))
                    {
                        if (vecparams.size() < 3)
                        {
                            logParseError(_I18n("Bad iteration attribute, expected number of lights."), context);
                        }
                        else
                        {
                            NIIi temp;
                            StrConv::conv(vecparams[2], temp);
                            context.pass->setLightRenderLimit((Nui16)temp);
                            if (vecparams.size() == 4)
                            {
                                parseIterationLightTypes(vecparams[3], context);
                            }
                            else
                            {
                                context.pass->setEachLight(true);
                                context.pass->setLightTypeLimit(LT_NONE);
                            }
                        }
                    }
                    else
                        logParseError(_I18n("Bad iteration attribute, valid parameters are <number> [per_light|per_n_lights <num_lights>] [light type]."), context);
                }
            }
            else
                logParseError(_I18n("Bad iteration attribute, valid parameters are 'once' or 'once_per_light' or <number> [per_light|per_n_lights <num_lights>] [light type]."), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePointSize(String & params, MaterialScriptContext & context)
    {
        NIIf temp;
        StrConv::conv(params, temp);
        context.pass->setPoint(ShaderCh::SOT_Set)->setSize(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePointSprites(String & params, MaterialScriptContext & context)
    {
        if (params== _T("on"))
            context.pass->setPoint(ShaderCh::SOT_Set)->setSpritesEnable(true);
        else if (params== _T("off"))
            context.pass->setPoint(ShaderCh::SOT_Set)->setSpritesEnable(false);
        else
            logParseError(_I18n("Bad point_sprites attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePointAttenuation(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        ShaderChPoint * scp = context.pass->setPoint(ShaderCh::SOT_Set);
        if (vecparams.size() != 1 && vecparams.size() != 4)
        {
            logParseError(_I18n("Bad point_size_attenuation attribute, 1 or 4 parameters expected"), context);
            return false;
        }
        if (vecparams[0] == _T("off"))
        {
            scp->setAttenuateEnable(false);
        }
        else if (vecparams[0] == _T("on"))
        {
            if (vecparams.size() == 4)
            {
                NIIf t1, t2, t3;
                StrConv::conv(vecparams[1], t1);
                StrConv::conv(vecparams[2], t2);
                StrConv::conv(vecparams[3], t3);
                scp->setAttenuateEnable(true);
                scp->setConstant(t1);
                scp->setLinear(t2);
                scp->setQuadratic(t3);
            }
            else
            {
                scp->setAttenuateEnable(true);
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePointSizeMin(String & params, MaterialScriptContext & context)
    {
        NIIf temp;
        StrConv::conv(params, temp);
        context.pass->setPoint(ShaderCh::SOT_Set)->setMinSize(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePointSizeMax(String & params, MaterialScriptContext & context)
    {
        NIIf temp;
        StrConv::conv(params, temp);
        context.pass->setPoint(ShaderCh::SOT_Set)->setMaxSize(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseFogging(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams[0] == _T("true"))
        {
            // if true, we need to see if they supplied all arguments, or just the 1... if just the one,
            // Assume they want to disable the default fog from effecting this material.
            if (vecparams.size() == 8)
            {
                FogMode mFogtype;
                if (vecparams[1] == _T("none"))
                    mFogtype = FM_NONE;
                else if (vecparams[1] == _T("linear"))
                    mFogtype = FM_LINEAR;
                else if (vecparams[1] == _T("exp"))
                    mFogtype = FM_EXP;
                else if (vecparams[1] == _T("exp2"))
                    mFogtype = FM_EXP2;
                else
                {
                    logParseError(_I18n("Bad fogging attribute, valid parameters are 'none', 'linear', 'exp', or 'exp2'."), context);
                    return false;
                }
                NIIf t1, t2, t3, t4, t5, t6;
                StrConv::conv(vecparams[2], t1);
                StrConv::conv(vecparams[3], t2);
                StrConv::conv(vecparams[4], t3);
                StrConv::conv(vecparams[5], t4);
                StrConv::conv(vecparams[6], t5);
                StrConv::conv(vecparams[7], t6);
                ShaderChFog tempfog(mFogtype, Colour(t1, t2, t3), t4, t5, t6);
                context.pass->setFog(ShaderCh::SOT_Set, &tempfog);
            }
            else
            {
                ShaderChFog tempfog;
                context.pass->setFog(ShaderCh::SOT_Set, &tempfog);
            }
        }
        else if (vecparams[0] == _T("false"))
        {
            context.pass->setFog(ShaderCh::SOT_Set, &ShaderChFog::None);
        }
        else
            logParseError(_I18n("Bad fog_override attribute, valid parameters are 'true' or 'false'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseShading(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params== _T("flat"))
            context.pass->setShadeMode(SM_FLAT);
        else if (params== _T("gouraud"))
            context.pass->setShadeMode(SM_GOURAUD);
        else if (params== _T("phong"))
            context.pass->setShadeMode(SM_PHONG);
        else
            logParseError(
                _I18n("Bad shading attribute, valid parameters are 'flat', 'gouraud' or 'phong'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePolygonMode(String& params, MaterialScriptContext& context)
    {
        StrUtil::toLower(params);
        if (params== _T("solid"))
            context.pass->setPolygonMode(PM_SOLID);
        else if (params== _T("wireframe"))
            context.pass->setPolygonMode(PM_LINE);
        else if (params== _T("points"))
            context.pass->setPolygonMode(PM_POINTS);
        else
            logParseError(_I18n("Bad polygon_mode attribute, valid parameters are 'solid', 'wireframe' or 'points'."), context);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parsePolygonModeOverrideable(String & params, MaterialScriptContext & context)
    {
        bool temp;
        StrUtil::toLower(params);
        StrConv::conv(params, temp);
        context.pass->setAutoReduce(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseFiltering(String & params, MaterialScriptContext& context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 1 or 3 parameters
        if(vecparams.size() == 1)
        {
            // Simple format
            if(vecparams[0]== _T("none"))
                context.textureUnit->getSample()->setFiltering(TFM_NONE);
            else if (vecparams[0]== _T("bilinear"))
                context.textureUnit->getSample()->setFiltering(TFM_BILINEAR);
            else if (vecparams[0]== _T("trilinear"))
                context.textureUnit->getSample()->setFiltering(TFM_TRILINEAR);
            else if (vecparams[0]== _T("anisotropic"))
                context.textureUnit->getSample()->setFiltering(TFM_ANISOTROPIC);
            else
            {
                logParseError(_I18n("Bad filtering attribute, valid parameters for simple format are 'none', 'bilinear', 'trilinear' or 'anisotropic'."), context);
                return false;
            }
        }
        else if(vecparams.size() == 3)
        {
            // Complex format
            context.textureUnit->getSample()->setFiltering(convertFiltering(vecparams[0]), convertFiltering(vecparams[1]), convertFiltering(vecparams[2]));
        }
        else
        {
            logParseError(_I18n("Bad filtering attribute, wrong number of parameters (expected 1 or 3)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    // Texture layer attributes
    bool parseTexture(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        const NCount numParams = vecparams.size();
        if (numParams > 5)
        {
            logParseError(_I18n("Invalid texture attribute - expected only up to 5 parameters."), context);
        }
        Texture::Type tt = Texture::T_2D;
        NIIi mipmaps = -1; // When passed to TextureManager::load, this means default to default number of mipmaps
        bool isAlpha = false;
        bool hwGamma = false;
        PixelFormat desiredFormat = PF_UNKNOWN;
        for (NCount p = 1; p < numParams; ++p)
        {
            StrUtil::toLower(vecparams[p]);
            if (vecparams[p] == _T("1d"))
            {
                tt = Texture::T_1D;
            }
            else if (vecparams[p] == _T("2d"))
            {
                tt = Texture::T_2D;
            }
            else if (vecparams[p] == _T("3d"))
            {
                tt = Texture::T_3D;
            }
            else if (vecparams[p] == _T("cubic"))
            {
                tt = Texture::T_CUBE;
            }
            else if (vecparams[p] == _T("unlimited"))
            {
                mipmaps = N_TEX_MIPMAP;
            }
            else if (StrConv::isNumber(vecparams[p]))
            {
                StrConv::conv(vecparams[p], mipmaps);
            }
            else if (vecparams[p] == _T("alpha"))
            {
                isAlpha = true;
            }
            else if (vecparams[p] == _T("gamma"))
            {
                hwGamma = true;
            }
            else if ((desiredFormat = PixelUtil::getFormat(vecparams[p], true)) != PF_UNKNOWN)
            {
                // nothing to do here
            }
            else
            {
                logParseError(_I18n("Invalid texture option - "+vecparams[p]+ _T(".")), context);
            }
        }
        Nui32 temprid;
        StrConv::conv(vecparams[0], temprid);
        ResourceIDList rlist;
        rlist.push_back(temprid);
        context.textureUnit->setSrc(rlist, tt);
        context.textureUnit->setMipmapCount(mipmaps);
        context.textureUnit->setIsAlpha(isAlpha);
        context.textureUnit->setFormat(desiredFormat);
        context.textureUnit->setHardwareSRGB(hwGamma);
        return false;
    }
    //---------------------------------------------------------------------
    bool parseBindingType(String & params, MaterialScriptContext & context)
    {
        if(params == _T("fragment"))
        {
            context.textureUnit->setFetchType(TDF_FS);
        }
        else if(params == _T("vertex"))
        {
            context.textureUnit->setFetchType(TDF_VS);
        }
        else
        {
            logParseError(_I18n("Invalid binding_type option - ") + params + _T("."), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseAnimTexture(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NCount numParams = vecparams.size();

        // Determine which form it is
        // Must have at least 3 params though
        if (numParams < 3)
        {
            logParseError(_I18n("Bad anim_texture attribute, wrong number of parameters (expected at least 3)"), context);
            return false;
        }
        Nui32 t1;
        Nui32 t2;
        ResourceIDList rlist;
        StrConv::conv(vecparams[1], t2);
        if (numParams == 3 && t2 != 0)
        {
            Nui32 rid;
            StrConv::conv(vecparams[0], rid);
            StrConv::conv(vecparams[2], t1);
            for (Nui32 j = 0; j < t2; ++j)
            {
                rlist.push_back(rid + j);
            }
            // First form using base name & number of frames
            context.textureUnit->setAni(rlist, t1);
        }
        else
        {
            for (NCount temp = 0; temp < numParams - 1; ++temp)
            {
                Nui32 rid;
                StrConv::conv(vecparams[temp], rid);
                rlist.push_back(rid);
            }
            StrConv::conv(vecparams[numParams - 1], t1);
            // Second form using individual names
            context.textureUnit->setAni(rlist, t1);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseCubicTexture(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NCount numParams = vecparams.size();

        // Get final param
        bool useUVW;
        String & uvOpt = vecparams[numParams - 1];
        StrUtil::toLower(uvOpt);
        if (uvOpt == _T("combineduvw"))
        {
            useUVW = true;
        }
        else if (uvOpt == _T("separateuv"))
        {
            useUVW = false;
        }
        else
        {
            logParseError(_I18n("Bad cubic_texture attribute, final parameter must be 'combinedUVW' or 'separateUV'."), context);
            return false;
        }
        ResourceIDList ridlist;
        // Determine which form it is
        if(numParams == 2)
        {
            Nui32 tempid;
            StrConv::conv(vecparams[0], tempid);
            ridlist.push_back(tempid);
            // First form using base name
            context.textureUnit->setSrc(ridlist, useUVW ? Texture::T_CUBE : Texture::T_CUBE_2D);
        }
        else if (numParams == 6)
        {
            for (NCount num = 0; num < numParams; ++num)
            {
                Nui32 tempid;
                StrConv::conv(vecparams[num], tempid);
                ridlist.push_back(tempid);
            }
            // Second form using individual names
            // Can use vecparams[0] as array start point
            context.textureUnit->setSrc(ridlist, Texture::T_CUBE_2D);
        }
        else
        {
            logParseError(_I18n("Bad cubic_texture attribute, wrong number of parameters (expected 2 or 7)"), context);
            return false;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseTexCoord(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        context.textureUnit->setCoordSet(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    TextureAddressingMode convTexAddressMode(const String & params, MaterialScriptContext & context)
    {
        if(params == _T("wrap"))
            return TAM_REPEAT;
        else if(params == _T("mirror"))
            return TAM_MIRROR;
        else if(params == _T("clamp"))
            return TAM_CLAMP;
        else if(params == _T("border"))
            return TAM_BORDER;
        else
            logParseError(_I18n("Bad tex_address_mode attribute, valid parameters are 'wrap', 'mirror', 'clamp' or 'border'."), context);
        return TAM_REPEAT;
    }
    //-----------------------------------------------------------------------
    bool parseTexAddressMode(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);

        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NCount numParams = vecparams.size();

        if(numParams > 3 || numParams < 1)
        {
            logParseError(_I18n("Invalid number of parameters to tex_address_mode - must be between 1 and 3"), context);
        }
        if(numParams == 1)
        {
            TextureAddressingMode tam = convTexAddressMode(vecparams[0], context);
            // Single-parameter option
            context.textureUnit->getSample()->setMode(tam, tam, tam);
        }
        else
        {
            // 2-3 parameter option
            TextureAddressing uvw;
            uvw.mS_U = convTexAddressMode(vecparams[0], context);
            uvw.mT_V = convTexAddressMode(vecparams[1], context);
            if(numParams == 3)
            {
                // w
                uvw.mR_W = convTexAddressMode(vecparams[2], context);
            }
            else
            {
                uvw.mR_W = TAM_REPEAT;
            }
            context.textureUnit->getSample()->setMode(uvw);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseTexBorderColour(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        // Must be 3 or 4 parameters
        if(vecparams.size() == 3 || vecparams.size() == 4)
        {
            context.textureUnit->getSample()->setBorder(_parseColourValue(vecparams));
        }
        else
        {
            logParseError(
                _I18n("Bad tex_border_colour attribute, wrong number of parameters (expected 3 or 4)"), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseColourOp(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params== _T("replace"))
            context.textureUnit->getColourBlend()->setColourBlend(TBM_Src_Replace_Dst);
        else if (params== _T("add"))
            context.textureUnit->getColourBlend()->setColourBlend(TBM_Src_Add_Dst);
        else if (params== _T("modulate"))
            context.textureUnit->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
        else if (params== _T("alpha_blend"))
            context.textureUnit->getColourBlend()->setColourBlend(TBM_Src_Alpha_Dst);
        else
            logParseError(_I18n("Bad colour_op attribute, valid parameters are 'replace', 'add', 'modulate' or 'alpha_blend'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseAlphaRejection(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Bad alpha_rejection attribute, wrong number of parameters (expected 2)"), context);
            return false;
        }

        CmpMode cmp;
        try
        {
            cmp = convertCompareFunction(vecparams[0]);
        }
        catch (...)
        {
            logParseError(_I18n("Bad alpha_rejection attribute, invalid compare function."), context);
            return false;
        }
        NIIi temp;
        StrConv::conv(vecparams[1], temp);
        ShaderChColour * tempalpha = context.pass->setColour(ShaderCh::SOT_Set);
        tempalpha->setAlphaReject(cmp);
        tempalpha->setValue(temp);
        tempalpha->setAlphaCoverage(false);
        return false;
    }
    //---------------------------------------------------------------------
    bool parseAlphaToCoverage(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(true);
        else if (params == _T("off"))
            context.pass->setColour(ShaderCh::SOT_Set)->setAlphaCoverage(false);
        else
            logParseError(_I18n("Bad alpha_to_coverage attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseTransparentSorting(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.pass->setDepthSort(true);
        else if (params == _T("off"))
            context.pass->setDepthSort(false);
        else
            logParseError(_I18n("Bad transparent_sorting attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    TextureBlendOp convertBlendOpEx(const String & param)
    {
        if (param == _T("source1"))
            return TBO_OPERAND1;
        else if (param == _T("source2"))
            return TBO_OPERAND2;
        else if (param == _T("modulate"))
            return TBO_MODULATE;
        else if (param == _T("modulate_x2"))
            return TBO_MODULATE_2X;
        else if (param == _T("modulate_x4"))
            return TBO_MODULATE_4X;
        else if (param == _T("add"))
            return TBO_ADD;
        else if (param == _T("add_signed"))
            return TBO_ADD_SIGNED;
        else if (param == _T("add_smooth"))
            return TBO_ADD_SMOOTH;
        else if (param == _T("subtract"))
            return TBO_SUB;
        else if (param == _T("blend_diffuse_colour"))
            return TBO_ADD_SPECULAR;
        else if (param == _T("blend_diffuse_alpha"))
            return TBO_ADD_DIFFUSE;
        else if (param == _T("blend_texture_alpha"))
            return TBO_ADD_TEXTURE;
        else if (param == _T("blend_current_alpha"))
            return TBO_ADD_PRE_TEXTURE;
        else if (param == _T("blend_manual"))
            return TBO_CONSTANT_FACTOR;
        else if (param == _T("dotproduct"))
            return TBO_DOT_PRODUCT;
        else
            N_EXCEPT(InvalidParam, _I18n("Invalid blend function"));
    }
    //-----------------------------------------------------------------------
    TextureBlendSource convertBlendSource(const String & param)
    {
        if (param == _T("src_current"))
            return TBS_PRE_TEXTURE;
        else if (param == _T("src_texture"))
            return TBS_TEXTURE;
        else if (param == _T("src_diffuse"))
            return TBS_DIFFUSE;
        else if (param == _T("src_specular"))
            return TBS_SPECULAR;
        else if (param == _T("src_manual"))
            return TBS_CONSTANT;
        else
            N_EXCEPT(InvalidParam, _I18n("Invalid blend source"));
    }
    //-----------------------------------------------------------------------
    bool parseColourOpEx(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NCount numParams = vecparams.size();

        if (numParams < 3 || numParams > 10)
        {
            logParseError(_I18n("Bad colour_op_ex attribute, wrong number of parameters (expected 3 to 10)"), context);
            return false;
        }
        TextureBlendOp op;
        TextureBlendSource src1, src2;
        NIIf manual = 0.0;
        Colour colSrc1 = Colour::White;
        Colour colSrc2 = Colour::White;

        try
        {
            op = convertBlendOpEx(vecparams[0]);
            src1 = convertBlendSource(vecparams[1]);
            src2 = convertBlendSource(vecparams[2]);

            if (op == TBO_CONSTANT_FACTOR)
            {
                if (numParams < 4)
                {
                    logParseError(_I18n("Bad colour_op_ex attribute, wrong number of parameters (expected 4 for manual blend)"), context);
                    return false;
                }
                StrConv::conv(vecparams[3], manual);
            }

            if (src1 == TBS_CONSTANT)
            {
                Nui32 parIndex = 3;
                if (op == TBO_CONSTANT_FACTOR)
                    parIndex++;

                if (numParams < parIndex + 3)
                {
                    logParseError(_I18n("Bad colour_op_ex attribute, wrong number of parameters (expected ") + N_conv(parIndex + 3) + _T(")"), context);
                    return false;
                }
                StrConv::conv(vecparams[parIndex++], colSrc1.r);
                StrConv::conv(vecparams[parIndex++], colSrc1.g);
                StrConv::conv(vecparams[parIndex++], colSrc1.b);
                if (numParams > parIndex)
                {
                    StrConv::conv(vecparams[parIndex], colSrc1.a);
                }
                else
                {
                    colSrc1.a = 1.0f;
                }
            }

            if (src2 == TBS_CONSTANT)
            {
                Nui32 parIndex = 3;
                if (op == TBO_CONSTANT_FACTOR)
                    parIndex++;
                if (src1 == TBS_CONSTANT)
                    parIndex += 3;

                if (numParams < parIndex + 3)
                {
                    logParseError(_I18n("Bad colour_op_ex attribute, wrong number of parameters (expected ") + N_conv(parIndex + 3) + _T(")"), context);
                    return false;
                }
                StrConv::conv(vecparams[parIndex++], colSrc2.r);
                StrConv::conv(vecparams[parIndex++], colSrc2.g);
                StrConv::conv(vecparams[parIndex++], colSrc2.b);
                if (numParams > parIndex)
                {
                    StrConv::conv(vecparams[parIndex], colSrc2.a);
                }
                else
                {
                    colSrc2.a = 1.0f;
                }
            }
        }
        catch (Exception& e)
        {
            logParseError(_I18n("Bad colour_op_ex attribute, ") + e.getVerbose(), context);
            return false;
        }
        TextureBlend * tb = context.textureUnit->getColourBlend();
        tb->setBlendType(TCT_RGB);
        tb->setBlend(op, src1, src2);
        tb->setConstant1(colSrc1);
        tb->setConstant2(colSrc2);
        tb->setConstantFactor(manual);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseColourOpFallback(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Bad colour_op_multipass_fallback attribute, wrong number of parameters (expected 2)"), context);
            return false;
        }

        //src/dest
        ColourFactor src, dest;

        try
        {
            src = convertBlendFactor(vecparams[0]);
            dest = convertBlendFactor(vecparams[1]);
        }
        catch (Exception& e)
        {
            logParseError(_I18n("Bad colour_op_multipass_fallback attribute, ") + e.getVerbose(), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseAlphaOpEx(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NCount numParams = vecparams.size();
        if (numParams < 3 || numParams > 6)
        {
            logParseError(_I18n("Bad alpha_op_ex attribute, wrong number of parameters (expected 3 to 6)"), context);
            return false;
        }
        TextureBlendOp op;
        TextureBlendSource src1, src2;
        NIIf manual = 0.0;
        NIIf arg1 = 1.0, arg2 = 1.0;

        try
        {
            op = convertBlendOpEx(vecparams[0]);
            src1 = convertBlendSource(vecparams[1]);
            src2 = convertBlendSource(vecparams[2]);
            if (op == TBO_CONSTANT_FACTOR)
            {
                if (numParams != 4)
                {
                    logParseError(_I18n("Bad alpha_op_ex attribute, wrong number of parameters (expected 4 for manual blend)"), context);
                    return false;
                }

                StrConv::conv(vecparams[3], manual);
            }
            if (src1 == TBS_CONSTANT)
            {
                Nui32 parIndex = 3;
                if (op == TBO_CONSTANT_FACTOR)
                    parIndex++;

                if(numParams < parIndex)
                {
                    logParseError(_I18n("Bad alpha_op_ex attribute, wrong number of parameters (expected ") +
                        N_conv(parIndex - 1) + _T(")"), context);
                    return false;
                }

                StrConv::conv(vecparams[parIndex], arg1);
            }

            if(src2 == TBS_CONSTANT)
            {
                Nui32 parIndex = 3;
                if(op == TBO_CONSTANT_FACTOR)
                    parIndex++;
                if(src1 == TBS_CONSTANT)
                    parIndex++;

                if(numParams < parIndex)
                {
                    logParseError(_I18n("Bad alpha_op_ex attribute, wrong number of parameters (expected ") + N_conv(parIndex - 1) + _T(")"), context);
                    return false;
                }

                StrConv::conv(vecparams[parIndex], arg2);
            }
        }
        catch(Exception & e)
        {
            logParseError(_I18n("Bad alpha_op_ex attribute, ") + e.getVerbose(), context);
            return false;
        }
        TextureBlend * tb =  context.textureUnit->getAlphaBlend();
        tb->setBlendType(TCT_AAA);
        tb->setBlend(op, src1, src2);
        tb->setConstant1(arg1);
        tb->setConstant2(arg2);
        tb->setConstantFactor(manual);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseEnvMap(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if(params == _T("off"))
            context.textureUnit->setEnvMap(false);
        else if (params == _T("spherical"))
        {
            context.textureUnit->setEnvMap(true);
            context.textureUnit->setEnvMapType(ShaderChTextureUnit::ENM_Sphere);
        }
        else if (params == _T("planar"))
        {
            context.textureUnit->setEnvMap(true);
            context.textureUnit->setEnvMapType(ShaderChTextureUnit::ENM_Planar);
        }
        else if (params == _T("cubic_reflection"))
        {
            context.textureUnit->setEnvMap(true);
            context.textureUnit->setEnvMapType(ShaderChTextureUnit::ENM_Reflection);
        }
        else if (params == _T("cubic_normal"))
        {
            context.textureUnit->setEnvMap(true);
            context.textureUnit->setEnvMapType(ShaderChTextureUnit::ENM_Normal);
        }
        else
            logParseError(_I18n("Bad env_map attribute, valid parameters are 'off','spherical', 'planar', 'cubic_reflection' and 'cubic_normal'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseScroll(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Bad scroll attribute, wrong number of parameters (expected 2)"), context);
            return false;
        }
        NIIf t1, t2;
        StrConv::conv(vecparams[0], t1);
        StrConv::conv(vecparams[1], t2);
        context.textureUnit->setUOft(t1);
        context.textureUnit->setVOft(t2);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseScrollAnim(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Bad scroll_anim attribute, wrong number of parameters (expected 2)"), context);
            return false;
        }
        NIIf t1, t2;
        StrConv::conv(vecparams[0], t1);
        StrConv::conv(vecparams[1], t2),
        context.textureUnit->setScrollAnimation(t1, t2);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseRotate(String & params, MaterialScriptContext & context)
    {
        Radian temp;
        StrConv::conv(params, temp);
        context.textureUnit->setRotate(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseRotateAnim(String & params, MaterialScriptContext & context)
    {
        NIIf temp;
        StrConv::conv(params, temp);
        context.textureUnit->setRotateAnimation(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseScale(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() != 2)
        {
            logParseError(_I18n("Bad scale attribute, wrong number of parameters (expected 2)"), context);
            return false;
        }
        NIIf t1, t2;
        StrConv::conv(vecparams[0], t1);
        StrConv::conv(vecparams[1], t2);
        context.textureUnit->setUScale(t1);
        context.textureUnit->setVScale(t2);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseWaveXform(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));

        if(vecparams.size() != 6)
        {
            logParseError(_I18n("Bad wave_xform attribute, wrong number of parameters (expected 6)"), context);
            return false;
        }
        ShaderChTextureUnit::TextureTransformType ttype;
        WaveformType waveType;
        DataValue<NIIf> * val;
        // Check transform type
        if(vecparams[0] == _T("scroll_x"))
        {
            ttype = ShaderChTextureUnit::TTT_U_OFT;
            val = N_new TexCoordModifierControllerValue(true, context.textureUnit, true);
        }
        else if(vecparams[0] == _T("scroll_y"))
        {
            ttype = ShaderChTextureUnit::TTT_V_OFT;
            val = N_new TexCoordModifierControllerValue(true, context.textureUnit, false, true);
        }
        else if(vecparams[0] == _T("rotate"))
        {
            ttype = ShaderChTextureUnit::TTT_ROTATE;
            val = N_new TexCoordModifierControllerValue(true, context.textureUnit,
                false, false, false, false, true);
        }
        else if(vecparams[0] == _T("scale_x"))
        {
            ttype = ShaderChTextureUnit::TTT_U_SCALE;
            val = N_new TexCoordModifierControllerValue(true, context.textureUnit,
                false, false, true);
        }
        else if(vecparams[0] == _T("scale_y"))
        {
            ttype = ShaderChTextureUnit::TTT_V_SCALE;
            val = N_new TexCoordModifierControllerValue(true, context.textureUnit,
                false, false, false, true);
        }
        else
        {
            logParseError(_I18n("Bad wave_xform attribute, parameter 1 must be 'scroll_x','scroll_y', 'rotate', 'scale_x' or 'scale_y'"), context);
            return false;
        }
        // Check wave type
        if (vecparams[1]== _T("sine"))
            waveType = WFT_SINE;
        else if (vecparams[1]== _T("triangle"))
            waveType = WFT_TRIANGLE;
        else if (vecparams[1]== _T("square"))
            waveType = WFT_SQUARE;
        else if (vecparams[1]== _T("sawtooth"))
            waveType = WFT_SAWTOOTH;
        else if (vecparams[1]== _T("inverse_sawtooth"))
            waveType = WFT_INVERSE_SAWTOOTH;
        else
        {
            logParseError(_I18n("Bad wave_xform attribute, parameter 2 must be 'sine','triangle', 'square', 'sawtooth' or 'inverse_sawtooth'"), context);
            return false;
        }

        NIIf t1, t2, t3, t4;
        StrConv::conv(vecparams[2], t1);
        StrConv::conv(vecparams[3], t2);
        StrConv::conv(vecparams[4], t3);
        StrConv::conv(vecparams[5], t4);

        DataFunc<TimeDurMS, NIIf> * func = N_new WaveformControllerFunction(true, waveType, t1, t2, t3, t4, true);
        context.textureUnit->addAnimation(ttype, val, func);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseTransform(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 16)
        {
            logParseError(_I18n("Bad transform attribute, wrong number of parameters (expected 16)"), context);
            return false;
        }
        Matrix4f xform;
        StrConv::conv(vecparams[0], xform._m[0]);StrConv::conv(vecparams[1], xform._m[1]);
        StrConv::conv(vecparams[2], xform._m[2]);StrConv::conv(vecparams[3], xform._m[3]);
        StrConv::conv(vecparams[4], xform._m[4]);StrConv::conv(vecparams[5], xform._m[5]);
        StrConv::conv(vecparams[6], xform._m[6]);StrConv::conv(vecparams[7], xform._m[7]);
        StrConv::conv(vecparams[8], xform._m[8]);StrConv::conv(vecparams[9], xform._m[9]);
        StrConv::conv(vecparams[10], xform._m[10]);StrConv::conv(vecparams[11], xform._m[11]);
        StrConv::conv(vecparams[12], xform._m[12]);StrConv::conv(vecparams[13], xform._m[13]);
        StrConv::conv(vecparams[14], xform._m[14]);StrConv::conv(vecparams[15], xform._m[15]);

        context.textureUnit->setMatrix(xform);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseDepthBias(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NIIf temp;
        StrConv::conv(vecparams[0], temp);

        NIIf constantBias = temp;
        NIIf slopeScaleBias = 0.0f;
        if (vecparams.size() > 1)
        {
            StrConv::conv(vecparams[1], temp);
            slopeScaleBias = temp;
        }

        context.pass->setStencil(ShaderCh::SOT_Set)->setBiasConstant(constantBias);
        context.pass->setStencil(ShaderCh::SOT_Set)->setBiasSlopeScale(slopeScaleBias);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseIterationDepthBias(String & params, MaterialScriptContext& context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        NIIf bias;
        StrConv::conv(vecparams[0], bias);
        context.pass->setStencil(ShaderCh::SOT_Set)->setBiasConstantFactor(bias);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseAnisotropy(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        context.textureUnit->getSample()->setAnisotropy(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseMipmapBias(String & params, MaterialScriptContext & context)
    {
        NIIf temp;
        StrConv::conv(params, temp);
        context.textureUnit->getSample()->setMipmapBias(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseContentType(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::tokenise(params, vecparams, _T(" \t"));
        if (vecparams.empty())
        {
            logParseError(_I18n("No content_type specified"), context);
            return false;
        }
        const String & paramType = vecparams[0];
        if(paramType == _T("named"))
        {
            context.textureUnit->setContentType(ShaderChTexture::T_NORMAL);
        }
        else if(paramType == _T("shadow"))
        {
            context.textureUnit->setContentType(ShaderChTexture::T_SHADOW);
        }
        else if(paramType == _T("compositor"))
        {
            context.textureUnit->setContentType(ShaderChTexture::T_FUSION);
            if(vecparams.size() == 3)
            {
                Ni32 fusionid;
                StrConv::conv(vecparams[1], fusionid);
                context.textureUnit->setFusion(fusionid, vecparams[2]);
            }
            else if(vecparams.size() == 4)
            {
                Nui32 temp;
                StrConv::conv(vecparams[3], temp);
                Ni32 fusionid;
                StrConv::conv(vecparams[1], fusionid);
                context.textureUnit->setFusion(fusionid, vecparams[2]);
                context.textureUnit->setMultiIndex(temp);
            }
            else
            {
                logParseError(_I18n("compositor content_type requires 2 or 3 extra params"), context);
            }
        }
        else
        {
            logParseError(_I18n("Invalid content_type specified : ") + paramType, context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseIlluminationStage(String& params, MaterialScriptContext& context)
    {
        if (params == _T("ambient"))
        {
            context.pass->setShaderStep(SS_Global);
        }
        else if (params == _T("per_light"))
        {
            context.pass->setShaderStep(SS_Detail_Light);
        }
        else if (params == _T("decal"))
        {
            context.pass->setShaderStep(SS_Decal);
        }
        else
        {
            logParseError(_I18n("Invalid illumination_stage specified."), context);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLodValues(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));

        MaterialLodScheme * scheme = context.material->getLodScheme();
        scheme->removeAll();
        scheme->add(0.0f);

        StringList::iterator i, iend = vecparams.end();
        for(i = vecparams.begin(); i != iend; ++i)
        {
            NIIf temp;
            StrConv::conv(*i, temp);
            scheme->add(temp);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLodIndex(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        context.technique->setLodIndex((Nui16)temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseScheme(String & params, MaterialScriptContext & context)
    {
        Nui32 temp;
        StrConv::conv(params, temp);
        context.technique->setSchemeID(temp);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseGPUVendorRule(String & params, MaterialScriptContext & context)
    {
        GpuVendorDetail rule;
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() != 3)
        {
            logParseError(_I18n("Wrong number of parameters for gpu_vendor_rule, expected 2"), context);
            return false;
        }

        rule.mVendor = GpuVendorDetail::toVendor(vecparams[0]);
        if(rule.mVendor == GT_Unknow)
        {
            logParseError(_I18n("Unknown vendor '") + vecparams[1] + _I18n("' ignored in gpu_vendor_rule"), context);
            return false;
        }
        rule.mSer = vecparams[1];
        StrConv::conv(vecparams[2], rule.mNum);
        context.technique->add(rule);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseShadowCasterMaterial(String & params, MaterialScriptContext & context)
    {
        Ni32 mid;
        StrConv::conv(params, mid);
        context.technique->setShadowCaster(static_cast<Material *>(N_Only(Material).get(mid)));
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseShadowReceiverMaterial(String & params, MaterialScriptContext & context)
    {
        Ni32 mid;
        StrConv::conv(params, mid);
        context.technique->setShadowReceiver(static_cast<Material *>(N_Only(Material).get(mid)));
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseSetTextureAlias(String & params, MaterialScriptContext & context)
    {
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Wrong number of parameters for texture_alias, expected 2"), context);
            return false;
        }
        // first parameter is alias name and second parameter is texture name
        context.textureAliases[vecparams[0]] = vecparams[1];

        return false;
    }
    //-----------------------------------------------------------------------
    void processManualProgramParam(bool isNamed, const String commandname,
        StringList & vecparams, MaterialScriptContext & context, NCount index = 0,
            const String & paramNamew = StrUtil::WBLANK)
    {
        VString paramName;
        StrConv::conv(paramNamew, paramName);
        // NB we assume that the first element of vecparams is taken up with either
        // the index or the parameter name, which we ignore

        // Determine type
        NCount start, dims, roundedDims, i;
        bool isReal;
        bool isMatrix4x4 = false;

        StrUtil::toLower(vecparams[1]);

        if (vecparams[1] == _T("matrix4x4"))
        {
            dims = 16;
            isReal = true;
            isMatrix4x4 = true;
        }
        else if ((start = vecparams[1].find(_T("float"))) != String::npos)
        {
            // find the dimensionality
            start = vecparams[1].find_first_not_of(_T("float"));
            // Assume 1 if not specified
            if (start == String::npos)
            {
                dims = 1;
            }
            else
            {
                StrConv::conv(vecparams[1].substr(start), dims);
            }
            isReal = true;
        }
        else if ((start = vecparams[1].find(_T("int"))) != String::npos)
        {
            // find the dimensionality
            start = vecparams[1].find_first_not_of(_T("int"));
            // Assume 1 if not specified
            if (start == String::npos)
            {
                dims = 1;
            }
            else
            {
                StrConv::conv(vecparams[1].substr(start), dims);
            }
            isReal = false;
        }
        else
        {
            logParseError(_I18n("Invalid ") + commandname + _I18n(" attribute - unrecognised parameter type ") +
                vecparams[1], context);
            return;
        }

        if (vecparams.size() != 2 + dims)
        {
            logParseError(_I18n("Invalid ") + commandname + _I18n(" attribute - you need ") +
                N_conv(2 + dims) + _I18n(" parameters for a parameter of type ") +
                    vecparams[1], context);
        }

        // clear any auto parameter bound to this constant, it would override this setting
        // can cause problems overriding materials or changing default params
        if (isNamed)
            context.programParams->removeSyncParam(paramName);
        else
            context.programParams->removeSyncParam(index);


        // Round dims to multiple of 4
        if (dims %4 != 0)
        {
            roundedDims = dims + 4 - (dims % 4);
        }
        else
        {
            roundedDims = dims;
        }

        // Now parse all the values
        if (isReal)
        {
            NIIf * realBuffer = N_alloc_t(NIIf, roundedDims);
            // Do specified values
            for (i = 0; i < dims; ++i)
            {
                StrConv::conv(vecparams[i+2], realBuffer[i]);
            }
            // Fill up to multiple of 4 with zero
            for (; i < roundedDims; ++i)
            {
                realBuffer[i] = 0.0f;

            }

            if (isMatrix4x4)
            {
                // its a Matrix4x4 so pass as a Matrix4
                // use specialized set that takes a matrix so matrix is transposed if required
                Matrix4f m4x4(
                    realBuffer[0],  realBuffer[1],  realBuffer[2],  realBuffer[3],
                    realBuffer[4],  realBuffer[5],  realBuffer[6],  realBuffer[7],
                    realBuffer[8],  realBuffer[9],  realBuffer[10], realBuffer[11],
                    realBuffer[12], realBuffer[13], realBuffer[14], realBuffer[15]
                    );
                if (isNamed)
                    context.programParams->set(paramName, 0, m4x4);
                else
                    context.programParams->set(index, 0, m4x4);
            }
            else
            {
                // Set
                if (isNamed)
                {
                    // For named, only set up to the precise number of elements
                    // (no rounding to 4 elements)
                    // GLSL can support sub-float4 elements and we support that
                    // in the buffer now. Note how we set the 'multiple' param to 1
                    context.programParams->set(paramName, 0, realBuffer, dims, 1);
                }
                else
                {
                    context.programParams->set(index, 0, realBuffer, (NCount)(roundedDims * 0.25));
                }
            }
            N_free(realBuffer);
        }
        else
        {
            NIIi* intBuffer = N_alloc_t(NIIi, roundedDims);
            // Do specified values
            for (i = 0; i < dims; ++i)
            {
                StrConv::conv(vecparams[i+2], intBuffer[i]);
            }
            // Fill to multiple of 4 with 0
            for (; i < roundedDims; ++i)
            {
                intBuffer[i] = 0;
            }
            // Set
            if (isNamed)
            {
                // For named, only set up to the precise number of elements
                // (no rounding to 4 elements)
                // GLSL can support sub-float4 elements and we support that
                // in the buffer now. Note how we set the 'multiple' param to 1
                context.programParams->set(paramName, 0, intBuffer, dims, 1);
            }
            else
            {
                context.programParams->set(index, 0, intBuffer, (NCount)(roundedDims * 0.25));
            }
            N_free(intBuffer);
        }
    }
    //-----------------------------------------------------------------------
    void processAutoProgramParam(bool isNamed, const String & commandname,
        StringList & vecparams, MaterialScriptContext & context, NCount index = 0,
            const String & paramNamew = StrUtil::WBLANK)
    {
        VString paramName;
        StrConv::conv(paramNamew, paramName);
        // NB we assume that the first element of vecparams is taken up with either
        // the index or the parameter name, which we ignore

        // make sure param is in lower case
        StrUtil::toLower(vecparams[1]);

        // lookup the param to see if its a valid auto constant
        const GpuParamUnit * syncParam = GpuProgramManager::getSyncParam(vecparams[1]);

        // exit with error msg if the auto constant definition wasn't found
        if (!syncParam)
        {
            logParseError(_I18n("Invalid ") + commandname + _I18n(" attribute - ") + vecparams[1], context);
            return;
        }

        // add AutoConstant based on the type of data it uses
        switch (syncParam->mInputType)
        {
        case SPI_None:
            if(isNamed)
                context.programParams->set(paramName, syncParam->mSyncParam, (NIIi)0);
            else
                context.programParams->set(index, syncParam->mSyncParam, (NIIi)0);
            break;

        case SPI_Int:
            {
                // Special case animation_parametric, we need to keep track of number of times used
                if(syncParam->mSyncParam == GSP_Ch_AniParam)
                {
                    if(isNamed)
                        context.programParams->set(paramName, syncParam->mSyncParam,
                            (NIIi)(context.numAnimationParametrics++));
                    else
                        context.programParams->set(index, syncParam->mSyncParam, 
                            (NIIi)(context.numAnimationParametrics++));
                }
                // Special case texture projector - assume 0 if data not specified
                else if ((syncParam->mSyncParam == GSP_Texture_Matrix_Proj_X_View_ParamIndex ||
                    syncParam->mSyncParam == GSP_Texture_Matrix_Proj_X_View_X_Space_ParamIndex ||
                    syncParam->mSyncParam == GSP_Light_Spot_Matrix_Proj_X_View_ParamIndex ||
                    syncParam->mSyncParam == GSP_Light_Spot_Matrix_Proj_X_View_X_Space)
                        && vecparams.size() == 2)
                {
                    if(isNamed)
                        context.programParams->set(paramName, syncParam->mSyncParam, (NIIi)0);
                    else
                        context.programParams->set(index, syncParam->mSyncParam, (NIIi)0);
                }
                else
                {
                    if(vecparams.size() != 3)
                    {
                        logParseError(_I18n("Invalid ") + commandname + _I18n(" attribute - expected 3 parameters."), context);
                        return;
                    }
                    Ni32 extraParam;
                    StrConv::conv(vecparams[2], extraParam);
                    if(isNamed)
                        context.programParams->set(paramName, syncParam->mSyncParam, (NIIi)extraParam);
                    else
                        context.programParams->set(index, syncParam->mSyncParam, (NIIi)extraParam);
                }
            }
            break;

        case SPI_Float:
            {
                // special handling for time
                if (syncParam->mSyncParam == GSP_Sys_CostTime ||
                    syncParam->mSyncParam == GSP_Frame_CostTime)
                {
                    NIIf factor;
                    if (vecparams.size() == 3)
                    {
                        StrConv::conv(vecparams[2], factor, 1.0f);
                    }

                    if (isNamed)
                        context.programParams->set(paramName, syncParam->mSyncParam, factor);
                    else
                        context.programParams->set(index, syncParam->mSyncParam, factor);
                }
                else // normal processing for auto constants that take an extra real value
                {
                    if (vecparams.size() != 3)
                    {
                        logParseError(_I18n("Invalid ") + commandname + _I18n(" attribute - expected 3 parameters."), context);
                        return;
                    }

                    NIIf rData;
                    StrConv::conv(vecparams[2], rData);
                    if (isNamed)
                        context.programParams->set(paramName, syncParam->mSyncParam, rData);
                    else
                        context.programParams->set(index, syncParam->mSyncParam, rData);
                }
            }
            break;
        } // end switch
    }
    //-----------------------------------------------------------------------
    bool parseParamIndexed(String & params, MaterialScriptContext & context)
    {
        // NB skip this if the program is not supported or could not be found
        if(!context.program || !context.program->isValid())
        {
            return false;
        }

        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() < 3)
        {
            logParseError(_I18n("Invalid param_indexed attribute - expected at least 3 parameters."), context);
            return false;
        }

        Nui32 index;
        // Get start index
        StrConv::conv(vecparams[0], index);

        processManualProgramParam(false, _T("param_indexed"), vecparams, context, index);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseParamIndexedAuto(String & params, MaterialScriptContext & context)
    {
        // NB skip this if the program is not supported or could not be found
        if (!context.program || !context.program->isValid())
        {
            return false;
        }

        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2 && vecparams.size() != 3)
        {
            logParseError(_I18n("Invalid param_indexed_auto attribute - expected 2 or 3 parameters."), context);
            return false;
        }

        Nui32 index;
        // Get start index
        StrConv::conv(vecparams[0], index);

        processAutoProgramParam(false, _T("param_indexed_auto"), vecparams, context, index);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseParamNamed(String & params, MaterialScriptContext & context)
    {
        // NB skip this if the program is not supported or could not be found
        if (!context.program || !context.program->isValid())
        {
            return false;
        }

        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() < 3)
        {
            logParseError(_I18n("Invalid param_named attribute - expected at least 3 parameters."), context);
            return false;
        }

        try
        {
            VString tempstr;
            StrConv::conv(vecparams[0], tempstr);
            const GpuParamUnit & def = context.programParams->getUnit(tempstr);
            (void)def; // Silence warning
        }
        catch (Exception & e)
        {
            logParseError(_I18n("Invalid param_named attribute - ") + e.getVerbose(), context);
            return false;
        }

        processManualProgramParam(true, _T("param_named"), vecparams, context, 0, vecparams[0]);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseParamNamedAuto(String & params, MaterialScriptContext & context)
    {
        // NB skip this if the program is not supported or could not be found
        if(!context.program || !context.program->isValid())
        {
            return false;
        }

        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() != 2 && vecparams.size() != 3)
        {
            logParseError(_I18n("Invalid param_indexed_auto attribute - expected 2 or 3 parameters."), context);
            return false;
        }

        // Get start index from name
        try
        {
            VString tempstr;
            StrConv::conv(vecparams[0], tempstr);
            const GpuParamUnit & def = context.programParams->getUnit(tempstr);
            (void)def; // Silence warning
        }
        catch(Exception & e)
        {
            logParseError(_I18n("Invalid param_named_auto attribute - ") + e.getVerbose(), context);
            return false;
        }

        processAutoProgramParam(true, _T("param_named_auto"), vecparams, context, 0, vecparams[0]);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseMaterial(String & params, MaterialScriptContext & context)
    {
        // nfz:
        // check params for reference to parent material to copy from
        // syntax: material name : parentMaterialName
        // check params for a colon after the first name and extract the parent name
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(":"), 1);
        Material * basematerial = 0;

        // Create a brand new material
        if (vecparams.size() >= 2)
        {
            // if a second parameter exists then assume its the name of the base material
            // that this new material should clone from
            StrUtil::trim(vecparams[1]);
            Ni32 tempid;
            StrConv::conv(vecparams[1], tempid);
            // make sure base material exists
            basematerial = static_cast<Material *>(N_Only(Material).get(tempid));
            // if it doesn't exist then report error in log and just create a new material
            if (!basematerial)
            {
                logParseError(_I18n("parent material: ") + vecparams[1] + _I18n(" not found for new material:")
                    + vecparams[0], context);
            }
        }

        // get rid of leading and trailing white space from material name
        StrUtil::trim(vecparams[0]);

        Ni32 tempid;
        StrConv::conv(vecparams[0], tempid);
        context.material = static_cast<Material *>(N_Only(Material).create(tempid, context.groupName));

        if(basematerial)
        {
            // copy parent material details to new material
            *(context.material) = *basematerial;
        }
        else
        {
            // Remove pre-created technique from defaults
            context.material->destroyAll();
        }

        context.material->setSrc(context.filename);

        // update section
        context.section = MSS_MATERIAL;

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseTechnique(String & params, MaterialScriptContext & context)
    {
        // if params is not empty then see if the technique name already exists
        if(!params.empty() && (context.material->getCount() > 0))
        {
            // find the technique with name = params
            ShaderFusion * foundTechnique = context.material->get(params);
            if(foundTechnique)
            {
                // figure out technique index by iterating through technique container
                // would be nice if each technique remembered its index
                NIIi count = 0;
                ShaderFusionList::const_iterator i, iend = context.material->getList().end();
                for(i = context.material->getList().begin(); i != iend; ++i)
                {
                    if(foundTechnique == *i)
                        break;
                    ++count;
                }

                context.techLev = count;
            }
            else
            {
                // name was not found so a new technique is needed
                // position technique level to the end index
                // a new technique will be created later on
                context.techLev = context.material->getCount();
            }

        }
        else
        {
            // no name was given in the script so a new technique will be created
            // Increase technique level depth
            ++context.techLev;
        }

        // Create a new technique if it doesn't already exist
        if (context.material->getCount() > context.techLev)
        {
            context.technique = context.material->get((Nidx)context.techLev);
        }
        else
        {
            context.technique = context.material->create();
            if (!params.empty())
                context.technique->setName(params);
        }

        // update section
        context.section = MSS_TECHNIQUE;

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parsePass(String & params, MaterialScriptContext & context)
    {
        // if params is not empty then see if the pass name already exists
        if(!params.empty() && (context.technique->getCount() > 0))
        {
            // find the pass with name = params
            ShaderCh * foundPass = context.technique->get(params);
            if (foundPass)
            {
                context.passLev = foundPass->getIndex();
            }
            else
            {
                // name was not found so a new pass is needed
                // position pass level to the end index
                // a new pass will be created later on
                context.passLev = context.technique->getCount();
            }

        }
        else
        {
            //Increase pass level depth
            ++context.passLev;
        }

        if (context.technique->getCount() > context.passLev)
        {
            context.pass = context.technique->get(context.passLev);
        }
        else
        {
            // Create a new pass
            context.pass = context.technique->create();
            if (!params.empty())
                context.pass->setName(params);
        }

        // update section
        context.section = MSS_PASS;

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseTextureUnit(String & params, MaterialScriptContext & context)
    {
        // if params is a name then see if that texture unit exists
        // if not then log the warning and just move on to the next TU from current
        if(!params.empty() && (context.pass->getTexture().getCount() > 0))
        {
            // specifying a TUS name in the script for a TU means that a specific TU is being requested
            // try to get the specific TU
            // if the index requested is not valid, just creat a new TU
            // find the TUS with name = params
            ShaderChTextureUnit * foundTUS = context.pass->getTexture().get(params);
            if(foundTUS)
            {
                context.stateLev = foundTUS->getLocalIndex(context.pass);
            }
            else
            {
                // name was not found so a new TUS is needed
                // position TUS level to the end index
                // a new TUS will be created later on
                context.stateLev = context.pass->getTexture().getCount();
            }
        }
        else
        {
            //Increase Texture Unit State level depth
            ++context.stateLev;
        }

        if(context.pass->getTexture().getCount() > (NCount)(context.stateLev))
        {
            context.textureUnit = context.pass->getTexture().get((Nidx)context.stateLev);
        }
        else
        {
            // Create a new texture unit
            context.textureUnit = context.pass->setTexture(ShaderCh::SOT_Set)->create();
            if(!params.empty())
                context.textureUnit->setName(params);
        }
        // update section
        context.section = MSS_TEXTUREUNIT;

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseVertexProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;
        Ni32 tempid;
        StrConv::conv(params, tempid);
        // check if pass has a vertex program already
        if (context.pass->getProgram().isExist(ShaderChProgram::ST_VS))
        {
            // if existing pass vertex program has same name as params
            // or params is empty then use current vertex program
            GpuProgram * prog;
            context.pass->getProgram().get(ShaderChProgram::ST_VS, prog);
            GpuProgramID id = prog->getOriginID();
            if(params.empty() || (id == tempid))
            {
                context.pass->getProgram().get(ShaderChProgram::ST_VS, context.program);
            }
        }

        // if context.program was not set then try to get the vertex program using the name
        // passed in params
        if(!context.program)
        {
            context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
            if(!context.program)
            {
                // Unknown program
                logParseError(_I18n("Invalid vertex_program_ref entry - vertex program ") + params +
                    _I18n(" has not been defined."), context);
                return true;
            }
            // Set the vertex program for this pass
            context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS, tempid);
        }

        context.isVertexProgramShadowCaster = false;
        context.isFragmentProgramShadowCaster = false;
        context.isVertexProgramShadowReceiver = false;
        context.isFragmentProgramShadowReceiver = false;

        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_VS, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseGeometryProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;
        Ni32 tempid;
        StrConv::conv(params, tempid);
        // check if pass has a vertex program already
        if (context.pass->getProgram().isExist(ShaderChProgram::ST_GS))
        {
            // if existing pass vertex program has same name as params
            // or params is empty then use current vertex program
            GpuProgram * prog;
            context.pass->getProgram().get(ShaderChProgram::ST_GS, prog);
            GpuProgramID id = prog->getOriginID();
            if(params.empty() || (id == tempid))
            {
                context.pass->getProgram().get(ShaderChProgram::ST_GS, context.program);
            }
        }

        // if context.program was not set then try to get the geometry program using the name
        // passed in params
        if (!context.program)
        {
            Ni32 tempid;
            StrConv::conv(params, tempid);
            context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
            if (!context.program)
            {
                // Unknown program
                logParseError(_I18n("Invalid geometry_program_ref entry - vertex program ") + params +
                    _I18n(" has not been defined."), context);
                return true;
            }

            // Set the vertex program for this pass
            context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_GS, tempid);
        }

        context.isVertexProgramShadowCaster = false;
        context.isFragmentProgramShadowCaster = false;
        context.isVertexProgramShadowReceiver = false;
        context.isFragmentProgramShadowReceiver = false;

        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_GS, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseShadowCasterVertexProgramRef(String & params, MaterialScriptContext& context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;

        Ni32 tempid;
        StrConv::conv(params, tempid);

        context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
        if(!context.program)
        {
            // Unknown program
            logParseError(_I18n("Invalid shadow_caster_vertex_program_ref entry - vertex program ") +
                params + _I18n(" has not been defined."), context);
            return true;
        }

        context.isVertexProgramShadowCaster = true;
        context.isFragmentProgramShadowCaster = false;
        context.isVertexProgramShadowReceiver = false;
        context.isFragmentProgramShadowReceiver = false;

        // Set the vertex program for this pass
        context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS_SC, tempid);
        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_VS_SC, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseShadowCasterFragmentProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;

        Ni32 tempid;
        StrConv::conv(params, tempid);
        context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
        if (!context.program)
        {
            // Unknown program
            logParseError(_T("Invalid shadow_caster_fragment_program_ref entry - fragment program ")
                + params + _T(" has not been defined."), context);
            return true;
        }

        context.isVertexProgramShadowCaster = false;
        context.isFragmentProgramShadowCaster = true;
        context.isVertexProgramShadowReceiver = false;
        context.isFragmentProgramShadowReceiver = false;

        // Set the vertex program for this pass
        context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS_SC, tempid);

        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_FS_SC, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseShadowReceiverVertexProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;

        Ni32 tempid;
        StrConv::conv(params, tempid);
        context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
        if (!context.program)
        {
            // Unknown program
            logParseError(_I18n("Invalid shadow_receiver_vertex_program_ref entry - vertex program ") + params +
                _I18n(" has not been defined."), context);
            return true;
        }

        context.isVertexProgramShadowCaster = false;
        context.isFragmentProgramShadowCaster = false;
        context.isVertexProgramShadowReceiver = true;
        context.isFragmentProgramShadowReceiver = false;

        // Set the vertex program for this pass
        context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_VS_SR, tempid);

        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_VS_SR, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseShadowReceiverFragmentProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;
        Ni32 tempid;
        StrConv::conv(params, tempid);
        context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
        if (!context.program)
        {
            // Unknown program
            logParseError(_I18n("Invalid shadow_receiver_fragment_program_ref entry - fragment program ") + params +
                _I18n(" has not been defined."), context);
            return true;
        }

        context.isVertexProgramShadowCaster = false;
        context.isFragmentProgramShadowCaster = false;
        context.isVertexProgramShadowReceiver = false;
        context.isFragmentProgramShadowReceiver = true;

        // Set the vertex program for this pass
        context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS_SR, tempid);

        // Create params? Skip this if program is not supported
        if(context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_FS_SR, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseFragmentProgramRef(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM_REF;
        Ni32 tempid;
        StrConv::conv(params, tempid);
        // check if pass has a fragment program already
        if(context.pass->getProgram().isExist(ShaderChProgram::ST_FS))
        {
            // if existing pass fragment program has same name as params
            // or params is empty then use current fragment program
            GpuProgram * prog;
            context.pass->getProgram().get(ShaderChProgram::ST_FS, prog);
            GpuProgramID id = prog->getOriginID();
            if(params.empty() || (id == tempid))
            {
                context.pass->getProgram().get(ShaderChProgram::ST_FS, context.program);
            }
        }

        // if context.program was not set then try to get the fragment program using the name
        // passed in params
        if (!context.program)
        {
            context.program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(tempid));
            if (!context.program)
            {
                // Unknown program
                logParseError(_I18n("Invalid fragment_program_ref entry - fragment program ") + params +
                    _I18n(" has not been defined."), context);
                return true;
            }

            // Set the vertex program for this pass
            context.pass->setProgram(ShaderCh::SOT_Set)->set(ShaderChProgram::ST_FS, tempid);
        }

        // Create params? Skip this if program is not supported
        if (context.program->isValid())
        {
            context.pass->getProgram().getParam(ShaderChProgram::ST_FS, context.programParams);
            context.numAnimationParametrics = 0;
        }

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseVertexProgram(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM;

        // Create new program definition-in-prniiss
        context.programDef = N_new_t(MaterialScriptProgramDefinition)();
        context.programDef->progType = GpuProgram::ST_VS;
        context.programDef->supportsSkeletalAnimation = false;
        context.programDef->supportsMorphAnimation = false;
        context.programDef->supportsPoseAnimation = 0;
        context.programDef->usesVertexTextureFetch = false;

        // Get name and language code
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Invalid vertex_program entry - expected 2 parameters."), context);
            return true;
        }
        // Name, preserve case
        StrConv::conv(vecparams[0], context.programDef->name);
        // language code, make lower case
        StrConv::conv(vecparams[1], context.programDef->language);
        //StrUtil::toLower(context.programDef->language);

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseGeometryProgram(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM;

        // Create new program definition-in-prniiss
        context.programDef = N_new_t(MaterialScriptProgramDefinition)();
        context.programDef->progType = GpuProgram::ST_GS;
        context.programDef->supportsSkeletalAnimation = false;
        context.programDef->supportsMorphAnimation = false;
        context.programDef->supportsPoseAnimation = 0;
        context.programDef->usesVertexTextureFetch = false;

        // Get name and language code
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if(vecparams.size() != 2)
        {
            logParseError(_I18n("Invalid geometry_program entry - expected 2 parameters."), context);
            return true;
        }
        // Name, preserve case
        StrConv::conv(vecparams[0], context.programDef->name);
        // language code, make lower case
        StrConv::conv(vecparams[1], context.programDef->language);
        //StrUtil::toLower(context.programDef->language);

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseFragmentProgram(String & params, MaterialScriptContext & context)
    {
        // update section
        context.section = MSS_PROGRAM;

        // Create new program definition-in-prniiss
        context.programDef = N_new_t(MaterialScriptProgramDefinition)();
        context.programDef->progType = GpuProgram::ST_FS;
        context.programDef->supportsSkeletalAnimation = false;
        context.programDef->supportsMorphAnimation = false;
        context.programDef->supportsPoseAnimation = 0;
        context.programDef->usesVertexTextureFetch = false;

        // Get name and language code
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Invalid fragment_program entry - expected 2 parameters."), context);
            return true;
        }
        // Name, preserve case
        StrConv::conv(vecparams[0], context.programDef->name);
        // language code, make lower case
        StrConv::conv(vecparams[1], context.programDef->language);
        //StrUtil::toLower(context.programDef->language);

        // Return TRUE because this must be followed by a {
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseProgramSource(String & params, MaterialScriptContext & context)
    {
        // Source filename, preserve case
        context.programDef->source = params;
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramSkeletalAnimation(String & params, MaterialScriptContext & context)
    {
        // Source filename, preserve case
        StrConv::conv(params, context.programDef->supportsSkeletalAnimation);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramMorphAnimation(String & params, MaterialScriptContext & context)
    {
        // Source filename, preserve case
        StrConv::conv(params, context.programDef->supportsMorphAnimation);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramPoseAnimation(String & params, MaterialScriptContext & context)
    {
        NIIi temp;
        StrConv::conv(params, temp);
        // Source filename, preserve case
        context.programDef->supportsPoseAnimation = (Nui16)temp;
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramVertexTextureFetch(String & params, MaterialScriptContext & context)
    {
        // Source filename, preserve case
        StrConv::conv(params, context.programDef->usesVertexTextureFetch);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramSyntax(String & params, MaterialScriptContext & context)
    {
        // Syntax code, make lower case
        StrUtil::toLower(params);
        StrConv::conv(params, context.programDef->syntax);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseProgramCustomParameter(String & params, MaterialScriptContext & context)
    {
        // This params object does not have the command stripped
        // Lower case the command, but not the value incase it's relevant
        // Split only up to first delimiter, program deals with the rest
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"), 1);
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Invalid custom program parameter entry; there must be a parameter name and at least one value."),
                context);
            return false;
        }

        context.programDef->customParameters.push_back(std::pair<String, String>(vecparams[0], vecparams[1]));

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseTextureSource(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));
        if (vecparams.size() != 1)
            logParseError(_I18n("Invalid texture source attribute - expected 1 parameter."),context);
        //The only param should identify which TextureFactory is needed
        N_Only(Texture).setPlugin(vecparams[0]);

        if(N_Only(Texture).getPlugin() != 0)
        {
            //String tps;
            //tps = StrConv::conv( context.techLev ) + _T(" ") + 
            //    StrConv::conv( context.passLev ) + _T(" ") + 
           //     StrConv::conv( context.stateLev);

            N_Only(Texture).getPlugin()->setShaderFusion(context.techLev);
            N_Only(Texture).getPlugin()->setShaderCh(context.passLev);
            N_Only(Texture).getPlugin()->setRenderLevel(context.stateLev);
        }

        // update section
        context.section = MSS_TEXTURESOURCE;
        // Return TRUE because this must be followed by a {
        return true;
    }

    //-----------------------------------------------------------------------
    bool parseTextureCustomParameter(String & params, MaterialScriptContext & context)
    {
        // This params object does not have the command stripped
        // Split only up to first delimiter, program deals with the rest
        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"), 1);
        if (vecparams.size() != 2)
        {
            logParseError(_I18n("Invalid texture parameter entry;there must be a parameter name and at least one value."), context);
            return false;
        }

        if(N_Only(Texture).getPlugin() != 0)
            ////First is command, next could be a string with one or more values
            N_Only(Texture).getPlugin()->set(vecparams[0], vecparams[1]);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseReceiveShadows(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.material->setReceiveShadow(true);
        else if (params == _T("off"))
            context.material->setReceiveShadow(false);
        else
            logParseError(_I18n("Bad receive_shadows attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseDefaultParams(String & params, MaterialScriptContext & context)
    {
        context.section = MSS_DEFAULT_PARAMETERS;
        // Should be a brace next
        return true;
    }
    //-----------------------------------------------------------------------
    bool parseTransparencyCastsShadows(String & params, MaterialScriptContext & context)
    {
        StrUtil::toLower(params);
        if (params == _T("on"))
            context.material->setAlphaShadowCast(true);
        else if (params == _T("off"))
            context.material->setAlphaShadowCast(false);
        else
            logParseError(_I18n("Bad transparency_casts_shadows attribute, valid parameters are 'on' or 'off'."), context);

        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLodStrategy(String & params, MaterialScriptContext & context)
    {
        Ni32 tempid;
        StrConv::conv(params, tempid);

        MaterialLodScheme * scheme = static_cast<MaterialLodScheme *>
            (N_Only(LodScheme).create(N_LOD_Material, tempid));

        if(scheme == 0)
            logParseError(_I18n("Bad lod_strategy attribute, available lod scheme name expected."), context);

        context.material->setLodScheme(scheme);
        return false;
    }
    //-----------------------------------------------------------------------
    bool parseLodDistances(String & params, MaterialScriptContext & context)
    {
        // Set to distance strategy
        context.material->setLodScheme(static_cast<MaterialLodScheme *>
            (N_Only(LodScheme).createDefault(N_LOD_Material)));

        StringList vecparams;
        StrUtil::split(params, vecparams, _T(" \t"));

        MaterialLodScheme * scheme = context.material->getLodScheme();
        scheme->removeAll();
        StringList::iterator i, iend = vecparams.end();
        for(i = vecparams.begin(); i != iend; ++i)
        {
            NIIf temp;
            StrConv::conv(*i, temp);
            scheme->add(temp);
        }

        return false;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // MaterialSerializer
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    MaterialSerializer::MaterialSerializer()
    {
        // Set up root attribute parsers
        mRootAttribParsers.insert(AttribParserList::value_type(_T("material"), (ATTRIBUTE_PARSER)parseMaterial));
        mRootAttribParsers.insert(AttribParserList::value_type(_T("vertex_program"), (ATTRIBUTE_PARSER)parseVertexProgram));
        mRootAttribParsers.insert(AttribParserList::value_type(_T("geometry_program"), (ATTRIBUTE_PARSER)parseGeometryProgram));
        mRootAttribParsers.insert(AttribParserList::value_type(_T("fragment_program"), (ATTRIBUTE_PARSER)parseFragmentProgram));

        // Set up material attribute parsers
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("lod_values"), (ATTRIBUTE_PARSER)parseLodValues));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("lod_strategy"), (ATTRIBUTE_PARSER)parseLodStrategy));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("lod_distances"), (ATTRIBUTE_PARSER)parseLodDistances));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("receive_shadows"), (ATTRIBUTE_PARSER)parseReceiveShadows));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("transparency_casts_shadows"), (ATTRIBUTE_PARSER)parseTransparencyCastsShadows));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("technique"), (ATTRIBUTE_PARSER)parseTechnique));
        mMaterialAttribParsers.insert(AttribParserList::value_type(_T("set_texture_alias"), (ATTRIBUTE_PARSER)parseSetTextureAlias));

        // Set up technique attribute parsers
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("lod_index"), (ATTRIBUTE_PARSER)parseLodIndex));
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("shadow_caster_material"), (ATTRIBUTE_PARSER)parseShadowCasterMaterial));
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("shadow_receiver_material"), (ATTRIBUTE_PARSER)parseShadowReceiverMaterial));
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("scheme"), (ATTRIBUTE_PARSER)parseScheme));
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("gpu_vendor_rule"), (ATTRIBUTE_PARSER)parseGPUVendorRule));
        mTechniqueAttribParsers.insert(AttribParserList::value_type(_T("pass"), (ATTRIBUTE_PARSER)parsePass));

        // Set up pass attribute parsers
        mPassAttribParsers.insert(AttribParserList::value_type(_T("ambient"), (ATTRIBUTE_PARSER)parseAmbient));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("diffuse"), (ATTRIBUTE_PARSER)parseDiffuse));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("specular"), (ATTRIBUTE_PARSER)parseSpecular));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("emissive"), (ATTRIBUTE_PARSER)parseEmissive));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("scene_blend"), (ATTRIBUTE_PARSER)parseSceneBlend));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("separate_scene_blend"), (ATTRIBUTE_PARSER)parseSeparateSceneBlend));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("depth_check"), (ATTRIBUTE_PARSER)parseDepthCheck));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("depth_write"), (ATTRIBUTE_PARSER)parseDepthWrite));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("depth_func"), (ATTRIBUTE_PARSER)parseDepthFunc));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("normalise_normals"), (ATTRIBUTE_PARSER)parseNormaliseNormals));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("alpha_rejection"), (ATTRIBUTE_PARSER)parseAlphaRejection));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("alpha_to_coverage"), (ATTRIBUTE_PARSER)parseAlphaToCoverage));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("transparent_sorting"), (ATTRIBUTE_PARSER)parseTransparentSorting));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("colour_write"), (ATTRIBUTE_PARSER)parseColourWrite));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("light_scissor"), (ATTRIBUTE_PARSER)parseLightScissor));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("light_clip_planes"), (ATTRIBUTE_PARSER)parseLightClip));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("cull_hardware"), (ATTRIBUTE_PARSER)parseCullHardware));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("cull_software"), (ATTRIBUTE_PARSER)parseCullSoftware));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("lighting"), (ATTRIBUTE_PARSER)parseLighting));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("fog_override"), (ATTRIBUTE_PARSER)parseFogging));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("shading"), (ATTRIBUTE_PARSER)parseShading));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("polygon_mode"), (ATTRIBUTE_PARSER)parsePolygonMode));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("polygon_mode_overrideable"), (ATTRIBUTE_PARSER)parsePolygonModeOverrideable));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("depth_bias"), (ATTRIBUTE_PARSER)parseDepthBias));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("iteration_depth_bias"), (ATTRIBUTE_PARSER)parseIterationDepthBias));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("texture_unit"), (ATTRIBUTE_PARSER)parseTextureUnit));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("vertex_program_ref"), (ATTRIBUTE_PARSER)parseVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("geometry_program_ref"), (ATTRIBUTE_PARSER)parseGeometryProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("shadow_caster_vertex_program_ref"), (ATTRIBUTE_PARSER)parseShadowCasterVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("shadow_caster_fragment_program_ref"), (ATTRIBUTE_PARSER)parseShadowCasterFragmentProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("shadow_receiver_vertex_program_ref"), (ATTRIBUTE_PARSER)parseShadowReceiverVertexProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("shadow_receiver_fragment_program_ref"), (ATTRIBUTE_PARSER)parseShadowReceiverFragmentProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("fragment_program_ref"), (ATTRIBUTE_PARSER)parseFragmentProgramRef));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("max_lights"), (ATTRIBUTE_PARSER)parseMaxLights));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("start_light"), (ATTRIBUTE_PARSER)parseStartLight));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("iteration"), (ATTRIBUTE_PARSER)parseIteration));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("point_size"), (ATTRIBUTE_PARSER)parsePointSize));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("point_sprites"), (ATTRIBUTE_PARSER)parsePointSprites));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("point_size_attenuation"), (ATTRIBUTE_PARSER)parsePointAttenuation));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("point_size_min"), (ATTRIBUTE_PARSER)parsePointSizeMin));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("point_size_max"), (ATTRIBUTE_PARSER)parsePointSizeMax));
        mPassAttribParsers.insert(AttribParserList::value_type(_T("illumination_stage"), (ATTRIBUTE_PARSER)parseIlluminationStage));

        // Set up texture unit attribute parsers
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("texture_source"), (ATTRIBUTE_PARSER)parseTextureSource));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("texture"), (ATTRIBUTE_PARSER)parseTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("anim_texture"), (ATTRIBUTE_PARSER)parseAnimTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("cubic_texture"), (ATTRIBUTE_PARSER)parseCubicTexture));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("binding_type"), (ATTRIBUTE_PARSER)parseBindingType));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("tex_coord_set"), (ATTRIBUTE_PARSER)parseTexCoord));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("tex_address_mode"), (ATTRIBUTE_PARSER)parseTexAddressMode));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("tex_border_colour"), (ATTRIBUTE_PARSER)parseTexBorderColour));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("colour_op"), (ATTRIBUTE_PARSER)parseColourOp));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("colour_op_ex"), (ATTRIBUTE_PARSER)parseColourOpEx));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("colour_op_multipass_fallback"), (ATTRIBUTE_PARSER)parseColourOpFallback));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("alpha_op_ex"), (ATTRIBUTE_PARSER)parseAlphaOpEx));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("env_map"), (ATTRIBUTE_PARSER)parseEnvMap));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("scroll"), (ATTRIBUTE_PARSER)parseScroll));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("scroll_anim"), (ATTRIBUTE_PARSER)parseScrollAnim));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("rotate"), (ATTRIBUTE_PARSER)parseRotate));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("rotate_anim"), (ATTRIBUTE_PARSER)parseRotateAnim));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("scale"), (ATTRIBUTE_PARSER)parseScale));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("wave_xform"), (ATTRIBUTE_PARSER)parseWaveXform));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("transform"), (ATTRIBUTE_PARSER)parseTransform));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("filtering"), (ATTRIBUTE_PARSER)parseFiltering));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("max_anisotropy"), (ATTRIBUTE_PARSER)parseAnisotropy));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("mipmap_bias"), (ATTRIBUTE_PARSER)parseMipmapBias));
        mTextureUnitAttribParsers.insert(AttribParserList::value_type(_T("content_type"), (ATTRIBUTE_PARSER)parseContentType));

        // Set up program reference attribute parsers
        mProgramRefAttribParsers.insert(AttribParserList::value_type(_T("param_indexed"), (ATTRIBUTE_PARSER)parseParamIndexed));
        mProgramRefAttribParsers.insert(AttribParserList::value_type(_T("param_indexed_auto"), (ATTRIBUTE_PARSER)parseParamIndexedAuto));
        mProgramRefAttribParsers.insert(AttribParserList::value_type(_T("param_named"), (ATTRIBUTE_PARSER)parseParamNamed));
        mProgramRefAttribParsers.insert(AttribParserList::value_type(_T("param_named_auto"), (ATTRIBUTE_PARSER)parseParamNamedAuto));

        // Set up program definition attribute parsers
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("source"), (ATTRIBUTE_PARSER)parseProgramSource));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("syntax"), (ATTRIBUTE_PARSER)parseProgramSyntax));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("includes_skeletal_animation"), (ATTRIBUTE_PARSER)parseProgramSkeletalAnimation));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("includes_morph_animation"), (ATTRIBUTE_PARSER)parseProgramMorphAnimation));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("includes_pose_animation"), (ATTRIBUTE_PARSER)parseProgramPoseAnimation));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("uses_vertex_texture_fetch"), (ATTRIBUTE_PARSER)parseProgramVertexTextureFetch));
        mProgramAttribParsers.insert(AttribParserList::value_type(_T("default_params"), (ATTRIBUTE_PARSER)parseDefaultParams));

        // Set up program default param attribute parsers
        mProgramDefaultParamAttribParsers.insert(AttribParserList::value_type(_T("param_indexed"), (ATTRIBUTE_PARSER)parseParamIndexed));
        mProgramDefaultParamAttribParsers.insert(AttribParserList::value_type(_T("param_indexed_auto"), (ATTRIBUTE_PARSER)parseParamIndexedAuto));
        mProgramDefaultParamAttribParsers.insert(AttribParserList::value_type(_T("param_named"), (ATTRIBUTE_PARSER)parseParamNamed));
        mProgramDefaultParamAttribParsers.insert(AttribParserList::value_type(_T("param_named_auto"), (ATTRIBUTE_PARSER)parseParamNamedAuto));

        mScriptContext.section = MSS_NONE;
        mScriptContext.material = 0;
        mScriptContext.technique = 0;
        mScriptContext.pass = 0;
        mScriptContext.textureUnit = 0;
        mScriptContext.program = 0;
        mScriptContext.lineNo = 0;
        mScriptContext.filename.clear();
        mScriptContext.techLev = -1;
        mScriptContext.passLev = -1;
        mScriptContext.stateLev = -1;

        mBuffer.clear();
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::parseScript(DataStream * stream, void * out, GroupID gid)
    {
        String line;
        bool nextIsOpenBrace = false;

        mScriptContext.section = MSS_NONE;
        mScriptContext.material = 0;
        mScriptContext.technique = 0;
        mScriptContext.pass = 0;
        mScriptContext.textureUnit = 0;
        mScriptContext.program = 0;
        mScriptContext.lineNo = 0;
        mScriptContext.techLev = -1;
        mScriptContext.passLev = -1;
        mScriptContext.stateLev = -1;
        mScriptContext.filename = stream->getName();
        mScriptContext.groupName = gid;
        while(!stream->end())
        {
            line = stream->readLine();
            mScriptContext.lineNo++;

            // DEBUG LINE
            // N_Only(Log).log("About to attempt line(#" +
            //    StrConv::conv(mScriptContext.lineNo) + "): " + line);

            // Ignore comments & blanks
            if (!(line.length() == 0 || line.substr(0,2) == _T("//")))
            {
                if (nextIsOpenBrace)
                {
                    // NB, parser will have changed context already
                    if (line != _T("{"))
                    {
                        logParseError(_I18n("Expecting '{' but got ") + line + _I18n(" instead."), mScriptContext);
                    }
                    nextIsOpenBrace = false;
                }
                else
                {
                    nextIsOpenBrace = parseScriptLine(line);
                }

            }
        }

        // Check all braces were closed
        if (mScriptContext.section != MSS_NONE)
        {
            logParseError(_I18n("Unexpected end of file."), mScriptContext);
        }

        // Make sure we invalidate our context shared pointer (don't wanna hold on)
        mScriptContext.material = 0;
    }
    //-----------------------------------------------------------------------
    bool MaterialSerializer::parseScriptLine(String & line)
    {
        switch(mScriptContext.section)
        {
        case MSS_NONE:
            if (line == _T("}"))
            {
                logParseError(_I18n("Unexpected terminating brace."), mScriptContext);
                return false;
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mRootAttribParsers);
            }
            break;
        case MSS_MATERIAL:
            if (line == _T("}"))
            {
                // End of material
                // if texture aliases were found, pass them to the material
                // to update texture names used in Texture unit states
                //if (!mScriptContext.textureAliases.empty())
                //{
                    // request material to update all texture names in TUS's
                    // that use texture aliases in the list
                //    mScriptContext.material->applyTextureAliases(mScriptContext.textureAliases);
                //}

                mScriptContext.section = MSS_NONE;
                mScriptContext.material = 0;
                //Reset all levels for next material
                mScriptContext.passLev = -1;
                mScriptContext.stateLev= -1;
                mScriptContext.techLev = -1;
                mScriptContext.textureAliases.clear();
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mMaterialAttribParsers);
            }
            break;
        case MSS_TECHNIQUE:
            if (line == _T("}"))
            {
                // End of technique
                mScriptContext.section = MSS_MATERIAL;
                mScriptContext.technique = NULL;
                mScriptContext.passLev = -1;    //Reset pass level (yes, the pass level)
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mTechniqueAttribParsers);
            }
            break;
        case MSS_PASS:
            if (line == _T("}"))
            {
                // End of pass
                mScriptContext.section = MSS_TECHNIQUE;
                mScriptContext.pass = NULL;
                mScriptContext.stateLev = -1;    //Reset state level (yes, the state level)
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mPassAttribParsers);
            }
            break;
        case MSS_TEXTUREUNIT:
            if (line == _T("}"))
            {
                // End of texture unit
                mScriptContext.section = MSS_PASS;
                mScriptContext.textureUnit = NULL;
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mTextureUnitAttribParsers);
            }
            break;
        case MSS_TEXTURESOURCE:
            if(line == _T("}"))
            {
                //End texture source section
                //Finish creating texture here
                ResourceID sMaterialName = mScriptContext.material->getOriginID();
                if(N_Only(Texture).getPlugin() != 0)
                    N_Only(Texture).getPlugin()->createTexture(sMaterialName, mScriptContext.groupName);
                //Revert back to texture unit
                mScriptContext.section = MSS_TEXTUREUNIT;
            }
            else
            {
                // custom texture parameter, use original line
                parseTextureCustomParameter(line, mScriptContext);
            }
            break;
        case MSS_PROGRAM_REF:
            if (line == _T("}"))
            {
                // End of program
                mScriptContext.section = MSS_PASS;
                mScriptContext.program = 0;
            }
            else
            {
                // find & invoke a parser
                return invokeParser(line, mProgramRefAttribParsers);
            }
            break;
        case MSS_PROGRAM:
            // Program definitions are slightly different, they are deferred
            // until all the information required is known
            if (line == _T("}"))
            {
                // End of program
                finishProgramDefinition();
                mScriptContext.section = MSS_NONE;
                N_delete_t(mScriptContext.programDef, MaterialScriptProgramDefinition);
                mScriptContext.defaultParamLines.clear();
                mScriptContext.programDef = NULL;
            }
            else
            {
                // find & invoke a parser
                // do this manually because we want to call a custom
                // routine when the parser is not found
                // First, split line on first divisor only
                StringList splitCmd;
                StrUtil::split(line, splitCmd, _T(" \t"), 1);
                // Find attribute parser
                AttribParserList::iterator iparser = mProgramAttribParsers.find(splitCmd[0]);
                if (iparser == mProgramAttribParsers.end())
                {
                    // custom parameter, use original line
                    parseProgramCustomParameter(line, mScriptContext);
                }
                else
                {
                    String cmd = splitCmd.size() >= 2? splitCmd[1]:StrUtil::WBLANK;
                    // Use parser with remainder
                    return iparser->second(cmd, mScriptContext );
                }
            }
            break;
        case MSS_DEFAULT_PARAMETERS:
            if (line == _T("}"))
            {
                // End of default parameters
                mScriptContext.section = MSS_PROGRAM;
            }
            else
            {
                // Save default parameter lines up until we finalise the program
                mScriptContext.defaultParamLines.push_back(line);
            }
            break;
        };

        return false;
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::finishProgramDefinition(void)
    {
        // Now it is time to create the program and propagate the parameters
        MaterialScriptProgramDefinition * def = mScriptContext.programDef;
        GpuProgram * gp;
        if(def->language == SL_ASM)
        {
            // Native assembler
            // Validate
            if(def->source.empty())
            {
                logParseError(_I18n("Invalid program definition for ") + N_conv(def->name) +
                    _I18n(", you must specify a source file."), mScriptContext);
            }
            if(def->syntax == 0)
            {
                logParseError(_I18n("Invalid program definition for ") + N_conv(def->name) +
                    _I18n(", you must specify a syntax code."), mScriptContext);
            }
            // Create
            gp = N_Only(GpuProgram).create(def->name, mScriptContext.groupName,
                def->source, def->progType, def->syntax);
        }
        else
        {
            // High-level program
            // Validate
            if(def->source.empty() && def->language != SL_ALTER)
            {
                logParseError(_I18n("Invalid program definition for ") + N_conv(def->name) +
                    _I18n(", you must specify a source file."), mScriptContext);
            }
            // Create
            try
            {
                HighLevelGpuProgram * hgp = N_Only(HighLevelGpuProgram).
                    create(def->name, mScriptContext.groupName, def->language, def->progType);

                // Assign to generalised version
                gp = hgp;
                // Set source file
                hgp->setProgramSrc(def->source);

                // Set custom parameters
                vector<std::pair<String, String> >::type::const_iterator i, iend;
                iend = def->customParameters.end();
                for (i = def->customParameters.begin(); i != iend; ++i)
                {
                    if (!hgp->set(i->first, i->second))
                    {
                        logParseError(_I18n("Error in program ") + N_conv(def->name) +
                            _I18n(" parameter ") + i->first + _I18n(" is not valid."), mScriptContext);
                    }
                }
            }
            catch(Exception & e)
            {
                logParseError(_I18n("Could not create GPU program '") + N_conv(def->name) +
                    _I18n("', error reported was: ") + e.getVerbose(), mScriptContext);
                mScriptContext.program = 0;
                mScriptContext.programParams = 0;
                return;
            }
        }
        // Set skeletal animation option
        gp->setVertexMatrixFusion(def->supportsSkeletalAnimation);
        // Set morph animation option
        gp->setVertexInterpolation(def->supportsMorphAnimation);
        // Set pose animation option
        gp->setVertexOffsetCount(def->supportsPoseAnimation);
        // Set vertex texture usage
        gp->setVertexTextureFetch(def->usesVertexTextureFetch);
        // set origin
        gp->setSrc(mScriptContext.filename);

        // Set up to receive default parameters
        if(gp->isValid() && !mScriptContext.defaultParamLines.empty())
        {
            mScriptContext.programParams = gp->createDefaultParam();
            mScriptContext.numAnimationParametrics = 0;
            mScriptContext.program = gp;
            StringList::iterator i, iend;
            iend = mScriptContext.defaultParamLines.end();
            for (i = mScriptContext.defaultParamLines.begin(); i != iend; ++i)
            {
                // find & invoke a parser
                // do this manually because we want to call a custom
                // routine when the parser is not found
                // First, split line on first divisor only
                StringList splitCmd;
                StrUtil::split(*i, splitCmd, _T(" \t"), 1);
                // Find attribute parser
                AttribParserList::iterator iparser = mProgramDefaultParamAttribParsers.find(splitCmd[0]);
                if (iparser != mProgramDefaultParamAttribParsers.end())
                {
                    String cmd = splitCmd.size() >= 2? splitCmd[1]:StrUtil::WBLANK;
                    // Use parser with remainder
                    iparser->second(cmd, mScriptContext );
                }

            }
            // Reset
            mScriptContext.program = 0;
            mScriptContext.programParams = 0;
        }
    }
    //-----------------------------------------------------------------------
    bool MaterialSerializer::invokeParser(String & line, AttribParserList & parsers)
    {
        // First, split line on first divisor only
        StringList splitCmd;
        StrUtil::split(line, splitCmd, _T(" \t"), 1);

        // Find attribute parser
        AttribParserList::iterator iparser = parsers.find(splitCmd[0]);
        if (iparser == parsers.end())
        {
            // BAD command. BAD!
            logParseError(_I18n("Unrecognised command: ") + splitCmd[0], mScriptContext);
            return false;
        }
        else
        {
            String cmd;
            if(splitCmd.size() >= 2)
                cmd = splitCmd[1];
            // Use parser, make sure we have 2 params before using splitCmd[1]
            return iparser->second( cmd, mScriptContext );
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::exportMaterial(const Material * pMat, const String & fileName,
        bool exportDefaults, const bool includeProgDef, const String & programFilename,
            const String & materialName)
    {
        clearQueue();
        mDefaults = exportDefaults;
        writeMaterial(pMat, materialName);
        exportQueued(fileName, includeProgDef, programFilename);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::exportQueued(const String & fileName, const bool includeProgDef,
        const String & programFilename)
    {
        // write out gpu program definitions to the buffer
        writeGpuPrograms();

        if (mBuffer.empty())
            N_EXCEPT(InvalidParam, _I18n("Queue is empty !"));

        N_Only(Log).log(_I18n("MaterialSerializer : writing material(s) to material script : ")
            + fileName, LL_Fatal);

        FILE * fp;
        fp = Nfopen(fileName.c_str(), _T("w"));
        if (!fp)
            N_EXCEPT(IO, _I18n("Cannot create material file."));

        // output gpu program definitions to material script file if includeProgDef is true
        if (includeProgDef && !mGpuProgramBuffer.empty())
        {
            Nfputs(mGpuProgramBuffer.c_str(), fp);
        }

        // output main buffer holding material script
        Nfputs(mBuffer.c_str(), fp);
        fclose(fp);

        // write program script if program filename and program definitions
        // were not included in material script
        if (!includeProgDef && !mGpuProgramBuffer.empty() && !programFilename.empty())
        {
            FILE * fp;
            fp = Nfopen(programFilename.c_str(), _T("w"));
            if (!fp)
                N_EXCEPT(IO, _I18n("Cannot create program material file."));
            Nfputs(mGpuProgramBuffer.c_str(), fp);
            fclose(fp);
        }

        N_Only(Log).log(_I18n("MaterialSerializer : done."), LL_Fatal);
        clearQueue();
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::queueForExport(const Material * pMat, bool clearQueued,
        bool exportDefaults, const String & materialName)
    {
        if (clearQueued)
            clearQueue();

        mDefaults = exportDefaults;
        writeMaterial(pMat, materialName);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::clearQueue()
    {
        mBuffer.clear();
        mGpuProgramBuffer.clear();
        mGpuProgramDefinitionContainer.clear();
    }
    //-----------------------------------------------------------------------
    const String &MaterialSerializer::getQueuedAsString() const
    {
        return mBuffer;
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeMaterial(const Material * pMat, const String & materialName)
    {
        String outMaterialName;

        if (materialName.length() > 0)
        {
            outMaterialName = materialName;
        }
        else
        {
            outMaterialName = pMat->getOriginID();
        }

        N_Only(Log).log(_I18n("MaterialSerializer : writing material ") +
            outMaterialName + _I18n(" to queue."), LL_Fatal);

        bool skipWriting = false;

        // Fire pre-write event.
        fireMaterialEvent(MSE_PRE_WRITE, skipWriting, pMat);
        if (skipWriting)
            return;

        // Material name
        writeAttribute(0, _T("material"));
        writeValue(quoteWord(outMaterialName));
        beginSection(0);
        {
            // Fire write begin event.
            fireMaterialEvent(MSE_WRITE_BEGIN, skipWriting, pMat);
            String attributeVal;

            // Write LOD information
            const MaterialLodScheme::LodList & valueIt = pMat->getLodScheme()->getList();
            MaterialLodScheme::LodList::const_iterator m = valueIt.begin();
            MaterialLodScheme::LodList::const_iterator mend = valueIt.end();

            if(m != mend)
            {
                for(++m; m != mend; ++m)
                {
                    attributeVal.append(N_conv(m->first));
                    if((m + 1) != mend)
                        attributeVal.append(_T(" "));
                }
            }

            if(!attributeVal.empty())
            {
                writeAttribute(1, _T("lod_values"));
                writeValue(attributeVal);
            }

            // Shadow receive
            if(mDefaults || pMat->isReceiveShadow() != true)
            {
                writeAttribute(1, _T("receive_shadows"));
                writeValue(pMat->isReceiveShadow() ? _T("on") : _T("off"));
            }

            // When rendering shadows, treat transparent things as opaque?
            if(mDefaults || pMat->isAlphaShadowCast() == true)
            {
                writeAttribute(1, _T("transparency_casts_shadows"));
                writeValue(pMat->isAlphaShadowCast() ? _T("on") : _T("off"));
            }

            // Iterate over techniques
            ShaderFusionList::const_iterator i, iend = pMat->getList().end();
            for(i = pMat->getList().begin(); i != iend; ++i)
            {
                writeTechnique(*i);
                mBuffer += _T("\n");
            }

            // Fire write end event.
            fireMaterialEvent(MSE_WRITE_END, skipWriting, pMat);
        }
        endSection(0);
        mBuffer += _T("\n");

        // Fire post section write event.
        fireMaterialEvent(MSE_POST_WRITE, skipWriting, pMat);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeTechnique(const ShaderFusion * pTech)
    {
        bool skipWriting = false;

        // Fire pre-write event.
        fireTechniqueEvent(MSE_PRE_WRITE, skipWriting, pTech);
        if(skipWriting)
            return;

        // ShaderFusion header
        writeAttribute(1, _T("technique"));
        // only output technique name if it exists.
        if(!pTech->getName().empty())
            writeValue(quoteWord(pTech->getName()));

        beginSection(1);
        {
            // Fire write begin event.
            fireTechniqueEvent(MSE_WRITE_BEGIN, skipWriting, pTech);

            // Lod index
            if(mDefaults || pTech->getLodIndex() != 0)
            {
                writeAttribute(2, _T("lod_index"));
                writeValue(N_conv(pTech->getLodIndex()));
            }

            // Scheme name
            if(mDefaults || pTech->getSchemeID() != 0)
            {
                writeAttribute(2, _T("scheme"));
                writeValue(quoteWord(N_conv(pTech->getSchemeID())));
            }

            // ShadowCasterMaterial name
            if(pTech->getShadowCaster() != 0)
            {
                writeAttribute(2, _T("shadow_caster_material"));
                writeValue(quoteWord(N_conv(pTech->getShadowCaster()->getOriginID())));
            }
            // ShadowReceiverMaterial name
            if(pTech->getShadowReceiver() != 0)
            {
                writeAttribute(2, _T("shadow_receiver_material"));
                writeValue(quoteWord(N_conv(pTech->getShadowReceiver()->getOriginID())));
            }
            // GPU vendor rules
            GpuVendorDetailList::const_iterator i, iend = pTech->getSupportVendor().end();
            for(i = pTech->getSupportVendor().begin(); i != iend; ++i)
            {
                const GpuVendorDetail & rule = *i;
                writeAttribute(2, _T("gpu_vendor_rule"));
                writeValue(quoteWord(GpuVendorDetail::toStr(rule.mVendor)));
                writeValue(quoteWord(rule.mSer));
                writeValue(N_conv(rule.mNum));
            }

            // Iterate over passes
            ShaderChList::const_iterator j, jend = pTech->getShaderChList().end();
            for(j = pTech->getShaderChList().begin(); j != jend; ++j)
            {
                writePass(*j);
                mBuffer += _T("\n");
            }

            // Fire write end event.
            fireTechniqueEvent(MSE_WRITE_END, skipWriting, pTech);
        }
        endSection(1);

        // Fire post section write event.
        fireTechniqueEvent(MSE_POST_WRITE, skipWriting, pTech);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writePass(const ShaderCh * pPass)
    {
        bool skipWriting = false;

        // Fire pre-write event.
        firePassEvent(MSE_PRE_WRITE, skipWriting, pPass);
        if (skipWriting)
            return;

        writeAttribute(2, _T("pass"));
        // only output pass name if its not the default name
        if (pPass->getName() != N_conv(pPass->getIndex()))
            writeValue(quoteWord(pPass->getName()));

        beginSection(2);
        {
            // Fire write begin event.
            firePassEvent(MSE_WRITE_BEGIN, skipWriting, pPass);

            //lighting
            if(mDefaults || pPass->isLight() != true)
            {
                writeAttribute(3, _T("lighting"));
                writeValue(pPass->isLight() ? _T("on") : _T("off"));
            }
            // max_lights
            if(mDefaults || pPass->getLightMax() != NII_MAX_LIGHT)
            {
                writeAttribute(3, _T("max_lights"));
                writeValue(N_conv(pPass->getLightMax()));
            }
            // start_light
            if(mDefaults || pPass->getLightBegin() != 0)
            {
                writeAttribute(3, _T("start_light"));
                writeValue(N_conv(pPass->getLightBegin()));
            }
            // iteration
            if (mDefaults || pPass->isEachLight() || (pPass->getRenderCount() > 1))
            {
                writeAttribute(3, _T("iteration"));
                // pass iteration count
                if (pPass->getRenderCount() > 1 || pPass->getLightRenderLimit() > 1)
                {
                    writeValue(N_conv(pPass->getRenderCount()));
                    if (pPass->isEachLight())
                    {
                        if (pPass->getLightRenderLimit() > 1)
                        {
                            writeValue(_T("per_n_lights"));
                            writeValue(N_conv(pPass->getLightRenderLimit()));
                        }
                        else
                        {
                            writeValue(_T("per_light"));
                        }
                    }
                }
                else
                {
                    writeValue(pPass->isEachLight() ? _T("once_per_light") : _T("once"));
                }

                if(pPass->isEachLight() && (pPass->getLightTypeLimit() != LT_NONE))
                {
                    switch(pPass->getLightTypeLimit())
                    {
                    case LT_DIRECTIONAL:
                        writeValue(_T("directional"));
                        break;
                    case LT_POINT:
                        writeValue(_T("point"));
                        break;
                    case LT_SPOT:
                        writeValue(_T("spot"));
                        break;
                    };
                }
            }

            if(mDefaults || pPass->getLightMark() != 0xFFFFFFFF)
            {
                writeAttribute(3, _T("light_mask"));
                writeValue(N_conv(pPass->getLightMark()));
            }

            if (pPass->isLight())
            {
                Colour temp = pPass->getColour().getAmbient();
                // Ambient
                if(mDefaults || temp.r != 1 || temp.g != 1 || temp.b != 1 ||
                    temp.a != 1 || (pPass->getColour().getColourTracking() &
                    CTM_T_AMBIENT))
                {
                    writeAttribute(3, _T("ambient"));
                    if(pPass->getColour().getColourTracking() & CTM_T_AMBIENT)
                        writeValue(_T("vertexcolour"));
                    else
                        writeColourValue(temp, true);
                }
                temp = pPass->getColour().getDiffuse();
                // Diffuse
                if(mDefaults || temp.r != 1 || temp.g != 1 || temp.b != 1 ||
                    temp.a != 1 || (pPass->getColour().getColourTracking() &
                    CTM_T_DIFFUSE))
                {
                    writeAttribute(3, _T("diffuse"));
                    if (pPass->getColour().getColourTracking() & CTM_T_DIFFUSE)
                        writeValue(_T("vertexcolour"));
                    else
                        writeColourValue(temp, true);
                }
                temp = pPass->getColour().getSpecular();
                // Specular
                if(mDefaults || temp.r != 0 || temp.g != 0 || temp.b != 0 ||
                    temp.a != 1 || pPass->getColour().getShininess() != 0 ||
                    (pPass->getColour().getColourTracking() & CTM_T_SPECULAR))
                {
                    writeAttribute(3, _T("specular"));
                    if (pPass->getColour().getColourTracking() & CTM_T_SPECULAR)
                    {
                        writeValue(_T("vertexcolour"));
                    }
                    else
                    {
                        writeColourValue(temp, true);
                    }
                    writeValue(N_conv(pPass->getColour().getShininess()));
                }
                temp = pPass->getColour().getEmissive();
                // Emissive
                if(mDefaults || temp.r != 0 || temp.g != 0 || temp.b != 0 ||
                    temp.a != 1 || (pPass->getColour().getColourTracking() & CTM_T_EMISSIVE))
                {
                    writeAttribute(3, _T("emissive"));
                    if (pPass->getColour().getColourTracking() & CTM_T_EMISSIVE)
                        writeValue(_T("vertexcolour"));
                    else
                        writeColourValue(temp, true);
                }
            }

            // Point size
            if(mDefaults || pPass->getPoint().getSize() != 1.0)
            {
                writeAttribute(3, _T("point_size"));
                writeValue(N_conv(pPass->getPoint().getSize()));
            }

            // Point sprites
            if(mDefaults || pPass->getPoint().isSpritesEnable())
            {
                writeAttribute(3, _T("point_sprites"));
                writeValue(pPass->getPoint().isSpritesEnable() ? _T("on") : _T("off"));
            }

            // Point attenuation
            if(mDefaults || pPass->getPoint().isAttenuateEnable())
            {
                writeAttribute(3, _T("point_size_attenuation"));
                writeValue(pPass->getPoint().isAttenuateEnable() ? _T("on") : _T("off"));
                if (pPass->getPoint().isAttenuateEnable() &&
                    (pPass->getPoint().getConstant() != 0.0 ||
                     pPass->getPoint().getLinear() != 1.0 ||
                     pPass->getPoint().getQuadratic() != 0.0))
                {
                    writeValue(N_conv(pPass->getPoint().getConstant()));
                    writeValue(N_conv(pPass->getPoint().getLinear()));
                    writeValue(N_conv(pPass->getPoint().getQuadratic()));
                }
            }

            // Point min size
            if(mDefaults || pPass->getPoint().getMinSize() != 0.0)
            {
                writeAttribute(3, _T("point_size_min"));
                writeValue(N_conv(pPass->getPoint().getMinSize()));
            }

            // Point max size
            if(mDefaults || pPass->getPoint().getMaxSize() != 0.0)
            {
                writeAttribute(3, _T("point_size_max"));
                writeValue(N_conv(pPass->getPoint().getMaxSize()));
            }

            // scene blend factor
            if(pPass->getBlend().isPartAlpha())
            {
                if(mDefaults || pPass->getBlend().mSrcFactor != CF_ONE ||
                    pPass->getBlend().mDstFactor != CF_ZERO ||
                    pPass->getBlend().mSrcAlphaFactor != CF_ONE ||
                    pPass->getBlend().mDstAlphaFactor != CF_ZERO)
                {
                    writeAttribute(3, _T("separate_scene_blend"));
                    writeSceneBlendFactor(pPass->getBlend().mSrcFactor, pPass->getBlend().mDstFactor,
                        pPass->getBlend().mSrcAlphaFactor, pPass->getBlend().mDstAlphaFactor);
                }
            }
            else
            {
                if(mDefaults || pPass->getBlend().mSrcFactor != CF_ONE ||
                    pPass->getBlend().mDstFactor != CF_ZERO)
                {
                    writeAttribute(3, _T("scene_blend"));
                    writeSceneBlendFactor(pPass->getBlend().mSrcFactor, pPass->getBlend().mDstFactor);
                }
            }

            //depth check
            if(mDefaults || pPass->getStencil().isDepthEnable() != true)
            {
                writeAttribute(3, _T("depth_check"));
                writeValue(pPass->getStencil().isDepthEnable() ? _T("on") : _T("off"));
            }
            // alpha_rejection
            if(mDefaults || pPass->getColour().getAlphaReject() != CPM_ALWAYS_PASS ||
                pPass->getColour().getValue() != 0)
            {
                writeAttribute(3, _T("alpha_rejection"));
                writeCompareFunction(pPass->getColour().getAlphaReject());
                writeValue(N_conv(pPass->getColour().getValue()));
            }
            // alpha_to_coverage
            if(mDefaults || pPass->getColour().isAlphaCoverage())
            {
                writeAttribute(3, _T("alpha_to_coverage"));
                writeValue(pPass->getColour().isAlphaCoverage() ? _T("on") : _T("off"));
            }
            // transparent_sorting
            if(mDefaults || pPass->isDepthSort() != true)
            {
                writeAttribute(3, _T("transparent_sorting"));
                writeValue(pPass->isDepthSort() ? _T("on") : _T("off"));
            }

            //depth write
            if(mDefaults || pPass->getStencil().isDepthWrite() != true)
            {
                writeAttribute(3, _T("depth_write"));
                writeValue(pPass->getStencil().isDepthWrite() ? _T("on") : _T("off"));
            }

            //depth function
            if(mDefaults || pPass->getStencil().getDepthCompareFunc() != CPM_LESS_EQUAL)
            {
                writeAttribute(3, _T("depth_func"));
                writeCompareFunction(pPass->getStencil().getDepthCompareFunc());
            }

            //depth bias
            if(mDefaults || pPass->getStencil().getBiasConstant() != 0 ||
                pPass->getStencil().getBiasSlopeScale() != 0)
            {
                writeAttribute(3, _T("depth_bias"));
                writeValue(N_conv(pPass->getStencil().getBiasConstant()));
                writeValue(N_conv(pPass->getStencil().getBiasSlopeScale()));
            }
            //iteration depth bias
            if (mDefaults || pPass->getStencil().getBiasConstantFactor() != 0)
            {
                writeAttribute(3, _T("iteration_depth_bias"));
                writeValue(N_conv(pPass->getStencil().getBiasConstantFactor()));
            }

            //light scissor
            if (mDefaults || pPass->isLightClip() != false)
            {
                writeAttribute(3, _T("light_scissor"));
                writeValue(pPass->isLightClip() ? _T("on") : _T("off"));
            }

            //light clip planes
            if(mDefaults || pPass->isLightClipPlane() != false)
            {
                writeAttribute(3, _T("light_clip_planes"));
                writeValue(pPass->isLightClipPlane() ? _T("on") : _T("off"));
            }

            // illumination stage
            if(pPass->getShaderStep() != SS_Unknow)
            {
                writeAttribute(3, _T("illumination_stage"));
                switch(pPass->getShaderStep())
                {
                case SS_Global:
                    writeValue(_T("ambient"));
                    break;
                case SS_Detail_Light:
                    writeValue(_T("per_light"));
                    break;
                case SS_Decal:
                    writeValue(_T("decal"));
                    break;
                case SS_Unknow:
                    break;
                };
            }

            // hardware culling mode
            if(mDefaults || pPass->getCullingMode() != CM_Clockwise)
            {
                CullingMode hcm = pPass->getCullingMode();
                writeAttribute(3, _T("cull_hardware"));
                switch (hcm)
                {
                case CM_None :
                    writeValue(_T("none"));
                    break;
                case CM_Clockwise :
                    writeValue(_T("clockwise"));
                    break;
                case CM_Anticlockwise :
                    writeValue(_T("anticlockwise"));
                    break;
                }
            }

            // software culling mode
            if(mDefaults || pPass->getSysCullingMode() != SCM_BACK)
            {
                SysCullingMode scm = pPass->getSysCullingMode();
                writeAttribute(3, _T("cull_software"));
                switch (scm)
                {
                case SCM_NONE :
                    writeValue(_T("none"));
                    break;
                case SCM_BACK :
                    writeValue(_T("back"));
                    break;
                case SCM_FRONT :
                    writeValue(_T("front"));
                    break;
                }
            }

            //shading
            if(mDefaults || pPass->getShaderMode() != SM_GOURAUD)
            {
                writeAttribute(3, _T("shading"));
                switch(pPass->getShaderMode())
                {
                case SM_FLAT:
                    writeValue(_T("flat"));
                    break;
                case SM_GOURAUD:
                    writeValue(_T("gouraud"));
                    break;
                case SM_PHONG:
                    writeValue(_T("phong"));
                    break;
                }
            }

            if(mDefaults || pPass->getPolygonMode() != PM_SOLID)
            {
                writeAttribute(3, _T("polygon_mode"));
                switch (pPass->getPolygonMode())
                {
                case PM_POINTS:
                    writeValue(_T("points"));
                    break;
                case PM_LINE:
                    writeValue(_T("wireframe"));
                    break;
                case PM_SOLID:
                    writeValue(_T("solid"));
                    break;
                }
            }

            // polygon mode overrideable
            if(mDefaults || !pPass->getAutoReduce())
            {
                writeAttribute(3, _T("polygon_mode_overrideable"));
                writeValue(pPass->getAutoReduce() ? _T("on") : _T("off"));
            }

            // normalise normals
            if(mDefaults || pPass->isUnitNormals() != false)
            {
                writeAttribute(3, _T("normalise_normals"));
                writeValue(pPass->isUnitNormals() ? _T("on") : _T("off"));
            }

            //fog override
            if(mDefaults || pPass->getFog().mMode != FM_NONE)
            {
                writeAttribute(3, _T("fog_override"));
                writeValue(pPass->getFog().mMode != FM_NONE ? _T("true") : _T("false"));
                if(pPass->getFog().mMode != FM_NONE)
                {
                    switch(pPass->getFog().mMode)
                    {
                    case FM_NONE:
                        writeValue(_T("none"));
                        break;
                    case FM_LINEAR:
                        writeValue(_T("linear"));
                        break;
                    case FM_EXP2:
                        writeValue(_T("exp2"));
                        break;
                    case FM_EXP:
                        writeValue(_T("exp"));
                        break;
                    }

                    if(pPass->getFog().mMode != FM_NONE)
                    {
                        writeColourValue(pPass->getFog().mColour);
                        writeValue(N_conv(pPass->getFog().mDensity));
                        writeValue(N_conv(pPass->getFog().mBegin));
                        writeValue(N_conv(pPass->getFog().mEnd));
                    }
                }
            }

            // nfz

            //  GPU Vertex and Fragment program references and parameters
            if (pPass->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                writeVertexProgramRef(pPass);
            }

            if (pPass->getProgram().isExist(ShaderChProgram::ST_FS))
            {
                writeFragmentProgramRef(pPass);
            }

            if (pPass->getProgram().isExist(ShaderChProgram::ST_VS_SC))
            {
                writeShadowCasterVertexProgramRef(pPass);
            }

            if (pPass->getProgram().isExist(ShaderChProgram::ST_VS_SR))
            {
                writeShadowReceiverVertexProgramRef(pPass);
            }

            if (pPass->getProgram().isExist(ShaderChProgram::ST_FS_SR))
            {
                writeShadowReceiverFragmentProgramRef(pPass);
            }

            // Nested texture layers
            ShaderChTexture::Textures::const_iterator i, iend = pPass->getTexture().getTextures().end();
            for(i = pPass->getTexture().getTextures().begin(); i != iend; ++i)
                writeTextureUnit(*i, pPass);

            // Fire write end event.
            firePassEvent(MSE_WRITE_END, skipWriting, pPass);
        }
        endSection(2);
        // Fire post section write event.
        firePassEvent(MSE_POST_WRITE, skipWriting, pPass);
        N_Only(Log).log(_I18n("MaterialSerializer : done."), LL_Fatal);
    }
    //-----------------------------------------------------------------------
    String MaterialSerializer::convertFiltering(TextureFilterOP fo)
    {
        switch(fo)
        {
        case TFOP_NONE:
            return _T("none");
        case TFOP_POINT:
            return _T("point");
        case TFOP_LINEAR:
            return _T("linear");
        case TFOP_ANISOTROPIC:
            return _T("anisotropic");
        }
        return _T("point");
    }
    //-----------------------------------------------------------------------
    String convertTexAddressMode(TextureAddressingMode tam)
    {
        switch (tam)
        {
        case TAM_BORDER:
            return _T("border");
        case TAM_CLAMP:
            return _T("clamp");
        case TAM_MIRROR:
            return _T("mirror");
        case TAM_REPEAT:
            return _T("wrap");
        }
        return _T("wrap");
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeTextureUnit(const ShaderChTextureUnit *pTex, const ShaderCh * ch)
    {
        bool skipWriting = false;

        // Fire pre-write event.
        fireTextureUnitStateEvent(MSE_PRE_WRITE, skipWriting, pTex);
        if (skipWriting)
            return;

        N_Only(Log).log(_I18n("MaterialSerializer : parsing texture layer."), LL_Fatal);
        mBuffer += _T("\n");
        writeAttribute(3, _T("texture_unit"));
        // only write out name if its not equal to the default name
        if (pTex->getName() != N_conv(pTex->getLocalIndex(ch)))
            writeValue(quoteWord(pTex->getName()));

        beginSection(3);
        {
            // Fire write begin event.
            fireTextureUnitStateEvent(MSE_WRITE_BEGIN, skipWriting, pTex);

            //texture name
            if (pTex->getFrameCount() == 1 && pTex->getResourceID() != 0 && !pTex->isCubic())
            {
                writeAttribute(4, _T("texture"));
                writeValue(quoteWord(N_conv(pTex->getResourceID())));

                switch (pTex->getType())
                {
                case Texture::T_1D:
                    writeValue(_T("1d"));
                    break;
                case Texture::T_2D:
                    // nothing, this is the default
                    break;
                case Texture::T_3D:
                    writeValue(_T("3d"));
                    break;
                case Texture::T_CUBE:
                    // nothing, deal with this as cubic_texture since it copes with all variants
                    break;
                default:
                    break;
                };

                if(pTex->getMipmapCount() != -1)
                {
                    writeValue(N_conv(pTex->getMipmapCount()));
                }

                if(pTex->getIsAlpha())
                {
                    writeValue(_T("alpha"));
                }

                if(pTex->getFormat() != PF_UNKNOWN)
                {
                    writeValue(PixelUtil::getMarkName(pTex->getFormat()));
                }
            }

            //anim. texture
            if(pTex->getFrameCount() > 1 && !pTex->isCubic())
            {
                writeAttribute(4, _T("anim_texture"));
                for(NCount n = 0; n < pTex->getFrameCount(); ++n)
                    writeValue(quoteWord(N_conv(pTex->getFrame(n))));
                writeValue(N_conv(pTex->getAniTime()));
            }

            //cubic texture
            if(pTex->isCubic())
            {
                writeAttribute(4, _T("cubic_texture"));
                for(NCount n = 0; n < pTex->getFrameCount(); ++n)
                    writeValue(quoteWord(N_conv(pTex->getFrame(n))));

                //combinedUVW/separateUW
                if(pTex->getType() == Texture::T_CUBE)
                    writeValue(_T("combinedUVW"));
                else
                    writeValue(_T("separateUV"));
            }

            //anisotropy level
            if (mDefaults || pTex->getSample()->getAnisotropy() != 1)
            {
                writeAttribute(4, _T("max_anisotropy"));
                writeValue(N_conv(pTex->getSample()->getAnisotropy()));
            }

            //texture coordinate set
            if (mDefaults || pTex->getCoordSet() != 0)
            {
                writeAttribute(4, _T("tex_coord_set"));
                writeValue(N_conv(pTex->getCoordSet()));
            }

            //addressing mode
            const TextureAddressing & uvw = pTex->getSample()->getMode();
            if(mDefaults || uvw.mS_U != TAM_REPEAT ||
                uvw.mT_V != TAM_REPEAT || uvw.mR_W != TAM_REPEAT)
            {
                writeAttribute(4, _T("tex_address_mode"));
                if (uvw.mS_U == uvw.mT_V && uvw.mS_U == uvw.mR_W)
                {
                    writeValue(convertTexAddressMode(uvw.mS_U));
                }
                else
                {
                    writeValue(convertTexAddressMode(uvw.mS_U));
                    writeValue(convertTexAddressMode(uvw.mT_V));
                    if (uvw.mR_W != TAM_REPEAT)
                    {
                        writeValue(convertTexAddressMode(uvw.mR_W));
                    }
                }
            }

            //border colour
            const Colour & borderColour = pTex->getSample()->getBorder();
            if(mDefaults || borderColour != Colour::Black)
            {
                writeAttribute(4, _T("tex_border_colour"));
                writeColourValue(borderColour, true);
            }

            //filtering
            if(mDefaults ||
                pTex->getSample()->getMinFiltering() != TFOP_LINEAR ||
                pTex->getSample()->getMagFiltering() != TFOP_LINEAR ||
                pTex->getSample()->getMipFiltering() != TFOP_POINT)
            {
                writeAttribute(4, _T("filtering"));
                writeValue(convertFiltering(pTex->getSample()->getMinFiltering()) + _T(" ") +
                    convertFiltering(pTex->getSample()->getMagFiltering()) + _T(" ") +
                    convertFiltering(pTex->getSample()->getMipFiltering()));
            }

            // Mip biasing
            if(mDefaults || pTex->getSample()->getMipmapBias() != 0.0f)
            {
                writeAttribute(4, _T("mipmap_bias"));
                writeValue(N_conv(pTex->getSample()->getMipmapBias()));
            }

            // colour_op_ex
            if (mDefaults ||
                pTex->getColourBlend()->mOP != TBO_MODULATE ||
                pTex->getColourBlend()->mColourSrc1 != TBS_TEXTURE ||
                pTex->getColourBlend()->mColourSrc2 != TBS_PRE_TEXTURE)
            {
                writeAttribute(4, _T("colour_op_ex"));
                writeLayerBlendOperationEx(pTex->getColourBlend()->mOP);
                writeLayerBlendSource(pTex->getColourBlend()->mColourSrc1);
                writeLayerBlendSource(pTex->getColourBlend()->mColourSrc2);
                if (pTex->getColourBlend()->mOP == TBO_CONSTANT_FACTOR)
                    writeValue(N_conv(pTex->getColourBlend()->mConstantFactor));
                if (pTex->getColourBlend()->mColourSrc1 == TBS_CONSTANT)
                    writeColourValue(pTex->getColourBlend()->mConstant1, false);
                if (pTex->getColourBlend()->mColourSrc2 == TBS_CONSTANT)
                    writeColourValue(pTex->getColourBlend()->mConstant2, false);
            }

            // alpha_op_ex
            if (mDefaults ||
                pTex->getAlphaBlend()->mOP != TBO_MODULATE ||
                pTex->getAlphaBlend()->mColourSrc1 != TBS_TEXTURE ||
                pTex->getAlphaBlend()->mColourSrc2 != TBS_PRE_TEXTURE)
            {
                writeAttribute(4, _T("alpha_op_ex"));
                writeLayerBlendOperationEx(pTex->getAlphaBlend()->mOP);
                writeLayerBlendSource(pTex->getAlphaBlend()->mColourSrc1);
                writeLayerBlendSource(pTex->getAlphaBlend()->mColourSrc2);
                if(pTex->getAlphaBlend()->mOP == TBO_CONSTANT_FACTOR)
                    writeValue(N_conv(pTex->getAlphaBlend()->mConstantFactor));
                else if(pTex->getAlphaBlend()->mColourSrc1 == TBS_CONSTANT)
                    writeValue(N_conv(pTex->getAlphaBlend()->mConstant1.a));
                else if(pTex->getAlphaBlend()->mColourSrc2 == TBS_CONSTANT)
                    writeValue(N_conv(pTex->getAlphaBlend()->mConstant2.a));
            }

            bool individualTransformElems = false;
            // rotate
            if(mDefaults || pTex->getRotate() != Radian(0))
            {
                writeAttribute(4, _T("rotate"));
                writeValue(N_conv(pTex->getRotate().getAngle()));
                individualTransformElems = true;
            }

            // scroll
            if (mDefaults || pTex->getUOft() != 0 || pTex->getVOft() != 0 )
            {
                writeAttribute(4, _T("scroll"));
                writeValue(N_conv(pTex->getUOft()));
                writeValue(N_conv(pTex->getVOft()));
                individualTransformElems = true;
            }
            // scale
            if(mDefaults || pTex->getUScale() != 1.0 || pTex->getVScale() != 1.0 )
            {
                writeAttribute(4, _T("scale"));
                writeValue(N_conv(pTex->getUScale()));
                writeValue(N_conv(pTex->getVScale()));
                individualTransformElems = true;
            }

            // free transform
            if(!individualTransformElems && (mDefaults || pTex->getMatrix() != Matrix4f::IDENTITY))
            {
                writeAttribute(4, _T("transform"));
                const Matrix4f & xform = pTex->getMatrix();
                for(NIIi row = 0; row < 4; ++row)
                {
                    for(NIIi col = 0; col < 4; ++col)
                    {
                        writeValue(N_conv(xform[row][col]));
                    }
                }
            }

            // Used to store the u and v speeds of scroll animation effects
            NIIf scrollAnimU = 0;
            //NIIf scrollAnimV = 0;
            if(pTex->isEnvMap())
                writeEnvironmentMapEffect(pTex);
            EffectMap effMap = pTex->getEffects();
            if(!effMap.empty())
            {
                EffectMap::const_iterator it;
                for(it = effMap.begin(); it != effMap.end(); ++it)
                {
                    const ShaderChTextureUnit::TextureEffect & ef = it->second;
                    switch(ef.mType)
                    {
                    case ShaderChTextureUnit::TTT_ROTATE :
                        writeRotationEffect(ef, pTex);
                        break;
                    case ShaderChTextureUnit::TTT_UV_OFT :
                        //scrollAnimU = scrollAnimV = ef.mFactor;
                        scrollAnimU = ef.mFactor;
                        break;
                    case ShaderChTextureUnit::TTT_U_OFT :
                        scrollAnimU = ef.mFactor;
                        break;
                    case ShaderChTextureUnit::TTT_V_OFT :
                        //scrollAnimV = ef.mFactor;
                        scrollAnimU = ef.mFactor;
                        break;
                    case ShaderChTextureUnit::TTT_FUNCTION :
                        writeTransformEffect(ef, pTex);
                        break;
                    default:
                        break;
                    }
                }
            }

            // u and v scroll animation speeds merged, if present serialize scroll_anim
            if(scrollAnimU)
            {
                ShaderChTextureUnit::TextureEffect texEffect;
                texEffect.mFactor = scrollAnimU;
                //texEffect.arg2 = scrollAnimV;
                writeScrollEffect(texEffect, pTex);
            }

            // Binding type
            TextureDataFetch bt = pTex->getFetchType();
            if (mDefaults || bt != TDF_FS)
            {
                writeAttribute(4, _T("binding_type"));
                switch(bt)
                {
                case TDF_FS:
                    writeValue(_T("fragment"));
                    break;
                case TDF_VS:
                    writeValue(_T("vertex"));
                    break;
                };

            }
            // Content type
            if(mDefaults || pTex->getContentType() != ShaderChTexture::T_NORMAL)
            {
                writeAttribute(4, _T("content_type"));
                switch(pTex->getContentType())
                {
                case ShaderChTexture::T_NORMAL:
                    writeValue(_T("named"));
                    break;
                case ShaderChTexture::T_SHADOW:
                    writeValue(_T("shadow"));
                    break;
                case ShaderChTexture::T_FUSION:
                    writeValue(_T("compositor"));
                    writeValue(quoteWord(N_conv(pTex->getFrameFusion())));
                    writeValue(quoteWord(pTex->getFusionTexture()));
                    writeValue(N_conv(pTex->getMultiIndex()));
                    break;
                };
            }

            // Fire write end event.
            fireTextureUnitStateEvent(MSE_WRITE_END, skipWriting, pTex);
        }
        endSection(3);

        // Fire post section write event.
        fireTextureUnitStateEvent(MSE_POST_WRITE, skipWriting, pTex);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeEnvironmentMapEffect(const ShaderChTextureUnit * pTex)
    {
        writeAttribute(4, _T("env_map"));
        switch(pTex->getEnvMapType())
        {
        case ShaderChTextureUnit::ENM_Planar:
            writeValue(_T("planar"));
            break;
        case ShaderChTextureUnit::ENM_Sphere:
            writeValue(_T("spherical"));
            break;
        case ShaderChTextureUnit::ENM_Normal:
            writeValue(_T("cubic_normal"));
            break;
        case ShaderChTextureUnit::ENM_Reflection:
            writeValue(_T("cubic_reflection"));
            break;
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeRotationEffect(const ShaderChTextureUnit::TextureEffect & effect,
        const ShaderChTextureUnit * pTex)
    {
        if(effect.mFactor)
        {
            writeAttribute(4, _T("rotate_anim"));
            writeValue(N_conv(effect.mFactor));
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeTransformEffect(const ShaderChTextureUnit::TextureEffect & effect,
        const ShaderChTextureUnit * pTex)
    {
        writeAttribute(4, _T("wave_xform"));

        switch(effect.mEffect)
        {
        case ShaderChTextureUnit::TTT_ROTATE:
            writeValue(_T("rotate"));
            break;
        case ShaderChTextureUnit::TTT_U_SCALE:
            writeValue(_T("scale_x"));
            break;
        case ShaderChTextureUnit::TTT_V_SCALE:
            writeValue(_T("scale_y"));
            break;
        case ShaderChTextureUnit::TTT_U_OFT:
            writeValue(_T("scroll_x"));
            break;
        case ShaderChTextureUnit::TTT_V_OFT:
            writeValue(_T("scroll_y"));
            break;
        }
        /*
        switch (effect.waveType)
        {
        case WFT_INVERSE_SAWTOOTH:
            writeValue("inverse_sawtooth");
            break;
        case WFT_SAWTOOTH:
            writeValue("sawtooth");
            break;
        case WFT_SINE:
            writeValue("sine");
            break;
        case WFT_SQUARE:
            writeValue("square");
            break;
        case WFT_TRIANGLE:
            writeValue("triangle");
            break;
        case WFT_PWM:
            writeValue("pwm");
            break;
        }

        writeValue(N_conv(effect.base));
        writeValue(N_conv(effect.frequency));
        writeValue(N_conv(effect.phase));
        writeValue(N_conv(effect.amplitude));*/
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeScrollEffect(const ShaderChTextureUnit::TextureEffect& effect,
        const ShaderChTextureUnit *pTex)
    {
        //if (effect.mFactor || effect.arg2)
        if (effect.mFactor)
        {
            writeAttribute(4, _T("scroll_anim"));
            writeValue(N_conv(effect.mFactor));
            //writeValue(N_conv(effect.arg2));
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeSceneBlendFactor(ColourFactor sbf)
    {
        switch (sbf)
        {
        case CF_DST_ALPHA:
            writeValue(_T("dest_alpha"));
            break;
        case CF_DST:
            writeValue(_T("dest_colour"));
            break;
        case CF_ONE:
            writeValue(_T("one"));
            break;
        case CF_INV_DST_ALPHA:
            writeValue(_T("one_minus_dest_alpha"));
            break;
        case CF_INV_DST:
            writeValue(_T("one_minus_dest_colour"));
            break;
        case CF_INV_SRC_ALPHA:
            writeValue(_T("one_minus_src_alpha"));
            break;
        case CF_INV_SRC:
            writeValue(_T("one_minus_src_colour"));
            break;
        case CF_SRC_ALPHA:
            writeValue(_T("src_alpha"));
            break;
        case CF_SRC:
            writeValue(_T("src_colour"));
            break;
        case CF_ZERO:
            writeValue(_T("zero"));
            break;
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeSceneBlendFactor(ColourFactor sbf_src, ColourFactor sbf_dst)
    {
        if (sbf_src == CF_ONE && sbf_dst == CF_ONE )
            writeValue(_T("add"));
        else if (sbf_src == CF_DST && sbf_dst == CF_ZERO)
            writeValue(_T("modulate"));
        else if (sbf_src == CF_SRC && sbf_dst == CF_INV_SRC)
            writeValue(_T("colour_blend"));
        else if (sbf_src == CF_SRC_ALPHA && sbf_dst == CF_INV_SRC_ALPHA)
            writeValue(_T("alpha_blend"));
        else
        {
            writeSceneBlendFactor(sbf_src);
            writeSceneBlendFactor(sbf_dst);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeSceneBlendFactor(ColourFactor c_src, ColourFactor c_dest,
        ColourFactor a_src, ColourFactor a_dest)
    {
        writeSceneBlendFactor(c_src, c_dest);
        writeSceneBlendFactor(a_src, a_dest);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeCompareFunction(const CmpMode cf)
    {
        switch (cf)
        {
        case CPM_ALWAYS_FAIL:
            writeValue(_T("always_fail"));
            break;
        case CPM_ALWAYS_PASS:
            writeValue(_T("always_pass"));
            break;
        case CPM_EQUAL:
            writeValue(_T("equal"));
            break;
        case CPM_GREATER:
            writeValue(_T("greater"));
            break;
        case CPM_GREATER_EQUAL:
            writeValue(_T("greater_equal"));
            break;
        case CPM_LESS:
            writeValue(_T("less"));
            break;
        case CPM_LESS_EQUAL:
            writeValue(_T("less_equal"));
            break;
        case CPM_NOT_EQUAL:
            writeValue(_T("not_equal"));
            break;
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeColourValue(const Colour &colour, bool writeAlpha)
    {
        writeValue(N_conv(colour.r));
        writeValue(N_conv(colour.g));
        writeValue(N_conv(colour.b));
        if(writeAlpha)
            writeValue(N_conv(colour.a));
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeLayerBlendOperationEx(TextureBlendOp op)
    {
        switch (op)
        {
        case TBO_ADD:
            writeValue(_T("add"));
            break;
        case TBO_ADD_SIGNED:
            writeValue(_T("add_signed"));
            break;
        case TBO_ADD_SMOOTH:
            writeValue(_T("add_smooth"));
            break;
        case TBO_ADD_PRE_TEXTURE:
            writeValue(_T("blend_current_alpha"));
            break;
        case TBO_ADD_SPECULAR:
            writeValue(_T("blend_diffuse_colour"));
            break;
        case TBO_ADD_DIFFUSE:
            writeValue(_T("blend_diffuse_alpha"));
            break;
        case TBO_CONSTANT_FACTOR:
            writeValue(_T("blend_manual"));
            break;
        case TBO_ADD_TEXTURE:
            writeValue(_T("blend_texture_alpha"));
            break;
        case TBO_MODULATE:
            writeValue(_T("modulate"));
            break;
        case TBO_MODULATE_2X:
            writeValue(_T("modulate_x2"));
            break;
        case TBO_MODULATE_4X:
            writeValue(_T("modulate_x4"));
            break;
        case TBO_OPERAND1:
            writeValue(_T("source1"));
            break;
        case TBO_OPERAND2:
            writeValue(_T("source2"));
            break;
        case TBO_SUB:
            writeValue(_T("subtract"));
            break;
        case TBO_DOT_PRODUCT:
            writeValue(_T("dotproduct"));
            break;
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeLayerBlendSource(TextureBlendSource lbs)
    {
        switch (lbs)
        {
        case TBS_PRE_TEXTURE:
            writeValue(_T("src_current"));
            break;
        case TBS_DIFFUSE:
            writeValue(_T("src_diffuse"));
            break;
        case TBS_CONSTANT:
            writeValue(_T("src_manual"));
            break;
        case TBS_SPECULAR:
            writeValue(_T("src_specular"));
            break;
        case TBS_TEXTURE:
            writeValue(_T("src_texture"));
            break;
        }
    }

    // nfz
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeVertexProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_VS, prog, param);
        writeGpuProgramRef(_T("vertex_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeShadowCasterVertexProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_VS_SC, prog, param);
        writeGpuProgramRef(_T("shadow_caster_vertex_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeShadowCasterFragmentProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_FS_SC, prog, param);
        writeGpuProgramRef(_T("shadow_caster_fragment_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeShadowReceiverVertexProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_VS_SR, prog, param);
        writeGpuProgramRef(_T("shadow_receiver_vertex_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeShadowReceiverFragmentProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_FS_SR, prog, param);
        writeGpuProgramRef(_T("shadow_receiver_fragment_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeFragmentProgramRef(const ShaderCh * pPass)
    {
        GpuProgramParam * param;
        GpuProgram * prog;
        pPass->getProgram().get(ShaderChProgram::ST_FS, prog, param);
        writeGpuProgramRef(_T("fragment_program_ref"), prog, param);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeGpuProgramRef(const String & attrib,
        const GpuProgram * program, const GpuProgramParam * params)
    {
        bool skipWriting = false;

        // Fire pre-write event.
        fireGpuProgramRefEvent(MSE_PRE_WRITE, skipWriting, attrib, program, params, NULL);
        if (skipWriting)
            return;

        mBuffer += _T("\n");
        writeAttribute(3, attrib);
        writeValue(quoteWord(N_conv(program->getOriginID())));
        beginSection(3);
        {
            // write out paramters
            GpuProgramParam * defaultParams= 0;
            // does the GPU program have default parameters?
            if(program->getDefaultParam() != 0)
            {
                defaultParams = program->getDefaultParam();
            }
            // Fire write begin event.
            fireGpuProgramRefEvent(MSE_WRITE_BEGIN, skipWriting, attrib, program, params, defaultParams);

            writeGPUProgramParameters(params, defaultParams);

            // Fire write end event.
            fireGpuProgramRefEvent(MSE_WRITE_END, skipWriting, attrib, program, params, defaultParams);
        }
        endSection(3);

        // add to GpuProgram contatiner
        mGpuProgramDefinitionContainer.insert(program->getOriginID());

        // Fire post section write event.
        fireGpuProgramRefEvent(MSE_POST_WRITE, skipWriting, attrib, program, params, NULL);
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeGPUProgramParameters(const GpuProgramParam * params,
        GpuProgramParam * defaultParams, Nui16 level, bool useMainBuffer)
    {
        // iterate through the constant definitions
        if(params->getDefine() != 0)
        {
            writeNamedGpuProgramParameters(params, defaultParams, level, useMainBuffer);
        }
        else
        {
            writeLowLevelGpuProgramParameters(params, defaultParams, level, useMainBuffer);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeNamedGpuProgramParameters(const GpuProgramParam * params,
        GpuProgramParam* defaultParams, Nui16 level, bool useMainBuffer)
    {
        GpuParamUnitList::const_iterator i, iend = params->getDefineList().end();
        for(i = params->getDefineList().begin(); i != iend; ++i)
        {
            // get the constant definition
            const VString & paramName = i->first;
            const GpuParamUnit & def = i->second;

            // get any auto-link
            const GpuParamBlock * autoEntry = params->getSyncParamBlock(paramName);
            const GpuParamBlock * defaultAutoEntry = 0;
            if(defaultParams)
            {
                defaultAutoEntry = defaultParams->getSyncParamBlock(paramName);
            }
            String tempstr;
            StrConv::conv(paramName, tempstr);
            writeGpuProgramParameter(_T("param_named"), tempstr, autoEntry, defaultAutoEntry,
                def.mDataType, def.mMemIndex, def.mUnitSize * def.mUnitCount,
                params, defaultParams, level, useMainBuffer);
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeLowLevelGpuProgramParameters(const GpuProgramParam * params, 
        GpuProgramParam * defaultParams, Nui16 level, bool useMainBuffer)
    {
        // Iterate over the logical->physical mappings
        // This will represent the values which have been set

        const GpuParamBlockMap & Logical = params->getBlockList();
        //if(Logical)
        //{
            // NIIf params
            GpuParamBlockMap::const_iterator i, iend = Logical.end();
            for(i = Logical.begin(); i != iend; ++i)
            {
                NCount logicalIndex = i->first;
                const GpuParamBlock & logicalUse = i->second;

                const GpuParamBlock * autoEntry = params->getSyncParamBlock(logicalIndex);
                const GpuParamBlock * defaultAutoEntry = 0;
                if (defaultParams)
                {
                    defaultAutoEntry = defaultParams->getSyncParamBlock(logicalIndex);
                }

                writeGpuProgramParameter(_T("param_indexed"), N_conv(logicalIndex),
                    autoEntry, defaultAutoEntry, logicalUse.mDataType, logicalUse.mMemIndex, logicalUse.m32bSize,
                        params, defaultParams, level, useMainBuffer);
            }
        //}
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeGpuProgramParameter(
        const String & commandName, const String & identifier,
        const GpuParamBlock * autoEntry,
        const GpuParamBlock * defaultAutoEntry,
        GpuDataType dtype, NCount mindex, NCount physicalSize,
        const GpuProgramParam * params, GpuProgramParam * defaultParams,
        const Nui16 level, const bool useMainBuffer)
    {
        // Skip any params with array qualifiers
        // These are only for convenience of setters, the full array will be
        // written using the base, non-array identifier
        if (identifier.find(_T("[")) != String::npos)
        {
            return;
        }

        // get any auto-link
        // don't duplicate constants that are defined as a default parameter
        bool different = false;
        if (defaultParams)
        {
            // if default is auto but we're not or vice versa
            if ((autoEntry == 0) != (defaultAutoEntry == 0))
            {
                different = true;
            }
            else if (autoEntry)
            {
                // both must be auto
                // compare the auto values
                different = (autoEntry->mSyncParam != defaultAutoEntry->mSyncParam
                    || autoEntry->mInputInt != defaultAutoEntry->mInputInt);
            }
            else
            {
                // compare the non-auto (raw buffer) values
                // param buffers are always initialised with all zeros
                // so unset == unset
                different = memcmp(params->getBufferData(mindex), defaultParams->getBufferData(mindex),
                    /*dtype == GDT_Double ? sizeof(NIId) * physicalSize : */sizeof(NIIf) * physicalSize) != 0;
            }
        }

        if (!defaultParams || different)
        {
            String label = commandName;

            // is it auto
            if (autoEntry)
                label += _T("_auto");

            writeAttribute(level, label, useMainBuffer);
            // output param index / name
            writeValue(quoteWord(identifier), useMainBuffer);

            // if auto output auto type name and data if needed
            if(autoEntry)
            {
                String tempu;
                const GpuEnvParamUnit * syncParam = GpuProgramManager::getSyncParam(autoEntry->mSyncParam, &tempu);

                N_assert(syncParam, "Bad auto constant Definition Table");
                // output auto constant name
                writeValue(quoteWord(tempu), useMainBuffer);
                // output data if it uses it
                switch(syncParam->mInputType)
                {
                case SPI_Float:
                    writeValue(N_conv(autoEntry->mInputFloat), useMainBuffer);
                    break;

                case SPI_Int:
                    writeValue(N_conv(autoEntry->mInputInt), useMainBuffer);
                    break;

                default:
                    break;
                }
            }
            else // not auto so output all the values used
            {
                String countLabel;

                if (dtype == GDT_Double)
                {
                    // only write a number if > 1
                    physicalSize /= 2;
                    if (physicalSize > 1)
                        countLabel = N_conv(physicalSize);

                    // Get pointer to start of values
                    const NIId * pDouble = (const NIId *)params->getBufferData(mindex);

                    writeValue(_T("double") + countLabel, useMainBuffer);
                    // iterate through real constants
                    for (NCount f = 0 ; f < physicalSize; ++f)
                    {
                        writeValue(N_conv(*pDouble++), useMainBuffer);
                    }
                }
                else if(dtype == GDT_Float)
                {
                    // only write a number if > 1
                    if (physicalSize > 1)
                        countLabel = N_conv(physicalSize);

                    // Get pointer to start of values
                    const NIIf * pFloat = (const NIIf *)params->getBufferData(mindex);

                    writeValue(_T("float") + countLabel, useMainBuffer);
                    // iterate through real constants
                    for(NCount f = 0 ; f < physicalSize; ++f)
                    {
                        writeValue(N_conv(*pFloat++), useMainBuffer);
                    }
                }
                else
                {
                    // only write a number if > 1
                    if (physicalSize > 1)
                        countLabel = N_conv(physicalSize);

                    // Get pointer to start of values
                    const NIIi * pInt = (const NIIi *)params->getBufferData(mindex);

                    writeValue(_T("int") + countLabel, useMainBuffer);
                    // iterate through real constants
                    for (NCount f = 0; f < physicalSize; ++f)
                    {
                        writeValue(N_conv(*pInt++), useMainBuffer);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void MaterialSerializer::writeGpuPrograms(void)
    {
        // iterate through gpu program names in container
        GpuProgramDefIterator currentDef = mGpuProgramDefinitionContainer.begin();
        GpuProgramDefIterator endDef = mGpuProgramDefinitionContainer.end();

        while (currentDef != endDef)
        {
            // get gpu program from gpu program manager
            GpuProgram * program = static_cast<GpuProgram *>(N_Only(GpuProgram).get(*currentDef));
            // write gpu program definition type to buffer
            // check program type for vertex program
            // write program type
            mGpuProgramBuffer += _T("\n");
            String srctype;
            program->get(N_PropertyGpuProgram_Type, srctype);
            writeAttribute(0, srctype, false);

            // write program name
            writeValue(quoteWord(N_conv(program->getOriginID())), false);
            // write program language
            ShaderLanguage language = program->getLanguage();
            writeValue(N_conv(language), false);
            // write opening braces
            beginSection(0, false);
            {
                // write program source + filename
                writeAttribute(1, _T("source"), false);
                writeValue(quoteWord(program->getProgramSrc()), false);
                // write special parameters based on language
                //const ParameterList & params = program->getParameters();
                //ParameterList::const_iterator currentParam = params.begin();
                //ParameterList::const_iterator endParam = params.end();

                //while(currentParam != endParam)
                //{
                //   if(currentParam->name != N_PropertyGpuProgram_Type &&
                //        currentParam->name != N_PropertyGpuProgram_ASM &&
                //        currentParam->name != N_PropertyGpuProgram_Micro &&
                //        currentParam->name != N_PropertyGpuProgram_ExtMicro)
                //    {
                String paramstr;
                //program->get(currentParam->name, paramstr);
                if (program->get(N_PropertyGpuProgram_Skeletal, paramstr) && (paramstr != _T("false")))
                {
                    writeAttribute(1, N_conv(N_PropertyGpuProgram_Skeletal), false);
                    writeValue(paramstr, false);
                }
                if (program->get(N_PropertyGpuProgram_VertexUnit, paramstr) && (paramstr != _T("false")))
                {
                    writeAttribute(1, N_conv(N_PropertyGpuProgram_VertexUnit), false);
                    writeValue(paramstr, false);
                }
                if (program->get(N_PropertyGpuProgram_VertexOffset, paramstr) && (paramstr != _T("0")))
                {
                    writeAttribute(1, N_conv(N_PropertyGpuProgram_VertexOffset), false);
                    writeValue(paramstr, false);
                }
                if (program->get(N_PropertyGpuProgram_VerTexFetch, paramstr) && (paramstr != _T("false")))
                {
                    writeAttribute(1, N_conv(N_PropertyGpuProgram_VerTexFetch), false);
                    writeValue(paramstr, false);
                }

                if (program->get(N_PropertyGpuProgram_Syntax, paramstr) && (language != SL_ASM))
                {
                    writeAttribute(1, N_conv(N_PropertyGpuProgram_Syntax), false);
                    writeValue(paramstr, false);
                }

                //if (!paramstr.empty())
                //{
                //    writeAttribute(1, currentParam->name, false);
                //    writeValue(paramstr, false);
                //}
                //    }
                    //++currentParam;
                //}

                // write default parameters
                if(program->getDefaultParam() != 0)
                {
                    mGpuProgramBuffer += _T("\n");
                    GpuProgramParam * gpuDefaultParams = program->getDefaultParam();
                    writeAttribute(1, _T("default_params"), false);
                    beginSection(1, false);
                    writeGPUProgramParameters(gpuDefaultParams, 0, 2, false);
                    endSection(1, false);
                }
            }
            // write closing braces
            endSection(0, false);
            ++currentDef;
        }
        mGpuProgramBuffer += _T("\n");
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::add(Listener * lis)
    {
        mListeners.push_back(lis);
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::remove(Listener * lis)
    {
        ListenerListIterator i, iend = mListeners.end();
        for (i = mListeners.begin(); i != iend; ++i)
        {
            if (*i == lis)
            {
                mListeners.erase(i);
                break;
            }
        }
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::fireMaterialEvent(SerializeEvent event, bool & skip,
        const Material * mat)
    {
        ListenerListIterator it = mListeners.begin();
        ListenerListIterator itEnd = mListeners.end();

        while (it != itEnd)
        {
            (*it)->materialEventRaised(this, event, skip, mat);
            if (skip)
                break;
            ++it;
        }
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::fireTechniqueEvent(SerializeEvent event, bool & skip,
        const ShaderFusion * tech)
    {
        ListenerListIterator it = mListeners.begin();
        ListenerListIterator itEnd = mListeners.end();

        while (it != itEnd)
        {
            (*it)->techniqueEventRaised(this, event, skip, tech);
            if (skip)
                break;
            ++it;
        }
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::firePassEvent(SerializeEvent event, bool & skip,
        const ShaderCh * pass)
    {
        ListenerListIterator it = mListeners.begin();
        ListenerListIterator itEnd = mListeners.end();

        while (it != itEnd)
        {
            (*it)->passEventRaised(this, event, skip, pass);
            if (skip)
                break;
            ++it;
        }
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::fireGpuProgramRefEvent(SerializeEvent event, bool & skip, 
        const String & attrib, const GpuProgram * program, const GpuProgramParam * params, 
        GpuProgramParam * defaultParams)
    {
        ListenerListIterator it = mListeners.begin();
        ListenerListIterator itEnd = mListeners.end();

        while (it != itEnd)
        {
            (*it)->gpuProgramRefEventRaised(this, event, skip, attrib, program, params, defaultParams);
            if (skip)
                break;
            ++it;
        }
    }
    //---------------------------------------------------------------------
    void MaterialSerializer::fireTextureUnitStateEvent(SerializeEvent event,
        bool & skip, const ShaderChTextureUnit * textureUnit)
    {
        ListenerListIterator it = mListeners.begin();
        ListenerListIterator itEnd = mListeners.end();

        while (it != itEnd)
        {
            (*it)->textureUnitStateEventRaised(this, event, skip, textureUnit);
            if (skip)
                break;
            ++it;
        }
    }
    //---------------------------------------------------------------------
}