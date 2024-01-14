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
#include "ThingRenderable.h"
#include <NiiVertexBuffer.h>
#include <NiiIndexBuffer.h>
#include <NiiGBufferManager.h>
#include <OgreCamera.h>
using namespace Ogre;

ThingRenderable::ThingRenderable(float radius, size_t count, float qsize):
	mRadius(radius),
	mCount(count),
	mQSize(qsize)
{
	mBox = Ogre::AABox(-radius, -radius, -radius, radius, radius, radius);
	init();
	fillBuffer();
}
ThingRenderable::~ThingRenderable()
{
    // need to release index and vertex created for renderable
    delete mRenderOp.mIndex;
    delete mRenderOp.mVertex;
}

void ThingRenderable::addTime(float t)
{
	for(size_t x = 0; x < mCount; x++)
	{
		Quaternion dest = things[x] * orbits[x];
		things[x] = things[x] + t * (dest - things[x]);
		things[x].normalise();
	}
	fillBuffer();
}
// Generate float between -1 and 1
float randFloat()
{
	return ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
}
void ThingRenderable::init()
{
	// Fill array with randomly oriented quads
	Vector3 ax, ay, az;
	size_t x;
	Quaternion q;
	things.clear(); orbits.clear();
	for(x = 0; x < mCount; x++)
	{
		ax = Vector3(randFloat(), randFloat(), randFloat());
		ay = Vector3(randFloat(), randFloat(), randFloat());
		az = ax.crossProduct(ay);
		ay = az.crossProduct(ax);
		ax.normalise(); ay.normalise(); az.normalise();
		q.from(ax, ay, az);
		//std::cerr << ax.dotProduct(ay) << " " << ay.dotProduct(az) << " " << az.dotProduct(ax) << std::endl;
		things.push_back(q);

		ax = Vector3(randFloat(), randFloat(), randFloat());
		ay = Vector3(randFloat(), randFloat(), randFloat());
		az = ax.crossProduct(ay);
		ay = az.crossProduct(ax);
		ax.normalise(); ay.normalise(); az.normalise();
		q.from(ax, ay, az);
		orbits.push_back(q);
	}

	// Create buffers
	size_t nvertices = mCount*4; // n+1 planes
	//size_t elemsize = 2*3; // position, normal
	//size_t dsize = elemsize*nvertices;

	Ogre::IndexData * idata = new Ogre::IndexData();
	Ogre::VertexData * vdata;
    NII::N_Only(GBuffer).create(vdata);

	// Quads
	unsigned short * faces = new unsigned short[mCount*6];
	for(x = 0; x < mCount; x++)
	{
		faces[x*6+0] = x*4+0;
		faces[x*6+1] = x*4+1;
		faces[x*6+2] = x*4+2;
		faces[x*6+3] = x*4+0;
		faces[x*6+4] = x*4+2;
		faces[x*6+5] = x*4+3;
	}
	// Setup buffers
	vdata->mOffset = 0;
	vdata->mCount = nvertices;

	size_t offset = 0;
	vdata->add(DT_Float3x, VT_Pos, 0, offset);
	offset += VertexUnit::getTypeSize(DT_Float3x);

    N_Only(GBuffer).create(vbuf, offset, nvertices, Buffer::M_WRITE);

	vdata->attach(0, vbuf);

	//vbuf->write(vertices, 0, vbuf->getSize());

	IndexBuffer * ibuf;
    N_Only(GBuffer).create(ibuf, 16, mCount * 6, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	idata->mBuffer = ibuf;
	idata->mCount = mCount * 6;
	idata->mOffset = 0;
	ibuf->write(faces, 0, ibuf->getSize());

	// Delete temporary buffers
	delete [] faces;

	// Now make the render operation
	mRenderOp.mType = Ogre::GeometryRaw::OT_TRIANGLE_LIST;
	mRenderOp.mIndex = idata;
	mRenderOp.mVertex = vdata;
	mRenderOp.add(GRT_Index);
}

void ThingRenderable::fillBuffer()
{
	// Transfer vertices and normals
	float * vIdx = static_cast<float *>(vbuf->lock(Ogre::Buffer::MM_WHOLE));
	size_t elemsize = 1 * 3; // position only
	size_t planesize = 4 * elemsize; // four vertices per plane
	for(size_t x = 0; x < mCount; ++x)
	{
		Vector3 ax, ay, az;
		things[x].to(ax, ay, az);
		Vector3 pos = az * mRadius; // scale to radius
		ax *= mQSize;
		ay *= mQSize;
		Vector3 pos1 = pos - ax - ay;
		Vector3 pos2 = pos + ax - ay;
		Vector3 pos3 = pos + ax + ay;
		Vector3 pos4 = pos - ax + ay;
		vIdx[x*planesize + 0*elemsize + 0] = pos1.x;
		vIdx[x*planesize + 0*elemsize + 1] = pos1.y;
		vIdx[x*planesize + 0*elemsize + 2] = pos1.z;
		vIdx[x*planesize + 1*elemsize + 0] = pos2.x;
		vIdx[x*planesize + 1*elemsize + 1] = pos2.y;
		vIdx[x*planesize + 1*elemsize + 2] = pos2.z;
		vIdx[x*planesize + 2*elemsize + 0] = pos3.x;
		vIdx[x*planesize + 2*elemsize + 1] = pos3.y;
		vIdx[x*planesize + 2*elemsize + 2] = pos3.z;
		vIdx[x*planesize + 3*elemsize + 0] = pos4.x;
		vIdx[x*planesize + 3*elemsize + 1] = pos4.y;
		vIdx[x*planesize + 3*elemsize + 2] = pos4.z;
	}
	vbuf->unlock();
}
Ogre::NIIf ThingRenderable::getCenterRange() const
{
	return mRadius;
}
Ogre::NIIf ThingRenderable::distanceSquared(const Ogre::Camera* cam) const
{
	Ogre::Vector3 min, max, mid, dist;

	min = mBox.getMin();
	max = mBox.getMax();
	mid = ((min - max) * 0.5) + min;
	dist = cam->getSpacePos() - mid;

	return dist.lengthSquared();
}

