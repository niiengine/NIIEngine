#include "ProceduralTools.h"

#include <Ogre.h>

using namespace Ogre;

//Constants (copied as is from sample)

// Grid sizes (in vertices)
#define X_SIZE_LOG2		6
#define Y_SIZE_LOG2		6
#define Z_SIZE_LOG2		6
#define TOTAL_POINTS	(1<<(X_SIZE_LOG2 + Y_SIZE_LOG2 + Z_SIZE_LOG2))
#define CELLS_COUNT		(((1<<X_SIZE_LOG2) - 1) * ((1<<Y_SIZE_LOG2) - 1) * ((1<<Z_SIZE_LOG2) - 1))

#define SWIZZLE	1

#define MAKE_INDEX(x, y, z, sizeLog2)	(int)((x) | ((y) << sizeLog2[0]) | ((z) << (sizeLog2[0] + sizeLog2[1])))

//--------------------------------------------------------------------------------------
// Fills pPos with x, y, z de-swizzled from index with bitsizes in sizeLog2
//
//  Traversing the grid in a swizzled fashion improves locality of reference,
// and this is very beneficial when sampling a texture.
//--------------------------------------------------------------------------------------
void UnSwizzle(Ogre::uint index, Ogre::uint sizeLog2[3], Ogre::uint * pPos)
{

    // force index traversal to occur in 2x2x2 blocks by giving each of x, y, and z one
    // of the bottom 3 bits
	pPos[0] = index & 1;
    index >>= 1;
    pPos[1] = index & 1;
    index >>= 1;
    pPos[2] = index & 1;
    index >>= 1;

    // Treat the rest of the index like a row, collumn, depth array
    // Each dimension needs to grab sizeLog2 - 1 bits
    // This will make the blocks traverse the grid in a raster style order
    index <<= 1;
    pPos[0] = pPos[0] | (index &  ( (1 << sizeLog2[0]) - 2));
    index >>=  sizeLog2[0] - 1;
    pPos[1] = pPos[1] | ( index &  ( (1 << sizeLog2[1]) - 2));
    index >>= sizeLog2[1] - 1;
    pPos[2] = pPos[2] | ( index &  ( (1 << sizeLog2[2]) - 2));
}


MeshPtr ProceduralTools::generateTetrahedra()
{
	MeshPtr tetrahedraMesh = NII::N_Only(Mesh).createManual("TetrahedraMesh", Ogre::GroupDefault);

	SubMesh * tetrahedraSubMesh = tetrahedraMesh->createSub();
	tetrahedraSubMesh->setGeometryType(GeometryRaw::OT_LINE_LIST);
	tetrahedraSubMesh->setMaterialName("Ogre/IsoSurf/TessellateTetrahedra");

	Ogre::uint sizeLog2[3] = { X_SIZE_LOG2, Y_SIZE_LOG2, Z_SIZE_LOG2 };
	Ogre::uint nTotalBits = sizeLog2[0] + sizeLog2[1] + sizeLog2[2];
	Ogre::uint nPointsTotal = 1 << nTotalBits;

	tetrahedraSubMesh->setSelfVertex(true);
    VertexData * tsmvd = new VertexData;
	tetrahedraSubMesh->setVertexData(tsmvd);
    IndexData * tsmid = new IndexData
	tetrahedraSubMesh->setIndexData(0, tsmid);

	tsmvd->add(DT_Float4x, VT_Pos, 0, 0);

	VertexBuffer * vertexBuffer;
    N_Only(GBuffer).create(vertexBuffer, tsmvd->getUnitSize(0),
        nPointsTotal, Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	IndexBuffer * indexBuffer;
    N_Only(GBuffer).create(indexBuffer, 32, CELLS_COUNT * sizeof(NII::uint) * 24,
        Buffer::Buffer::M_DEV | Buffer::M_WRITE);

	tsmvd->attach(0, vertexBuffer);
	tsmvd->mCount = nPointsTotal;
	tsmvd->mOffset = 0;

	tetrahedraSubMesh->getIndexData(0)->mBuffer = indexBuffer;

	float* positions = static_cast<float*>(vertexBuffer->lock(Buffer::MM_WHOLE));

	//Generate positions
	for(Ogre::uint i=0; i<nPointsTotal; i++) 
    {
		Ogre::uint pos[3];
		pos[0] = i & ((1<<X_SIZE_LOG2)-1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1<<Y_SIZE_LOG2)-1);
		pos[2] = (i >> (X_SIZE_LOG2+Y_SIZE_LOG2)) & ((1<<Z_SIZE_LOG2)-1);

		*positions++ = (float(pos[0]) / float(1<<X_SIZE_LOG2))*2.0f-1.0f;
		*positions++ = (float(pos[1]) / float(1<<Y_SIZE_LOG2))*2.0f-1.0f;
		*positions++ = (float(pos[2]) / float(1<<Z_SIZE_LOG2))*2.0f-1.0f;
		*positions++ = 1.0f;
	}
	vertexBuffer->unlock();

	Ogre::uint numIndices = 0;

	//Generate indices
	Ogre::uint32 * indices = static_cast<Ogre::uint32*>(indexBuffer->lock(Buffer::MM_WHOLE));

	for(Ogre::uint i = 0; i<nPointsTotal; i++) 
    {
		Ogre::uint pos[3];
#if SWIZZLE
		UnSwizzle(i, sizeLog2, pos);	// un-swizzle current index to get x, y, z for the current sampling point
#else
		pos[0] = i & ((1<<X_SIZE_LOG2)-1);
		pos[1] = (i >> X_SIZE_LOG2) & ((1<<Y_SIZE_LOG2)-1);
		pos[2] = (i >> (X_SIZE_LOG2+Y_SIZE_LOG2)) & ((1<<Z_SIZE_LOG2)-1);
#endif
		if ((int)pos[0] == (1 << sizeLog2[0]) - 1 || (int)pos[1] == (1 << sizeLog2[1]) - 1 || (int)pos[2] == (1 << sizeLog2[2]) - 1)
			continue;	// skip extra cells

		numIndices += 24; //Got to this point, adding 24 indices

		// NOTE: order of vertices matters! important for backface culling

		// T0
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T1
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);

		// T2
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T3
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1] + 1, pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T4
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);

		// T5
		*indices++ = MAKE_INDEX(pos[0], pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2], sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1], pos[2] + 1, sizeLog2);
		*indices++ = MAKE_INDEX(pos[0] + 1, pos[1] + 1, pos[2] + 1, sizeLog2);
	}

	indexBuffer->unlock();

	tetrahedraSubMesh->getIndexData(0)->mCount = numIndices;
	tetrahedraSubMesh->getIndexData(0)->mOffset = 0;

	AABox meshBounds;
	meshBounds.setMin(-1,-1,-1);
	meshBounds.setMax(1,1,1);
	tetrahedraMesh->setAABB(meshBounds);
    tetrahedraMesh->setRangeFactor(0.01);
	tetrahedraMesh->setCenterRange(2);

	return tetrahedraMesh;
}
