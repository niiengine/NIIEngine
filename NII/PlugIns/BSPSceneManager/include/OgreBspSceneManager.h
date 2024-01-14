/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __BspSceneManager_H__
#define __BspSceneManager_H__

#include "OgreBspPrerequisites.h"
#include "OgreSceneManager.h"
#include "OgreStaticFaceGroup.h"
#include "OgreRenderOperation.h"
#include "OgreBspLevel.h"
#include <set>

namespace Ogre 
{
    /** 保存位置与方向对的结构
    @version NIIEngine 3.0.0
    */
    struct ViewPoint
    {
        Vector3f position;
        Quaternion orientation;
    };
    
    /** Specialisation of the SpaceManager class to deal with indoor scenes
        based on a BSP tree.
        This class refines the behaviour of the default SpaceManager to manage
        a scene whose bulk of geometry is made up of an indoor environment which
        is organised by a Binary Space Partition (BSP) tree. </p>
        A BSP tree progressively subdivides the space using planes which are the nodes of the tree.
        At some point we stop subdividing and everything in the remaining space is part of a 'leaf' which
        contains a number of polygons. Typically we traverse the tree to locate the leaf in which a
        point in space is (say the camera origin) and work from there. A second structure, the
        Potentially Visible Set, tells us which other leaves can been seen from this
        leaf, and we test their bounding boxes against the camera frustum to see which
        we need to draw. Leaves are also a good place to start for collision detection since
        they divide the level into discrete areas for testing.</p>
        This BSP and PVS technique has been made famous by engines such as Quake and Unreal. Ogre
        provides support for loading Quake3 level files to populate your world through this class,
        by calling the BspSceneManager::setSceneGeo. Note that this interface is made
        available at the top level of the SpaceManager class so you don't have to write your code
        specifically for this class - just call Root::getSpaceManager passing a SpaceType of ST_Intersect
        and in the current implementation you will get a BspSceneManager silently disguised as a
        standard SpaceManager.
    */
    class BspSceneManager : public SpaceManager
    {
    protected:
        // World geometry
        BspLevelPtr mLevel;

        // State variables for rendering WIP
        // Set of face groups (by index) already included
		typedef set<int>::type FaceGroupSet;
        FaceGroupSet mFaceGroupSet;
        // Material -> face group hashmap
		typedef map<Material *, vector<StaticFaceGroup *>::type, Material::LessSort>::type MaterialFaceGroupMap;
        MaterialFaceGroupMap mMatFaceGroupMap;

        GeometryRaw mRenderOp;

        // Debugging features
        bool mShowNodeAABs;
        GeometryRaw mAABGeometry;

        /** Walks the BSP tree looking for the node which the camera
            is in, and tags any geometry which is in a visible leaf for
            later processing.
            @param camera Pointer to the viewpoint.
            @return The BSP node the camera was found in, for info.
        */
        BspNode* walkTree(Camera * camera, RenderBound * visibleBounds, bool onlyShadowCasters);
        /** Tags geometry in the leaf specified for later rendering. */
        void processVisibleLeaf(BspNode * leaf, Camera * cam, 
			RenderBound * visibleBounds, bool onlyShadowCasters);

        /** Caches a face group for imminent rendering. */
        unsigned int cacheGeometry(unsigned int* pIndexes, const StaticFaceGroup* faceGroup);

        /** Frees up allocated memory for geometry caches. */
        void free(void);

        /** Adds a bounding box to draw if turned on. */
        void addBoundingBox(const AABox& aab, bool visible);

        /** Renders the static level geometry tagged in walkTree. */
        void renderStaticSpace(void);

		/** @copydoc SpaceManager::destroyAllCreateObj */
		void destroyAllCreateObj(void);

		// Overridden so we can manually render world geometry
		bool onEndRenderQueue(GroupID id);

		typedef set<const SpaceObj *>::type MovablesForRendering;
        MovablesForRendering mMovablesForRendering;
    public:
        BspSceneManager(const String & name);
        ~BspSceneManager();

        /** Specialised from SpaceManager to support Quake3 bsp files. */
        void setSceneGeo(const String & filename);
        
        /** Specialised from SpaceManager to support Quake3 bsp files. */
        void setSceneGeo(DataStream * stream, const String & type);

		/** Tells the manager whether to draw the axis-aligned boxes that surround
            nodes in the Bsp tree. For debugging purposes.
        */
        void showNodeBoxes(bool show);

        /** Specialised to suggest viewpoints. 
        */
        ViewPoint getSuggestedViewpoint(bool random = false);

        const BspLevelPtr & getLevel(void) {return mLevel; }

        /** Overriden from SpaceManager. */
        void findSpace(Camera * cam, bool onlycast);

        /** Creates a specialized BspSceneNode */
        SpaceNode * createNodeImpl(void);
        
        /** Creates a specialized BspSceneNode */
        SpaceNode * createNodeImpl(const String & name);

        /** Internal method for tagging BspNodes with objects which intersect them. */
        void _notifyObjectMoved(const SpaceObj * mov, const Vector3 & pos);
        
		/** Internal method for notifying the level that an object has been detached from a node */
		void _notifyObjectDetached(const SpaceObj * mov);

        /** Creates an AABSpaceQuery for this scene manager. 
        @remarks
            This method creates a new instance of a query object for this scene manager, 
            for an axis aligned box region. See SpaceQuery and AABSpaceQuery 
            for full details.
        @par
            The instance returned from this method must be destroyed by calling
            SpaceManager::destroyQuery when it is no longer required.
        @param box Details of the box which describes the region for this query.
        @param mask The query mask to apply to this query; can be used to filter out
            certain objects; see SpaceQuery for details.
        */
        /*
        virtual AABSpaceQuery * createAABBQuery(const AABox & box, Nmark mark = 0xFFFFFFFF);
        */
        /** Creates a SphereSpaceQuery for this scene manager. 
        @remarks
            This method creates a new instance of a query object for this scene manager, 
            for a spherical region. See SpaceQuery and SphereSpaceQuery 
            for full details.
        @par
            The instance returned from this method must be destroyed by calling
            SpaceManager::destroyQuery when it is no longer required.
        @param sphere Details of the sphere which describes the region for this query.
        @param mask The query mask to apply to this query; can be used to filter out
            certain objects; see SpaceQuery for details.
        */
        /*
        virtual SphereSpaceQuery * createSphereQuery(const Sphere & sphere, Nmark mark = 0xFFFFFFFF);
        */
        /** Creates a RaySpaceQuery for this scene manager. 
        @remarks
            This method creates a new instance of a query object for this scene manager, 
            looking for objects which fall along a ray. See SpaceQuery and RaySpaceQuery 
            for full details.
        @par
            The instance returned from this method must be destroyed by calling
            SpaceManager::destroyQuery when it is no longer required.
        @param ray Details of the ray which describes the region for this query.
        @param mask The query mask to apply to this query; can be used to filter out
            certain objects; see SpaceQuery for details.
        */
        virtual RaySpaceQuery * createRayQuery(const Ray & ray, Nmark mark = 0xFFFFFFFF);
        
        /** Creates an AreaSpaceQuery for this scene manager. 
        @remarks
            This method creates a new instance of a query object for locating
            intersecting objects. See SpaceQuery and AreaSpaceQuery
            for full details.
        @par
            The instance returned from this method must be destroyed by calling
            SpaceManager::destroyQuery when it is no longer required.
        @param mark The query mask to apply to this query; can be used to filter out
            certain objects; see SpaceQuery for details.
        */
        virtual AreaSpaceQuery * createIntersectQuery(Nmark mark = 0xFFFFFFFF);
    };

    /** BSP specialisation of AreaSpaceQuery */
    class BspIntersectionSpaceQuery : public AreaSpaceQuery
    {
    public:
        BspIntersectionSpaceQuery(SpaceManager * mag);

        /// @copydetails AreaSpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);
    };

    /** BSP specialisation of RaySpaceQuery */
    class BspRaySpaceQuery : public RaySpaceQuery
    {
    public:
        BspRaySpaceQuery(SpaceManager * mag);
        ~BspRaySpaceQuery();

        /// @copydetails RaySpaceQuery::execute
        void execute(SpaceQueryCmd * cmd);
    protected:
        void clearTemporaries(void);
        /** Internal processing of a single node.
        @return true if we should continue tracing, false otherwise
        */
        bool processNode(const BspNode * node, const Ray & tracingRay, SpaceQueryCmd * listener,
            NIIf maxDistance = Math::POS_INFINITY, NIIf traceDistance = 0.0f);
            
        /** Internal processing of a single leaf.
        @return true if we should continue tracing, false otherwise
        */
        bool processLeaf(const BspNode* node, const Ray& tracingRay, SpaceQueryCmd * listener,
            NIIf maxDistance = Math::POS_INFINITY, NIIf traceDistance = 0.0f);
    protected:
        /// Set for eliminating duplicates since objects can be in > 1 node
		set<SpaceObj *>::type mObjsThisQuery;
        /// list of the last single intersection world fragments (derived)
		vector<SpaceQueryCmd::QueryDetail *>::type mSingleIntersections;
    };

	/// Factory for BspSceneManager
	class BspSceneManagerFactory : public SpaceTypeFactory
	{
	protected:
		const SpaceTypeVerbose & getVerboseImpl() const;
	public:
		BspSceneManagerFactory() {}
		~BspSceneManagerFactory() {}
		/// Factory type name
		static const String FACTORY_TYPE_NAME;
        
		SpaceManager * create(const String & name);
        
		void destroy(SpaceManager * obj);
	};
}

#endif
