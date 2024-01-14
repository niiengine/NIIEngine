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
#include "OgreBspLevel.h"
#include "OgreBspResourceManager.h"
#include "OgreException.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreMovableObject.h"
#include "OgreSceneManager.h"
#include "OgrePatchSurface.h"
#include "OgreQuake3ShaderManager.h"
#include "OgreQuake3Shader.h"
#include "OgreMath.h"
#include "OgreStringList.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"
#include "NiiSpaceTypeManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreTextureUnitState.h"
#include "OgreResourceLoadManager.h"

namespace Ogre {

    #define NUM_FACES_PER_PROGRESS_REPORT 100
    #define NUM_NODES_PER_PROGRESS_REPORT 50
    #define NUM_LEAVES_PER_PROGRESS_REPORT 50
    #define NUM_BRUSHES_PER_PROGRESS_REPORT 50

    //-----------------------------------------------------------------------
    BspLevel::BspLevel(ResourceManager * creator, const String & name,
        ResourceHandle handle, const String & group, bool isManual,
        ManualResourceLoader * loader) :
            Resource(creator, name, handle, group, isManual, loader),
            mRootNode(0),
            mVertexData(0),
            mLeafFaceGroups(0),
            mFaceGroups(0),
            mBrushes(0),
            mSkyEnabled(false)
    {
        mVisData.tableData = 0;

        if (createParamDictionary("BspLevel"))
        {
            // nothing
        }
    }

    //-----------------------------------------------------------------------
    BspLevel::~BspLevel()
    {
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();

    }

    //-----------------------------------------------------------------------
    void BspLevel::loadImpl()
    {
		mSkyEnabled = false;

        // Use Quake3 file loader
        Quake3Level q3;
        DataStream * stream = N_Only(ResourceScheme).openResource(mName,
            ResourceSchemeManager::getSingleton().getWorldResourceGroupName());

        q3.loadFromStream(stream);

        N_delete stream;
        loadQuake3Level(q3);

    }
	//-----------------------------------------------------------------------
	bool BspLevel::isSkyEnabled(void) const
	{
		return mSkyEnabled;
	}
	//-----------------------------------------------------------------------
	const String& BspLevel::getSkyMaterialName(void) const
	{
		return mSkyMaterial;
	}
	//-----------------------------------------------------------------------
	NIIf BspLevel::getSkyCurvature(void) const
	{
		return mSkyCurvature;
	}
    //-----------------------------------------------------------------------
    void BspLevel::load(DataStream * * stream)
    {
        // Use Quake3 file loader
        Quake3Level q3;
        q3.loadFromStream(stream);

        loadQuake3Level(q3);

    }
    //-----------------------------------------------------------------------
    void BspLevel::unloadImpl()
    {
        if(mVertexData)
            N_delete mVertexData;
        mIndexes.setNull();
        if(mFaceGroups)
            OGRE_DELETE_ARRAY_T(mFaceGroups, StaticFaceGroup, (size_t)mNumFaceGroups, MEMCATEGORY_GEOMETRY);
        if(mLeafFaceGroups)
            OGRE_FREE(mLeafFaceGroups, MEMCATEGORY_GEOMETRY);
        if(mRootNode)
            N_delete [] mRootNode;
        if(mVisData.tableData)
            OGRE_FREE(mVisData.tableData, MEMCATEGORY_GEOMETRY);
        if(mBrushes)
			OGRE_DELETE_ARRAY_T(mBrushes, Brush, (size_t)mNumBrushes, MEMCATEGORY_GEOMETRY);

        mVertexData = 0;
        mRootNode = 0;
        mFaceGroups = 0;
        mLeafFaceGroups = 0;
        mBrushes = 0;
        mVisData.tableData = 0;
        for(PatchMap::iterator pi = mPatches.begin(); pi != mPatches.end(); ++pi)
        {
            N_delete pi->second;
        }
        mPatches.clear();
    }
    //-----------------------------------------------------------------------
    size_t BspLevel::calculateLoadingStages(const String & levelName)
    {
        NCount temp;
        DataStream * stream =
            ResourceSchemeManager::getSingleton().openResource(levelName,
            ResourceSchemeManager::getSingleton().getWorldResourceGroupName());
		temp = calculateLoadingStages(stream);
        N_delete stream;
        return temp;
	}
    //-----------------------------------------------------------------------
    size_t BspLevel::calculateLoadingStages(DataStream * stream)
    {
        Quake3Level q3;

        // Load header only
        q3.loadHeaderFromStream(stream);

        // Ok, count up the things that we will report
        size_t stages = 0;

        // loadEntities (1 stage)
        ++stages;
        // extractLightmaps (external, 1 stage)
        ++stages;
        // initQuake3Patches
        ++stages;
        // vertex setup
        ++stages;
        // face setup
        ++stages;
        // patch building
        ++stages;
        // material setup
        // this is not strictly based on load, since we only know the number
        // of faces, not the number of materials
        // raise one event for every 50 faces, plus one at the end
        stages += (q3.mNumFaces / NUM_FACES_PER_PROGRESS_REPORT) + 1;
        // node setup
        stages += (q3.mNumNodes / NUM_NODES_PER_PROGRESS_REPORT) + 1;
        // brush setup
        stages += (q3.mNumBrushes / NUM_BRUSHES_PER_PROGRESS_REPORT) + 1;
        // leaf setup
        stages += (q3.mNumLeaves / NUM_LEAVES_PER_PROGRESS_REPORT) + 1;
        // vis
        ++stages;

        return stages;
    }
    //-----------------------------------------------------------------------
    void BspLevel::loadQuake3Level(const Quake3Level & q3lvl)
    {
        MaterialManager & mm = MaterialManager::getSingleton();
        ResourceSchemeManager & rgm = ResourceSchemeManager::getSingleton();

        rgm._notifyWorldGeometryStageStarted("Parsing entities");
        loadEntities(q3lvl);
        rgm._notifyWorldGeometryStageEnded();

        // Extract lightmaps into textures
        rgm._notifyWorldGeometryStageStarted("Extracting lightmaps");
        q3lvl.extractLightmaps();
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Vertices
        //-----------------------------------------------------------------------
        // Allocate memory for vertices & copy
        N_Only(GBuffer).create(mVertexData);
        size_t offset = 0;
        mVertexData->add(DT_Float3x, VT_Pos, 0, offset);
        offset += VertexUnit::getTypeSize(DT_Float3x);
        mVertexData->add(DT_Float3x, VT_Normals, 0, offset);
        offset += VertexUnit::getTypeSize(DT_Float3x);
        mVertexData->add(DT_Colour, VT_Diffuse, 0, offset);
        offset += VertexUnit::getTypeSize(DT_Colour);
        mVertexData->add(DT_Float2x, VT_Tex_Coord, 0, offset, 0);
        offset += VertexUnit::getTypeSize(DT_Float2x);
        mVertexData->add(DT_Float2x, VT_Tex_Coord, 0, offset, 1);

        // Build initial patches - we need to know how big the vertex buffer needs to be
        // to accommodate the subdivision
        rgm._notifyWorldGeometryStageStarted("Initialising patches");
        initQuake3Patches(q3lvl, mVertexData);
        rgm._notifyWorldGeometryStageEnded();

        /// Create the vertex buffer, allow space for patches
        rgm._notifyWorldGeometryStageStarted("Setting up vertex data");
        VertexBuffer * vbuf;
        N_Only(GBuffer).create(vbuf, sizeof(BspVertex),
            q3lvl.mNumVertices + mPatchVertexCount, Buffer::Buffer::M_DEV | Buffer::M_WRITE);
        //COPY static vertex data - Note that we can't just block-copy the vertex data because we have to reorder
        //    our vertex elements; this is to ensure compatibility with older cards when using
        //    hardware vertex buffers - Direct3D requires that the buffer format maps onto a
        //    FVF in those older drivers.
        // Lock just the non-patch area for now
        BspVertex * pVert = static_cast<BspVertex *>(
            vbuf->lock(0, q3lvl.mNumVertices * sizeof(BspVertex), Buffer::MM_WHOLE));
        // Keep another base pointer for use later in patch building
        for(int v = 0; v < q3lvl.mNumVertices; ++v)
        {
            quakeVertexToBspVertex(&q3lvl.mVertices[v], pVert++);
        }
        vbuf->unlock();
        // Setup binding
        mVertexData->attach(0, vbuf);
        // Set other data
        mVertexData->mOffset = 0;
        mVertexData->mCount = q3lvl.mNumVertices + mPatchVertexCount;
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Faces
        // --------
        rgm._notifyWorldGeometryStageStarted("Setting up face data");
        mNumLeafFaceGroups = q3lvl.mNumLeafFaces;
        mLeafFaceGroups = OGRE_ALLOC_T(int, mNumLeafFaceGroups, MEMCATEGORY_GEOMETRY);
        memcpy(mLeafFaceGroups, q3lvl.mLeafFaces, sizeof(int) * mNumLeafFaceGroups);
        mNumFaceGroups = q3lvl.mNumFaces;
        mFaceGroups = OGRE_NEW_ARRAY_T(StaticFaceGroup, mNumFaceGroups, MEMCATEGORY_GEOMETRY);
        // Set up index buffer
        // NB Quake3 indexes are 32-bit
        // Copy the indexes into a software area for staging
        mNumIndexes = q3lvl.mNumElements + mPatchIndexCount;
        // Create an index buffer manually in system memory, allow space for patches
        mIndexes.bind(N_new DefaultHardwareIndexBuffer(
            32,
            mNumIndexes,
            Buffer::M_READ | Buffer::M_WRITE));
        // Write main indexes
        mIndexes->write(q3lvl.mElements, 0, sizeof(unsigned int) * q3lvl.mNumElements);
        rgm._notifyWorldGeometryStageEnded();

        // now build patch information
        rgm._notifyWorldGeometryStageStarted("Building patches");
        buildQuake3Patches(q3lvl.mNumVertices, q3lvl.mNumElements);
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Create materials for shaders
        //-----------------------------------------------------------------------
        // NB this only works for the 'default' shaders for now
        //  i.e. those that don't have a .shader script and thus default
        //  to just texture + lightmap
        // TODO: pre-parse all .shader files and create lookup for next stage (use ROGL shader_file_t)

        // Material names are shadername#lightmapnumber
        // This is because I like to define materials up front completely
        //  rather than combine lightmap and shader dynamically (it's
        //  more generic). It results in more materials, but they're small
        //  beer anyway. Texture duplication is prevented by infrastructure.
        // To do this I actually need to parse the faces since they have the
        //  shader/lightmap combo (lightmap number is not in the shader since
        //  it can be used with multiple lightmaps)
        String shaderName;
        int face;
        face = q3lvl.mNumFaces;
        int matHandle;
        String meshName;

        String resourceGroup = ResourceSchemeManager::getSingleton().getWorldResourceGroupName();
        size_t progressCountdown = NUM_FACES_PER_PROGRESS_REPORT;
        size_t progressCount = 0;

        while(face--)
        {
            // Progress reporting
            if (progressCountdown == NUM_FACES_PER_PROGRESS_REPORT)
            {
                ++progressCount;
                StringUtil::StrStreamType str;
                str << "Loading materials (phase " << progressCount << ")";
                rgm._notifyWorldGeometryStageStarted(str.str());
            }
            else if (progressCountdown == 0)
            {
                // stage report
                rgm._notifyWorldGeometryStageEnded();
                progressCountdown = NUM_FACES_PER_PROGRESS_REPORT + 1;

            }

            // Check to see if existing material
            // Format shader#lightmap
            int shadIdx = q3lvl.mFaces[face].shader;
			StringUtil::StrStreamType tmp;
			tmp << q3lvl.mShaders[shadIdx].name << "#" << q3lvl.mFaces[face].lm_texture;
			shaderName = tmp.str();

            MaterialPtr shadMat = MaterialManager::getSingleton().getByName(shaderName);
            if(shadMat.isNull())
            {
                // Build new material

                // Colour layer
                // NB no extension in Q3A(doh), have to try shader, .jpg, .tga
                String tryName = q3lvl.mShaders[shadIdx].name;
                // Try shader first
                Quake3Shader * pShad = Quake3ShaderManager::getSingleton().getByName(tryName);
                if(pShad)
                {
                    shadMat = pShad->createAsMaterial(q3lvl.mFaces[face].lm_texture);
					// Do skydome (use this material)
					if(pShad->skyDome)
					{
						mSkyEnabled = true;
						mSkyMaterial = shadMat->getName();
						mSkyCurvature = 20 - (pShad->cloudHeight / 256 * 18);
					}
                }
                else
                {
                    // No shader script, try default type texture
                    shadMat = mm.create(shaderName, resourceGroup);
                    ShaderCh * shadPass = shadMat->get(0)->get(0);
                    // Try jpg
                    ShaderChTextureUnit * tex = 0;
                    if(ResourceSchemeManager::getSingleton().isExist(resourceGroup, tryName + ".jpg"))
                    {
                        tex = shadPass->getTexture().create(tryName + ".jpg");
                    }
                    else if(ResourceSchemeManager::getSingleton().isExist(resourceGroup, tryName + ".tga"))
                    {
                        tex = shadPass->getTexture().create(tryName + ".tga");
                    }

                    if(tex)
                    {
                        // Set replace on all first layer textures for now
                        tex->getColourBlend()->setColourBlend(TBM_Src_Replace_Dst);
                        tex->setUAddressing(TAM_REPEAT);
                        tex->setVAddressing(TAM_REPEAT);
                        tex->setWAddressing(TAM_REPEAT);
                    }

                    if(q3lvl.mFaces[face].lm_texture >= 0)
                    {
                        // Add lightmap, additive blending
						StringUtil::StrStreamType lightmapName;
                        lightmapName << "@lightmap" << q3lvl.mFaces[face].lm_texture;
                        tex = shadPass->getTexture().create(lightmapName.str());
                        // Blend
                        tex->getColourBlend()->setColourBlend(TBM_Src_Modulate_Dst);
                        // Use 2nd texture co-ordinate set
                        tex->setCoordSet(1);
                        // Clamp
                        tex->setUAddressing(TAM_CLAMP);
                        tex->setVAddressing(TAM_CLAMP);
                        tex->setWAddressing(TAM_CLAMP);
                    }
                    // Set culling mode to none
                    shadMat->setCullingMode(CM_None);
                    // No dynamic lighting
                    shadMat->setLight(false);
                }
            }
            matHandle = shadMat->getPrc();
            shadMat->load();

            // Copy face data
            StaticFaceGroup * dest = &mFaceGroups[face];
            bsp_face_t * src = &q3lvl.mFaces[face];

            if(q3lvl.mShaders[src->shader].surface_flags & SURF_SKY)
            {
                dest->isSky = true;
            }
            else
            {
                dest->isSky = false;
            }

            dest->materialHandle = matHandle;
            dest->elementStart = src->elem_start;
            dest->numElements = src->elem_count;
            dest->numVertices = src->vert_count;
            dest->vertexStart = src->vert_start;
            if (src->type == BSP_FACETYPE_NORMAL)
            {
                dest->fType = FGT_FACE_LIST;
                // Assign plane
                dest->plane.normal = Vector3(src->normal[0], src->normal[1], src->normal[2]);
                dest->plane.d = -dest->plane.normal.dotProduct(
					Vector3(src->org[0], src->org[1], src->org[2]));

                // Don't rebase indexes here - Quake3 re-uses some indexes for multiple vertex
                // groups eg repeating small details have the same relative vertex data but
                // use the same index data.

            }
            else if (src->type == BSP_FACETYPE_PATCH)
            {
                // Seems to be some crap in the Q3 level where vertex count = 0 or num control points = 0?
                if (dest->numVertices == 0 || src->mesh_cp[0] == 0)
                {
                    dest->fType = FGT_UNKNOWN;
                }
                else
                {

                    // Set up patch surface
                    dest->fType = FGT_PATCH;

                    // Locate the patch we already built
                    PatchMap::iterator p = mPatches.find(face);
                    if (p == mPatches.end())
                    {
                        N_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Patch not found from previous built state");
                    }

                    dest->patchSurf = p->second;
                }
            }
			else if (src->type == BSP_FACETYPE_MESH)
			{
				dest->fType = FGT_FACE_LIST;
				// Assign plane
				dest->plane.normal = Vector3(src->normal[0], src->normal[1], src->normal[2]);
				dest->plane.d = -dest->plane.normal.dotProduct(
					Vector3(src->org[0], src->org[1], src->org[2]));
			}
			else
			{
				LogManager::getSingleton().logMessage("!!! Unknown Face Type !!!", LML_CRITICAL);
			}

            // progress reporting
            --progressCountdown;

        }
        // final stage report
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Nodes
        //-----------------------------------------------------------------------
        // Allocate memory for all nodes (leaves and splitters)
        mNumNodes = q3lvl.mNumNodes + q3lvl.mNumLeaves;
        mNumLeaves = q3lvl.mNumLeaves;
        mLeafStart = q3lvl.mNumNodes;
        mRootNode = N_new BspNode[mNumNodes];
        int i;
        // Convert nodes
        // In our array, first q3lvl.mNumNodes are non-leaf, others are leaves
        progressCountdown = NUM_NODES_PER_PROGRESS_REPORT;
        progressCount = 0;

        for (i = 0; i < q3lvl.mNumNodes; ++i)
        {
            // Progress reporting
            if (progressCountdown == NUM_NODES_PER_PROGRESS_REPORT)
            {
                ++progressCount;
                StringUtil::StrStreamType str;
                str << "Loading nodes (phase " << progressCount << ")";
                    rgm._notifyWorldGeometryStageStarted(str.str());
            }
            else if (progressCountdown == 0)
            {
                // stage report
                rgm._notifyWorldGeometryStageEnded();
                progressCountdown = NUM_NODES_PER_PROGRESS_REPORT + 1;

            }

            BspNode* node = &mRootNode[i];
            bsp_node_t* q3node = &q3lvl.mNodes[i];

            // Set non-leaf
            node->mIsLeaf = false;
            // Set owner
            node->mOwner = this;
            // Set plane
            node->mSplitPlane.normal.x = q3lvl.mPlanes[q3node->plane].normal[0];
            node->mSplitPlane.normal.y = q3lvl.mPlanes[q3node->plane].normal[1];
            node->mSplitPlane.normal.z = q3lvl.mPlanes[q3node->plane].normal[2];
            node->mSplitPlane.d = -q3lvl.mPlanes[q3node->plane].dist;
            // Set bounding box
            node->mBounds.setMin(Vector3(&q3node->bbox[0]));
            node->mBounds.setMax(Vector3(&q3node->bbox[3]));
            // Set back pointer
            // Negative indexes in Quake3 mean leaves
            if (q3node->back < 0)
            {
                // Points to leaf, offset to leaf start and negate index
                node->mBack = &mRootNode[mLeafStart + (~(q3node->back))];

            }
            else
            {
                // Points to node
                node->mBack = &mRootNode[q3node->back];
            }
            // Set front pointer
            // Negative indexes in Quake3 mean leaves
            if (q3node->front < 0)
            {
                // Points to leaf, offset to leaf start and negate index
                node->mFront = &mRootNode[mLeafStart + (~(q3node->front))];

            }
            else
            {
                // Points to node
                node->mFront = &mRootNode[q3node->front];
            }

            --progressCountdown;


        }
        // final stage report
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Brushes
        //-----------------------------------------------------------------------
        // Reserve enough memory for all brushes, solid or not (need to maintain indexes)
		mNumBrushes = q3lvl.mNumBrushes;
        mBrushes = OGRE_NEW_ARRAY_T(BspNode::Brush, mNumBrushes, MEMCATEGORY_GEOMETRY);
        progressCountdown = NUM_BRUSHES_PER_PROGRESS_REPORT;
        progressCount = 0;

        for(i = 0; i < q3lvl.mNumBrushes; ++i)
        {
            // Progress reporting
            if(progressCountdown == NUM_BRUSHES_PER_PROGRESS_REPORT)
            {
                ++progressCount;
                StringUtil::StrStreamType str;
                str << "Loading brushes (phase " << progressCount << ")";
                    rgm._notifyWorldGeometryStageStarted(str.str());
            }
            else if(progressCountdown == 0)
            {
                // stage report
                rgm._notifyWorldGeometryStageEnded();
                progressCountdown = NUM_BRUSHES_PER_PROGRESS_REPORT + 1;
            }
            bsp_brush_t * q3brush = &q3lvl.mBrushes[i];

            // Create a new OGRE brush
            BspNode::Brush * pBrush = &(mBrushes[i]);
            int brushSideIdx, numBrushSides;
            numBrushSides = q3brush->numsides;
            brushSideIdx = q3brush->firstside;
            // Iterate over the sides and create plane for each
            while(numBrushSides--)
            {
                bsp_brushside_t * q3brushside = &q3lvl.mBrushSides[brushSideIdx];
                bsp_plane_t * q3brushplane = &q3lvl.mPlanes[q3brushside->planenum];
                // Notice how we normally invert Q3A plane distances, but here we do not
                // Because we want plane normals pointing out of solid brushes, not in
                Plane brushSide(Vector3(q3brushplane->normal[0], 
                    q3brushplane->normal[1], q3brushplane->normal[2]), q3brushplane->dist);
                pBrush->mPlaneList.push_back(brushSide);
                ++brushSideIdx;
            }
            // Build world fragment
            pBrush->fragment.mType = SpaceQuery::PT_PlaneList;
            pBrush->fragment.mPlaneList = &(pBrush->planes);

            --progressCountdown;
        }
        // final stage report
        rgm._notifyWorldGeometryStageEnded();

        //-----------------------------------------------------------------------
        // Leaves
        //-----------------------------------------------------------------------
        progressCountdown = NUM_LEAVES_PER_PROGRESS_REPORT;
        progressCount = 0;
        for (i = 0; i < q3lvl.mNumLeaves; ++i)
        {
            // Progress reporting
            if (progressCountdown == NUM_LEAVES_PER_PROGRESS_REPORT)
            {
                ++progressCount;
                StringUtil::StrStreamType str;
                str << "Loading leaves (phase " << progressCount << ")";
                    rgm._notifyWorldGeometryStageStarted(str.str());
            }
            else if (progressCountdown == 0)
            {
                // stage report
                rgm._notifyWorldGeometryStageEnded();
                progressCountdown = NUM_LEAVES_PER_PROGRESS_REPORT + 1;

            }
            BspNode* node = &mRootNode[i + mLeafStart];
            bsp_leaf_t* q3leaf = &q3lvl.mLeaves[i];

            // Set leaf
            node->mIsLeaf = true;
            // Set owner
            node->mOwner = this;
            // Set bounding box
            node->mBounds.setMin(Vector3(&q3leaf->bbox[0]));
            node->mBounds.setMax(Vector3(&q3leaf->bbox[3]));
            // Set faces
            node->mFaceGroupStart = q3leaf->face_start;
            node->mNumFaceGroups = q3leaf->face_count;

            node->mVisCluster = q3leaf->cluster;

            // Load Brushes for this leaf
            int brushIdx, brushCount, realBrushIdx;
            brushCount = q3leaf->brush_count;
            brushIdx = q3leaf->brush_start;

            while(brushCount--)
            {
                realBrushIdx = q3lvl.mLeafBrushes[brushIdx];
                bsp_brush_t* q3brush = &q3lvl.mBrushes[realBrushIdx];
                // Only load solid ones, we don't care about any other types
                // Shader determines this
                bsp_shader_t* brushShader = &q3lvl.mShaders[q3brush->shaderIndex];
                if (brushShader->content_flags & CONTENTS_SOLID)
                {
                    // Get brush
                    BspNode::Brush *pBrush = &(mBrushes[realBrushIdx]);
                    assert(pBrush->fragment.mType == SpaceQuery::PT_PlaneList);
                    // Assign node pointer
                    node->mSolidBrushes.push_back(pBrush);
                }
                ++brushIdx;
            }

            --progressCountdown;

        }
        // final stage report
        rgm._notifyWorldGeometryStageEnded();



        // Vis - just copy
        // final stage report
        rgm._notifyWorldGeometryStageStarted("Copying Vis data");
        mVisData.numClusters = q3lvl.mVis->cluster_count;
        mVisData.rowLength = q3lvl.mVis->row_size;
        mVisData.tableData = OGRE_ALLOC_T(unsigned char, q3lvl.mVis->row_size * q3lvl.mVis->cluster_count, MEMCATEGORY_GEOMETRY);
        memcpy(mVisData.tableData, q3lvl.mVis->data, q3lvl.mVis->row_size * q3lvl.mVis->cluster_count);
        rgm._notifyWorldGeometryStageEnded();
    }
    //-----------------------------------------------------------------------
    void BspLevel::initQuake3Patches(const Quake3Level & q3lvl, VertexData * vd)
    {
        int face;

        mPatchVertexCount = 0;
        mPatchIndexCount = 0;

        // We're just building the patch here to get a hold on the size of the mesh
        // although we'll reuse this information later
        face = q3lvl.mNumFaces;
        while(face--)
        {
            bsp_face_t * src = &q3lvl.mFaces[face];

            if(src->type == BSP_FACETYPE_PATCH)
            {
                // Seems to be some crap in the Q3 level where vertex count = 0 or num control points = 0?
                if(src->vert_count == 0 || src->mesh_cp[0] == 0)
                {
                    continue;
                }
                CurvedSurface * ps = N_new CurvedSurface();
                // Set up control points & format
                // Reuse the vertex declaration
                // Copy control points into a buffer so we can convert their format
                BspVertex * pControlPoints = OGRE_ALLOC_T(BspVertex, src->vert_count, MEMCATEGORY_GEOMETRY);
                bsp_vertex_t* pSrc = q3lvl.mVertices + src->vert_start;
                for(int v = 0; v < src->vert_count; ++v)
                {
                    quakeVertexToBspVertex(pSrc++, &pControlPoints[v]);
                }
                // Define the surface, but don't build it yet (no vertex / index buffer)
                ps->define(
                    pControlPoints,
                    vd,
                    src->mesh_cp[0],
                    src->mesh_cp[1],
                    CurvedSurface::T_Bezier);
                // Get stats
                mPatchVertexCount += ps->getVertexCount();
                mPatchIndexCount += ps->getIndexCount();

                // Save the surface for later
                mPatches[face] = ps;
            }
        }
    }
    //-----------------------------------------------------------------------
    void BspLevel::buildQuake3Patches(size_t vertOffset, size_t indexOffset)
    {
        // Loop through the patches
        PatchMap::iterator i, iend;
        iend = mPatches.end();

        size_t currVertOffset = vertOffset;
        size_t currIndexOffset = indexOffset;

        VertexBufferSPtr vbuf = mVertexData->getBuffer(0);

        for (i = mPatches.begin(); i != iend; ++i)
        {
            CurvedSurface* ps = i->second;

            ps->build(vbuf, currVertOffset, mIndexes, currIndexOffset);

            // No need for control points anymore
            BspVertex* pCP = static_cast<BspVertex*>(ps->getData()->lock(Buffer::MM_READ));
            OGRE_FREE(pCP, MEMCATEGORY_GEOMETRY);

            currVertOffset += ps->getVertexCount();
            currIndexOffset += ps->getIndexCount();

        }
    }
    //-----------------------------------------------------------------------
    bool BspLevel::isLeafVisible(const BspNode* from, const BspNode* to) const
    {
        if (to->mVisCluster == -1)
            return false;
        if (from->mVisCluster == -1)
            // Camera outside world?
            return true;


        if (!from->isLeaf() || !to->isLeaf())
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                "Both nodes must be leaf nodes for visibility testing.",
                "BspLevel::isLeafVisible");

        // Use PVS to determine visibility

        /*
        // In wordier terms, the fairly cryptic (but fast) version is doing this:
        //   Could make it a macro for even more speed?

        // Row offset = from cluster number * row size
        int offset = from->mVisCluster*mVisData.rowLength;

        // Column offset (in bytes) = to cluster number divided by 8 (since 8 bits per bytes)
        offset += to->mVisCluster >> 3;

        // Get the right bit within the byte, i.e. bitwise 'and' with bit at remainder position
        int result = *(mVisData.tableData + offset) & (1 << (to->mVisCluster & 7));

        return (result != 0);
        */

        //return ((*(mVisData.tableData + from->mVisCluster * mVisData.rowLength +
        //           ((to->mVisCluster)>>3)) & (1 << ((to->mVisCluster) & 7))) != 0);

        return (*(mVisData.tableData + from->mVisCluster*mVisData.rowLength +
                   ((to->mVisCluster)>>3)) & (1 << ((to->mVisCluster) & 7))) != 0;

    }
    //-----------------------------------------------------------------------
    const BspNode* BspLevel::getRootNode(void)
    {
        return mRootNode;
    }
    //-----------------------------------------------------------------------
    BspNode* BspLevel::findLeaf(const Vector3& point) const
    {
        BspNode* node = mRootNode;

        while (!node->isLeaf())
        {
            node = node->getNextNode(point);
        }

        return node;

    }
    //-----------------------------------------------------------------------
    void BspLevel::loadEntities(const Quake3Level& q3lvl)
    {
        char* strEnt;
        String line;
        StringList vecparams;
        Vector3 origin = Vector3::ZERO;
        Radian angle(0);
        size_t pos;
        char* lineend;
        bool isPlayerStart;

        isPlayerStart = false;
        strEnt = (char*)q3lvl.mEntities;

        lineend = strchr(strEnt, '\n');
        while (lineend != 0)
        {
            *lineend = '\0';
            line = strEnt;
            strEnt = lineend+1;
            StringUtil::trim(line);
            if (line.length() > 0)
            {
                StringUtil::toLowerCase(line);
                // Remove quotes
				while( ( pos = line.find("\"",0) ) != String::npos )
                {
                    line = line.substr(0,pos) + line.substr(pos+1,line.length()-(pos+1));
                }
                vecparams = StringUtil::split(line);
                StringList::iterator params = vecparams.begin();

                if (params[0] == "origin")
                {
                    origin.x = atof(params[1].c_str());
                    origin.y = atof(params[2].c_str());
                    origin.z = atof(params[3].c_str());
                }
                if (params[0] == "angle")
                {
                    angle = Angle(atof(params[1].c_str()));
                }
                if (params[0] == "classname" && params[1] == "info_player_deathmatch")
                {
                    isPlayerStart = true;
                }
                if (params[0] == "}")
                {
                    if (isPlayerStart)
                    {
                        // Save player start
                        ViewPoint vp;
                        vp.position = origin;
                        vp.orientation.from(angle, Vector3::Z);
                        mPlayerStarts.push_back(vp);
                    }
                    isPlayerStart = false;
                }
            }

            lineend = strchr(strEnt, '\n');
        }
    }
    //-----------------------------------------------------------------------
    void BspLevel::_notifyObjectMoved(const SpaceObj * mov, const Vector3 & pos)
    {
        // Locate any current nodes the object is supposed to be attached to
        MovableToNodeMap::iterator i = mMovableToNodeMap.find(mov);
        if (i != mMovableToNodeMap.end())
        {
			list<BspNode*>::type::iterator nodeit, nodeitend;
            nodeitend = i->second.end();
            for (nodeit = i->second.begin(); nodeit != nodeitend; ++nodeit)
            {
                // Tell each node
                (*nodeit)->_removeMovable(mov);
            }
            // Clear the existing list of nodes because we'll reevaluate it
            i->second.clear();
        }

        tagNodesWithMovable(mRootNode, mov, pos);
    }
    //-----------------------------------------------------------------------
    void BspLevel::tagNodesWithMovable(BspNode* node, const SpaceObj* mov,
        const Vector3& pos)
    {
        if (node->isLeaf())
        {
            // Add to movable->node map
            // Insert all the time, will get current if already there
            std::pair<MovableToNodeMap::iterator, bool> p =
                mMovableToNodeMap.insert(
                    MovableToNodeMap::value_type(mov, list<BspNode*>::type()));

            p.first->second.push_back(node);

            // Add movable to node
            node->_addMovable(mov);

        }
        else
        {
            // Find distance to dividing plane
            NIIf dist = node->getDistance(pos);
            if(Math::Abs(dist) < mov->getCenterRange())
            {
                // Bounding sphere crosses the plane, do both
                tagNodesWithMovable(node->getBack(), mov, pos);
                tagNodesWithMovable(node->getFront(), mov, pos);
            }
            else if(dist < 0)
            {    //-----------------------------------------------------------------------

                // Do back
                tagNodesWithMovable(node->getBack(), mov, pos);
            }
            else
            {
                // Do front
                tagNodesWithMovable(node->getFront(), mov, pos);
            }
        }
    }
    //-----------------------------------------------------------------------
	void BspLevel::_notifyObjectDetached(const SpaceObj* mov)
	{
        // Locate any current nodes the object is supposed to be attached to
        MovableToNodeMap::iterator i = mMovableToNodeMap.find(mov);
        if (i != mMovableToNodeMap.end())
        {
			list<BspNode*>::type::iterator nodeit, nodeitend;
            nodeitend = i->second.end();
            for (nodeit = i->second.begin(); nodeit != nodeitend; ++nodeit)
            {
                // Tell each node
                (*nodeit)->_removeMovable(mov);
            }
            // delete the entry for this SpaceObj
            mMovableToNodeMap.erase(i);
        }
	}
    //-----------------------------------------------------------------------
    void BspLevel::quakeVertexToBspVertex(const bsp_vertex_t* src, BspVertex* dest)
    {
        memcpy(dest->position, src->point, sizeof(float) * 3);
        memcpy(dest->normal, src->normal,  sizeof(float) * 3);
        dest->colour = src->color;
        dest->texcoords[0]  = src->texture[0];
        dest->texcoords[1]  = src->texture[1];
        dest->lightmap[0]  = src->lightmap[0];
        dest->lightmap[1]  = src->lightmap[1];
    }
    //-----------------------------------------------------------------------
    size_t BspLevel::calcSize(void) const
    {
        return 0; // TODO
    }
}
