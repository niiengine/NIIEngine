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
#ifndef __XSIMATERIALEXPORTER_H__
#define __XSIMATERIALEXPORTER_H__

#include "OgreXSIHelper.h"
#include "OgreBlendMode.h"
#include "OgreMaterialSerializer.h"

namespace Ogre {

	class XsiMaterialExporter
	{
	public:
		XsiMaterialExporter();
		virtual ~XsiMaterialExporter();

		/** Export a set of XSI materials to a material script.
		@param materials List of materials to export
		@param filename Name of the script file to create
		@param copyTextures Whether to copy any textures used into the same
			folder as the material script.
		*/
		void exportMaterials(MaterialMap& materials, TextureProjectionMap& texProjMap, 
			const String& filename, bool copyTextures);
	protected:	
		MaterialSerializer mMatSerializer;
		
		typedef std::multimap<long,ShaderChTextureUnit*> TextureUnitTargetMap;
		/// Map of target id -> texture unit to match up tex transforms
		TextureUnitTargetMap mTextureUnitTargetMap;
		/// ShaderCh queue, used to invert ordering
		PassQueue mPassQueue;
		/// Texture projection map
		TextureProjectionMap mTextureProjectionMap;

		
		// Export a single material
		void exportMaterial(MaterialEntry* matEntry, bool copyTextures, 
			const String& texturePath);
		/// Fill in all the details of a pass
		void populatePass(ShaderCh* pass, XSI::Shader& xsishader);
		/// Fill in the texture units - note this won't pick up transforms yet
		void populatePassTextures(ShaderCh* pass, PassEntry* passEntry, 
			bool copyTextures, const String& texturePath);
		/// Find any texture transforms and hook them up via 'target'
		void populatePassTextureTransforms(ShaderCh* pass, XSI::Shader& xsishader);
		/// Populate basic rejection parameters for the pass
		void populatePassDepthCull(ShaderCh* pass, XSI::Shader& xsishader);
		/// Populate lighting parameters for the pass
		void populatePassLighting(ShaderCh* pass, XSI::Shader& xsishader);
		/// Populate scene blending parameters for the pass
		void populatePassSceneBlend(ShaderCh* pass, XSI::Shader& xsishader);
		void populatePassCgPrograms(ShaderCh* pass, XSI::Shader& xsishader);
		void populatePassHLSLPrograms(ShaderCh* pass, XSI::Shader& xsishader);
		void populatePassD3DAssemblerPrograms(ShaderCh* pass, XSI::Shader& xsishader);
		void populateOGLFiltering(ShaderChTextureUnit* tex, XSI::Shader& xsishader);
		void populateDXFiltering(ShaderChTextureUnit* tex, XSI::Shader& xsishader);

		// Utility method to get texture coord set from tspace_id name
		unsigned short getTextureCoordIndex(const String& tspace);

		/// Add a 2D texture from a shader
		ShaderChTextureUnit * add2DTexture(ShaderCh* pass, XSI::Shader& shader, 
			bool copyTextures, const String& targetFolder);
		/// Add a cubic texture from a shader
		ShaderChTextureUnit * addCubicTexture(ShaderCh* pass, XSI::Shader& shader, 
			bool copyTextures, const String& targetFolder);

		void clearPassQueue(void);

		ColourFactor convertSceneBlend(short xsiVal);
		TextureAddressingMode convertAddressingMode(short xsiVal);
		void convertTexGenOGL(ShaderChTextureUnit* tex, long xsiVal, XSI::Shader& shader);
		void convertTexGenDX(ShaderChTextureUnit* tex, long xsiVal, XSI::Shader& shader);
	};
}
#endif