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

#include "GBufferMaterialGenerator.h"

#include "OgreMaterialManager.h"
#include "OgreGpuProgramManager.h"
#include "OgreStringConverter.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreTechnique.h"

//Use this directive to control whether you are writing projective (regular) or linear depth.
#define WRITE_LINEAR_DEPTH

//This is the concrete implementation of the material generator.
class GBufferMaterialGeneratorImpl : public MaterialGenerator::Impl
{
public:
	GBufferMaterialGeneratorImpl(const Ogre::String& baseName) : mBaseName(baseName) {}
protected:
	Ogre::String mBaseName;
	virtual Ogre::GpuProgram * generateVertexShader(MaterialGenerator::Perm permutation);
	virtual Ogre::GpuProgram * generateFragmentShader(MaterialGenerator::Perm permutation);
	virtual Ogre::MaterialPtr generateTemplateMaterial(MaterialGenerator::Perm permutation);

};

GBufferMaterialGenerator::GBufferMaterialGenerator() 
{
	vsMask = VS_MASK;
	fsMask = FS_MASK;
	matMask = MAT_MASK;
	materialBaseName = "DeferredShading/GBuffer/";
	mImpl = new GBufferMaterialGeneratorImpl(materialBaseName);
}

Ogre::GpuProgram * GBufferMaterialGeneratorImpl::generateVertexShader(MaterialGenerator::Perm permutation)
{
	Ogre::StringStream ss;

	ss << "void ToGBufferVP(" << std::endl;
	ss << "	float4 iPosition : POSITION," << std::endl;
	ss << "	float3 iNormal   : NORMAL," << std::endl;

	Ogre::uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;
	for (Ogre::uint32 i=0; i<numTexCoords; i++)
	{
		ss << "	float2 iUV" << i << " : TEXCOORD" << i << ',' << std::endl;
	}

	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	float3 iTangent : TANGENT0," << std::endl;
	}

	//TODO : Skinning inputs
	ss << std::endl;



	ss << "	out float4 oPosition : POSITION," << std::endl;
#ifdef WRITE_LINEAR_DEPTH
    ss << "	out float3 oViewPos : TEXCOORD0," << std::endl;
#else
	ss << "	out float oDepth : TEXCOORD0," << std::endl;
#endif
	ss << "	out float3 oNormal : TEXCOORD1," << std::endl;
	int texCoordNum = 2;
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	out float3 oTangent : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		ss << "	out float3 oBiNormal : TEXCOORD" << texCoordNum++ << ',' << std::endl;
	}
	for (Ogre::uint32 i=0; i<numTexCoords; i++)
	{
		ss << "	out float2 oUV" << i << " : TEXCOORD" << texCoordNum++ << ',' << std::endl;
	}

	ss << std::endl;

	ss << "	uniform float4x4 cWorldViewProj," << std::endl;
	ss << "	uniform float4x4 cWorldView" << std::endl;

	ss << "	)" << std::endl;


	ss << "{" << std::endl;
	ss << "	oPosition = mul(cWorldViewProj, iPosition);" << std::endl;
	ss << "	oNormal = mul(cWorldView, float4(iNormal,0)).xyz;" << std::endl;
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	oTangent = mul(cWorldView, float4(iTangent,0)).xyz;" << std::endl;
		ss << "	oBiNormal = cross(oNormal, oTangent);" << std::endl;
	}

#ifdef WRITE_LINEAR_DEPTH
    ss << "	oViewPos = mul(cWorldView, iPosition).xyz;" << std::endl;
#else
	ss << "	oDepth = oPosition.w;" << std::endl;
#endif

	for (Ogre::uint32 i=0; i<numTexCoords; i++) {
		ss << "	oUV" << i << " = iUV" << i << ';' << std::endl;
	}

	ss << "}" << std::endl;

	Ogre::String programSource = ss.str();
	Ogre::String programName = mBaseName + "VP_" + Ogre::StrConv::conv(permutation);

#if OGRE_DEBUG_MODE
	Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programSource);
#endif

	// Create shader object
	Ogre::HighLevelGpuProgramPtr ptrProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
		programName, NII::GroupDefault, "cg", Ogre::GPT_VERTEX_PROGRAM);
	ptrProgram->setProgramCode(programSource);
	ptrProgram->set("entry_point","ToGBufferVP");
	ptrProgram->set("profiles","vs_1_1 arbvp1");

	const Ogre::GpuProgramParam * params = ptrProgram->createDefaultParam();
	params->set("cWorldViewProj", Ogre::GpuProgramParam::GSP_Matrix_Proj_X_View_X_Space);
	params->set("cWorldView", Ogre::GpuProgramParam::GSP_Matrix_View_X_Space);
	ptrProgram->load();

	return Ogre::GpuProgram(ptrProgram);
}

Ogre::GpuProgram * GBufferMaterialGeneratorImpl::generateFragmentShader(MaterialGenerator::Perm permutation)
{
	Ogre::StringStream ss;

	ss << "void ToGBufferFP(" << std::endl;
#ifdef WRITE_LINEAR_DEPTH
    ss << "	float3 iViewPos : TEXCOORD0," << std::endl;
#else
    ss << "	float1 iDepth : TEXCOORD0," << std::endl;
#endif
	ss << "	float3 iNormal   : TEXCOORD1," << std::endl;

	int texCoordNum = 2;
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	float3 iTangent : TEXCOORD" << texCoordNum++ << ',' << std::endl;
		ss << "	float3 iBiNormal : TEXCOORD" << texCoordNum++ << ',' << std::endl;
	}

	Ogre::uint32 numTexCoords = (permutation & GBufferMaterialGenerator::GBP_TEXCOORD_MASK) >> 8;
	for (Ogre::uint32 i=0; i<numTexCoords; i++)
	{
		ss << "	float2 iUV" << i << " : TEXCOORD" << texCoordNum++ << ',' << std::endl;
	}

	ss << std::endl;

	ss << "	out float4 oColor0 : COLOR0," << std::endl;
	ss << "	out float4 oColor1 : COLOR1," << std::endl;

	ss << std::endl;

	int samplerNum = 0;
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	uniform sampler sNormalMap : register(s" << samplerNum++ << ")," << std::endl;
	}
	Ogre::uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;
	for (Ogre::uint32 i=0; i<numTextures; i++) {
		ss << "	uniform sampler sTex" << i << " : register(s" << samplerNum++ << ")," << std::endl;
	}
    if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
	{
		ss << "	uniform float4 cDiffuseColour," << std::endl;
	}

#ifdef WRITE_LINEAR_DEPTH
    ss << "	uniform float cFarDistance," << std::endl;
#endif

	ss << "	uniform float cSpecularity" << std::endl;

	ss << "	)" << std::endl;


	ss << "{" << std::endl;

	if (numTexCoords > 0 && numTextures > 0)
	{
		ss << "	oColor0.rgb = tex2D(sTex0, iUV0);" << std::endl;
        if (permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
        {
            ss << "	oColor0.rgb *= cDiffuseColour.rgb;" << std::endl;
        }
	}
    else
	{
		ss << "	oColor0.rgb = cDiffuseColour.rgb;" << std::endl;
	}


	ss << "	oColor0.a = cSpecularity;" << std::endl;
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		ss << "	float3 texNormal = (tex2D(sNormalMap, iUV0)-0.5)*2;" << std::endl;
		ss << "	float3x3 normalRotation = float3x3(iTangent, iBiNormal, iNormal);" << std::endl;
		ss << "	oColor1.rgb = normalize(mul(texNormal, normalRotation));" << std::endl;
	} else
	{
		ss << "	oColor1.rgb = normalize(iNormal);" << std::endl;
	}
#ifdef WRITE_LINEAR_DEPTH
    ss << "	oColor1.a = length(iViewPos) / cFarDistance;" << std::endl;
#else
    ss << "	oColor1.a = iDepth;" << std::endl;
#endif

	ss << "}" << std::endl;

	Ogre::String programSource = ss.str();
	Ogre::String programName = mBaseName + "FP_" + Ogre::StrConv::conv(permutation);

#if OGRE_DEBUG_MODE
	Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(programSource);
#endif

	// Create shader object
	Ogre::HighLevelGpuProgramPtr ptrProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
		programName, NII::GroupDefault, "cg", Ogre::GPT_FRAGMENT_PROGRAM);
	ptrProgram->setProgramCode(programSource);
	ptrProgram->set("entry_point","ToGBufferFP");
	ptrProgram->set("profiles","ps_2_0 arbfp1");

	const Ogre::GpuProgramParam * params = ptrProgram->createDefaultParam();
	params->set("cSpecularity", Ogre::GpuProgramParam::GSP_Ch_Shininess);
	if (numTextures == 0 || permutation & GBufferMaterialGenerator::GBP_HAS_DIFFUSE_COLOUR)
	{
		params->set("cDiffuseColour", Ogre::GpuProgramParam::GSP_Ch_Diffuse);
	}

#ifdef WRITE_LINEAR_DEPTH
    //TODO : Should this be the distance to the far corner, not the far clip distance?
    params->set("cFarDistance", Ogre::GpuProgramParam::GSP_Camera_FarClip);
#endif

	ptrProgram->load();
	return Ogre::GpuProgram(ptrProgram);
}

Ogre::MaterialPtr GBufferMaterialGeneratorImpl::generateTemplateMaterial(MaterialGenerator::Perm permutation)
{
	Ogre::String matName = mBaseName + "Mat_" + Ogre::StrConv::conv(permutation);

	Ogre::MaterialPtr matPtr = NII::N_Only(Material).create(matName, NII::GroupDefault);
	Ogre::ShaderCh * pass = matPtr->get(0)->get(0);
	pass->setName(mBaseName + "Pass_" + Ogre::StrConv::conv(permutation));
	pass->setLight(false);
	if (permutation & GBufferMaterialGenerator::GBP_NORMAL_MAP)
	{
		pass->getTexture().create();
	}
	Ogre::uint32 numTextures = permutation & GBufferMaterialGenerator::GBP_TEXTURE_MASK;
	for (Ogre::uint32 i=0; i<numTextures; i++)
	{
		pass->getTexture().create();
	}

	return matPtr;
}
