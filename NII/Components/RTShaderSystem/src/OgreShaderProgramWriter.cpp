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
#include "OgreShaderProgramWriter.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#	define ENDL "\n"
#else
#	define ENDL std::endl
#endif

namespace Ogre {
namespace RTShader {

//-----------------------------------------------------------------------
void ProgramWriter::writeProgramTitle(
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	StringSerialiser& os,
#else
	std::ostream &os,
#endif
	Program* program)
{
	os << "//-----------------------------------------------------------------------------" << ENDL;
	os << "// Program Type: ";
	switch(program->getType())
	{
	case GPT_VERTEX_PROGRAM:
		os << "Vertex shader";
		break;
	case GPT_FRAGMENT_PROGRAM:
		os << "Fragment shader";
		break;
	case GPT_GEOMETRY_PROGRAM:
		os << "Geometry shader";
		break;	
	default:
		break;
	}
	os << ENDL;
	os << "// Language: " <<  getTargetLanguage() << ENDL;
	os << "// Created by Ogre RT Shader Generator. All rights reserved." << ENDL;
	os << "//-----------------------------------------------------------------------------" << ENDL;
}

//-----------------------------------------------------------------------
void ProgramWriter::writeUniformParametersTitle(
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	StringSerialiser& os,
#else
	std::ostream &os,
#endif
	Program* program)
{
	os << "//-----------------------------------------------------------------------------" << ENDL;
	os << "//                         GLOBAL PARAMETERS" << ENDL;
	os << "//-----------------------------------------------------------------------------" << ENDL;
}
//-----------------------------------------------------------------------
void ProgramWriter::writeFunctionTitle(
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
	StringSerialiser& os,
#else
	std::ostream &os,
#endif 
	Function* function)
{
	os << "//-----------------------------------------------------------------------------" << ENDL;
	os << "// Function Name: " <<  function->getName() << ENDL;
	os << "// Function Desc: " <<  function->getDescrip() << ENDL;
	os << "//-----------------------------------------------------------------------------" << ENDL;
}

}
}
