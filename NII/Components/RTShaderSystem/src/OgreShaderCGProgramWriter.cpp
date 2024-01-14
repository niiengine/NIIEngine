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

#include "OgreShaderCGProgramWriter.h"
#include "OgreStringConverter.h"
#include "OgreGpuProgramManager.h"

namespace Ogre {
namespace RTShader {

String CGProgramWriter::TargetLanguage = "cg";

//-----------------------------------------------------------------------
CGProgramWriter::CGProgramWriter()
{
	initializeStringMaps();
}

//-----------------------------------------------------------------------
CGProgramWriter::~CGProgramWriter()
{

}

//-----------------------------------------------------------------------
void CGProgramWriter::initializeStringMaps()
{
	mGpuConstTypeMap[GDT_Float] = "float";
	mGpuConstTypeMap[GDT_Float2X] = "float2";
	mGpuConstTypeMap[GDT_Float3X] = "float3";
	mGpuConstTypeMap[GDT_Float4X] = "float4";
	mGpuConstTypeMap[GDT_Sampler1D] = "sampler1D";
	mGpuConstTypeMap[GDT_Sampler2D] = "sampler2D";
	mGpuConstTypeMap[GDT_Sampler3D] = "sampler3D";
	mGpuConstTypeMap[GDT_SamplerCube] = "samplerCUBE";
	mGpuConstTypeMap[GDT_Matrix2X2] = "float2x2";
	mGpuConstTypeMap[GDT_Matrix2X3] = "float2x3";
	mGpuConstTypeMap[GDT_Matrix2X4] = "float2x4";
	mGpuConstTypeMap[GDT_Matrix3X2] = "float3x2";
	mGpuConstTypeMap[GDT_Matrix3X3] = "float3x3";
	mGpuConstTypeMap[GDT_Matrix3X4] = "float3x4";
	mGpuConstTypeMap[GDT_Matrix4X2] = "float4x2";
	mGpuConstTypeMap[GDT_Matrix4X3] = "float4x3";
	mGpuConstTypeMap[GDT_Matrix4X4] = "float4x4";
	mGpuConstTypeMap[GDT_Int] = "int";
	mGpuConstTypeMap[GDT_Int2X] = "int2";
	mGpuConstTypeMap[GDT_Int3X] = "int3";
	mGpuConstTypeMap[GDT_Int4X] = "int4";


	mParamSemanticMap[Parameter::SPS_POSITION] = "POSITION";
	mParamSemanticMap[Parameter::SPS_BLEND_WEIGHTS] = "BLENDWEIGHT";
	mParamSemanticMap[Parameter::SPS_BLEND_INDICES] = "BLENDINDICES";
	mParamSemanticMap[Parameter::SPS_NORMAL] = "NORMAL";
	mParamSemanticMap[Parameter::SPS_COLOR] = "COLOR";
	mParamSemanticMap[Parameter::SPS_TEXTURE_COORDINATES] = "TEXCOORD";
	mParamSemanticMap[Parameter::SPS_BINORMAL] = "BINORMAL";
	mParamSemanticMap[Parameter::SPS_TANGENT] = "TANGENT";
	mParamSemanticMap[Parameter::SPS_UNKNOWN] = "";
}

//-----------------------------------------------------------------------
void CGProgramWriter::writeSourceCode(std::ostream& os, Program* program)
{
	const ShaderFunctionList& functionList = program->getFunctions();
	ShaderFunctionConstIterator itFunction;

	const UniformParameterList& parameterList = program->getParameters();
	UniformParameterConstIterator itUniformParam = parameterList.begin();

	// Generate source code header.
	writeProgramTitle(os, program);
	os << std::endl;

	// Generate dependencies.
	writeProgramDependencies(os, program);
	os << std::endl;

	// Generate global variable code.
	writeUniformParametersTitle(os, program);
	os << std::endl;

	for (itUniformParam=parameterList.begin();  itUniformParam != parameterList.end(); ++itUniformParam)
	{
		writeUniformParameter(os, *itUniformParam);			
		os << ";" << std::endl;				
	}
	os << std::endl;

	// Write program function(s).
	for (itFunction=functionList.begin(); itFunction != functionList.end(); ++itFunction)
	{
		Function* curFunction = *itFunction;
		ParameterPtr colorParameter;

		writeFunctionTitle(os, curFunction);
		writeFunctionDeclaration(os, curFunction, colorParameter);

		os << "{" << std::endl;

		// Write local parameters.
		const ShaderParameterList& localParams = curFunction->getLocalParameters();
		ShaderParameterConstIterator itParam; 

		for (itParam=localParams.begin();  itParam != localParams.end(); ++itParam)
		{
			os << "\t";
			writeLocalParameter(os, *itParam);			
			os << ";" << std::endl;						
		}

		// Sort and write function atoms.
		curFunction->sortAtomInstances();

		const FunctionAtomInstanceList& atomInstances = curFunction->getAtomInstances();
		FunctionAtomInstanceConstIterator itAtom;

		for (itAtom=atomInstances.begin(); itAtom != atomInstances.end(); ++itAtom)
		{			
			writeAtomInstance(os, *itAtom);
		}
		os << "}" << std::endl;
	}
	os << std::endl;
}
//-----------------------------------------------------------------------
void CGProgramWriter::writeProgramDependencies(std::ostream& os, Program* program)
{
	os << "//-----------------------------------------------------------------------------" << std::endl;
	os << "//                         PROGRAM DEPENDENCIES" << std::endl;
	os << "//-----------------------------------------------------------------------------" << std::endl;

	for(unsigned int i=0; i < program->getDependencyCount(); ++i)
	{
		const String& curDependency = program->getDependency(i);

		os << "#include " << '\"' << curDependency << "." << getTargetLanguage() << '\"' << std::endl;
	}
}

//-----------------------------------------------------------------------
void CGProgramWriter::writeUniformParameter(std::ostream& os, UniformParameterPtr parameter)
{
	os << mGpuConstTypeMap[parameter->getType()];
	os << "\t";	
	os << parameter->getName();	
	if(parameter->isArray() == true)
	{
		os << "[" << parameter->getSize() << "]";	
	}
	
	if(parameter->isSampler())
	{
		os << " : register(s" << parameter->getIndex() << ")";		
	}
}
//-----------------------------------------------------------------------
void CGProgramWriter::writeFunctionParameter(std::ostream& os, ParameterPtr parameter)
{
	os << mGpuConstTypeMap[parameter->getType()];

	os << "\t";	
	os << parameter->getName();	
	if(parameter->isArray() == true)
	{
		os << "[" << parameter->getSize() << "]";	
	}
	
	if(parameter->getSemantic() != Parameter::SPS_UNKNOWN)
	{
		os << " : ";
		os << mParamSemanticMap[parameter->getSemantic()];

		if(parameter->getSemantic() != Parameter::SPS_POSITION && 
			parameter->getSemantic() != Parameter::SPS_NORMAL &&
			parameter->getSemantic() != Parameter::SPS_TANGENT &&
			parameter->getSemantic() != Parameter::SPS_BLEND_INDICES &&
			parameter->getSemantic() != Parameter::SPS_BLEND_WEIGHTS &&
			(!(parameter->getSemantic() == Parameter::SPS_COLOR && parameter->getIndex() == 0)) &&
			parameter->getIndex() >= 0)
		{			
			os << StrConv::conv(parameter->getIndex()).c_str();
		}
	}
}

//-----------------------------------------------------------------------
void CGProgramWriter::writeLocalParameter(std::ostream& os, ParameterPtr parameter)
{
	os << mGpuConstTypeMap[parameter->getType()];
	os << "\t";	
	os << parameter->getName();		
	if (parameter->isArray() == true)
	{
		os << "[" << parameter->getSize() << "]";	
	}
}

//-----------------------------------------------------------------------
void CGProgramWriter::writeFunctionDeclaration(std::ostream & os, Function * function,
    ParameterPtr & colorParameter)
{
	const ShaderParameterList& inParams  = function->getInputParameters();
	const ShaderParameterList& outParams = function->getOutputParameters();


	os << "void";
	os << " ";

	os << function->getName();
	os << std::endl << "\t(" << std::endl;

	ShaderParameterConstIterator it;
	size_t paramsCount = inParams.size() + outParams.size();
	size_t curParamIndex = 0;

	// Write input parameters.
	for (it=inParams.begin(); it != inParams.end(); ++it)
	{					
		os << "\t in ";

		writeFunctionParameter(os, *it);

		if (curParamIndex + 1 != paramsCount)		
			os << ", " << std::endl;

		curParamIndex++;
	}


	// Write output parameters.
	for (it=outParams.begin(); it != outParams.end(); ++it)
	{
		os << "\t out ";
		writeFunctionParameter(os, *it);

		if (curParamIndex + 1 != paramsCount)				
			os << ", " << std::endl;

		curParamIndex++;
	}	

	os << std::endl << "\t)" << std::endl;
}

//-----------------------------------------------------------------------
void CGProgramWriter::writeAtomInstance(std::ostream& os, FunctionAtom* atom)
{
	os << std::endl << "\t";
	atom->writeSourceCode(os, getTargetLanguage());
	os << std::endl;
}

}
}
