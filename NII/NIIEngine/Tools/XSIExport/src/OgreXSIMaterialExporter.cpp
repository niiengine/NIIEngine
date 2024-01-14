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
#include "OgreXSIMaterialExporter.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreTextureUnitState.h"

#include <xsi_shader.h>
#include <xsi_imageclip.h>
#include <xsi_image.h>

namespace Ogre
{
	//-------------------------------------------------------------------------
	XsiMaterialExporter::XsiMaterialExporter()
	{

	}
	//-------------------------------------------------------------------------
	XsiMaterialExporter::~XsiMaterialExporter()
	{
		clearPassQueue();
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::exportMaterials(MaterialMap& materials,
		TextureProjectionMap& texProjMap, const String& filename,
		bool copyTextures)
	{
		LogOgreAndXSI("** Begin OGRE Material Export **");

		mTextureProjectionMap = texProjMap;

		String texturePath;
		if (copyTextures)
		{
			// derive the texture path
			String::size_type pos = filename.find_last_of("\\");
			if (pos == String::npos)
			{
				pos = filename.find_last_of("/");
			}
			if (pos != String::npos)
			{
				texturePath = filename.substr(0, pos + 1);
			}
		}

		mMatSerializer.clearQueue();

		for (MaterialMap::iterator m = materials.begin(); m != materials.end(); ++m)
		{
			exportMaterial(m->second, copyTextures, texturePath);
		}

		mMatSerializer.exportQueued(filename);

		LogOgreAndXSI("** OGRE Material Export Complete **");
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::exportMaterial(MaterialEntry * matEntry,
		bool copyTextures, const String & texturePath)
	{
		LogOgreAndXSI("Exporting " + matEntry->name);

		MaterialPtr mat = MaterialManager::getSingleton().create(
			matEntry->name, GroupDefault);

		ShaderFusion * t;
        mat->create(t);

		// collect the passes into our queue
		// XSI stores passes in reverse order, so invert them
		clearPassQueue();
		XSI::Shader shader(matEntry->xsiShader);
		PassEntry* passEntry = new PassEntry();
		mPassQueue.push_front(passEntry);
		while (1)
		{
			passEntry->shaders.Add(shader);

			XSI::CRef source = shader.GetParameter(L"previous").GetSource();
			if(!source.IsValid() || !source.IsA(XSI::siShaderID))
			{
				// finish
				break;
			}

			shader = XSI::Shader(source);
			// If we find a 'blending' parameter, we're on a new pass
			if (shader.GetParameter(L"blending").IsValid())
			{
				passEntry = new PassEntry();
				mPassQueue.push_front(passEntry); // push front to invert order
			}
		}

		// Now go through each pass and create OGRE version
		for (PassQueue::iterator p = mPassQueue.begin(); p != mPassQueue.end(); ++p)
		{
			PassEntry* passEntry = *p;
			ShaderCh* pass = t->create();
			LogOgreAndXSI("Added ShaderCh");

			// Need to pre-populate pass textures to match up transforms
			populatePassTextures(pass, passEntry, copyTextures, texturePath);
			// Do the rest
			for (int s = 0; s < passEntry->shaders.GetCount(); ++s)
			{
				XSI::Shader shader(passEntry->shaders[s]);
				populatePass(pass, shader);
			}
		}

		mMatSerializer.queueForExport(mat);
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::clearPassQueue(void)
	{
		for (PassQueue::iterator i = mPassQueue.begin(); i != mPassQueue.end(); ++i)
		{
			delete *i;
		}
		mPassQueue.clear();

	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePass(ShaderCh* pass, XSI::Shader& xsishader)
	{
		populatePassDepthCull(pass, xsishader);
		populatePassSceneBlend(pass, xsishader);
		populatePassLighting(pass, xsishader);
		populatePassTextureTransforms(pass, xsishader);
		populatePassCgPrograms(pass, xsishader);
		populatePassHLSLPrograms(pass, xsishader);
		populatePassD3DAssemblerPrograms(pass, xsishader);
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassCgPrograms(ShaderCh* pass,
		XSI::Shader& xsishader)
	{
		XSI::Parameter param = xsishader.GetParameter(L"Cg_Program");
		if (param.IsValid())
		{
			// TODO
			// XSI can't reference external files which makes it v.difficult to
			// re-use shaders - mod XSI plugin?
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassHLSLPrograms(ShaderCh* pass,
		XSI::Shader& xsishader)
	{
		XSI::Parameter param = xsishader.GetParameter(L"HLSL_Program");
		if (param.IsValid())
		{
			// TODO
			// XSI can't reference external files which makes it v.difficult to
			// re-use shaders - mod XSI plugin?
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassD3DAssemblerPrograms(ShaderCh* pass,
		XSI::Shader& xsishader)
	{
		XSI::Parameter param = xsishader.GetParameter(L"Vertex_Shader");
		if (param.IsValid())
		{
			// TODO
			// XSI can't reference external files which makes it v.difficult to
			// re-use shaders - mod XSI plugin?
		}
		param = xsishader.GetParameter(L"Pixel_Shader");
		if (param.IsValid())
		{
			// TODO
			// XSI can't reference external files which makes it v.difficult to
			// re-use shaders - mod XSI plugin?
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassDepthCull(ShaderCh * pass, XSI::Shader & xsishader)
	{
		XSI::Parameter param = xsishader.GetParameter(L"cullingmode");
		if(param.IsValid())
		{
			short xsiCull = param.GetValue();
			switch(xsiCull)
			{
			case 0:
				pass->setCullingMode(CM_None);
				pass->setSysCullingMode(SCM_NONE);
				break;
			case 1:
				pass->setCullingMode(CM_Clockwise);
				pass->setSysCullingMode(SCM_BACK);
				break;
			case 2:
				pass->setCullingMode(CM_Anticlockwise);
				pass->setSysCullingMode(SCM_FRONT);
				break;
			};
		}

		param = xsishader.GetParameter(L"depthtest");
		if(param.IsValid())
		{
			bool depthTest = param.GetValue();
			pass->setStencil(ShaderCh::SOT_Set)->setDepthEnable(depthTest);
		}
		param = xsishader.GetParameter(L"depthwrite");
		if (param.IsValid())
		{
			bool depthWrite = param.GetValue();
			pass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(depthWrite);
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassSceneBlend(ShaderCh * pass, XSI::Shader & xsishader)
	{
		XSI::Parameter param = xsishader.GetParameter(L"blending");
		if(param.IsValid() && (bool)param.GetValue())
		{
			ColourFactor src = CF_ONE;
			ColourFactor dst = CF_ONE;

			param = xsishader.GetParameter(L"srcblendingfunction");
			if(param.IsValid())
			{
				src = convertSceneBlend(param.GetValue());
			}
			param = xsishader.GetParameter(L"dstblendingfunction");
			if(param.IsValid())
			{
				dst = convertSceneBlend(param.GetValue());
			}

			pass->setBlend(ShaderCh::SOT_Set)->setBlend(src, dst);
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassLighting(ShaderCh * pass, XSI::Shader & xsishader)
	{
        ShaderChColour * scc = pass->setColour(ShaderCh::SOT_Set);
		XSI::Parameter param = xsishader.GetParameter(L"Enable_Lighting");
		if (param.IsValid())
		{
			pass->setLight(param.GetValue());

			Colour tmpColour;
			xsishader.GetColorParameterValue(L"Ambient", tmpColour.r, tmpColour.g,
				tmpColour.b, tmpColour.a);
			scc->setAmbient(tmpColour);
			xsishader.GetColorParameterValue(L"Diffuse", tmpColour.r, tmpColour.g,
				tmpColour.b, tmpColour.a);
			scc->setDiffuse(tmpColour);
			xsishader.GetColorParameterValue(L"Emissive", tmpColour.r, tmpColour.g,
				tmpColour.b, tmpColour.a);
			scc->setEmissive(tmpColour);
			xsishader.GetColorParameterValue(L"Specular", tmpColour.r, tmpColour.g,
				tmpColour.b, tmpColour.a);
			scc->setSpecular(tmpColour);

			scc->setShininess(xsishader.GetParameter(L"Shininess").GetValue());
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassTextures(ShaderCh * pass,
		PassEntry * passEntry, bool copyTextures, const String & targetFolder)
	{
		// We need to search all shaders back to the point we would change
		// passes, and add all the textures. This is because we don't know
		// where in the shaders the texture transforms might be, since they
		// are linked via 'target' not by being on the same object.
		mTextureUnitTargetMap.clear();

		for (int s = 0; s < passEntry->shaders.GetCount(); ++s)
		{
			XSI::Shader shader(passEntry->shaders[s]);
			ShaderChTextureUnit * tex = 0;

			String progID = XSItoOgre(shader.GetProgID());
			if (progID.find("OGL13Texture") != String::npos ||
				progID.find("DXTexture") != String::npos ||
				progID.find("OGLCom") != String::npos)
			{
				if (!shader.GetParameter(L"bottom").IsValid())
				{
					tex = add2DTexture(pass, shader, copyTextures, targetFolder);
				}
				else if (shader.GetParameter(L"bottom").IsValid())
				{
					tex = addCubicTexture(pass, shader, copyTextures, targetFolder);
				}
			}
			else
			{
				continue; // not a texture so skip the rest
			}


			// texture coordinate set
			XSI::Parameter param = shader.GetParameter(L"tspace_id");
			if (param.IsValid())
			{
				// this is a name, need to look up index
				tex->setCoordSet(
					getTextureCoordIndex(XSItoOgre(XSI::CString(param.GetValue()))));
			}

			// filtering & anisotropy
			// DX and GL shaders deal differently
			if (progID.find("OGL") != String::npos)
			{
				populateOGLFiltering(tex, shader);
			}
			else if (progID.find("DX") != String::npos)
			{
				populateDXFiltering(tex, shader);
			}

			// colour operation
			param = shader.GetParameter(L"modulation");
			if (param.IsValid())
			{
				long colourop = ((LONG)param.GetValue());
				switch (colourop)
				{
				case 0:
					// modulate
					tex->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
					break;
				case 1:
					// decal
					tex->getColourBlend()->setColourBlend(TBM_Src_Alpha_Dst);
					break;
				case 2:
					// blend
					tex->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
					break;
				case 3:
					// replace
					tex->getColourBlend()->setColourBlend(TBM_Src_Replace_Dst);
					break;
				case 4:
					// add
					tex->getColourBlend()->setColourBlend(TBM_Src_Add_Dst);
					break;
				}
			}
		}
	}
	//-------------------------------------------------------------------------
	ShaderChTextureUnit * XsiMaterialExporter::add2DTexture(ShaderCh * pass,
        XSI::Shader & shader, bool copyTextures, const String & targetFolder)
	{
		// create texture unit state and map from target incase future xforms
		ShaderChTextureUnit* tex = pass->getTexture().create();

		XSI::Parameter param = shader.GetParameter(L"target"); // OGL
		if (!param.IsValid())
			param = shader.GetParameter(L"Texture_Target"); // DX

		long target = ((LONG)param.GetValue());
		mTextureUnitTargetMap.insert(TextureUnitTargetMap::value_type(target, tex));

		// Get image
		XSI::CRef src = shader.GetParameter(L"Texture").GetSource();
		if (!src.IsValid() || !src.IsA(XSI::siImageClipID))
		{
			// Try Texture_1 (OGL Combined)
			src = shader.GetParameter(L"Texture_1").GetSource();

		}
		if (src.IsValid() && src.IsA(XSI::siImageClipID))
		{
			XSI::ImageClip imgClip(src);
			String srcTextureName =
				XSItoOgre(XSI::CString(imgClip.GetParameter(L"SourceFileName").GetValue()));

			String::size_type pos = srcTextureName.find_last_of("\\");
			if (pos == String::npos)
			{
				pos = srcTextureName.find_last_of("/");
			}
			String textureName =
				srcTextureName.substr(pos+1, srcTextureName.size() - pos - 1);
			String destTextureName = targetFolder + textureName;

			// copy texture if required
			if (copyTextures)
			{
				copyFile(srcTextureName, destTextureName);
			}

			LogOgreAndXSI("Adding texture " + textureName);
			tex->setSrc(textureName, Texture::T_2D);
		}
		return tex;
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populateOGLFiltering(ShaderChTextureUnit * tex,
		XSI::Shader & shader)
	{
		TextureFilterOP minFilter, mipFilter, magFilter;
		minFilter = TFOP_LINEAR;
		magFilter = TFOP_LINEAR;
		mipFilter = TFOP_POINT;
		XSI::Parameter param = shader.GetParameter(L"minfilter");
		if(param.IsValid())
		{
			// XSI OGL shader uses minfilter to determine mip too
			long filt = ((LONG)param.GetValue());
			switch(filt)
			{
			case 0:
				minFilter = TFOP_POINT;
				mipFilter = TFOP_NONE;
				break;
			case 1:
				minFilter = TFOP_LINEAR;
				mipFilter = TFOP_NONE;
				break;
			case 2:
				minFilter = TFOP_POINT;
				mipFilter = TFOP_POINT;
				break;
			case 3:
				minFilter = TFOP_POINT;
				mipFilter = TFOP_LINEAR;
				break;
			case 4:
				minFilter = TFOP_LINEAR;
				mipFilter = TFOP_POINT;
				break;
			case 5:
				minFilter = TFOP_LINEAR;
				mipFilter = TFOP_LINEAR;
				break;
			};

		}
		param = shader.GetParameter(L"magfilter");
		if (param.IsValid())
		{
			long filt = ((LONG)param.GetValue());
			switch(filt)
			{
			case 0:
				magFilter = TFOP_POINT;
				break;
			case 1:
				magFilter = TFOP_LINEAR;
				break;
			};
		}

		param = shader.GetParameter(L"anisotropy");
		if(param.IsValid())
		{
			long aniso = ((LONG)param.GetValue());
			if(aniso > 1)
			{
				// No specific aniso filtering option, so upgrade linear -> aniso
				if(minFilter == TFOP_LINEAR)
					minFilter = TFOP_ANISOTROPIC;
				if(magFilter == TFOP_LINEAR)
					magFilter = TFOP_ANISOTROPIC;
			}
			tex->setAnisotropy(aniso);
		}

		tex->setMinFilter(minFilter);
        tex->setMagFilter(magFilter);
        tex->setMipFilter(mipFilter);
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populateDXFiltering(ShaderChTextureUnit * tex,
		XSI::Shader & shader)
	{
		TextureFilterOP minFilter, mipFilter, magFilter;
		minFilter = TFOP_LINEAR;
		magFilter = TFOP_LINEAR;
		mipFilter = TFOP_POINT;
		XSI::Parameter param = shader.GetParameter(L"minfilter");
		if (param.IsValid())
		{
			// XSI DX shader has min/mag and mip, and has more options
			long filt = ((LONG)param.GetValue());
			switch(filt)
			{
			case 0:
				minFilter = TFOP_NONE;
				break;
			case 1:
				minFilter = TFOP_POINT;
				break;
			case 2:
				minFilter = TFOP_LINEAR;
				break;
			case 3:
			case 4: // we don't support cubic/gaussian, use aniso
			case 5:
				minFilter = TFOP_ANISOTROPIC;
				break;
			};
		}
		param = shader.GetParameter(L"magfilter");
		if (param.IsValid())
		{
			// XSI DX shader has mag/mag and mip, and has more options
			long filt = ((LONG)param.GetValue());
			switch(filt)
			{
			case 0:
				magFilter = TFOP_NONE;
				break;
			case 1:
				magFilter = TFOP_POINT;
				break;
			case 2:
				magFilter = TFOP_LINEAR;
				break;
			case 3:
			case 4: // we don't support cubic/gaussian, use aniso
			case 5:
				magFilter = TFOP_ANISOTROPIC;
				break;
			};
		}
		param = shader.GetParameter(L"mipfilter");
		if (param.IsValid())
		{
			// XSI DX shader has mip/mag and mip, and has more options
			long filt = ((LONG)param.GetValue());
			switch(filt)
			{
			case 0:
				mipFilter = TFOP_NONE;
				break;
			case 1:
				mipFilter = TFOP_POINT;
				break;
			case 2:
				mipFilter = TFOP_LINEAR;
				break;
			case 3:
			case 4: // we don't support cubic/gaussian, use aniso
			case 5:
				mipFilter = TFOP_ANISOTROPIC;
				break;
			};
		}
		// Aniso
		param = shader.GetParameter(L"anisotropy");
		if(param.IsValid())
		{
			long aniso = ((LONG)param.GetValue());
			tex->setAnisotropy(aniso);
		}

		tex->setMinFilter(minFilter);
        tex->setMagFilter(magFilter);
        tex->setMipFilter(mipFilter);
	}
	//-------------------------------------------------------------------------
	ShaderChTextureUnit * XsiMaterialExporter::addCubicTexture(ShaderCh * pass,
        XSI::Shader & shader, bool copyTextures, const String & targetFolder)
	{
		// create texture unit state and map from target incase future xforms
		ShaderChTextureUnit * tex = pass->getTexture().create();

		XSI::Parameter param = shader.GetParameter(L"target"); // OGL
		if(!param.IsValid())
			param = shader.GetParameter(L"Texture_Target"); // DX

		long target = ((LONG)param.GetValue());
		mTextureUnitTargetMap.insert(TextureUnitTargetMap::value_type(target, tex));

		// Get images
		wchar_t * cubeFaceName[6] = {L"front", L"back", L"top", L"bottom", L"left", L"right" };

		String finalNames[6];


		for (int face = 0; face < 6; ++face)
		{
			XSI::CRef src = shader.GetParameter(cubeFaceName[face]).GetSource();
			if (src.IsValid() && src.IsA(XSI::siImageClipID))
			{
				XSI::ImageClip imgClip(src);
				String srcTextureName =
					XSItoOgre(XSI::CString(imgClip.GetParameter(L"SourceFileName").GetValue()));

				String::size_type pos = srcTextureName.find_last_of("\\");
				if (pos == String::npos)
				{
					pos = srcTextureName.find_last_of("/");
				}
				finalNames[face] = srcTextureName.substr(pos+1, srcTextureName.size() - pos - 1);
				String destTextureName = targetFolder + finalNames[face];

				// copy texture if required
				if (copyTextures)
				{
					copyFile(srcTextureName, destTextureName);
				}


				LogOgreAndXSI("Cubemap face: " + srcTextureName);
			}
		}

		LogOgreAndXSI("Adding cubic texture");
		// Cannot do combinedUVW for now, DevIL can't write DDS cubemap so
		// go for separates (user can modify)
		tex->setSrc(finalNames, T_CUBE_2D);

		return tex;
	}
	//-------------------------------------------------------------------------
	unsigned short XsiMaterialExporter::getTextureCoordIndex(const String & tspace)
	{
		TextureProjectionMap::iterator i = mTextureProjectionMap.find(tspace);
		if(i != mTextureProjectionMap.end())
		{
			return i->second;
		}
		else
		{
			// default
			return 0;
		}
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::populatePassTextureTransforms(ShaderCh * pass,
		XSI::Shader & shader)
	{
		// TODO
		// Check we have the right object
		XSI::Parameter param = shader.GetParameter(L"wrap_u");
		if(param.IsValid())
		{

			// addressing mode
            TextureAddressing uvwadd;
			uvwadd.mS_U = convertAddressingMode(param.GetValue());
			// default other dimensions incase not supplied
			uvwadd.mT_V = uvwadd.mS_U;
			uvwadd.mR_W = uvwadd.mS_U;

			param = shader.GetParameter(L"wrap_v");
			if(param.IsValid())
			{
				uvwadd.mT_V = convertAddressingMode(param.GetValue());
			}
			param = shader.GetParameter(L"wrap_w");
			if(param.IsValid())
			{
				uvwadd.mR_W = convertAddressingMode(param.GetValue());
			}

			// transform
			bool usexform = false;
			Matrix4 xform = Matrix4::IDENTITY;
			param = shader.GetParameter(L"Transform");
			if(param.IsValid() && (bool)param.GetValue())
			{
				Quaternion qx, qy, qz, qfinal;
				qx.from(Angle(shader.GetParameter(L"rotx").GetValue()), Vector3::X);
				qy.from(Angle(shader.GetParameter(L"roty").GetValue()), Vector3::Y);
				qz.from(Angle(shader.GetParameter(L"rotz").GetValue()), Vector3::Z);
				qfinal = qx * qy * qz;

				Vector3 trans;
				trans.x = shader.GetParameter(L"trsx").GetValue();
				trans.y = shader.GetParameter(L"trsy").GetValue();
				trans.z = shader.GetParameter(L"trsz").GetValue();

				Matrix3 rot3x3, scale3x3;
				qfinal.to(rot3x3);
				scale3x3 = Matrix3::ZERO;
				scale3x3[0][0] = shader.GetParameter(L"sclx").GetValue();
				scale3x3[1][1] = shader.GetParameter(L"scly").GetValue();
				scale3x3[2][2] = shader.GetParameter(L"sclz").GetValue();

				xform = rot3x3 * scale3x3;
				xform.setTrans(trans);
				usexform = true;
			}

			// Look up texture unit(s) that are using this target
			long target = ((LONG)shader.GetParameter(L"Texture_Target").GetValue());
			TextureUnitTargetMap::iterator i = mTextureUnitTargetMap.find(target);
			while(i != mTextureUnitTargetMap.end() && i->first == target)
			{
				ShaderChTextureUnit * tex = i->second;
                tex->setUAddressing(uvwadd.mS_U);
                tex->setVAddressing(uvwadd.mT_V);
                tex->setWAddressing(uvwadd.mR_W);
				if(usexform)
					tex->setMatrix(xform);

				// texgen (not texcoord_index as in OGRE!)
				// Can turn into 2 different calls
				param = shader.GetParameter(L"texcoord_index");
				if(param.IsValid())
				{
					long e = ((LONG)param.GetValue());
					if(e != 0)
					{
						// Not Explicit
						// details differ per DX/OGL
						if(XSItoOgre(shader.GetProgID()).find("DX") != String::npos)
						{
							convertTexGenDX(tex, e, shader);
						}
						else
						{
							convertTexGenOGL(tex, e, shader);
						}
					}
				}
				++i;
			}
		}
		param = shader.GetParameter(L"Additive_Transform");
		if(param.IsValid())
		{
			unsigned short target = shader.GetParameter(L"Texture_Coord_ID").GetValue();
			if(pass->getTexture().getCount() > target)
			{
				ShaderChTextureUnit * tex = pass->getTexture().get(target);

				long uvType = ((LONG)shader.GetParameter(L"UV_Type").GetValue());
				if(uvType != 0)
				{
					double val1 = shader.GetParameter(L"Val1").GetValue();
					double val2 = shader.GetParameter(L"Val2").GetValue();
					long wave = ((LONG)shader.GetParameter(L"Wave").GetValue());
					WaveformType wft;
					switch(wave)
					{
					case 1:
						wft = WFT_SINE;
						break;
					case 2:
						wft = WFT_TRIANGLE;
						break;
					case 3:
						wft = WFT_SQUARE;
						break;
					case 4:
						wft = WFT_SAWTOOTH;
						break;
					case 5:
						wft = WFT_INVERSE_SAWTOOTH;
						break;
					}
					double base = shader.GetParameter(L"Base").GetValue();
					double amp = shader.GetParameter(L"Amplitude").GetValue();
					double phase = shader.GetParameter(L"Phase").GetValue();
					double freq = shader.GetParameter(L"Frequency").GetValue();

					switch(uvType)
					{
					case 1:
						// translate
                        tex->setUOft(val1);
                        tex->setVOft(val2);
						break;
					case 2:
						// rotate
						tex->setRotate(Angle(val1));
						break;
					case 3:
						// scale
                        tex->setUScale(val1);
                        tex->setVScale(val2);
						break;
					case 4:
						// scroll
						if(wave != 0)
						{
                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, tex,
                                true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(
                                true, wft, base, freq, phase, amp, true);
                            tex->addAnimation(TTT_U_OFT, val, func);

                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, tex,
                                false, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(
                                true, wft, base, freq, phase, amp, true);
                            tex->addAnimation(TTT_V_OFT, val, func);
						}
						else
						{
							tex->setScrollAnimation(val1, val2);
						}
						break;
					case 5:
						// turn
						if(wave != 0)
						{
                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, tex,
                                false, false, false, false, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(
                                true, wft, base, freq, phase, amp, true);
                            tex->addAnimation(TTT_ROTATE, val, func);
						}
						else
						{
							tex->setRotateAnimation(val1 / 360.0f);
						}
						break;
					case 6:
						// stretch (only wave)
						if(wave != 0)
						{
                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, tex,
                                false, false, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(
                                true, wft, base, freq, phase, amp, true);
                            tex->addAnimation(TTT_U_SCALE, val, func);

                            DataValue<NIIf> * val = N_new TexCoordModifierControllerValue(true, tex,
                                false, false, false, true);
                            DataFunc<NIIf, NIIf> * func = N_new WaveformControllerFunction(
                                true, wft, base, freq, phase, amp, true);
                            tex->addAnimation(TTT_V_SCALE, val, func);
						}
						break;
					}
				}
			}
		}

		// if more than one entry for the same target is found, it is ok for the
		// latter to take precedence since this is what happens in XSI.
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::convertTexGenOGL(ShaderChTextureUnit * tex,
		long xsiVal, XSI::Shader & shader)
	{
		switch(xsiVal)
		{
			// no 0
		case 1:
			// Object linear
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Planar);
			break;
		case 2:
			// Eye linear (texture projection)
            tex->setProjTexture(true);
            tex->setProjFrustum(0);
			break;
		case 3:
			// Sphere map
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Sphere);
			break;
		case 4:
			// Reflection map
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Reflection);
			break;
		case 5:
			// Normal map
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Normal);
			break;
		};
	}
	//-------------------------------------------------------------------------
	void XsiMaterialExporter::convertTexGenDX(ShaderChTextureUnit * tex,
		long xsiVal, XSI::Shader & shader)
	{
		switch(xsiVal)
		{
			// no 0
		case 1:
			// Normal in camera space
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Sphere);
			break;
		case 2:
			// Position in camera space
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Planar);
			break;
		case 3:
			// Reflection vector in camera space
            tex->setEnvMap(true);
            tex->setEnvMapType(ShaderChTextureUnit::ENM_Reflection);
			break;
		};
	}
	//-------------------------------------------------------------------------
	TextureAddressingMode XsiMaterialExporter::convertAddressingMode(short xsiVal)
	{
		// same for OGL and DX
		switch(xsiVal)
		{
		case 0:
			return ShaderChTextureUnit::TAM_REPEAT;
		case 1:
			return ShaderChTextureUnit::TAM_MIRROR;
		case 2:
			return ShaderChTextureUnit::TAM_CLAMP;
		case 3:
			// border?
			return ShaderChTextureUnit::TAM_CLAMP;
		case 4:
			// mirror once
			return ShaderChTextureUnit::TAM_MIRROR;
		case 5:
			// clamp to edge
			return ShaderChTextureUnit::TAM_CLAMP;
		};

		// Keep compiler happy
		return ShaderChTextureUnit::TAM_REPEAT;
	}
	//-------------------------------------------------------------------------
	ColourFactor XsiMaterialExporter::convertSceneBlend(short xsiVal)
	{
		switch(xsiVal)
		{
		case 0:
			return CF_ZERO;
		case 1:
			return CF_ONE;
		case 2:
			return CF_DST;
		case 3:
			return CF_INV_DST;
		case 4:
			return CF_SRC_ALPHA;
		case 5:
			return CF_INV_SRC_ALPHA;
		case 6:
			return CF_DST_ALPHA;
		case 7:
			return CF_INV_DST_ALPHA;
		};
		return CF_ZERO;
	}
    //-------------------------------------------------------------------------
}