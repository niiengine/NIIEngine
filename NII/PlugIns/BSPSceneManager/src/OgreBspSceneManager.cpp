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
#include "OgreBspSceneManager.h"
#include "OgreBspResourceManager.h"
#include "OgreBspNode.h"
#include "OgreException.h"
#include "OgreRenderSystem.h"
#include "OgreCamera.h"
#include "OgreMaterial.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"
#include "OgreMath.h"
#include "NiiEquationManager.h"
#include "OgreLogManager.h"
#include "OgreBspSceneNode.h"
#include "OgreStringConverter.h"
#include "OgreLogManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"
#include "OgreMaterialManager.h"
#include <fstream>

namespace Ogre
{
    //-----------------------------------------------------------------------
    BspSceneManager::BspSceneManager(const String & name) :
        SpaceManager(name),
        mDomeSky(0)
    {
        // Set features for debugging render
        mShowNodeAABs = false;

        // No sky by default
        mSkyPlaneEnabled = false;
        mSkyBoxEnabled = false;
        mSkyDomeEnabled = false;

        mLevel.setNull();
        
        mType = BspSceneManagerFactory::FACTORY_TYPE_NAME;
    }
    //-----------------------------------------------------------------------
    BspSceneManager::~BspSceneManager()
    {
        free();
        mLevel.setNull();
        if(mDomeSky)
        {
            N_delete mDomeSky;
            removeSky(mDomeSky);
            mDomeSky = 0;
        }
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::setSceneGeo(const String & filename)
    {
        mLevel.setNull();
        // Check extension is .bsp
        char extension[6];
        size_t pos = filename.find_last_of(".");
		if(pos == String::npos)
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                "Unable to load world geometry. Invalid extension (must be .bsp).");

        strncpy(extension, filename.substr(pos + 1, filename.length() - pos).c_str(), 5);
		extension[5] = 0;

        if(stricmp(extension, "bsp"))
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                "Unable to load world geometry. Invalid extension (must be .bsp).");

        // Load using resource manager
        mLevel = BspResourceManager::getSingleton().load(filename,
            ResourceSchemeManager::getSingleton().getWorldResourceGroupName());

		if(mLevel->isSkyEnabled())
		{
			// Quake3 is always aligned with Z upwards
			Quaternion q;
			q.from(Radian(Math::PI_2), Vector3::X);
			// Also draw last, and make close to camera (far clip plane is shorter)
            mDomeSky = N_new DomeSky(this);
            mDomeSky->setRenderGroup(RQG_NearEnd);
            mDomeSky->setMaterial(mLevel->getSkyMaterialName());
			mDomeSky->buildGeo(mLevel->getSkyCurvature(), 2000, 12, 12, 20, 20, q);
            add(mDomeSky);
		}
		else
		{
            if(mDomeSky != 0)
            {
                removeSky(mDomeSky);
                N_delete mDomeSky;
                mDomeSky = 0;
            }
		}

        // Init static render operation
        mRenderOp.mVertex = mLevel->mVertexData;
        // index data is per-frame
        mRenderOp.mIndex = N_new IndexData();
        mRenderOp.mIndex->mOffset = 0;
        mRenderOp.mIndex->mCount = 0;
        // Create enough index space to render whole level
        N_Only(GBuffer).create(mRenderOp.mIndex->mBuffer, 32,
            mLevel->mNumIndexes, Buffer::M_WRITE | Buffer::M_WHOLE);

        mRenderOp.mType = GeometryRaw::OT_TRIANGLE_LIST;
        mRenderOp.add(GRT_Index);
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::setSceneGeo(DataStream * stream, const String & typeName)
    {
        mLevel.setNull();

        // Load using resource manager
        mLevel = BspResourceManager::getSingleton().load(stream,
			ResourceSchemeManager::getSingleton().getWorldResourceGroupName());

		if (mLevel->isSkyEnabled())
		{
			// Quake3 is always aligned with Z upwards
			Quaternion q;
			q.from(Radian(Math::PI_2), Vector3::X);
			// Also draw last, and make close to camera (far clip plane is shorter)
            if(mDomeSky)
            {
                removeSky(mDomeSky);
                N_delete mDomeSky;
            }
            mDomeSky = N_new DomeSky(this)
            mDomeSky->setMaterial(mLevel->getSkyMaterialName())
            mDomeSky->seRenderGroup(RQG_Sky);
			mDomeSky->buildGeo(mLevel->getSkyCurvature(), 2000, 12, 12, 20, 20, q);
		}
		else
		{
            removeSky(mDomeSky);
            N_delete mDomeSky;
            mDomeSky = 0;
		}

        // Init static render operation
        mRenderOp.mVertex = mLevel->mVertexData;
        // index data is per-frame
        mRenderOp.mIndex = N_new IndexData();
        mRenderOp.mIndex->mOffset = 0;
        mRenderOp.mIndex->mCount = 0;
        // Create enough index space to render whole level
        N_Only(GBuffer).create(mRenderOp.mIndex->mBuffer, 32,
            mLevel->mNumIndexes, Buffer::M_WRITE | Buffer::M_WHOLE);

        mRenderOp.mType = GeometryRaw::OT_TRIANGLE_LIST;
        mRenderOp.add(GRT_Index);
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::findSpace(Camera * cam, bool onlycast)
    {
        // Clear unique list of movables for this frame
        mMovablesForRendering.clear();

        // Walk the tree, tag static geometry, return camera's node (for info only)
        // Movables are now added to the render queue in processVisibleLeaf
        walkTree(cam, onlycast);
    }
	//---------------------------------------------------------------------
	bool BspSceneManager::onEndRenderQueue(GroupID id)
	{
		bool repeat = SpaceManager::onEndRenderQueue(id);
		// Trigger level render just after skies
		// can't trigger on mSceneGeoRenderGroup because we're not queueing there
		if(id == RQG_Sky)
		{
			renderStaticSpace();
		}
		return repeat;
	}
    //-----------------------------------------------------------------------
    void BspSceneManager::renderStaticSpace()
    {
		// Check we should be rendering
		if(isExcludeRenderExist(mSceneGeoRenderGroup))
			return;

        // Cache vertex/face data first
		vector<StaticFaceGroup *>::type::const_iterator faceGrpi;
        static GeometryRaw patchOp;

        // no world transform required
        mDestRenderSystem->_setModel(Matrix4::IDENTITY);
        // Set view / proj
        setViewMatrix(mTempViewMatrix);
        mDestRenderSystem->_setProj(mCameraInProgress->getSysProjMatrix());

        // For each material in turn, cache rendering data & render
        MaterialFaceGroupMap::const_iterator mati;

        for(mati = mMatFaceGroupMap.begin(); mati != mMatFaceGroupMap.end(); ++mati)
        {
            // Get Material
            Material * m = mati->first;

            // Empty existing cache
            mRenderOp.mIndex->mCount = 0;
            // lock index buffer ready to receive data
            unsigned int * pIdx = static_cast<unsigned int *>(
                mRenderOp.mIndex->mBuffer->lock(Buffer::MM_WHOLE));

            for(faceGrpi = mati->second.begin(); faceGrpi != mati->second.end(); ++faceGrpi)
            {
                // Cache each
                unsigned int numelems = cacheGeometry(pIdx, *faceGrpi);
                mRenderOp.mIndex->mCount += numelems;
                pIdx += numelems;
            }
            // Unlock the buffer
            mRenderOp.mIndex->mBuffer->unlock();

            // Skip if no faces to process (we're not doing flare types yet)
            if(mRenderOp.mIndex->mCount == 0)
                continue;

            ShaderChList::iterator i, iend = m->getLOD()->getShaderChList().end();
            for(i = m->getLOD()->getShaderChList().begin(); i != iend; ++i)
            {
                _setPass(*i);
                mDestRenderSystem->_render(mRenderOp);
            }
        } // for each material

        /*
        if (mShowNodeAABs)
        {
            mDestRenderSystem->_render(mAABGeometry);
        }
        */
    }
    //-----------------------------------------------------------------------
    // REMOVE THIS CRAP
    //-----------------------------------------------------------------------
    // Temp debug lines
    bool firstTime = true;
    std::ofstream of;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    BspNode * BspSceneManager::walkTree(Camera * camera,
		RenderBound * visibleBounds, bool onlyShadowCasters)
    {
		if (mLevel.isNull()) return 0;

        // Locate the leaf node where the camera is located
        BspNode * cameraNode = mLevel->findLeaf(camera->getSpacePos());

        mMatFaceGroupMap.clear();
        mFaceGroupSet.clear();

        // Scan through all the other leaf nodes looking for visibles
        int i = mLevel->mNumNodes - mLevel->mLeafStart;
        BspNode * nd = mLevel->mRootNode + mLevel->mLeafStart;

        /*
        if (firstTime)
        {
            camera->getViewMatrix(); // Force update view before report
            of.open("BspSceneManager.log");
            of << *camera << std::endl;
            of << "Camera Node: " << *cameraNode << std::endl;
            of << "Vertex Data: " << std::endl;
            for (int testi = 0; testi < mLevel->mNumVertices; ++testi)
            {
                of << " " << testi << ": pos(" <<
                  mLevel->mVertices[testi].position[0] << ", " <<
                    mLevel->mVertices[testi].position[1] << ", " << mLevel->mVertices[testi].position[2] << ")" <<
                    " uv(" << mLevel->mVertices[testi].texcoords[0] << ", " << mLevel->mVertices[testi].texcoords[1] << ")" <<
                    " lm(" << mLevel->mVertices[testi].lightmap[0] << ", " << mLevel->mVertices[testi].lightmap[1] << ")" << std::endl;
            }
            of << "Element data:" << std::endl;
            for (testi = 0; testi < mLevel->mNumElements; ++testi)
            {
                of << " " << testi << ": " << mLevel->mElements[testi] << std::endl;

            }
        }
        */

        while(i--)
        {
            if(mLevel->isLeafVisible(cameraNode, nd))
            {
                // Visible according to PVS, check bounding box against frustum
                if(camera->isIntersect(nd->getAABB()))
                {
                    //if (firstTime)
                    //{
                    //    of << "Visible Node: " << *nd << std::endl;
                    //}
                    processVisibleLeaf(nd, camera, visibleBounds, onlyShadowCasters);
                    if(mShowNodeAABs)
                        addBoundingBox(nd->getAABB(), true);
                }
            }
            nd++;
        }

        // TEST
        //if (firstTime)
        //{
        //    of.close();
        //    firstTime = false;
        //}
        return cameraNode;
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::processVisibleLeaf(BspNode * leaf, Camera * cam,
		RenderBound * visibleBounds, bool onlyShadowCasters)
    {
        MaterialPtr pMat;
        // Skip world geometry if we're only supposed to process shadow casters
        // World is pre-lit
        if(!onlyShadowCasters)
        {
            // Parse the leaf node's faces, add face groups to material map
            int numGroups = leaf->getNumFaceGroups();
            int idx = leaf->getFaceGroupStart();

            while(numGroups--)
            {
                int realIndex = mLevel->mLeafFaceGroups[idx++];
                // Check not already included
                if(mFaceGroupSet.find(realIndex) != mFaceGroupSet.end())
                    continue;
                StaticFaceGroup * faceGroup = mLevel->mFaceGroups + realIndex;
                // Get Material pointer by handle
                pMat = MaterialManager::getSingleton().getByHandle(faceGroup->materialHandle);
                assert (!pMat.isNull());
                // Check normal (manual culling)
                SysCullingMode cullMode = pMat->get(0)->get(0)->getSysCullingMode();
                if(cullMode != SCM_NONE)
                {
                    NIIf dist = faceGroup->plane.distance(cam->getSpacePos());
                    if((dist < 0 && cullMode == SCM_BACK) || (dist > 0 && cullMode == SCM_FRONT))
                        continue; // skip
                }
                mFaceGroupSet.insert(realIndex);
                // Try to insert, will find existing if already there
                std::pair<MaterialFaceGroupMap::iterator, bool> matgrpi;
                matgrpi = mMatFaceGroupMap.insert(
					MaterialFaceGroupMap::value_type(pMat.getPointer(), vector<StaticFaceGroup*>::type()));
                // Whatever happened, matgrpi.first is map iterator
                // Need to get second part of that to get vector
                matgrpi.first->second.push_back(faceGroup);

                //if (firstTime)
                //{
                //    of << "  Emitting faceGroup: index=" << realIndex << ", " << *faceGroup << std::endl;
                //}
            }
        }

        // Add movables to render queue, provided it hasn't been seen already
        const BspNode::IntersectingObjectSet & objects = leaf->getObjects();
        BspNode::IntersectingObjectSet::const_iterator oi, oiend = objects.end();
        for(oi = objects.begin(); oi != oiend; ++oi)
        {
            if(mMovablesForRendering.find(*oi) == mMovablesForRendering.end())
            {
                // It hasn't been seen yet
                SpaceObj * mov = const_cast<SpaceObj *>(*oi); // hacky

				mov->queue(getRenderQueue(), cam, visibleBounds, onlyShadowCasters);

            }
        }
    }
    //-----------------------------------------------------------------------
    unsigned int BspSceneManager::cacheGeometry(unsigned int * pIndexes, const StaticFaceGroup * faceGroup)
    {
        // Skip sky always
        if(faceGroup->isSky)
            return 0;

        size_t idxStart, numIdx, vertexStart;

        if(faceGroup->fType == FGT_FACE_LIST)
        {
            idxStart = faceGroup->elementStart;
            numIdx = faceGroup->numElements;
            vertexStart = faceGroup->vertexStart;
        }
        else if (faceGroup->fType == FGT_PATCH)
        {

            idxStart = faceGroup->patchSurf->getIndexOffset();
            numIdx = faceGroup->patchSurf->getCurrentIndexCount();
            vertexStart = faceGroup->patchSurf->getVertexOffset();
        }
		else
		{
			// Unsupported face type
			return 0;
		}
        // Copy index data
        unsigned int * pSrc = static_cast<unsigned int *>(
            mLevel->mIndexes->lock(idxStart * sizeof(Nui32), numIdx * sizeof(Nui32), Buffer::MM_READ));
        // Offset the indexes here
        // we have to do this now rather than up-front because the
        // indexes are sometimes reused to address different vertex chunks
        for(size_t elem = 0; elem < numIdx; ++elem)
        {
            *pIndexes++ = *pSrc++ + vertexStart;
        }
        mLevel->mIndexes->unlock();

        // return number of elements
        return static_cast<unsigned int>(numIdx);
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::free(void)
    {
        // no need to delete index buffer, will be handled by shared pointer
        N_delete mRenderOp.mIndex;
		mRenderOp.mIndex = 0;
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::showNodeBoxes(bool show)
    {
        mShowNodeAABs = show;
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::addBoundingBox(const AABox & aab, bool visible)
    {
        /*
        unsigned long visibleColour;
        unsigned long nonVisibleColour;
        RenderSys * rs;
        Root::getSingleton().get(rs);
        rs->convertType(Colour::White, &visibleColour);
        rs->convertType(Colour::Blue, &nonVisibleColour);
        if(mShowNodeAABs)
        {
            // Add set of lines
            float * pVertices = (float*)mAABGeometry.pVertices + (mAABGeometry.numVertices*3);
            unsigned short * pIndexes = mAABGeometry.pIndexes + mAABGeometry.numIndexes;
            unsigned long * pColours = (unsigned long*)mAABGeometry.pDiffuseColour + mAABGeometry.numVertices;

            const Vector3 corner[8];

            corner[0] = aab.mMinimum;

            corner[1].x = aab.mMinimum.x;
            corner[1].y = aab.mMaximum.y;
            corner[1].z = aab.mMinimum.z;

            corner[2].x = aab.mMaximum.x;
            corner[2].y = aab.mMaximum.y;
            corner[2].z = aab.mMinimum.z;

            corner[3].x = aab.mMaximum.x;
            corner[3].y = aab.mMinimum.y;
            corner[3].z = aab.mMinimum.z;

            corner[4] = aab.mMaximum;

            corner[5].x = aab.mMinimum.x;
            corner[5].y = aab.mMaximum.y;
            corner[5].z = aab.mMaximum.z;

            corner[6].x = aab.mMinimum.x;
            corner[6].y = aab.mMinimum.y;
            corner[6].z = aab.mMaximum.z;

            corner[7].x = aab.mMaximum.x;
            corner[7].y = aab.mMinimum.y;
            corner[7].z = aab.mMaximum.z;

            int i;
            for(i = 0; i < 8; ++i)
            {
                *pVertices++ = corner->x;
                *pVertices++ = corner->y;
                *pVertices++ = corner->z;
                corner++;

                if (visible)
                {
                    *pColours++ = visibleColour;
                }
                else
                {
                    *pColours++ = nonVisibleColour;
                }

            }

            *pIndexes++ = 0 + mAABGeometry.numVertices;
            *pIndexes++ = 1 + mAABGeometry.numVertices;
            *pIndexes++ = 1 + mAABGeometry.numVertices;
            *pIndexes++ = 2 + mAABGeometry.numVertices;
            *pIndexes++ = 2 + mAABGeometry.numVertices;
            *pIndexes++ = 3 + mAABGeometry.numVertices;
            *pIndexes++ = 3 + mAABGeometry.numVertices;
            *pIndexes++ = 1 + mAABGeometry.numVertices;
            *pIndexes++ = 4 + mAABGeometry.numVertices;
            *pIndexes++ = 5 + mAABGeometry.numVertices;
            *pIndexes++ = 5 + mAABGeometry.numVertices;
            *pIndexes++ = 6 + mAABGeometry.numVertices;
            *pIndexes++ = 6 + mAABGeometry.numVertices;
            *pIndexes++ = 7 + mAABGeometry.numVertices;
            *pIndexes++ = 7 + mAABGeometry.numVertices;
            *pIndexes++ = 4 + mAABGeometry.numVertices;
            *pIndexes++ = 1 + mAABGeometry.numVertices;
            *pIndexes++ = 5 + mAABGeometry.numVertices;
            *pIndexes++ = 2 + mAABGeometry.numVertices;
            *pIndexes++ = 4 + mAABGeometry.numVertices;
            *pIndexes++ = 0 + mAABGeometry.numVertices;
            *pIndexes++ = 6 + mAABGeometry.numVertices;
            *pIndexes++ = 3 + mAABGeometry.numVertices;
            *pIndexes++ = 7 + mAABGeometry.numVertices;


            mAABGeometry.numVertices += 8;
            mAABGeometry.numIndexes += 24;
        }
        */
    }
    //-----------------------------------------------------------------------
    ViewPoint BspSceneManager::getSuggestedViewpoint(bool random)
    {
        if(mLevel.isNull() || mLevel->mPlayerStarts.size() == 0)
        {
            // No level, use default
            ViewPoint vp;
            vp.position = Vector3f::ZERO;
            vp.orientation = Quaternion::IDENTITY;
            return vp;
    
        }
        else
        {
            if(random)
            {
                size_t idx = (size_t)( Math::randomf() * mLevel->mPlayerStarts.size() );
                return mLevel->mPlayerStarts[idx];
            }
            else
            {
                return mLevel->mPlayerStarts[0];
            }
        }
    }
    //-----------------------------------------------------------------------
    SpaceNode * BspSceneManager::createNodeImpl()
    {
        return N_new BspSceneNode(this);
    }
    //-----------------------------------------------------------------------
    SpaceNode * BspSceneManager::createNodeImpl(const String & name)
    {
        return N_new BspSceneNode(this, name);
    }
    //-----------------------------------------------------------------------
    void BspSceneManager::_notifyObjectMoved(const SpaceObj * mov, const Vector3 & pos)
    {
		if (!mLevel.isNull())
		{
			mLevel->_notifyObjectMoved(mov, pos);
		}
    }
    //-----------------------------------------------------------------------
	void BspSceneManager::_notifyObjectDetached(const SpaceObj* mov)
	{
		if (!mLevel.isNull())
		{
			mLevel->_notifyObjectDetached(mov);
		}
	}
	//-----------------------------------------------------------------------
	void BspSceneManager::destroyAllCreateObj(void)
	{
		SpaceManager::destroyAllCreateObj();
		free();
		// Clear level
		mLevel.setNull();
	}
    //-----------------------------------------------------------------------
    /*
    AABSpaceQuery * BspSceneManager::createAABBQuery(const AABox & box, Nmark mark)
    {
        // TODO
        return NULL;
    }
    //-----------------------------------------------------------------------
    SphereSpaceQuery * BspSceneManager::createSphereQuery(const Sphere & sphere, Nmark mark)
    {
        // TODO
        return NULL;
    }
    */
    //-----------------------------------------------------------------------
    RaySpaceQuery * BspSceneManager::createRayQuery(const Ray & ray, Nmark mark)
    {
        BspRaySpaceQuery * q = N_new BspRaySpaceQuery(this);
        q->setRay(ray);
        q->setQueryMask(mark);
        return q;
    }
    //-----------------------------------------------------------------------
    IntersectionSpaceQuery * BspSceneManager::createIntersectQuery(Nmark mark)
    {
        BspIntersectionSpaceQuery * q = N_new BspIntersectionSpaceQuery(this);
        q->setQueryMask(mark);
        return q;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // BspIntersectionSpaceQuery
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    BspIntersectionSpaceQuery::BspIntersectionSpaceQuery(SpaceManager * creator) : 
        IntersectionSpaceQuery(creator)
    {
        // Add bounds fragment type
        mSupportPrimitivesMark &= SpaceQuery::PT_PlaneList;
    }
    //-----------------------------------------------------------------------
    void BspIntersectionSpaceQuery::execute(SpaceQueryCmd * listener)
    {
        /*
        Go through each leaf node in BspLevel and check movables against each other and world
        Issue: some movable-movable intersections could be reported twice if 2 movables
        overlap 2 leaves?
        */
        const BspLevelPtr & lvl = ((BspSceneManager *)listener->mArea)->getLevel();
		if(lvl.isNull())
            return;

        BspNode * leaf = lvl->getLeafStart();
        int numLeaves = lvl->getNumLeaves();

        while(numLeaves--)
        {
            const BspNode::IntersectingObjectSet& objects = leaf->getObjects();
            int numObjects = (int)objects.size();

            BspNode::IntersectingObjectSet::const_iterator a, b, theEnd;
            theEnd = objects.end();
            a = objects.begin();
            for(int oi = 0; oi < numObjects; ++oi, ++a)
            {
                const SpaceObj * aObj = *a;
                // Skip this object if collision not enabled
                if(!(aObj->getQueryFlags() & mQueryMask) ||
					!(aObj->getTypeMark() & mQueryTypeMask) ||
                        !aObj->isActive())
                    continue;

                if(oi < (numObjects-1))
                {
                    // Check object against others in this node
                    b = a;
                    for(++b; b != theEnd; ++b)
                    {
                        const SpaceObj * bObj = *b;
                        // Apply mask to b (both must pass)
                        if((bObj->getQueryFlags() & mQueryMask) &&
							(bObj->getTypeMark() & mQueryTypeMask) && bObj->isActive())
                        {
                            const AABox & box1 = aObj->getSpaceAABB();
                            const AABox & box2 = bObj->getSpaceAABB();

                            if(box1.intersects(box2))
                            {
                                if(!listener->onQuery(CollisionQueryDetail(aObj, bObj))
									return;
                            }
                        }
                    }
                }
                // Check object against brushes
                if(mQueryTypeMask & SpaceManager::SpaceGeoMark)
                {
                    const BspNode::NodeBrushList & brushes = leaf->getSolidBrushes();
                    BspNode::NodeBrushList::const_iterator bi, biend = brushes.end();
                    NIIf radius = aObj->getCenterRange();
                    const Vector3 & pos = aObj->getSpaceNode()->getSpacePos();

                    for(bi = brushes.begin(); bi != biend; ++bi)
                    {
						list<Plane>::type::const_iterator planeit, planeitend;
                        planeitend = (*bi)->planes.end();
                        bool brushIntersect = true; // Assume intersecting for now

                        for(planeit = (*bi)->planes.begin(); planeit != planeitend; ++planeit)
                        {
                            NIIf dist = planeit->distance(pos);
                            if(dist > radius)
                            {
                                // Definitely excluded
                                brushIntersect = false;
                                break;
                            }
                        }
                        if(brushIntersect)
                        {
                            // report this brush as it's QueryDetail
                            assert((*bi)->fragment.mType == SpaceQuery::PT_PlaneList);
                            if(!listener->queryResult(aObj, const_cast<QueryDetail*>(&((*bi)->fragment))))
								return;
                        }
                    }
                }
            }
            ++leaf;
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // BspRaySpaceQuery
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    BspRaySpaceQuery::BspRaySpaceQuery(SpaceManager * creator) :
        RaySpaceQuery(creator)
    {
        // Add supported fragment types
        mSupportPrimitivesMark &= SpaceQuery::PT_Point;
        mSupportPrimitivesMark &= SpaceQuery::PT_PlaneList;
    }
    //-----------------------------------------------------------------------
    void BspRaySpaceQuery::execute(SpaceQueryCmd * listener)
    {
        clearTemporaries();
		BspLevelPtr lvl = static_cast<BspSceneManager *>(listener->mArea)->getLevel();
		if(!lvl.isNull())
		{
			processNode(lvl->getRootNode(), mRay, listener);
		}
    }
    //-----------------------------------------------------------------------
    BspRaySpaceQuery::~BspRaySpaceQuery()
    {
        clearTemporaries();
    }
    //-----------------------------------------------------------------------
    void BspRaySpaceQuery::clearTemporaries()
    {
        mObjsThisQuery.clear();
		vector<QueryDetail*>::type::iterator i, iend = mSingleIntersections.end();
        for(i = mSingleIntersections.begin(); i != iend; ++i)
        {
            OGRE_FREE(*i, MEMCATEGORY_SCENE_CONTROL);
        }
        mSingleIntersections.clear();
    }
    //-----------------------------------------------------------------------
    bool BspRaySpaceQuery::processNode(const BspNode* node, const Ray& tracingRay,
        SpaceQueryCmd* listener, NIIf maxDistance, NIIf traceDistance)
    {
        if (node->isLeaf())
        {
            return processLeaf(node, tracingRay, listener, maxDistance, traceDistance);
        }

        bool res = true;
        std::pair<bool, NIIf> result = tracingRay.intersects(node->getSplitPlane());
        if (result.first && result.second < maxDistance)
        {
            // Crosses the split plane, need to perform 2 queries
            // Calculate split point ray
            Vector3 splitPoint = tracingRay.getOrigin()
                + tracingRay.getDirection() * result.second;
            Ray splitRay(splitPoint, tracingRay.getDirection());

            if (node->getSide(tracingRay.getOrigin()) == Plane::PT_Negative)
            {
                // Intersects from -ve side, so do back then front
                res = processNode(
                    node->getBack(), tracingRay, listener, result.second, traceDistance);
                if (!res) return res;

                res = processNode(
                    node->getFront(), splitRay, listener,
                    maxDistance - result.second,
                    traceDistance + result.second);
            }
            else
            {
                // Intersects from +ve side, so do front then back
                res = processNode(node->getFront(), tracingRay, listener,
                    result.second, traceDistance);
                if (!res) return res;
                res = processNode(node->getBack(), splitRay, listener,
                    maxDistance - result.second,
                    traceDistance + result.second);
            }
        }
        else
        {
            // Does not cross the splitting plane, just cascade down one side
            res = processNode(node->getNextNode(tracingRay.getOrigin()),
                tracingRay, listener, maxDistance, traceDistance);
        }

        return res;
    }
    //-----------------------------------------------------------------------
    bool BspRaySpaceQuery::processLeaf(const BspNode* leaf, const Ray& tracingRay,
        SpaceQueryCmd * listener, NIIf maxDistance, NIIf traceDistance)
    {
        const BspNode::IntersectingObjectSet& objects = leaf->getObjects();

        BspNode::IntersectingObjectSet::const_iterator i, iend = objects.end();
        //Check ray against objects
        for(i = objects.begin(); i != iend; ++i)
        {
            // cast away constness, constness of node is nothing to do with objects
            SpaceObj* obj = const_cast<SpaceObj*>(*i);
            // Skip this object if not enabled
            if(!(obj->getQueryFlags() & mQueryMask) || !((obj->getTypeMark() & mQueryTypeMask)))
                continue;

            // check we haven't reported this one already
            // (objects can be intersecting more than one node)
            if(mObjsThisQuery.find(obj) != mObjsThisQuery.end())
                continue;

            //Test object as bounding box
            std::pair<bool, NIIf> result = tracingRay.intersects(obj->getSpaceAABB());
            // if the result came back positive and intersection point is inside
            // the node, fire the event handler
            if(result.first && result.second <= maxDistance)
            {
                if(!listener->onQuery(SpaceObjQueryDetail(obj, result.second + traceDistance)))
					return false;
            }
        }

        // Check ray against brushes
        if(mQueryTypeMask & SpaceManager::SpaceGeoMark)
        {
            const BspNode::NodeBrushList & brushList = leaf->getSolidBrushes();
            BspNode::NodeBrushList::const_iterator bi, biend = brushList.end();
            bool intersectedBrush = false;
            for(bi = brushList.begin(); bi != biend; ++bi)
            {
                BspNode::Brush * brush = *bi;

                std::pair<bool, NIIf> result = tracingRay.intersects(brush->planes, Plane::PT_Positive);
                // if the result came back positive and intersection point is inside
                // the node, check if this brush is closer
                if(result.first && result.second <= maxDistance)
                {
                    intersectedBrush = true;
                    if(mPrimitivesMark & SpaceQuery::PT_Point)
                    {
                        // We're interested in a single intersection
                        // Have to create these
                        const Vector3f & vt = tracingRay.getPoint(result.second);
                        // save this so we can clean up later
                        mSingleIntersections.push_back(vt);
                        if(!listener->onQuery(VertexQueryDetail(vt, result.second + traceDistance)))
							return false;
                    }
                    if(mPrimitivesMark & SpaceQuery::PT_PlaneList)
                    {
                        // We want the whole bounded volume
                        assert((*bi)->fragment.mType == SpaceQuery::PT_PlaneList);
                        if(!listener->onQuery(PlaneListQueryDetail(&(brush->fragment),
                            result.second + traceDistance)))
							return false;
                    }
                }
            }
            if (intersectedBrush)
            {
                return false; // stop here
            }
        }

        return true;
    }
    //-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	const String BspSceneManagerFactory::FACTORY_TYPE_NAME = "BspSceneManager";
	//-----------------------------------------------------------------------
	const SpaceTypeVerbose & BspSceneManagerFactory::getVerboseImpl() const
	{
		mVerbose.mTypeID = FACTORY_TYPE_NAME;
		mVerbose.mVerbose = "Scene manager for loading Quake3 .bsp files.";
		mVerbose.mMark = SpaceTypeVerbose::ST_Intersect | SpaceTypeVerbose::ST_World;
        return mVerbose;
	}
	//-----------------------------------------------------------------------
	SpaceManager * BspSceneManagerFactory::create(const String & name)
	{
		return N_new BspSceneManager(name);
	}
	//-----------------------------------------------------------------------
	void BspSceneManagerFactory::destroy(SpaceManager * obj)
	{
		N_delete obj;
	}
    //-----------------------------------------------------------------------
}

