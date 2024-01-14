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

#include "GeomUtils.h"

#include "OgreMeshManager.h"
#include "OgreSubMesh.h"
#include "OgreGBufferManager.h"

using namespace Ogre;

void GeomUtils::createSphere(const String & strName, float radius, int nRings,
    int nSegments, bool bNormals, bool bTexCoords)
{
	MeshPtr pSphere = N_Only(Mesh).createManual(strName, GroupDefault);
	SubMesh * pSphereVertex = pSphere->createSub();
    N_Only(GBuffer).create(pSphere->getVertexData());
	createSphere(pSphere->getVertexData(), pSphereVertex->getIndexData(0)
		, radius
		, nRings, nSegments
		, bNormals // need normals
		, bTexCoords // need texture co-ordinates
		);

	// Generate face list
	pSphereVertex->setSelfVertex(false);

	// the original code was missing this line:
	pSphere->setAABB(AABox(Vector3(-radius, -radius, -radius), Vector3(radius, radius, radius));
	pSphere->setCenterRange(radius);
	// this line makes clear the mesh is loaded (avoids memory leaks)
	pSphere->load();
}

void GeomUtils::createSphere(VertexData *& vertexData, IndexData *& indexData,
    float radius, int nRings, int nSegments, bool bNormals, bool bTexCoords)
{
	assert(vertexData && indexData);

	// define the vertex format
	size_t currOffset = 0;
	// positions
	vertexData->add(DT_Float3x, VT_Pos, 0, currOffset);
	currOffset += DataTypeUtil::getSize(DT_Float3x);

	if(bNormals)
	{
		// normals
		vertexData->add(DT_Float3x, VT_Normals, 0, currOffset);
		currOffset += DataTypeUtil::getSize(DT_Float3x);

	}
	// two dimensional texture coordinates
	if (bTexCoords)
	{
		vertexData->add(DT_Float2x, VT_Tex_Coord, 0, currOffset, 0);
	}

	// allocate the vertex buffer
	vertexData->mCount = (nRings + 1) * (nSegments+1);
	VertexBuffer * vBuf;
    N_Only(GBuffer).create(vBuf, vertexData->getUnitSize(0),
        vertexData->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
	vertexData->attach(0, vBuf);
	float * pVertex = static_cast<float *>(vBuf->lock(Buffer::MM_WHOLE));

	// allocate index buffer
	indexData->mCount = 6 * nRings * (nSegments + 1);
    N_Only(GBuffer).create(indexData->mBuffer, 16,
        indexData->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
	IndexBufferSPtr iBuf = indexData->mBuffer;
	unsigned short * pIndices = static_cast<unsigned short *>(iBuf->lock(Buffer::MM_WHOLE));

	float fDeltaRingAngle = (Math::PI / nRings);
	float fDeltaSegAngle = (2 * Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for(int ring = 0; ring <= nRings; ring++)
    {
		float r0 = radius * sinf (ring * fDeltaRingAngle);
		float y0 = radius * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++)
        {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			*pVertex++ = x0;
			*pVertex++ = y0;
			*pVertex++ = z0;

			if (bNormals)
			{
				Vector3 vNormal;
                Vector3(x0, y0, z0).normalise(vNormal);
				*pVertex++ = vNormal.x;
				*pVertex++ = vNormal.y;
				*pVertex++ = vNormal.z;
			}
			if (bTexCoords)
			{
				*pVertex++ = (float) seg / (float) nSegments;
				*pVertex++ = (float) ring / (float) nRings;
			}

			if (ring != nRings)
			{
				// each vertex (except the last) has six indices pointing to it
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex;
				*pIndices++ = wVerticeIndex + nSegments;
				*pIndices++ = wVerticeIndex + nSegments + 1;
				*pIndices++ = wVerticeIndex + 1;
				*pIndices++ = wVerticeIndex;
				wVerticeIndex ++;
			}
		}; // end for seg
	} // end for ring

	// Unlock
	vBuf->unlock();
	iBuf->unlock();
}

void GeomUtils::createQuad(VertexData *& vertexData)
{
	assert(vertexData);

	vertexData->mOffset = 0;
	vertexData->mCount = 4;

	vertexData->add(DT_Float3x, VT_Pos, 0, 0,);

	VertexBuffer * vbuf;
    N_Only(GBuffer).create(vbuf, vertexData->getUnitSize(0),
        vertexData->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	// Bind buffer
	vertexData->attach(0, vbuf);
	// Upload data
	float data[] =
    {
		-1,1,-1,  // corner 1
		-1,-1,-1, // corner 2
		1,1,-1,   // corner 3
		1,-1,-1
    }; // corner 4
    vbuf->write(data, 0, sizeof(data));
}

void GeomUtils::createCone(const Ogre::String & strName, float radius, float height, int nVerticesInBase)
{
	MeshPtr pCone = N_Only(Mesh).createManual(strName, GroupDefault);
	SubMesh * pConeVertex = pCone->createSub();
    N_Only(GBuffer).create(pCone->getVertexData());
	createCone(pCone->getVertexData(), pConeVertex->getIndexData(0), radius, height, nVerticesInBase);

	// Generate face list
	pConeVertex->setSelfVertex(false);

	// the original code was missing this line:
	pCone->setAABB(AABox(Vector3(-radius, 0, -radius), Vector3(radius, height, radius)));

	pCone->setCenterRange(Math::sqrt(height * height + radius * radius));
	// this line makes clear the mesh is loaded (avoids memory leaks)
	pCone->load();
}

void GeomUtils::createCone(Ogre::VertexData *& vertexData, Ogre::IndexData *& indexData,
    float radius , float height, int nVerticesInBase)
{
	assert(vertexData && indexData);
	// positions
	vertexData->add(DT_Float3x, VT_Pos, 0, 0);

	// allocate the vertex buffer
	vertexData->mCount = nVerticesInBase + 1;
	VertexBuffer * vBuf;
    N_Only(GBuffer).create(vBuf, vertexData->getUnitSize(0),
        vertexData->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
	vertexData->attach(0, vBuf);
	float * pVertex = static_cast<float*>(vBuf->lock(Buffer::MM_WHOLE));

	// allocate index buffer - cone and base
	indexData->mCount = (3 * nVerticesInBase) + (3 * (nVerticesInBase - 2));
    N_Only(GBuffer).create(indexData->mBuffer, 16, indexData->mCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
	IndexBufferSPtr iBuf = indexData->mBuffer;
	unsigned short* pIndices = static_cast<unsigned short*>(iBuf->lock(Buffer::MM_WHOLE));

	//Positions : cone head and base
	for(int i=0; i<3; i++)
		*pVertex++ = 0.0f;

	//Base :
	float fDeltaBaseAngle = (2 * Math::PI) / nVerticesInBase;
	for(int i=0; i<nVerticesInBase; i++)
	{
		float angle = i * fDeltaBaseAngle;
		*pVertex++ = radius * cosf(angle);
		*pVertex++ = height;
		*pVertex++ = radius * sinf(angle);
	}

	//Indices :
	//Cone head to vertices
	for(int i=0; i<nVerticesInBase; i++)
	{
		*pIndices++ = 0;
		*pIndices++ = (i%nVerticesInBase) + 1;
		*pIndices++ = ((i+1)%nVerticesInBase) + 1;
	}
	//Cone base
	for(int i=0; i<nVerticesInBase-2; i++)
	{
		*pIndices++ = 1;
		*pIndices++ = i + 3;
		*pIndices++ = i + 2;
	}

	// Unlock
	vBuf->unlock();
	iBuf->unlock();
}
