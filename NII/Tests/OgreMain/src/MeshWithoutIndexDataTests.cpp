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
#include <stdio.h>
#include "Ogre.h"
#include "OgreProgressiveMesh.h"
#include "NiiMemGBufferManager.h"
#include "OgreFileSystem.h"
#include "OgreArchiveManager.h"
#include "MeshWithoutIndexDataTests.h"


// Register the suite
CPPUNIT_TEST_SUITE_REGISTRATION( MeshWithoutIndexDataTests );

void MeshWithoutIndexDataTests::setUp()
{
	LogManager::getSingleton().createLog("MeshWithoutIndexDataTests.log", true);
	N_new ResourceSchemeManager();
	N_new LodStrategyManager();
    mBufMgr = N_new MemGBufferManager();
    mMeshMgr = N_new MeshManager();
    archiveMgr = N_new ArchiveManager();
    archiveMgr->addArchiveFactory(N_new FileSystemArchiveFactory());

	MaterialManager* matMgr = N_new MaterialManager();
	matMgr->init();
}
void MeshWithoutIndexDataTests::tearDown()
{
    N_delete mMeshMgr;
    N_delete mBufMgr;
    N_delete archiveMgr;
	N_delete MaterialManager::getSingletonPtr();
	N_delete LodStrategyManager::getSingletonPtr();
	N_delete ResourceSchemeManager::getSingletonPtr();
}

void MeshWithoutIndexDataTests::testCreateSimpleLine()
{
    CustomSpaceObj * line = N_new CustomSpaceObj("line");
    line->begin(GeometryRaw::OT_LINE_LIST, MaterialManager::WhiteColour->getOriginID());
    line->position(0, 50, 0);
    line->position(50, 100, 0);
    line->end();
    String fileName = "line.mesh";
    Mesh * lineMesh = line->createMesh(fileName);
    lineMesh->ref(false);
    N_delete line;

    CPPUNIT_ASSERT(lineMesh->getSubCount() == 1);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getIndexData(0)->mCount == 0);
    GeometryRaw rop;
    lineMesh->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getVertexData()->mCount == 2);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(lineMesh.get(), fileName);

    mMeshMgr->remove( fileName );

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedLine = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedLine->getSubCount() == 1);
    CPPUNIT_ASSERT(loadedLine->getSub(0)->getIndexData(0)->mCount == 0);
    loadedLine->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getVertexData()->mCount == 2);

    mMeshMgr->remove( fileName );
}

void MeshWithoutIndexDataTests::testCreateLineList()
{
    CustomSpaceObj * lineList = N_new CustomSpaceObj("line");
    lineList->begin(GeometryRaw::OT_LINE_LIST, MaterialManager::WhiteColour->getOriginID());
    lineList->position(0, 50, 0);
    lineList->position(50, 100, 0);
    lineList->position(50, 50, 0);
    lineList->position(100, 100, 0);
    lineList->position(0, 50, 0);
    lineList->position(50, 50, 0);
    lineList->end();
    String fileName = "lineList.mesh";
    Mesh * lineListMesh = lineList->createMesh(fileName);
    lineListMesh->ref(false);
    N_delete lineList;

    CPPUNIT_ASSERT(lineListMesh->getSubCount() == 1);
    CPPUNIT_ASSERT(lineListMesh->getSub(0)->getIndexData(0)->mCount == 0);
    GeometryRaw rop;
    lineListMesh->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineListMesh->getSub(0)->getVertexData()->mCount == 6);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(lineListMesh.get(), fileName);

    mMeshMgr->remove( fileName );

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedLineList = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedLineList->getSubCount() == 1);
    CPPUNIT_ASSERT(loadedLineList->getSub(0)->getIndexData(0)->mCount == 0);
    loadedLineList->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(loadedLineList->getSub(0)->getVertexData()->mCount == 6);

    mMeshMgr->remove( fileName );
}

void MeshWithoutIndexDataTests::testCreateLineStrip()
{
    CustomSpaceObj* lineStrip = N_new CustomSpaceObj("line");
    lineStrip->begin(GeometryRaw::OT_LINE_STRIP, MaterialManager::WhiteColour->getOriginID());
    lineStrip->position(50, 100, 0);
    lineStrip->position(0, 50, 0);
    lineStrip->position(50, 50, 0);
    lineStrip->position(100, 100, 0);
    lineStrip->end();
    String fileName = "lineStrip.mesh";
    Mesh * lineStripMesh = lineStrip->createMesh(fileName);
    lineStripMesh->ref(false);
    N_delete lineStrip;

    CPPUNIT_ASSERT(lineStripMesh->getSubCount() == 1);
    CPPUNIT_ASSERT(lineStripMesh->getSub(0)->getIndexData(0)->mCount == 0);
    GeometryRaw rop;
    lineStripMesh->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineStripMesh->getSub(0)->getVertexData()->mCount == 4);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(lineStripMesh.get(), fileName);

    mMeshMgr->remove( fileName );

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedLineStrip = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedLineStrip->getSubCount() == 1);
    CPPUNIT_ASSERT(loadedLineStrip->getSub(0)->getIndexData(0)->mCount == 0);
    loadedLineStrip->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(loadedLineStrip->getSub(0)->getVertexData()->mCount == 4);

    mMeshMgr->remove( fileName );
}

void MeshWithoutIndexDataTests::testCreatePointList()
{
    CustomSpaceObj * pointList = N_new CustomSpaceObj("line");
    pointList->begin(GeometryRaw::OT_POINT_LIST, MaterialManager::WhiteColour->getOriginID());
    pointList->position(50, 100, 0);
    pointList->position(0, 50, 0);
    pointList->position(50, 50, 0);
    pointList->position(100, 100, 0);
    pointList->end();
    String fileName = "pointList.mesh";
    Mesh * pointListMesh = pointList->createMesh(fileName);
    pointListMesh->ref(false);
    N_delete pointList;

    CPPUNIT_ASSERT(pointListMesh->getSubCount() == 1);
    CPPUNIT_ASSERT(pointListMesh->getSub(0)->getIndexData(0)->mCount == 0);
    GeometryRaw rop;
    pointListMesh->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(pointListMesh->getSub(0)->getVertexData()->mCount == 4);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(pointListMesh.get(), fileName);

    mMeshMgr->remove(fileName);

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedPointList = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedPointList->getSubCount() == 1);
    CPPUNIT_ASSERT(loadedPointList->getSub(0)->getIndexData(0)->mCount == 0);
    loadedPointList->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(loadedPointList->getSub(0)->getVertexData()->mCount == 4);

    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testCreateLineWithMaterial()
{
    String matName = "lineMat";
    MaterialPtr matPtr = MaterialManager::getSingleton().create(matName, "General");
    Pass * pass = matPtr->get(0)->get(0);
    pass->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(1.0, 0.1, 0.1, 0));

    CustomSpaceObj* line = N_new CustomSpaceObj("line");
    line->begin(GeometryRaw::OT_LINE_LIST, matName);
    line->position(0, 50, 0);
    line->position(50, 100, 0);
    line->end();
    String fileName = "lineWithMat.mesh";
    Mesh * lineMesh = line->createMesh(fileName);
    lineMesh->ref(false);
    N_delete line;

    CPPUNIT_ASSERT(lineMesh->getSubCount() == 1);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getIndexData(0)->mCount == 0);
    GeometryRaw rop;
    lineMesh->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getVertexData()->mCount == 2);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(lineMesh.get(), fileName);
    MaterialSerializer matWriter;
    matWriter.exportMaterial(
        MaterialManager::getSingleton().getByName(matName), matName + ".material");

    mMeshMgr->remove(fileName);

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedLine = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());
    remove((matName + ".material").c_str());

    CPPUNIT_ASSERT(loadedLine->getSubCount() == 1);
    CPPUNIT_ASSERT(loadedLine->getSub(0)->getIndexData(0)->mCount == 0);
    loadedLine->getSub(0)->getGeometry(rop);
    CPPUNIT_ASSERT((rop.mMark & GRT_Index) == 0);
    CPPUNIT_ASSERT(lineMesh->getSub(0)->getVertexData()->mCount == 2);

    mMeshMgr->remove(fileName);
}

void createMeshWithMaterial(String fileName)
{
    String matFileNameSuffix = ".material";
    String matName1 = "red";
    String matFileName1 = matName1 + matFileNameSuffix;
    MaterialPtr matPtr = MaterialManager::getSingleton().create(matName1, "General");
    Pass* pass = matPtr->get(0)->get(0);
    pass->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(1.0, 0.1, 0.1, 0));

    String matName2 = "green";
    String matFileName2 = matName2 + matFileNameSuffix;
    matPtr = MaterialManager::getSingleton().create(matName2, "General");
    pass = matPtr->get(0)->get(0);
    pass->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(0.1, 1.0, 0.1, 0));

    String matName3 = "blue";
    String matFileName3 = matName3 + matFileNameSuffix;
    matPtr = MaterialManager::getSingleton().create(matName3, "General");
    pass = matPtr->get(0)->get(0);
    pass->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(0.1, 0.1, 1.0, 0));

    String matName4 = "yellow";
    String matFileName4 = matName4 + matFileNameSuffix;
    matPtr = MaterialManager::getSingleton().create(matName4, "General");
    pass = matPtr->get(0)->get(0);
    pass->setColour(ShaderCh::SOT_Set)->setDiffuse(Colour(1.0, 1.0, 0.1, 0));

    CustomSpaceObj* manObj = N_new CustomSpaceObj(0);
    manObj->begin(GeometryRaw::OT_TRIANGLE_LIST, matName1);
    manObj->position(0, 50, 0);
    manObj->position(50, 50, 0);
    manObj->position(0, 100, 0);
    manObj->triangle(0, 1, 2);
    manObj->position(50, 100, 0);
    manObj->position(0, 100, 0);
    manObj->position(50, 50, 0);
    manObj->triangle(3, 4, 5);
    manObj->end();
    manObj->begin(GeometryRaw::OT_LINE_LIST, matName2);
    manObj->position(0, 100, 0);
    manObj->position(-50, 50, 0);
    manObj->position(-50, 0, 0);
    manObj->position(-50, 50, 0);
    manObj->position(-100, 0, 0);
    manObj->position(-50, 0, 0);
    manObj->end();
    manObj->begin(GeometryRaw::OT_LINE_STRIP, matName3);
    manObj->position(50, 100, 0);
    manObj->position(100, 50, 0);
    manObj->position(100, 0, 0);
    manObj->position(150, 0, 0);
    manObj->end();
    manObj->begin(GeometryRaw::OT_POINT_LIST, matName4);
    manObj->position(50, 0, 0);
    manObj->position(0, 0, 0);
    manObj->end();
    manObj->createMesh(fileName)->ref(false);
    
    N_delete manObj;
}

void MeshWithoutIndexDataTests::testCreateMesh()
{
    String fileName = "indexMix.mesh";
    createMeshWithMaterial(fileName);
    MeshPtr mesh = mMeshMgr->getByName(fileName);

    CPPUNIT_ASSERT(mesh->getSubCount() == 4);
    GeometryRaw rop;
    for(int i = 0; i < 4; ++i)
    {
        mesh->getSub(i)->getGeometry(rop);
        // first submesh has indexes; the others not
        CPPUNIT_ASSERT((rop.mMark & GRT_Index) == (i == 0));
    }

    MeshSerializer meshWriter;
    meshWriter.exportMesh(mesh.get(), fileName);

    mMeshMgr->remove(fileName);

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedMesh = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedMesh->getSubCount() == 4);

    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testCloneMesh()
{
    String originalName = "toClone.mesh";
    createMeshWithMaterial(originalName);
    MeshPtr mesh = mMeshMgr->getByName(originalName);

    String fileName = "clone.mesh";
    MeshPtr clone = mesh->clone(fileName);
    CPPUNIT_ASSERT(mesh->getSubCount() == 4);

    MeshSerializer meshWriter;
    meshWriter.exportMesh(mesh.get(), fileName);

    mMeshMgr->remove( fileName );

    N_Only(ResourceScheme).add(".", "FileSystem");
    MeshPtr loadedMesh = mMeshMgr->load(fileName, "General");

    remove(fileName.c_str());

    CPPUNIT_ASSERT(loadedMesh->getSubCount() == 4);

    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testEdgeList()
{
    String fileName = "testEdgeList.mesh";
    CustomSpaceObj* line = N_new CustomSpaceObj("line");
    line->begin(GeometryRaw::OT_LINE_LIST, MaterialManager::WhiteColour->getOriginID());
    line->position(0, 50, 0);
    line->position(50, 100, 0);
    line->end();
    Mesh * mesh = line->createMesh(fileName);
    mesh->ref(false);
    N_delete line;

    // whole mesh must not contain index data, for this test
    CPPUNIT_ASSERT(mesh->getSubCount() == 1);
    CPPUNIT_ASSERT(mesh->getSub(0)->getIndexData(0)->mCount == 0);

    mesh->buildEdgeList(0);
    MeshSerializer meshWriter;
    // if it does not crash here, test is passed
    meshWriter.exportMesh(mesh.get(), fileName);

    remove(fileName.c_str());

    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testGenerateExtremes()
{
    String fileName = "testGenerateExtremes.mesh";
    createMeshWithMaterial(fileName);
    MeshPtr mesh = mMeshMgr->getByName(fileName);

    const size_t NUM_EXTREMES = 4;
    for(ushort i = 0; i < mesh->getSubCount(); ++i)
    {
        mesh->getSub(i)->buildEdge(NUM_EXTREMES);
    }
    for(ushort i = 0; i < mesh->getSubCount(); ++i)
    {
        SubMesh * subMesh = mesh->getSub(i);
        if(subMesh->getIndexData(0)->mCount > 0)
        {
            CPPUNIT_ASSERT(subMesh->getEdgePoints().size() == NUM_EXTREMES);
        }
        else
        {
            CPPUNIT_ASSERT(subMesh->getEdgePoints().size() == 0);
        }
    }
    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testBuildTangentVectors()
{
    String fileName = "testBuildTangentVectors.mesh";
    createMeshWithMaterial(fileName);
    MeshPtr mesh = mMeshMgr->getByName(fileName);

    try
    {
        // make sure correct exception is thrown
        mesh->buildTangentBuffer();
        CPPUNIT_FAIL("Expected InvalidParametersException!");
    }
    catch(const InvalidParametersException &)
    {
    	// ok
    }
    mMeshMgr->remove(fileName);
}

void MeshWithoutIndexDataTests::testGenerateLodLevels()
{
    String fileName = "testGenerateLodLevels.mesh";
    createMeshWithMaterial(fileName);
    MeshPtr mesh = mMeshMgr->getByName(fileName);

    LodScheme::LodList lodDistanceList;
    lodDistanceList.push_back(600.0);
    ProgressiveMesh::generateLodLevels(mesh.get(), lodDistanceList, ProgressiveMesh::VRQ_CONSTANT, 2);

    CPPUNIT_ASSERT(mesh->getLodCount() == 2);
    for(ushort i = 0; i < mesh->getSubCount(); ++i)
    {
        SubMesh * subMesh = mesh->getSub(i);
        for(ushort j = 0; j < mesh->getLodCount() - 1; ++j)
        {
            if(subMesh->getIndexData(0)->mCount > 0)
            {
                CPPUNIT_ASSERT(subMesh->getIndexData(j + 1)->mCount > 0);
            }
            else
            {
                CPPUNIT_ASSERT(subMesh->getIndexData(j + 1)->mCount == 0);
            }
        }
    }
    MeshSerializer meshWriter;
    meshWriter.exportMesh(mesh.get(), fileName);
    remove(fileName.c_str());
    mMeshMgr->remove(fileName);
}