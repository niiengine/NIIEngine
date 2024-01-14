/***************************************************************************
octreescenemanager.h  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net
***************************************************************************/

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

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

#ifndef OCTREESCENEMANAGER_H
#define OCTREESCENEMANAGER_H

#include "OgreOctreePrerequisites.h"
#include "OgreSceneManager.h"
#include "OgreGeometryRaw.h"
#include "OgreSphere.h"

#include <list>
#include <algorithm>

#include "OgreOctree.h"


namespace Ogre
{
    class OctreeNode;

    class OctreeCamera;
    class OctreeIntersectionSpaceQuery;
    class OctreeRaySpaceQuery;
    class OctreeSphereSpaceQuery;
    class OctreeAxisAlignedBoxSpaceQuery;
    class OctreePlaneSpaceQuery;

    typedef list<WireAABGeo *>::type BoxList;
    typedef list<unsigned long>::type ColorList;
    //typedef list< SpaceNode * >::type SceneNodeList;

    /** Specialized SpaceManager that divides the geometry into an octree in order to facilitate spatial queries.
    @remarks
    */
    class _OgreOctreePluginExport OctreeSceneManager : public SpaceManager
    {
        friend class OctreeIntersectionSpaceQuery;
        friend class OctreeRaySpaceQuery;
        friend class OctreeSphereSpaceQuery;
        friend class OctreeAxisAlignedBoxSpaceQuery;
        friend class OctreePlaneSpaceQuery;

    public:
        static int intersect_call;
        /** Standard Constructor.  Initializes the octree to -10000,-10000,-10000 to 10000,10000,10000 with a depth of 8. */
        OctreeSceneManager(const String & name);
        OctreeSceneManager(const String & name, AABox & box, int max_depth);
        ~OctreeSceneManager();

        /** Initializes the manager to the given box and depth.
        */
        void init(AABox & box, int d);

        /** Creates a specialized OctreeNode */
        virtual SpaceNode * createNodeImpl ();
        
        /** Creates a specialized OctreeNode */
        virtual SpaceNode * createNodeImpl(SpaceID sid);
        
        /** Creates a specialized OctreeCamera */
        virtual Camera * createCamera(SpaceID sid);

        /** Deletes a scene node */
        virtual void destroySpaceNode(SpaceID sid);
        
        /** Recurses through the octree determining which nodes are visible. */
        virtual void findSpace(Camera * cam, bool onlycast);

        /** Alerts each unculled object, notifying it that it will be drawn.
         * Useful for doing calculations only on nodes that will be drawn, prior
         * to drawing them...
         */
        virtual void _alertVisibleObjects( void );

        /** Walks through the octree, adding any visible objects to the render queue.
        @remarks
        If any octant in the octree if completely within the view frustum,
        all subchildren are automatically added with no visibility tests.
        */
        void walkOctree(OctreeCamera *, RenderQueue *, Octree *, 
            RenderBound * visibleBounds, bool foundvisible, bool onlyShadowCasters);

        /** Checks the given OctreeNode, and determines if it needs to be moved
        * to a different octant.
        */
        void _updateOctreeNode(OctreeNode *);
        /** Removes the given octree node */
        void _removeOctreeNode(OctreeNode *);
        /** Adds the Octree Node, starting at the given octree, and recursing at max to the specified depth.
        */
        void _addOctreeNode(OctreeNode *, Octree *octree, int depth = 0);

        /** Recurses the octree, adding any nodes intersecting with the box into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const AABox & box, list<SpaceNode *>::type &list, SpaceNode *exclude = 0 );

        /** Recurses the octree, adding any nodes intersecting with the sphere into the given list.
        It ignores the exclude scene node.
        */
        void findNodesIn(const Sphere & sphere, list<SpaceNode *>::type & list, SpaceNode * exclude = 0);

        /** Recurses the octree, adding any nodes intersecting with the volume into the given list.
          It ignores the exclude scene node.
          */
        void findNodesIn(const PlaneSpace & volume, list<SpaceNode *>::type & list, SpaceNode * exclude = 0);

        /** Recurses the octree, adding any nodes intersecting with the ray into the given list.
          It ignores the exclude scene node.
          */
        void findNodesIn(const Ray & ray, list<SpaceNode *>::type & list, SpaceNode * exclude = 0);

        /** Sets the box visibility flag 
        */
        void setShowBoxes(bool b)
        {
            mShowBoxes = b;
        };

        /**
        @version NIIEngine 3.0.0
        */
        void setLooseOctree(bool b)
        {
            mLoose = b;
        };

        /** Resizes the octree to the given size 
        */
        void resize(const AABox & box);

        /// @copydetails 
        virtual bool setProperty(PropertyID, const void *);
        
        /// @copydetails 
        virtual bool getProperty(PropertyID, void *) const;
        
        /// @copydetails 
        bool getPropertyList(PropertyArray * list) const;
        
        /// @copydetails
        void destroyAllCreateObj();

        /// @copydetails
        AABSpaceQuery * createAABBQuery(const AABox & box, Nmark mask);
        
        /// @copydetails
        SphereSpaceQuery * createSphereQuery(const Sphere & sphere, Nmark mask);
        
        /// @copydetails
        PlaneSpaceQuery * createPlaneSpaceQuery(const PlaneSpaceList& volumes, Nmark mask);
        
        /// @copydetails
        RaySpaceQuery * createRayQuery(const Ray & ray, Nmark mask);
        
        /// @copydetails
        AreaSpaceQuery * createIntersectQuery(Nmark mask);
    protected:
        Octree::NodeList mVisible;

        /// The root octree
        Octree * mOctree;

        /// List of boxes to be rendered
        BoxList mBoxes;

        /// Number of rendered objs
        int mNumObjects;

        /// Max depth for the tree
        int mMaxDepth;
        /// Size of the octree
        AABox mBox;

        /// Boxes visibility flag
        bool mShowBoxes;

        bool mLoose;

        NIIf mCorners[ 24 ];
        static unsigned long mColors[ 8 ];
        static unsigned short mIndexes[ 24 ];

        Matrix4 mScaleFactor;

    };

    /// Factory for OctreeSceneManager
    class OctreeSceneManagerFactory : public SpaceTypeFactory
    {
    protected:
        const SpaceTypeVerbose & getVerboseImpl() const;
    public:
        OctreeSceneManagerFactory() {}
        ~OctreeSceneManagerFactory() {}
        /// Factory type name
        static const String FACTORY_TYPE_NAME;
        
        SpaceManager * create(const String & name);
        
        void destroy(SpaceManager* instance);
    };
}
#endif