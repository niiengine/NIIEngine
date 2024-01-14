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
#include "NiiRenderFeatureSerializer.h"
#include "NiiRenderSysManager.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"
#include "NiiException.h"

#include <fstream>

namespace NII
{
    //-----------------------------------------------------------------------
    RenderFeatureSerializer::RenderFeatureSerializer() :
        mNumber(0),
        mLine(0),
        mFeature(0),
        mGpuInfo(0),
        mStream(0)
    {
        addKeyDataType(_T("render_system_name"), DT_String);
        add(_T("render_system_name"), &RenderFeature::setName);

        //addKeyDataType("driver_version", DT_GpuInfo);
        //add("driver_version", &GpuInfo::setVersion);

        //addKeyDataType("device_name", DT_GpuInfo);
        //add("device_name", &GpuInfo::setName);

        //addKeyDataType("vendor", DT_GpuInfo);
        //add("vendor", &GpuInfo::setVendor);

        // initialize NIIi types
        addKeyDataType(_T("num_world_matrices"), DT_Int16);
        addKeyDataType(_T("num_texture_units"), DT_Int16);
        addKeyDataType(_T("stencil_buffer_bit_depth"), DT_Int16);
        addKeyDataType(_T("num_vertex_blend_matrices"), DT_Int16);
        addKeyDataType(_T("num_multi_render_targets"), DT_Int16);
        addKeyDataType(_T("vertex_program_constant_float_count"), DT_Int16);
        addKeyDataType(_T("vertex_program_constant_int_count"), DT_Int16);
        addKeyDataType(_T("vertex_program_constant_bool_count"), DT_Int16);
        addKeyDataType(_T("fragment_program_constant_float_count"), DT_Int16);
        addKeyDataType(_T("fragment_program_constant_int_count"), DT_Int16);
        addKeyDataType(_T("fragment_program_constant_bool_count"), DT_Int16);
        addKeyDataType(_T("num_vertex_texture_units"), DT_Int16);

        // initialize NIIi setters
        add(_T("num_world_matrices"), &RenderFeature::setMaxSpace);
        add(_T("num_texture_units"), &RenderFeature::setMaxTexUnit);
        add(_T("stencil_buffer_bit_depth"), &RenderFeature::setStencilBit);
        add(_T("num_multi_render_targets"), &RenderFeature::setMaxMultiFrameObj);
        add(_T("vertex_program_constant_float_count"), &RenderFeature::setMaxVertexProgramParam);
        add(_T("fragment_program_constant_float_count"), &RenderFeature::setMaxFragmentProgramParam);
        add(_T("geometry_program_constant_float_count"), &RenderFeature::setMaxGeometryProgramParam);
        add(_T("num_vertex_texture_units"), &RenderFeature::setMaxVertexTexUnit);

        // initialize bool types
        addKeyDataType(_T("non_pow2_textures_limited"), DT_Bool);
        addKeyDataType(_T("vertex_texture_units_shared"), DT_Bool);

        // initialize bool setters
        add(_T("non_pow2_textures_limited"), &RenderFeature::setOnlyPow2Texture);

        // initialize NIIf types
        addKeyDataType(_T("max_point_size"), DT_Float);

        // initialize NIIf setters
        add(_T("max_point_size"), &RenderFeature::setMaxPointSize);

        // there is no dispatch table for shader profiles, just the type
        addKeyDataType(_T("shader_profile"), DT_ShaderLang);

        // set up RSC_XXX style capabilities
        addKeyDataType(_T("fixed_function"), DT_Feature);
        addKeyDataType(_T("automipmapcompress"), DT_Feature);
        addKeyDataType(_T("automipmap"), DT_Feature);
        addKeyDataType(_T("blending"), DT_Feature);
        addKeyDataType(_T("anisotropy"), DT_Feature);
        addKeyDataType(_T("dot3"), DT_Feature);
        addKeyDataType(_T("cubemapping"), DT_Feature);
        addKeyDataType(_T("hwstencil"), DT_Feature);
        addKeyDataType(_T("vbo"), DT_Feature);
        addKeyDataType(_T("vertex_program"), DT_Feature);
        addKeyDataType(_T("geometry_program"), DT_Feature);
        addKeyDataType(_T("fragment_program"), DT_Feature);
        addKeyDataType(_T("scissor_test"), DT_Feature);
        addKeyDataType(_T("two_sided_stencil"), DT_Feature);
        addKeyDataType(_T("stencil_wrap"), DT_Feature);
        addKeyDataType(_T("hwocclusion"), DT_Feature);
        addKeyDataType(_T("user_clip_planes"), DT_Feature);
        addKeyDataType(_T("vertex_format_ubyte4"), DT_Feature);
        addKeyDataType(_T("infinite_far_plane"), DT_Feature);
        addKeyDataType(_T("hwrender_to_texture"), DT_Feature);
        addKeyDataType(_T("texture_float"), DT_Feature);
        addKeyDataType(_T("non_power_of_2_textures"), DT_Feature);
        addKeyDataType(_T("texture_3d"), DT_Feature);
        addKeyDataType(_T("point_sprites"), DT_Feature);
        addKeyDataType(_T("point_extended_parameters"), DT_Feature);
        addKeyDataType(_T("vertex_texture_fetch"), DT_Feature);
        addKeyDataType(_T("mipmap_lod_bias"), DT_Feature);
        addKeyDataType(_T("texture_compression"), DT_Feature);
        addKeyDataType(_T("texture_compression_dxt"), DT_Feature);
        addKeyDataType(_T("texture_compression_vtc"), DT_Feature);
        addKeyDataType(_T("texture_compression_pvrtc"), DT_Feature);
        addKeyDataType(_T("glew1_5_novbo"), DT_Feature);
        addKeyDataType(_T("fbo"), DT_Feature);
        addKeyDataType(_T("fbo_arb"), DT_Feature);
        addKeyDataType(_T("fbo_ati"), DT_Feature);
        addKeyDataType(_T("pbuffer"), DT_Feature);
        addKeyDataType(_T("glew1_5_nohwocclusion"), DT_Feature);
        addKeyDataType(_T("perstageconstant"), DT_Feature);
        addKeyDataType(_T("separate_shader_objects"), DT_Feature);

        add(_T("fixed_function"), GF_Fixed_Pipeline);
        add(_T("automipmapcompress"), GF_Tex_AutoMipmapCompress);
        add(_T("anisotropy"), GF_Tex_Anisotropy);
        add(_T("dot3"), GF_Tex_Dot3);
        add(_T("hwstencil"), GF_HW_Stencil);
        add(_T("vbo"), GF_HW_Buffer);
        add(_T("vertex_program"), GF_Program_Vertex);
        add(_T("geometry_program"), GF_Program_Geometry);
        add(_T("fragment_program"), GF_Program_Fragment);
        add(_T("scissor_test"), GF_ClipRect);
        add(_T("two_sided_stencil"), GF_Stencil_BothSide);
        add(_T("stencil_wrap"), GF_Stencil_Wrap);
        add(_T("hwocclusion"), GF_HW_PixelQuery);
        add(_T("user_clip_planes"), GF_ClipPlanes);
        add(_T("vertex_format_ubyte4"), GF_Vertex_UByte4);
        add(_T("infinite_far_plane"), GF_Infinite_FarPlane);
        add(_T("hwrender_to_texture"), GF_Render_To_Texture);
        add(_T("texture_float"), GF_Tex_Float);
        add(_T("non_power_of_2_textures"), GF_Tex_NonPow2);
        add(_T("texture_3d"), GF_Tex_3D);
        add(_T("point_sprites"), GF_Point_Sprites);
        add(_T("point_extended_parameters"), GF_Point_Param);
        add(_T("vertex_texture_fetch"), GF_Program_VertexTexFetch);
        add(_T("mipmap_lod_bias"), GF_Tex_MipMapBias);
        add(_T("texture_compression"), GF_Tex_Compress);
        add(_T("texture_compression_dxt"), GF_Tex_Fromat_DXT);
        add(_T("texture_compression_vtc"), GF_Tex_Fromat_VTC);
        add(_T("texture_compression_pvrtc"), GF_Tex_Fromat_PVRTC);
        add(_T("hwrender_to_vertex_buffer"), GF_Render_To_Vertex);
        add(_T("pbuffer"), GF_GL_PBuffer);
        add(_T("perstageconstant"), GF_D3D_PerStageConstant);
    }
    //-----------------------------------------------------------------------
    RenderFeatureSerializer::~RenderFeatureSerializer()
    {
    }
    //-----------------------------------------------------------------------
    static void __rsyfeature_write(const RenderFeature * caps, const String & name,
        Nostream & file)
    {
        using namespace std;

        file << "render_system_capabilities \"" << name << "\"" << endl;
        file << "{" << endl;

        file << "\t" << "render_system_name " << caps->getName() << endl;
        file << endl;


        file << "\t" << "device_name " << caps->getGpuInfo()->getName() << endl;
        const DriverVersion& driverVer = caps->getGpuInfo()->getVersion();
        file << "\t" << "driver_version " << driverVer.toString() << endl;
        file << "\t" << "vendor " << GpuVendorDetail::toStr(caps->getGpuInfo()->getVendor());

        file << endl;

        file << endl;
        file << "\t" << "fixed_function " << N_conv(caps->isSupport(GF_Fixed_Pipeline)) << endl;
        file << "\t" << "automipmapcmpress " << N_conv(caps->isSupport(GF_Tex_AutoMipmapCompress)) << endl;
        file << "\t" << "anisotropy " << N_conv(caps->isSupport(GF_Tex_Anisotropy)) << endl;
        file << "\t" << "dot3 " << N_conv(caps->isSupport(GF_Tex_Dot3)) << endl;
        file << "\t" << "hwstencil " << N_conv(caps->isSupport(GF_HW_Stencil)) << endl;
        file << "\t" << "vbo " << N_conv(caps->isSupport(GF_HW_Buffer)) << endl;
        file << "\t" << "vertex_program " << N_conv(caps->isSupport(GF_Program_Vertex)) << endl;
        file << "\t" << "fragment_program " << N_conv(caps->isSupport(GF_Program_Fragment)) << endl;
        file << "\t" << "scissor_test " << N_conv(caps->isSupport(GF_ClipRect)) << endl;
        file << "\t" << "two_sided_stencil " << N_conv(caps->isSupport(GF_Stencil_BothSide)) << endl;
        file << "\t" << "stencil_wrap " << N_conv(caps->isSupport(GF_Stencil_Wrap)) << endl;
        file << "\t" << "hwocclusion " << N_conv(caps->isSupport(GF_HW_PixelQuery)) << endl;
        file << "\t" << "user_clip_planes " << N_conv(caps->isSupport(GF_ClipPlanes)) << endl;
        file << "\t" << "vertex_format_ubyte4 " << N_conv(caps->isSupport(GF_Vertex_UByte4)) << endl;
        file << "\t" << "infinite_far_plane " << N_conv(caps->isSupport(GF_Infinite_FarPlane)) << endl;
        file << "\t" << "hwrender_to_texture " << N_conv(caps->isSupport(GF_Render_To_Texture)) << endl;
        file << "\t" << "texture_float " << N_conv(caps->isSupport(GF_Tex_Float)) << endl;
        file << "\t" << "non_power_of_2_textures " << N_conv(caps->isSupport(GF_Tex_NonPow2)) << endl;
        file << "\t" << "texture_3d " << N_conv(caps->isSupport(GF_Tex_3D)) << endl;
        file << "\t" << "point_sprites " << N_conv(caps->isSupport(GF_Point_Sprites)) << endl;
        file << "\t" << "point_extended_parameters " << N_conv(caps->isSupport(GF_Point_Param)) << endl;
        file << "\t" << "vertex_texture_fetch " << N_conv(caps->isSupport(GF_Program_VertexTexFetch)) << endl;
        file << "\t" << "mipmap_lod_bias " << N_conv(caps->isSupport(GF_Tex_MipMapBias)) << endl;
        file << "\t" << "texture_compression " << N_conv(caps->isSupport(GF_Tex_Compress)) << endl;
        file << "\t" << "texture_compression_dxt " << N_conv(caps->isSupport(GF_Tex_Fromat_DXT)) << endl;
        file << "\t" << "texture_compression_vtc " << N_conv(caps->isSupport(GF_Tex_Fromat_VTC)) << endl;
        file << "\t" << "texture_compression_pvrtc " << N_conv(caps->isSupport(GF_Tex_Fromat_PVRTC)) << endl;
        file << "\t" << "pbuffer " << N_conv(caps->isSupport(GF_GL_PBuffer)) << endl;
        file << "\t" << "separate_shader_objects " << N_conv(caps->isSupport(GF_GLSL_SeparateShaderObject)) << endl;
        file << endl;

        file << "\t" << "shader_profile " << N_conv(caps->getShaderMark()) << endl;

        file << endl;
        file << "\t" << "max_point_size " << N_conv(caps->getMaxPointSize()) << endl;

        file << endl;
        file << "\t" << "non_pow2_textures_limited " << N_conv(caps->isOnlyPow2Texture()) << endl;

        file << endl;
        file << "\t" << "num_world_matrices " << N_conv(caps->getMaxSpace()) << endl;
        file << "\t" << "num_texture_units " << N_conv(caps->getMaxTexUnit()) << endl;
        file << "\t" << "stencil_buffer_bit_depth " << N_conv(caps->getStencilBit()) << endl;
        file << "\t" << "num_multi_render_targets " << N_conv(caps->getMaxMultiFrameObj()) << endl;
        file << "\t" << "vertex_program_constant_float_count " << N_conv(caps->getMaxVPFloatParamCount()) << endl;
        file << "\t" << "vertex_program_constant_int_count " << N_conv(caps->getMaxVPIntParamCount()) << endl;
        file << "\t" << "vertex_program_constant_bool_count " << N_conv(caps->getMaxVPBoolParamCount()) << endl;
        file << "\t" << "fragment_program_constant_float_count " << N_conv(caps->getMaxFPFloatCount()) << endl;
        file << "\t" << "fragment_program_constant_int_count " << N_conv(caps->getMaxFPIntCount()) << endl;
        file << "\t" << "fragment_program_constant_bool_count " << N_conv(caps->getMaxFPBoolCount()) << endl;
        file << "\t" << "geometry_program_constant_float_count " << N_conv(caps->getMaxGPFloatCount()) << endl;
        file << "\t" << "geometry_program_constant_int_count " << N_conv(caps->getMaxGPIntCount()) << endl;
        file << "\t" << "geometry_program_constant_bool_count " << N_conv(caps->getMaxGPBoolCount()) << endl;
        file << "\t" << "num_vertex_texture_units " << N_conv(caps->getMaxVertexTexUnit()) << endl;

        file << endl;

        file << "}" << endl;
    }
    //-----------------------------------------------------------------------
    void RenderFeatureSerializer::write(const RenderFeature * caps,
        const String & name, const String & filename)
    {
        Nofstream file(filename.c_str());
        __rsyfeature_write(caps, name, file);
        file.close();
    }
    //-----------------------------------------------------------------------
    String RenderFeatureSerializer::write(const RenderFeature * caps,
        const String & name)
    {
        StringStream stream;
        __rsyfeature_write(caps, name, stream);
        return stream.str();
    }
    //-----------------------------------------------------------------------
    void RenderFeatureSerializer::read(DataStream * stream)
    {
        enum ParseAction
        {
            PARSE_HEADER,
            FIND_OPEN_BRACE,
            COLLECT_LINES
        };
        // reset parsing data to NULL
        mNumber = 0;
        mLine = 0;
        mFeature = 0;
        mGpuInfo = 0;
        mStream = stream;

        // parser operating data
        String line;
        ParseAction parseAction = PARSE_HEADER;
        StringList tokens;
        bool parsedAtLeastOneRSC = false;

        // collect capabilities lines (i.e. everything that is not header, "{", "}",
        // comment or empty line) for further processing
        CollectList capabilitiesLines;

        // for reading data
        Ntchar tmpBuf[NII_STREAM_TEMP_SIZE];

        // TODO: build a smarter tokenizer so that "{" and "}"
        // don't need separate lines
        while(!stream->end())
        {
            stream->readLine(tmpBuf, NII_STREAM_TEMP_SIZE - 1);
            line = String(tmpBuf);
            StrUtil::trim(line);

            // keep track of parse position
            mLine = &line;
            mNumber++;

            StrUtil::split(line, tokens);

            // skip empty and comment lines
            // TODO: handle end of line comments
            if(tokens[0] == _T("") || tokens[0].substr(0,2) == _T("//"))
                continue;

            switch (parseAction)
            {
                // header line must look like this:
                // render_system_capabilities "Vendor Card Name Version xx.xxx"
                case PARSE_HEADER:
                {
                    if(tokens[0] != _T("render_system_capabilities"))
                    {
                        logError(_I18n("The first keyword must be render_system_capabilities. RenderFeature NOT created!"));
                        return;
                    }
                    else
                    {
                        // the rest of the tokens are irrevelant, beause everything between "..." is one name
                        String rscName = line.substr(tokens[0].size());
                        StrUtil::trim(rscName);

                        // the second argument must be a "" delimited string
                        if (!StrUtil::match(rscName, _T("\"*\""), false))
                        {
                            logError(_I18n("The argument to render_system_capabilities must be a quote delimited (\"...\") string. RenderFeature NOT created!"));
                            return;
                        }
                        else
                        {
                            // we have a valid header

                            // remove quotes
                            rscName = rscName.substr(1);
                            rscName = rscName.substr(0, rscName.size() - 1);

                            // create RSC
                            mFeature = N_new RenderFeature();
                            mGpuInfo = const_cast<GPUInfo *>(mFeature->getGpuInfo());
                            // RSCManager is responsible for deleting mFeature
                            N_Only(RenderSys).add(rscName, mFeature);

                            N_Only(Log).log(_I18n("Created RenderFeature") + rscName);

                            // do next action
                            parseAction = FIND_OPEN_BRACE;
                            parsedAtLeastOneRSC = true;
                        }
                    }
                }
                break;
                case FIND_OPEN_BRACE:
                {
                    if(tokens[0] != _T("{") || tokens.size() != 1)
                    {
                        logError(_I18n("Expected '{' got: ") + line + _I18n(". Continuing to next line."));
                    }
                    else
                    {
                        parseAction = COLLECT_LINES;
                    }
                }
                break;
                case COLLECT_LINES:
                {
                    if(tokens[0] == _T("}"))
                    {
                        // this render_system_capabilities section is over
                        // let's process the data and look for the next one
                        parseUnit(capabilitiesLines);
                        capabilitiesLines.clear();
                        parseAction = PARSE_HEADER;

                    }
                    else
                    {
                        capabilitiesLines.push_back(CollectList::value_type(line, mNumber));
                    }
                }
                break;
            }
        }
        // Datastream is empty
        // if we are still looking for header, this means that we have either
        // finished reading one, or this is an empty file
        if(parseAction == PARSE_HEADER && parsedAtLeastOneRSC == false)
        {
            logError (_I18n("The file is empty"));
        }
        if(parseAction == FIND_OPEN_BRACE)
        {
            logError (_I18n("Bad .gpufeature file. Were not able to find a '{'"));
        }
        if(parseAction == COLLECT_LINES)
        {
            logError (_I18n("Bad .gpufeature file. Were not able to find a '}'"));
        }
    }
    //------------------------------------------------------------
    void RenderFeatureSerializer::parseUnit(CollectList & lines)
    {
        StringList tokens;

        for (CollectList::iterator it = lines.begin(), end = lines.end();
            it != end; ++it)
        {
            // restore the current line information for debugging
            mLine = &(it->first);
            mNumber = it->second;

            StrUtil::split(it->first, tokens);
            // check for incomplete lines
            if(tokens.size() < 2)
            {
                logError(_I18n("No parameters given for the capability keyword"));
                continue;
            }

            // the first token must the the keyword identifying the capability
            // the remaining tokens are the parameters
            String keyword = tokens[0];
            String everythingElse = _T("");
            for(NCount i = 1; i < tokens.size() - 1; ++i)
            {
                everythingElse = everythingElse + tokens[i] + _T(" ");
            }
            everythingElse = everythingElse + tokens[tokens.size() - 1];

            DataType keywordType = getKeyDataType(keyword);

            switch(keywordType)
            {
                case DT_Unknow:
                {
                    logError(_I18n("Unknown capability keyword: ") + keyword);
                }
                break;
                case DT_String:
                {
                    exec(keyword, everythingElse);
                }
                break;
                case DT_Int16:
                {
                    NIIi integer;
                    StrConv::conv(tokens[1], integer);
                    exec(keyword, (Nui16)integer);
                }
                break;
                case DT_Bool:
                {
                    bool b;
                    StrConv::conv(tokens[1], b);
                    exec(keyword, b);
                }
                break;
                case DT_Float:
                {
                    NIIf real;
                    StrConv::conv(tokens[1], real);
                    exec(keyword, real);
                }
                break;
                case DT_ShaderLang:
                {
                    Nui32 integer;
                    StrConv::conv(tokens[1], integer);
                    mFeature->add((ShaderLanguage)integer);
                }
                break;
                case DT_GpuInfo:
                {
                    if(keyword == _T("driver_version"))
                        mGpuInfo->setVersion(everythingElse);
                    else if(keyword == _T("device_name"))
                        mGpuInfo->setName(everythingElse);
                    else if(keyword == _T("vendor"))
                        mGpuInfo->setVendor(everythingElse);
                }
                break;
                case DT_Feature:
                {
                    bool b;
                    StrConv::conv(tokens[1], b);
                    exec(tokens[0], b);
                }
                break;
                default:
                    break;
            }
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::addKeyDataType(const String & key, DataType type)
    {
        mKeyTypeList.insert(KeyTypeList::value_type(key, type));
    }
    //----------------------------------------------------------------
    RenderFeatureSerializer::DataType RenderFeatureSerializer::getKeyDataType(const String & key) const
    {
        KeyTypeList::const_iterator it = mKeyTypeList.find(key);
        if(it != mKeyTypeList.end())
            return (*it).second;
        else
        {
            logError(_I18n("Can't find the type for key: ") + key);
            return DT_Unknow;
        }
   }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & name, GpuFeature cap)
    {
        mFeatureMap.insert(FeatureMap::value_type(name, cap));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::setEnable(const String & name, bool val)
    {
        if(mFeatureMap.find(name) == mFeatureMap.end())
        {
            logError(_I18n("Undefined capability: ") + name);
            return;
        }
        if(val)
        {
            GpuFeature cap = mFeatureMap[name];
            mFeature->add(cap);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & key, SetStringMethod method)
    {
        mStringTableMap.insert(StringTableMap::value_type(key, method));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::exec(const String & key, String & val)
    {
        StringTableMap::iterator me = mStringTableMap.find(key);
        if (me != mStringTableMap.end())
        {
            SetStringMethod m = me->second;
            (mFeature->*m)(val);
        }
        else
        {
            logError(_I18n("undefined key: ") + key);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & key, SetInt16Method method)
    {
        mInt16TableMap.insert(Int16TableMap::value_type(key, method));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & key, SetInt32Method method)
    {
        mInt32TableMap.insert(Int32TableMap::value_type(key, method));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::exec(const String & key, Nui16 val)
    {
        Int16TableMap::iterator me = mInt16TableMap.find(key);
        if (me != mInt16TableMap.end())
        {
            SetInt16Method m = me->second;
            (mFeature->*m)(val);
        }
        else
        {
            logError(_I18n("undefined key: ") + key);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::exec(const String & key, Nui32 val)
    {
        Int32TableMap::iterator me = mInt32TableMap.find(key);
        if (me != mInt32TableMap.end())
        {
            SetInt32Method m = me->second;
            (mFeature->*m)(val);
        }
        else
        {
            logError(_I18n("undefined key: ") + key);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & key, SetBoolMethod method)
    {
        mBoolTableMap.insert(BoolTableMap::value_type(key, method));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::exec(const String & key, bool val)
    {
        BoolTableMap::iterator me = mBoolTableMap.find(key);
        if (me != mBoolTableMap.end())
        {
            SetBoolMethod m = me->second;
            (mFeature->*m)(val);
        }
        else
        {
            logError(_I18n("undefined key: ") + key);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::add(const String & key, SetFloatMethod method)
    {
        mFloatTableMap.insert(FloatTableMap::value_type(key, method));
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::exec(const String & key, NIIf val)
    {
        FloatTableMap::iterator me = mFloatTableMap.find(key);
        if(me != mFloatTableMap.end())
        {
            SetFloatMethod m = me->second;
            (mFeature->*m)(val);
        }
        else
        {
        logError(_I18n("undefined key: ") + key);
        }
    }
    //----------------------------------------------------------------
    void RenderFeatureSerializer::logError(const String & error) const
    {
        // log the line with error in it if the current line is available
        if(mLine != 0 && mStream != 0)
        {
            N_Only(Log).log(_I18n("Error in .gpufeature ") + mStream->getName() + _T(":") +
                N_conv(mNumber) + _T(" : ") + error);
        }
        else if (mStream != 0)
        {
            N_Only(Log).log(_I18n("Error in .gpufeature ") + mStream->getName() +
                _T(" : ") + error);
        }
    }
    //----------------------------------------------------------------
}