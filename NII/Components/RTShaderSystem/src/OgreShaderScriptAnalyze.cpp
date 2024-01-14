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
#include "OgreShaderScriptAnalyze.h"
#include "OgrePass.h"
#include "OgreTechnique.h"
#include "OgreMaterial.h"
#include "OgreShaderGenerator.h"


namespace Ogre 
{
namespace RTShader 
{

//-----------------------------------------------------------------------------
SGScriptAnalyze::SGScriptAnalyze() :
	mGeneratedRenderState(NULL)
{
}
//-----------------------------------------------------------------------------
void SGScriptAnalyze::translate(ScriptCompiler* compiler, const ScriptNode * node)
{
	ObjectScriptNode * obj = reinterpret_cast<ObjectScriptNode *>(node);
	ObjectScriptNode * parent = reinterpret_cast<ObjectScriptNode *>(obj->parent);

	// Translate section within a pass context.
	if(parent->mClass == "pass")
	{
		translatePass(compiler, node);
	}
	if(parent->mClass == "texture_unit")
	{
		translateTextureUnit(compiler, node);
	}
}
//-----------------------------------------------------------------------------
SubRenderState* SGScriptAnalyze::getGeneratedSubRenderState(const String& typeName)
{
	//check if we are in the middle of parsing
	if (mGeneratedRenderState)
	{
		/** Get the list of the template sub render states composing this render state. */
		const SubRenderStateList& rsList =
			mGeneratedRenderState->getTemplateSubRenderStateList();
		
		SubRenderStateList::const_iterator it = rsList.begin();
		SubRenderStateList::const_iterator itEnd = rsList.end();
		for(; it != itEnd; ++it)
		{
			if ((*it)->getType() == typeName)
				return *it;
		}
	}
	return NULL;
}
	
//-----------------------------------------------------------------------------
/*
note: we can know the texture unit index by getting parent then finding it in the list of children
*/
void SGScriptAnalyze::translateTextureUnit(ScriptCompiler* compiler, const ScriptNode * node)
{
	ObjectScriptNode *obj = reinterpret_cast<ObjectScriptNode *>(node);	
	ShaderChTextureUnit* texState = any_cast<ShaderChTextureUnit *>(obj->parent->context);
	ShaderCh* pass = texState->getParent();
	ShaderFusion* technique = pass->getParent();
	Material* material = technique->getParent();
	ShaderGenerator* shaderGenerator = ShaderGenerator::getSingletonPtr();
	String dstTechniqueSchemeName = obj->name;
	bool techniqueCreated;

	// Make sure the scheme name is valid - use default if none exists.
	if (dstTechniqueSchemeName.empty())	
		dstTechniqueSchemeName = ShaderGenerator::DEFAULT_SCHEME_NAME;	


	//check if technique already created
	techniqueCreated = shaderGenerator->hasShaderBasedTechnique(material->getName(), 
		material->getGroup(),
		technique->getSchemeID(), 
		dstTechniqueSchemeName);
	
	if (techniqueCreated == false)
	{
		// Create the shader based technique.
		techniqueCreated = shaderGenerator->createShaderBasedTechnique(material->getName(), 
			material->getGroup(),
			technique->getSchemeID(), 
			dstTechniqueSchemeName,
			shaderGenerator->getCreateShaderOverProgrammablePass());
	}
			
	// Case technique successfully created.
	if (techniqueCreated)
	{
		//Attempt to get the render state which might have been created by the pass parsing
		mGeneratedRenderState = shaderGenerator->getRenderState(dstTechniqueSchemeName, 
            material->getName(), material->getGroup(), pass->getIndex());
	
		// Go over all the render state properties.
		for(ScriptNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
		{
			if((*i)->type == SNT_Property)
			{
				PropertyScriptNode * prop = reinterpret_cast<PropertyScriptNode *>(*i);
				SubRenderState * subRenderState = ShaderGenerator::getSingleton().createSubRenderState(compiler, prop, texState, this);
				
				if (subRenderState)
				{
					addSubRenderState(subRenderState, dstTechniqueSchemeName, material->getName(), 
						material->getGroup(), pass->getIndex());
				}
			}
			else
			{
				ScriptAnalyze::analyze(compiler, *i);
			}
		}
		mGeneratedRenderState = NULL;
	}	
}


//-----------------------------------------------------------------------------
void SGScriptAnalyze::translatePass(ScriptCompiler* compiler, const ScriptNode * node)
{
	ObjectScriptNode * obj = reinterpret_cast<ObjectScriptNode*>(node);	
	ShaderCh * pass = any_cast<ShaderCh*>(obj->parent->context);
	ShaderFusion * technique = pass->getParent();
	Material * material = technique->getParent();
	ShaderGenerator * shaderGenerator = ShaderGenerator::getSingletonPtr();
	String dstTechniqueSchemeName = obj->name;
	bool techniqueCreated;

	// Make sure the scheme name is valid - use default if none exists.
	if(dstTechniqueSchemeName.empty())	
		dstTechniqueSchemeName = ShaderGenerator::DEFAULT_SCHEME_NAME;	

	// Create the shader based technique.
	techniqueCreated = shaderGenerator->createShaderBasedTechnique(material->getName(), 
		material->getGroup(), 
		technique->getSchemeID(), 
		dstTechniqueSchemeName,
		shaderGenerator->getCreateShaderOverProgrammablePass());


	// Case technique successfully created.
	if (techniqueCreated)
	{
		// Go over all the render state properties.
		for(ScriptNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
		{
			if((*i)->type == SNT_Property)
			{
				PropertyScriptNode *prop = reinterpret_cast<PropertyScriptNode*>((*i).get());
				SubRenderState* subRenderState;

				// Handle light count property.
				if (prop->name == "light_count")
				{
					if (prop->values.size() != 3)
					{
						compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
					}
					else
					{
						int lightCount[3];

						if (false == SGScriptAnalyze::get(prop->values, 0, 3, lightCount))
						{
							compiler->error(prop, ScriptCompiler::CE_INVALIDPARAMETERS);
						}
						else
						{
							shaderGenerator->createScheme(dstTechniqueSchemeName);
							RenderState* renderState = shaderGenerator->getRenderState(dstTechniqueSchemeName, 
								material->getName(), material->getGroup(), pass->getIndex());

							renderState->setLightCount(lightCount);
							renderState->setLightCountAutoUpdate(false);
						}
					}					
				}

				// Handle the rest of the custom properties.
				else
				{
					subRenderState = ShaderGenerator::getSingleton().createSubRenderState(compiler, prop, pass, this);
					if(subRenderState)
					{
						addSubRenderState(subRenderState, dstTechniqueSchemeName, material->getName(), material->getGroup(), pass->getIndex());
					}
				}				
			}
			else
			{
				ScriptAnalyze::analyze(compiler, *i);
			}
		}

		mGeneratedRenderState = NULL;
	}
}
//-----------------------------------------------------------------------------
void SGScriptAnalyze::addSubRenderState(SubRenderState* newSubRenderState, 
		const String& dstTechniqueSchemeName, const String& materialName, const String& groupName, unsigned short passIndex)
{
	assert(newSubRenderState);

	//check if a different sub render state of the same type already exists
	ShaderGenerator* shaderGenerator = ShaderGenerator::getSingletonPtr();
	
	//create a new scheme if needed
	shaderGenerator->createScheme(dstTechniqueSchemeName);
	
	//update the active render state
	mGeneratedRenderState = shaderGenerator->getRenderState(dstTechniqueSchemeName, materialName, groupName, passIndex);

	//add the new sub render state
	mGeneratedRenderState->addTemplateSubRenderState(newSubRenderState);
}

}
}
