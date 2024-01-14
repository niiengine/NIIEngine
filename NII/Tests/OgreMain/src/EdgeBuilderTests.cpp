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
#include "EdgeBuilderTests.h"
#include "NiiMemGBufferManager.h"
#include "NiiVertexData.h"
#include "NiiIndexData.h"
#include "NiiEdgeListBuilder.h"

// Regsiter the suite
CPPUNIT_TEST_SUITE_REGISTRATION(EdgeBuilderTests);

void EdgeBuilderTests::setUp()
{
    mBufMgr = N_new MemGBufferManager();
    //mLogMgr = N_new LogManager();
    LogManager::getSingleton().createLog("EdgeBuilderTests.log", true);
}

void EdgeBuilderTests::tearDown()
{
    N_delete mBufMgr;
    //N_delete mLogMgr;
}

void EdgeBuilderTests::testSingleIndexBufSingleVertexBuf()
{
    /* This tests the edge builders ability to find shared edges in the simple case
    of a single index buffer referencing a single vertex buffer
    */
    VertexData * vd;
    N_Only(GBuffer).create(vd);
    IndexData id;
    // Test pyramid
    vd.mCount = 4;
    vd.mOffset = 0;
    vd->add(DT_Float3x, VT_Pos, 0, 0);
    VertexBuffer * vbuf;
    N_Only(GBuffer).create(vbuf, sizeof(float) * 3, 4, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd->attach(0, vbuf);
    float * pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 50 ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 100; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = -50;
    vbuf->unlock();

    N_Only(GBuffer).create(id.mBuffer, 16, 12, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    id.mCount = 12;
    id.mOffset = 0;
    unsigned short* pIdx = static_cast<unsigned short*>(id.mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 1; *pIdx++ = 2;
    *pIdx++ = 0; *pIdx++ = 2; *pIdx++ = 3;
    *pIdx++ = 1; *pIdx++ = 3; *pIdx++ = 2;
    *pIdx++ = 0; *pIdx++ = 3; *pIdx++ = 1;
    id.mBuffer->unlock();

    EdgeListBuilder edgeBuilder;
    edgeBuilder.addVertex(&vd);
    edgeBuilder.addIndex(&id);
    GeoEdget* edgeData = edgeBuilder.build();

    // Should be only one group, since only one vertex buffer
    CPPUNIT_ASSERT(edgeData->mGeoList.size() == 1);
    // 4 tris
    CPPUNIT_ASSERT(edgeData->mTriangleList.size() == 4);
    GeoEdget::EdgeGroup& eg = edgeData->mGeoList[0];
    // 6 edges
    CPPUNIT_ASSERT(eg.edges.size() == 6);

    delete edgeData;
}

void EdgeBuilderTests::testMultiIndexBufSingleVertexBuf()
{
    /* This tests the edge builders ability to find shared edges when there are
    multiple index sets (submeshes) using a single vertex buffer.
    */
    VertexData * vd;
    N_Only(GBuffer).create(vd);
    IndexData id[4];
    // Test pyramid
    vd.mCount = 4;
    vd.mOffset = 0;
    vd->add(DT_Float3x, VT_Pos, 0, 0);
    VertexBuffer * vbuf;
    N_Only(GBuffer).create(vbuf, sizeof(float)*3, 4, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd->attach(0, vbuf);
    float* pFloat = static_cast<float*>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 50 ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 100; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = -50;
    vbuf->unlock();

    N_Only(GBuffer).create(id[0].mBuffer, 16, 3, Buffer::Buffer::M_DEV| Buffer::M_HOST);
    id[0].mCount = 3;
    id[0].mOft = 0;
    unsigned short* pIdx = static_cast<unsigned short*>(id[0].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 1; *pIdx++ = 2;
    id[0].mBuffer->unlock();

    N_Only(GBuffer).create(id[1].mBuffer, 16, 3, Buffer::Buffer::M_DEV| Buffer::M_HOST);
    id[1].mCount = 3;
    id[1].mOft = 0;
    pIdx = static_cast<unsigned short*>(id[1].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 2; *pIdx++ = 3;
    id[1].mBuffer->unlock();

    N_Only(GBuffer).create(id[2].mBuffer, 16, 3, Buffer::Buffer::M_DEV| Buffer::M_HOST);
    id[2].mCount = 3;
    id[2].mOft = 0;
    pIdx = static_cast<unsigned short*>(id[2].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 1; *pIdx++ = 3; *pIdx++ = 2;
    id[2].mBuffer->unlock();

    N_Only(GBuffer).create(id[3].mBuffer, 16, 3, Buffer::Buffer::M_DEV| Buffer::M_HOST);
    id[3].mCount = 3;
    id[3].mOft = 0;
    pIdx = static_cast<unsigned short*>(id[3].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 3; *pIdx++ = 1;
    id[3].mBuffer->unlock();

    EdgeListBuilder edgeBuilder;
    edgeBuilder.addVertex(&vd);
    edgeBuilder.addIndex(&id[0]);
    edgeBuilder.addIndex(&id[1]);
    edgeBuilder.addIndex(&id[2]);
    edgeBuilder.addIndex(&id[3]);
    GeoEdget * edgeData = edgeBuilder.build();

    // Should be only one group, since only one vertex buffer
    CPPUNIT_ASSERT(edgeData->mGeoList.size() == 1);
    // 4 tris
    CPPUNIT_ASSERT(edgeData->mTriangleList.size() == 4);
    GeoEdget::EdgeGroup & eg = edgeData->mGeoList[0];
    // 6 edges
    CPPUNIT_ASSERT(eg.edges.size() == 6);

    delete edgeData;
}

void EdgeBuilderTests::testMultiIndexBufMultiVertexBuf()
{
    /* This tests the edge builders ability to find shared edges when there are
    both multiple index sets (submeshes) each using a different vertex buffer
    (not using shared geoemtry).
    */

    VertexData * vd[4];
    N_Only(GBuffer).create(vd[0]);
    N_Only(GBuffer).create(vd[1]);
    N_Only(GBuffer).create(vd[2]);
    N_Only(GBuffer).create(vd[3]);
    IndexData id[4];
    // Test pyramid
    vd[0].mCount = 3;
    vd[0].mOffset = 0;
    vd[0]->add(DT_Float3x, VT_Pos, 0, 0);
    VertexBuffer * vbuf;
    N_Only(GBuffer).create(vbuf, sizeof(float) * 3, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd[0]->attach(0, vbuf);
    float * pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 50 ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 100; *pFloat++ = 0  ;
    vbuf->unlock();

    vd[1].mCount = 3;
    vd[1].mOffset = 0;
    vd[1]->add(DT_Float3x, VT_Pos, 0, 0);
    N_Only(GBuffer).create(vbuf, sizeof(float) * 3, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd[1]->attach(0, vbuf);
    pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 100; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = -50;
    vbuf->unlock();

    vd[2].mCount = 3;
    vd[2].mOffset = 0;
    vd[2]->add(DT_Float3x, VT_Pos, 0, 0);
    N_Only(GBuffer).create(vbuf, sizeof(float) * 3, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd[2]->attach(0, vbuf);
    pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 50 ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 100; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = -50;
    vbuf->unlock();

    vd[3].mCount = 3;
    vd[3].mOffset = 0;
    vd[3]->add(DT_Float3x, VT_Pos, 0, 0);
    N_Only(GBuffer).create(vbuf, sizeof(float) * 3, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    vd[3]->attach(0, vbuf);
    pFloat = static_cast<float *>(vbuf->lock(Buffer::MM_WHOLE));
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 50 ; *pFloat++ = 0  ; *pFloat++ = 0  ;
    *pFloat++ = 0  ; *pFloat++ = 0  ; *pFloat++ = -50;
    vbuf->unlock();

    N_Only(GBuffer).create(id[0].mBuffer, 16, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    id[0].mCount = 3;
    id[0].mOft = 0;
    unsigned short* pIdx = static_cast<unsigned short*>(id[0].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 1; *pIdx++ = 2;
    id[0].mBuffer->unlock();

    N_Only(GBuffer).create(id[1].mBuffer, 16, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    id[1].mCount = 3;
    id[1].mOft = 0;
    pIdx = static_cast<unsigned short *>(id[1].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 1; *pIdx++ = 2;
    id[1].mBuffer->unlock();

    N_Only(GBuffer).create(id[2].mBuffer, 16, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    id[2].mCount = 3;
    id[2].mOft = 0;
    pIdx = static_cast<unsigned short*>(id[2].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 2; *pIdx++ = 1;
    id[2].mBuffer->unlock();

    N_Only(GBuffer).create(id[3].mBuffer, 16, 3, Buffer::Buffer::M_DEV | Buffer::M_HOST);
    id[3].mCount = 3;
    id[3].mOft = 0;
    pIdx = static_cast<unsigned short*>(id[3].mBuffer->lock(Buffer::MM_WHOLE));
    *pIdx++ = 0; *pIdx++ = 2; *pIdx++ = 1;
    id[3].mBuffer->unlock();

    EdgeListBuilder edgeBuilder;
    edgeBuilder.addVertex(&vd[0]);
    edgeBuilder.addVertex(&vd[1]);
    edgeBuilder.addVertex(&vd[2]);
    edgeBuilder.addVertex(&vd[3]);
    edgeBuilder.addIndex(&id[0], 0);
    edgeBuilder.addIndex(&id[1], 1);
    edgeBuilder.addIndex(&id[2], 2);
    edgeBuilder.addIndex(&id[3], 3);
    GeoEdget * edgeData = edgeBuilder.build();

    // Should be 4 groups
    CPPUNIT_ASSERT(edgeData->mGeoList.size() == 4);
    // 4 tris
    CPPUNIT_ASSERT(edgeData->mTriangleList.size() == 4);
    // 6 edges in total
    CPPUNIT_ASSERT(
        (edgeData->mGeoList[0].edges.size() +
        edgeData->mGeoList[1].edges.size() +
        edgeData->mGeoList[2].edges.size() +
        edgeData->mGeoList[3].edges.size()) == 6);

    delete edgeData;
}