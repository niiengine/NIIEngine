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
#include "VolumeRenderable.h"
#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <NiiVertexBuffer.h>
#include <NiiIndexBuffer.h>
#include <OgreGBufferManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreTextureUnitState.h>
#include <OgreTextureManager.h>
#include <OgreMaterialManager.h>
using namespace Ogre;

VolumeRenderable::VolumeRenderable(size_t nSlices, float size, const String &texture):
	mSlices(nSlices),
	mSize(size),
	mTexture(texture)
{
	mRadius = sqrtf(size*size+size*size+size*size)/2.0f;
	mBox = Ogre::AABox(-size, -size, -size, size, size, size);

	// No shadows
	setCastShadow(false);

	init();
}
VolumeRenderable::~VolumeRenderable()
{
	// Remove private material
	MaterialManager::getSingleton().remove(mTexture);
    // need to release index and vertex created for renderable
    delete mRenderOp.mIndex;
    delete mRenderOp.mVertex;
}

void VolumeRenderable::_notify(Camera * cam)
{
	SpaceObj::_notify(cam);

	// Fake orientation toward camera
	Vector3 zVec = getSpaceNode()->getSpacePos() - cam->getSpacePos();
	zVec.normalise();
	Vector3 fixedAxis = cam->getSpaceOri() * Vector3::Y ;

	Vector3 xVec = fixedAxis.crossProduct(zVec);
	xVec.normalise();

	Vector3 yVec = zVec.crossProduct(xVec);
	yVec.normalise();

	Quaternion oriQuat;
	oriQuat.from(xVec, yVec, zVec);

	oriQuat.to(mFakeOrientation);

	Matrix3 tempMat;
	Quaternion q = getSpaceNode()->getSpaceOri().inverseUnit() * oriQuat ;
	q.to(tempMat);

	Matrix4 rotMat = Matrix4::IDENTITY;
	rotMat = tempMat;
	rotMat.setTrans(Vector3(0.5f, 0.5f, 0.5f));
	mUnit->setMatrix(rotMat);
}

void VolumeRenderable::getMatrix(Matrix4 * xform, NCount & count) const
{
	Matrix4 destMatrix(Matrix4::IDENTITY); // this initialisation is needed

	const Vector3 & position = getSpaceNode()->getSpacePos();
	const Vector3 & scale = getSpaceNode()->getSpaceScale();
	Matrix3 scale3x3(Matrix3::ZERO);
	scale3x3[0][0] = scale.x;
	scale3x3[1][1] = scale.y;
	scale3x3[2][2] = scale.z;

	destMatrix = mFakeOrientation * scale3x3;
	destMatrix.setTrans(position);

	*xform = destMatrix;
    count = 1;
}

void VolumeRenderable::init()
{
	// Create geometry
	size_t nvertices = mSlices*4; // n+1 planes
	size_t elemsize = 3*3;
	size_t dsize = elemsize*nvertices;
	size_t x;

	Ogre::IndexData *idata = new Ogre::IndexData();
	Ogre::VertexData *vdata = new Ogre::VertexData();

	// Create  structures
	float *vertices = new float[dsize];

	float coords[4][2] = {
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f}
	};
	for(x=0; x<mSlices; x++)
	{
		for(size_t y=0; y<4; y++)
		{
			float xcoord = coords[y][0]-0.5f;
			float ycoord = coords[y][1]-0.5f;
			float zcoord = -((float)x/(float)(mSlices-1)  - 0.5f);
			// 1.0f .. a/(a+1)
			// coordinate
			vertices[x*4*elemsize+y*elemsize+0] = xcoord*(mSize/2.0f);
			vertices[x*4*elemsize+y*elemsize+1] = ycoord*(mSize/2.0f);
			vertices[x*4*elemsize+y*elemsize+2] = zcoord*(mSize/2.0f);
			// normal
			vertices[x*4*elemsize+y*elemsize+3] = 0.0f;
			vertices[x*4*elemsize+y*elemsize+4] = 0.0f;
			vertices[x*4*elemsize+y*elemsize+5] = 1.0f;
			// tex
			vertices[x*4*elemsize+y*elemsize+6] = xcoord*sqrtf(3.0f);
			vertices[x*4*elemsize+y*elemsize+7] = ycoord*sqrtf(3.0f);
			vertices[x*4*elemsize+y*elemsize+8] = zcoord*sqrtf(3.0f);
		}
	}
	unsigned short * faces = new unsigned short[mSlices * 6];
	for(x = 0; x < mSlices; ++x)
	{
		faces[x * 6 + 0] = x * 4 + 0;
		faces[x * 6 + 1] = x * 4 + 1;
		faces[x * 6 + 2] = x * 4 + 2;
		faces[x * 6 + 3] = x * 4 + 1;
		faces[x * 6 + 4] = x * 4 + 2;
		faces[x * 6 + 5] = x * 4 + 3;
	}
	// Setup buffers
	vdata->mOffset = 0;
	vdata->mCount = nvertices;

	NCount offset = 0;
	vdata->add(DT_Float3x, VT_Pos, 0, offset);
	offset += VertexUnit::getTypeSize(DT_Float3x);
	vdata->add(DT_Float3x, VT_Normals, 0, offset);
	offset += VertexUnit::getTypeSize(DT_Float3x);
	vdata->add(DT_Float3x, VT_Tex_Coord, 0, offset);
	offset += VertexUnit::getTypeSize(DT_Float3x);

	VertexBuffer * vbuf;
	N_Only(GBuffer).create(vbuf, offset, nvertices, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	vdata->attach(0, vbuf);

	vbuf->write(vertices, 0, vbuf->getSize());

	IndexBuffer * ibuf;
    N_Only(GBuffer).create(ibuf, 16, mSlices * 6, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	idata->mBuffer = ibuf;
	idata->mCount = mSlices * 6;
	idata->mOffset = 0;
	ibuf->write(faces, 0, ibuf->getSize());

	// Delete temporary buffers
	delete [] vertices;
	delete [] faces;

	// Now make the render operation
	mRenderOp.mType = Ogre::GeometryRaw::OT_TRIANGLE_LIST;
	mRenderOp.mIndex = idata;
	mRenderOp.mVertex = vdata;
	mRenderOp.add(GRT_Index);

	 // Create a brand new private material
	if (!ResourceSchemeManager::getSingleton().isExist(GroupID "VolumeRenderable"))
	{
		ResourceSchemeManager::getSingleton().create(GroupID "VolumeRenderable");
	}
	MaterialPtr material = N_Only(Material).create(mTexture, "VolumeRenderable", false, 0); // Manual, loader

	// Remove pre-created technique from defaults
	material->removeAllTechniques();

	// Create a techinique and a pass and a texture unit
 	ShaderFusion * technique = material->createTechnique();
	ShaderCh * pass = technique->create();
	ShaderChTextureUnit * textureUnit = pass->getTexture().create();

	// Set pass parameters
	pass->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_ALPHA);
	pass->setStencil(ShaderCh::SOT_Set)->setDepthWrite(false);
	pass->setCullingMode(CM_None);
	pass->setLight(false);

	// Set texture unit parameters
    textureUnit->setUAddressing(TAM_CLAMP);
    textureUnit->setVAddressing(TAM_CLAMP);
    stextureUnit->etWAddressing(TAM_CLAMP);
	textureUnit->setSrc(mTexture, TEX_TYPE_3D);
	textureUnit->setFilter(TFM_TRILINEAR);

	mUnit = textureUnit;
	mMaterial = material;
}

Ogre::NIIf VolumeRenderable::getCenterRange() const
{
	return mRadius;
}
Ogre::NIIf VolumeRenderable::distanceSquared(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;

	min = mBox.getMin();
	max = mBox.getMax();
	mid = ((min - max) * 0.5) + min;
	dist = cam->getSpacePos() - mid;

	return dist.lengthSquared();
}

