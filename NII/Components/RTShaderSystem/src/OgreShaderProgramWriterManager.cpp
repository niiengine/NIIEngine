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

#include "OgreShaderProgramWriterManager.h"

namespace Ogre {

//-----------------------------------------------------------------------
template<> 
RTShader::ProgramWriterManager* Singleton<RTShader::ProgramWriterManager>::msSingleton = 0;

namespace RTShader {
//-----------------------------------------------------------------------
ProgramWriterManager* ProgramWriterManager::getSingletonPtr(void)
{
	assert( msSingleton );  
	return msSingleton;
}
//-----------------------------------------------------------------------
ProgramWriterManager& ProgramWriterManager::getSingleton(void)
{  
	assert( msSingleton );  
	return ( *msSingleton );  
}
//-----------------------------------------------------------------------
ProgramWriterManager::ProgramWriterManager()
{

}
//-----------------------------------------------------------------------
ProgramWriterManager::~ProgramWriterManager()
{

}
//-----------------------------------------------------------------------
void ProgramWriterManager::add(ProgramWriterFactory* factory)
{
	mFactories[factory->getTargetLanguage()] = factory;
}
//-----------------------------------------------------------------------
void ProgramWriterManager::remove(ProgramWriterFactory* factory)
{
	// Remove only if equal to registered one, since it might overridden
	// by other plugins
	FactoryMap::iterator it = mFactories.find(factory->getTargetLanguage());
	if (it != mFactories.end() && it->second == factory)
	{
		mFactories.erase(it);
	}
}
//-----------------------------------------------------------------------
bool ProgramWriterManager::isLanguageSupported(const String& lang)
{
	FactoryMap::iterator i = mFactories.find(lang);

	return i != mFactories.end();
}
//-----------------------------------------------------------------------
ProgramWriter* ProgramWriterManager::createProgramWriter( const String& language)
{
	FactoryMap::iterator it = mFactories.find(language);

	if (it != mFactories.end())
	{
		return (it->second)->create();
	}
	
	N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, 
		"Could not create ShaderProgramWriter unknown language ", 
		"ShaderProgramWriterManager::createProgramWriter" );
}

}
}
