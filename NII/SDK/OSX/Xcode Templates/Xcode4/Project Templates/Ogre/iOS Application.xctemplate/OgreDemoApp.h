//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_H
#define OGRE_DEMO_H

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

#ifdef USE_RTSHADER_SYSTEM
#include "OgreRTShaderSystem.h"

/** This class demonstrates basic usage of the RTShader system.
 It sub class the material manager listener class and when a target scheme callback
 is invoked with the shader generator scheme it tries to create an equivalent shader
 based technique based on the default technique of the given material.
 */
class ShaderGeneratorTechniqueResolverListener : public NII::MaterialMatchListener
{
public:

	ShaderGeneratorTechniqueResolverListener(NII::RTShader::ShaderGenerator* pShaderGenerator)
	{
		mShaderGenerator = pShaderGenerator;
	}

	/** This is the hook point where shader based technique will be created.
     It will be called whenever the material manager won't find appropriate technique
     that satisfy the target scheme name. If the scheme name is out target RT Shader System
     scheme name we will try to create shader generated technique for it.
     */
	virtual NII::ShaderFusion * miss(NII::Material * originalMaterial, NII::SchemeID schemeName,
        const NII::GeometryObj * rend, unsigned short lodIndex)
	{
		NII::Technique * generatedTech = NULL;

		// Case this is the default shader generator scheme.
		if (schemeName == NII::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
		{
			bool techniqueCreated;

			// Create shader generated technique for this material.
			techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
                originalMaterial->getName(), 0, schemeName);

			// Case technique registration succeeded.
			if (techniqueCreated)
			{
				// Force creating the shaders for the generated technique.
				mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());

				// Grab the generated technique.
				NII::ShaderFusionList::iterator i, iend = originalMaterial->getFusionList().end();
                for(i = originalMaterial->getFusionList().begin(); i != iend; ++i)
				{
					NII::ShaderFusion * sf = *i;
					if(sf->getSchemeID() == schemeName)
					{
						generatedTech = sf;
						break;
					}
				}
			}
		}
		return generatedTech;
	}

protected:
	Ogre::RTShader::ShaderGenerator*	mShaderGenerator;			// The shader generator instance.
};
#endif

class DemoApp : public OIS::KeyListener
{
public:
	DemoApp();
	~DemoApp();

	void startDemo();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

private:
    void setupDemoScene();
	void runDemo();
    bool initializeRTShaderSystem(Ogre::SpaceManager* sceneMgr);
    void finalizeRTShaderSystem();

	Ogre::SpaceNode*			m_pCubeNode;
	Ogre::GeometrySpace*				m_pCubeEntity;

	bool					m_bShutdown;
#ifdef USE_RTSHADER_SYSTEM
    Ogre::RTShader::ShaderGenerator*			mShaderGenerator;			// The Shader generator instance.
    ShaderGeneratorTechniqueResolverListener*	mMaterialMgrListener;		// Shader generator material manager listener.
#endif // USE_RTSHADER_SYSTEM

};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
