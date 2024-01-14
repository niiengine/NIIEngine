#ifndef __BezierPatch_H__
#define __BezierPatch_H__

#include "SdkSample.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_BezierPatch : public SdkSample
{
public:
	Sample_BezierPatch()
	{
		mInfo["Title"] = "Bezier Patch";
		mInfo["Description"] = "A demonstration of the Bezier patch support.";
		mInfo["Thumbnail"] = "thumb_bezier.png";
		mInfo["Category"] = "Geometry";
	}

	void checkBoxToggled(CheckBox * box)
	{
		mPatchPass->setPolygonMode(box->isChecked() ? PM_LINE : PM_SOLID);

#ifdef USE_RTSHADER_SYSTEM

		// Invalidate material in order to reflect polygon mode change in the generated shader based pass.
		mShaderGenerator->invalidateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, mPatchPass->getParent()->getParent()->getName());
#endif
	}

	void sliderMoved(Slider* slider)
	{
		mPatch->setSubdivision(slider->getValue());
	}

protected:

#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #pragma pack(push, 1)
#endif
    struct PatchVertex
	{
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    #pragma pack(pop)
#endif

	void setupContent()
	{
		// setup some basic lighting for our scene
		mSceneMgr->setAmbient(Colour(0.5, 0.5, 0.5));
        mSceneMgr->createLight(0)->setPos(100, 100, 100);

		// define the control point vertices for our patch
		PatchVertex verts[9] =
		{
			{-50, -35, -50, -0.5, 0.5, 0.0, 0.0, 0.0},
			{  0,   0, -50,  0.0, 0.5, 0.0, 0.5, 0.0},
			{ 50,  35, -50,  0.5, 0.5, 0.0, 1.0, 0.0},
			{-50,   0,   0, -0.5, 0.5, 0.0, 0.0, 0.5},
			{  0,   0,   0,  0.0, 0.5, 0.0, 0.5, 0.5},
			{ 50,   0,   0,  0.5, 0.5, 0.0, 1.0, 0.5},
			{-50,  35,  50, -0.5, 0.5, 0.0, 0.0, 1.0},
			{  0,   0,  50,  0.0, 0.5, 0.0, 0.5, 1.0},
			{ 50, -35,  50,  0.5, 0.5, 0.0, 1.0, 1.0}
		};

        VertexBuffer * vb;
		// 3 floats for position, 3 floats for normal, 2 floats for UV
        N_Only(GBuffer).create(mDecl);
        N_Only(GBuffer).create(vb, 32, 9, Buffer::M_DEVWRITE);
        mDecl->add(DT_Float3x, VT_Pos, 0, 0);
        mDecl->add(DT_Float3x, VT_Normals, 0, sizeof(float) * 3);
        mDecl->add(DT_Float2x, VT_Tex_Coord, 0, sizeof(float) * 6, 0);
        mDecl->attach(0, mDecl);
        void * lockP = vb->lock(MM_WRITE | MM_WHOLE);
        memcpy(lockP, (void *)verts, 288);
        vb->unlock();
		// create a patch mesh using vertices and declaration
        mPatch = N_Only(Mesh).createBezierPatch("patch",
			GroupDefault, mDecl, 3, 3, 5, 5, MeshManager::FST_Both);

        mPatch->setSubdivision(0);   // start at 0 detail

		// create a patch entity from the mesh, give it a material, and attach it to the origin
        GeometrySpace * ent = mSceneMgr->createGeo("Patch", "patch");
		ent->setMaterialName("Examples/BumpyMetal");
        mSceneMgr->getOriginNode()->attachObject(ent);

		// save the main pass of the material so we can toggle wireframe on it
		mPatchPass = ent->getCom(0)->getMaterial()->get(0)->get(0);

		// use an orbit style camera
		mCameraMan->setStyle(CS_ORBIT);
		mCameraMan->setYawPitchDist(Angle(0), Angle(30), 250);

		mTrayMgr->showCursor();

		// create slider to adjust detail and checkbox to toggle wireframe
		mTrayMgr->createThickSlider(TL_TOPLEFT, "Detail", "Detail", 120, 44, 0, 1, 6);
		mTrayMgr->createCheckBox(TL_TOPLEFT, "Wireframe", "Wireframe", 120);
	}

    void cleanupContent()
    {
        N_Only(GBuffer).destroy(mDecl);
		mPatchPass->setPolygonMode(PM_SOLID);
		MeshManager::getSingleton().remove(mPatch->getPrc());
    }

    VertexData * mDecl;
	PatchMesh * mPatch;
	Pass * mPatchPass;
};

#endif
