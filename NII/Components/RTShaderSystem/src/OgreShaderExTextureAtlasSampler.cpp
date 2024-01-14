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
#include "OgreShaderExTextureAtlasSampler.h"

#ifdef RTSHADER_SYSTEM_BUILD_EXT_SHADERS
#include "OgreShaderFFPRenderState.h"
#include "OgreShaderProgram.h"
#include "OgreShaderParameter.h"
#include "OgreShaderProgramSet.h"
#include "OgreTechnique.h"

#define SGX_LIB_TEXTURE_ATLAS "SGXLib_TextureAtlas"

#define SGX_FUNC_ATLAS_SAMPLE_AUTO_ADJUST "SGX_Atlas_Sample_Auto_Adjust"
#define SGX_FUNC_ATLAS_SAMPLE_NORMAL "SGX_Atlas_Sample_Normal"

#define SGX_FUNC_ATLAS_WRAP "SGX_Atlas_Wrap"
#define SGX_FUNC_ATLAS_MIRROR "SGX_Atlas_Mirror"
#define SGX_FUNC_ATLAS_CLAMP "SGX_Atlas_Clamp"
#define SGX_FUNC_ATLAS_BORDER "SGX_Atlas_Border"
#define TAS_MAX_SAFE_ATLASED_TEXTURES 250

namespace Ogre
{
    template<> RTShader::TextureAtlasSamplerFactory* Singleton<RTShader::TextureAtlasSamplerFactory>::msSingleton = 0;

    namespace RTShader
    {
    void operator<<(std::ostream& o, const TextureAtlasSamplerFactory::TextureAtlasAttib& tai)
    {
        o << tai.autoBorderAdjust << tai.positionMode << tai.positionOffset;
    }

    const TextureAtlasTablePtr c_BlankAtlasTable;
    const String c_ParamTexel("texel_");
    String TextureAtlasSampler::Type = "SGX_TextureAtlasSampler";
    String c_RTAtlasKey = "RTAtlas";

    //-----------------------------------------------------------------------
    TextureAtlasSampler::TextureAtlasSampler() :
        mAtlasTexcoordPos(0),
        mIsTableDataUpdated(false),
        mAutoAdjustPollPosition(true)
    {
        memset(mIsAtlasTextureUnits, 0, sizeof(bool) * TAS_MAX_TEXTURES);
    }

    //-----------------------------------------------------------------------
    const String& TextureAtlasSampler::getType() const
    {
        return Type;
    }


    //-----------------------------------------------------------------------
    int	TextureAtlasSampler::getExecutionOrder() const
    {
        return FFP_TEXTURING + 25;
    }

    //-----------------------------------------------------------------------
    bool TextureAtlasSampler::resolveParameters(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Program* psProgram = programSet->getCpuFragmentProgram();
        Function* vsMain   = vsProgram->getEntryPointFunction();
        Function* psMain   = psProgram->getEntryPointFunction();


        //
        // Define vertex shader parameters used to find the position of the textures in the atlas
        //
        Parameter::Content indexContent = (Parameter::Content)((int)Parameter::SPC_TEXTURE_COORDINATE0 + mAtlasTexcoordPos);
        GpuDataType indexType = GDT_Float4X;

        mVSInpTextureTableIndex = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
            mAtlasTexcoordPos, indexContent, indexType);


        //
        // Define parameters to carry the information on the location of the texture from the vertex to
        // the pixel shader
        //
        for(ushort i = 0 ; i < TAS_MAX_TEXTURES ; ++ i)
        {
            if(mIsAtlasTextureUnits[i] == true)
            {
                mVSTextureTable[i] = vsProgram->resolveParameter(GDT_Float4X, -1, GPT_Render, "AtlasData", mAtlasTableDatas[i]->size());
                mVSOutTextureDatas[i] = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES,
                    -1, Parameter::SPC_UNKNOWN, GDT_Float4X);
                mPSInpTextureDatas[i] = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,
                    mVSOutTextureDatas[i]->getIndex(), Parameter::SPC_UNKNOWN, GDT_Float4X);
                mPSTextureSizes[i] = psProgram->resolveParameter(GDT_Float2X,-1, GPT_Space, "AtlasSize");
            }
        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool TextureAtlasSampler::resolveDependencies(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Program* psProgram = programSet->getCpuFragmentProgram();
        vsProgram->addDependency(FFP_LIB_COMMON);
        psProgram->addDependency(SGX_LIB_TEXTURE_ATLAS);

        return true;
    }

    //-----------------------------------------------------------------------
    bool TextureAtlasSampler::addFunctionInvocations(ProgramSet* programSet)
    {
        Program* vsProgram = programSet->getCpuVertexProgram();
        Function* vsMain   = vsProgram->getEntryPointFunction();
        Program* psProgram = programSet->getCpuFragmentProgram();
        Function* psMain   = psProgram->getEntryPointFunction();
        FunctionInvocation* curFuncInvocation = NULL;

        //
        // Calculate the position and size of the texture in the atlas in the vertex shader
        //
        int groupOrder = (FFP_VS_TEXTURING - FFP_VS_LIGHTING) / 2;
        int internalCounter = 0;

        for(ushort i = 0 ; i <  TAS_MAX_TEXTURES; ++i)
        {
            if (mIsAtlasTextureUnits[i] == true)
            {
                Operand::OpMask textureIndexMask = Operand::OPM_X;
                switch (i)
                {
                case 1: textureIndexMask = Operand::OPM_Y; break;
                case 2: textureIndexMask = Operand::OPM_Z; break;
                case 3: textureIndexMask = Operand::OPM_W; break;
                }

                curFuncInvocation = N_new FunctionInvocation(FFP_FUNC_ASSIGN, groupOrder, internalCounter++);
                curFuncInvocation->pushOperand(mVSTextureTable[i], Operand::OPS_IN);
                curFuncInvocation->pushOperand(mVSInpTextureTableIndex, Operand::OPS_IN, textureIndexMask, 1);
                curFuncInvocation->pushOperand(mVSOutTextureDatas[i], Operand::OPS_OUT);
                vsMain->addAtomInstance(curFuncInvocation);
            }
        }

        //
        // sample the texture in the fragment shader given the extracted data in the pixel shader
        //


        groupOrder = (FFP_PS_SAMPLING + FFP_PS_TEXTURING) / 2;

        internalCounter = 0;

        const ShaderParameterList& inpParams = psMain->getInputParameters();
        const ShaderParameterList& localParams = psMain->getLocalParameters();

        ParameterPtr psAtlasTextureCoord = psMain->resolveLocalParameter(Parameter::SPS_UNKNOWN,
            -1, "atlasCoord", GDT_Float2X);

        for(ushort j = 0 ; j <  TAS_MAX_TEXTURES; ++j)
        {
            if (mIsAtlasTextureUnits[j] == true)
            {
                //Find the texture coordinates texel and sampler from the original FFPTexturing
                ParameterPtr texcoord = psMain->getParameterByContent(inpParams, (Parameter::Content)(Parameter::SPC_TEXTURE_COORDINATE0 + j), GDT_Float2X);
                ParameterPtr texel = psMain->getParameterByName(localParams, c_ParamTexel + Ogre::StrConv::conv(j));
                UniformParameterPtr sampler = psProgram->getParameterByType(GDT_Sampler2D, j);

                const char* addressUFuncName = getAdressingFunctionName(mTextureAddressings[j].u);
                const char* addressVFuncName = getAdressingFunctionName(mTextureAddressings[j].v);

                //Create a function which will replace the texel with the texture texel
                if ((texcoord.isNull() == false) && (texel.isNull() == false) &&
                    (sampler.isNull() == false) && (addressUFuncName != NULL) && (addressVFuncName != NULL))
                {
                    //calculate the U value due to addressing mode
                    curFuncInvocation = N_new FunctionInvocation(addressUFuncName, groupOrder, internalCounter++);
                    curFuncInvocation->pushOperand(texcoord, Operand::OPS_IN, Operand::OPM_X);
                    curFuncInvocation->pushOperand(psAtlasTextureCoord, Operand::OPS_OUT, Operand::OPM_X);
                    psMain->addAtomInstance(curFuncInvocation);

                    //calculate the V value due to addressing mode
                    curFuncInvocation = N_new FunctionInvocation(addressVFuncName, groupOrder, internalCounter++);
                    curFuncInvocation->pushOperand(texcoord, Operand::OPS_IN, Operand::OPM_Y);
                    curFuncInvocation->pushOperand(psAtlasTextureCoord, Operand::OPS_OUT, Operand::OPM_Y);
                    psMain->addAtomInstance(curFuncInvocation);

                    //sample the texel color
                    curFuncInvocation = N_new FunctionInvocation(
                        mAutoAdjustPollPosition ? SGX_FUNC_ATLAS_SAMPLE_AUTO_ADJUST : SGX_FUNC_ATLAS_SAMPLE_NORMAL, groupOrder, internalCounter++);
                    curFuncInvocation->pushOperand(sampler, Operand::OPS_IN);
                    curFuncInvocation->pushOperand(texcoord, Operand::OPS_IN, Operand::OPM_X | Operand::OPM_Y);
                    curFuncInvocation->pushOperand(psAtlasTextureCoord, Operand::OPS_IN);
                    curFuncInvocation->pushOperand(mPSInpTextureDatas[j], Operand::OPS_IN);
                    curFuncInvocation->pushOperand(mPSTextureSizes[j], Operand::OPS_IN);
                    curFuncInvocation->pushOperand(texel, Operand::OPS_OUT);
                    psMain->addAtomInstance(curFuncInvocation);

                }
            }
        }
        return true;
    }

    //-----------------------------------------------------------------------
    const char* TextureAtlasSampler::getAdressingFunctionName(TextureAddressingMode mode)
    {
        switch (mode)
        {
        case ShaderChTextureUnit::TAM_REPEAT: 
            return SGX_FUNC_ATLAS_WRAP;
        case ShaderChTextureUnit::TAM_MIRROR: 
            return SGX_FUNC_ATLAS_MIRROR;
        case ShaderChTextureUnit::TAM_CLAMP: 
            return SGX_FUNC_ATLAS_CLAMP;
        case ShaderChTextureUnit::TAM_BORDER: 
            return SGX_FUNC_ATLAS_BORDER;
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSampler::write(const SubRenderState& rhs)
    {
        const TextureAtlasSampler& rhsColour = static_cast<const TextureAtlasSampler&>(rhs);

        mAtlasTexcoordPos = rhsColour.mAtlasTexcoordPos;
        for(ushort j = 0 ; j < TAS_MAX_TEXTURES ; ++j)
        {
            mIsAtlasTextureUnits[j] = rhsColour.mIsAtlasTextureUnits[j];
            mTextureAddressings[j] = rhsColour.mTextureAddressings[j];
            mAtlasTableDatas[j] = rhsColour.mAtlasTableDatas[j];
            mIsAtlasTextureUnits[j] = rhsColour.mIsAtlasTextureUnits[j];
        }
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSampler::updateGpuProgramsParams(GeometryObj * rend,
        ShaderCh * pass,  const SysSyncParam * source, const LightList * pLightList)
    {
        if(mIsTableDataUpdated == false)
        {
            mIsTableDataUpdated = true;
            for(ushort j = 0 ; j < TAS_MAX_TEXTURES ; ++j)
            {
                if(mIsAtlasTextureUnits[j] == true)
                {
                    //
                    // Update the information of the size of the atlas textures
                    //
                    Texture * tex = pass->getTexture().get(j)->getTexture(0);
                    if(tex == 0)
                        N_EXCEPT(NoExist, _T("Could not find texture "));

                    NCount ttwidth = tex->getWidth();
                    NCount ttheight = tex->getHeight();
                    Vector2 texSize(ttwidth, ttheight);
                    mPSTextureSizes[j]->setGpuParameter(texSize);

                    //
                    //Update the information of which texture exist where in the atlas
                    //
                    GpuProgramParam * vsGpuParams;
                    pass->getProgram().get(0, ST_VS, NULL, vsGpuParams);
                    vector<float>::type buffer(mAtlasTableDatas[j]->size() * 4);
                    for(size_t i = 0 ; i < mAtlasTableDatas[j]->size() ; ++i)
                    {
                        buffer[i*4] = (*(mAtlasTableDatas[j]))[i].posU;
                        buffer[i*4 + 1] = (*(mAtlasTableDatas[j]))[i].posV;
                        buffer[i*4 + 2] = (float)Ogre::Math::Log2((*(mAtlasTableDatas[j]))[i].width * texSize.x);
                        buffer[i*4 + 3] = (float)Ogre::Math::Log2((*(mAtlasTableDatas[j]))[i].height * texSize.y);
                    }
                    vsGpuParams->set(mVSTextureTable[j]->getName(), (const float*)(&(buffer[0])), (mAtlasTableDatas[j])->size());
                }
            }
        }
    }

    //-----------------------------------------------------------------------
    bool TextureAtlasSampler::preAddToRenderState(const RenderState* renderState,
        ShaderCh* srcPass, ShaderCh* dstPass)
    {
        mAtlasTexcoordPos = 0;

        const TextureAtlasSamplerFactory& factory = TextureAtlasSamplerFactory::getSingleton();

        bool hasAtlas = false;
        NCount texCount = srcPass->getTexture().getCount();
        for(NCount i = 0 ; i < texCount ; ++i)
        {
            ShaderChTextureUnit* pState = srcPass->getTexture().get(i);

            const TextureAtlasTablePtr& table = factory.getTextureAtlasTable(pState->getResourceID());
            if (table.isNull() == false)
            {
                if (table->size() > TAS_MAX_SAFE_ATLASED_TEXTURES)
                {
                    LogManager::getSingleton().logMessage(LML_CRITICAL,
                        "Warning : Compiling atlas texture has to many internally defined textures. Shader may fail to compile.");
                }
                if (i >= TAS_MAX_TEXTURES)
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                        "Texture atlas sub-render does not support more than TAS_MAX_TEXTURES (4) atlas textures");
                }
                if (pState->getType() != TEX_TYPE_2D)
                {
                    N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                        "Texture atlas sub-render state only supports 2d textures.");
                }

                mAtlasTableDatas[i] = table;
                mTextureAddressings[i] = pState->getAddressingMode();
                mIsAtlasTextureUnits[i] = true;
                hasAtlas = true;
            }
        }

        //gather the materials atlas processing attributes
        //and calculate the position of the indexes
        TextureAtlasSamplerFactory::TextureAtlasAttib attrib;
        factory.hasMaterialAtlasingAttributes(srcPass->getParent()->getParent(), &attrib);

        mAutoAdjustPollPosition = attrib.autoBorderAdjust;
        mAtlasTexcoordPos = attrib.positionOffset;
        if (attrib.positionMode == TextureAtlasSamplerFactory::ipmRelative)
        {
            mAtlasTexcoordPos += texCount - 1;
        }

        return hasAtlas;
    }

    TextureAtlasSamplerFactory::TextureAtlasSamplerFactory()
    {

    }

    TextureAtlasSamplerFactory* TextureAtlasSamplerFactory::getSingletonPtr(void)
    {
        return msSingleton;
    }

    TextureAtlasSamplerFactory& TextureAtlasSamplerFactory::getSingleton(void)
    {
        assert( msSingleton );  return ( *msSingleton );
    }
    //-----------------------------------------------------------------------
    const String & TextureAtlasSamplerFactory::getType() const
    {
        return TextureAtlasSampler::Type;
    }
    //-----------------------------------------------------------------------
    SubRenderState*	TextureAtlasSamplerFactory::createInstance(ScriptCompiler* compiler,
        PropertyScriptNode* prop, ShaderCh* pass, SGScriptAnalyze* translator)
    {
        return NULL;
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::writeInstance(MaterialSerializer* ser,
        SubRenderState * subRenderState, ShaderCh * srcPass, ShaderCh * dstPass)
    {
    }
    //-----------------------------------------------------------------------
    bool TextureAtlasSamplerFactory::addTexutreAtlasDefinition(const Ogre::String & filename,
        TextureAtlasTablePtr textureAtlasTable)
    {
        std::ifstream inp;
        bool temp;
        inp.open(filename.c_str(), std::ios::in | std::ios::binary);
        if(!inp)
        {
            N_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "'" + filename + "' file not found!",
                "TextureAtlasSamplerFactory::addTexutreAtlasDefinition" );
        }
        DataStream * stream = N_new FileStreamDataStream(filename, &inp, false);
        temp = addTexutreAtlasDefinition(stream, textureAtlasTable);
        N_delete stream;
        return temp;
    }
    //-----------------------------------------------------------------------
    bool TextureAtlasSamplerFactory::addTexutreAtlasDefinition(DataStream * stream,
        TextureAtlasTablePtr textureAtlasTable)
    {
        stream->seek(0);

        bool isSuccess = false;
        if(stream->isReadable() == true)
        {
            TextureAtlasMap tmpMap;

            while(stream->end() == false)
            {
                String line = stream->readLine(true);
                size_t nonWhiteSpacePos = line.find_first_not_of(" \t\r\n");
                //check this is a line with information
                if((nonWhiteSpacePos != String::npos) && (line[nonWhiteSpacePos] != '#'))
                {
                    //parse the line
                    vector<String>::type strings = StringUtil::split(line, ",\t");

                    if (strings.size() > 8)
                    {
                        String textureName = strings[1];

                        TextureAtlasMap::iterator it = tmpMap.find(textureName);
                        if (tmpMap.find(textureName) == tmpMap.end())
                        {
                            it = tmpMap.insert(TextureAtlasMap::value_type(textureName, TextureAtlasTablePtr(new TextureAtlasTable))).first;
                        }

                        // file line format:  <original texture filename>/t/t<atlas filename>, <atlas idx>, <atlas type>, <woffset>, <hoffset>, <depth offset>, <width>, <height>
                        //                              0                           1              2            3             4          5          6               7        8
                        TextureAtlasRecord newRecord(
                            strings[0], // original texture filename
                            strings[1], // atlas filename
                            (float)StringConverter::parseReal(strings[4]), // woffset
                            (float)StringConverter::parseReal(strings[5]), // hoffset
                            (float)StringConverter::parseReal(strings[7]), // width
                            (float)StringConverter::parseReal(strings[8]), // height
                            it->second->size() // texture index in atlas texture
                            );

                        it->second->push_back(newRecord);
                        if (!textureAtlasTable.isNull())
                        {
                            textureAtlasTable->push_back(newRecord);
                        }

                        isSuccess = true;
                    }
                }
            }

            //place the information in the main texture
            size_t maxTextureCount = 0;
            TextureAtlasMap::const_iterator it = tmpMap.begin();
            TextureAtlasMap::const_iterator itEnd = tmpMap.end();
            for(;it != itEnd; ++it)
            {
                setTextureAtlasTable(it->first, it->second);
                maxTextureCount = std::max<size_t>(maxTextureCount, it->second->size());
            }

            if (maxTextureCount > TAS_MAX_SAFE_ATLASED_TEXTURES)
            {
                LogManager::getSingleton().logMessage(LML_CRITICAL,
                    ("Warning : " + stream->getName() +
                    " atlas texture has to many internally defined textures. Shader may fail to compile."));
            }
        }
        return isSuccess;
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::setTextureAtlasTable(const String & textureName,
        const TextureAtlasTablePtr & atlasData, bool autoBorderAdjust)
    {
        if ((atlasData.isNull() == true) || (atlasData->empty()))
            removeTextureAtlasTable(textureName);
        else mAtlases.insert(TextureAtlasMap::value_type(textureName, atlasData));
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::removeTextureAtlasTable(const String& textureName)
    {
        mAtlases.erase(textureName);
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::removeAllTextureAtlasTables()
    {
        mAtlases.clear();
    }
    //-----------------------------------------------------------------------
    const TextureAtlasTablePtr & TextureAtlasSamplerFactory::getTextureAtlasTable(const String& textureName) const
    {
        TextureAtlasMap::const_iterator it = mAtlases.find(textureName);
        if (it != mAtlases.end())
        {
            return it->second;
        }
        else return c_BlankAtlasTable;
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::setDefaultAtlasingAttributes(
        IndexPositionMode mode, ushort offset, bool autoAdjustBorders)
    {
        mDefaultAtlasAttrib = TextureAtlasAttib(mode, offset, autoAdjustBorders);
    }
    //-----------------------------------------------------------------------
    const TextureAtlasSamplerFactory::TextureAtlasAttib & TextureAtlasSamplerFactory::getDefaultAtlasingAttributes() const
    {
        return mDefaultAtlasAttrib;
    }
    //-----------------------------------------------------------------------
    void TextureAtlasSamplerFactory::setMaterialAtlasingAttributes(Ogre::Material* material,
        IndexPositionMode mode, ushort offset, bool autoAdjustBorders)
    {
        if((material) && (material->getCount()))
        {
            TextureAtlasAttib * temp = N_new TextureAtlasAttib(mode, offset, autoAdjustBorders)
            material->get(0)->setExtData(c_RTAtlasKey, temp);
        }
    }
    //-----------------------------------------------------------------------
    bool TextureAtlasSamplerFactory::hasMaterialAtlasingAttributes(
        Ogre::Material * material, TextureAtlasAttib * attrib) const
    {
        bool isMaterialSpecific = false;
        if((material) && (material->getCount()))
        {
            void * anyAttrib =
                //find if the "IsTerrain" flag exists in the first technique
                material->get(0)->getExtData(c_RTAtlasKey);
            isMaterialSpecific = (anyAttrib != 0);
            if((isMaterialSpecific) && (attrib))
            {
                *attrib = static_cast<TextureAtlasAttib *>(anyAttrib);
            }
        }
        if ((!isMaterialSpecific) && (attrib))
        {
            *attrib = mDefaultAtlasAttrib;
        }
        return isMaterialSpecific;
    }

    //-----------------------------------------------------------------------
    SubRenderState*	TextureAtlasSamplerFactory::createInstanceImpl()
    {
        return N_new TextureAtlasSampler;
    }
}
}
#endif