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
/***************************************************************************
OgreOctreeSpaceQuery.h  -  description
-------------------
begin                : Tues July 20, 2004
copyright            : (C) 2004 by Jon Anderson
email                : janders@users.sf.net
***************************************************************************/

#ifndef OCTREESCENEQUERY_H
#define OCTREESCENEQUERY_H

#include "OgreOctreePrerequisites.h"
#include <OgreSceneManager.h>


namespace Ogre
{
/** Octree implementation of AreaSpaceQuery. */
class _OgreOctreePluginExport OctreeIntersectionSpaceQuery :  public AreaSpaceQuery
{
public:
    OctreeIntersectionSpaceQuery(SpaceManager * creator);
    ~OctreeIntersectionSpaceQuery();

    /** See AreaSpaceQuery. */
    void execute(SpaceQueryCmd * cmd);
};

/** Octree implementation of RaySpaceQuery. */
class _OgreOctreePluginExport OctreeRaySpaceQuery : public RaySpaceQuery
{
public:
    OctreeRaySpaceQuery(SpaceManager * creator);
    ~OctreeRaySpaceQuery();

    /** See RayScenQuery. */
    void execute(SpaceQueryCmd * cmd);
};
/** Octree implementation of SphereSpaceQuery. */
class _OgreOctreePluginExport OctreeSphereSpaceQuery : public SphereSpaceQuery
{
public:
    OctreeSphereSpaceQuery(SpaceManager* creator);
    ~OctreeSphereSpaceQuery();

    /** See SpaceQuery. */
    void execute(SpaceQueryCmd* cmd);
};
/** Octree implementation of PlaneSpaceQuery. */
class _OgreOctreePluginExport OctreePlaneSpaceQuery : public PlaneSpaceQuery
{
public:
    OctreePlaneSpaceQuery(SpaceManager* creator);
    ~OctreePlaneSpaceQuery();

    /** See SpaceQuery. */
    void execute(SpaceQueryCmd* cmd);
};
/** Octree implementation of AABSpaceQuery. */
class _OgreOctreePluginExport OctreeAxisAlignedBoxSpaceQuery : public AABSpaceQuery
{
public:
    OctreeAxisAlignedBoxSpaceQuery(SpaceManager* creator);
    ~OctreeAxisAlignedBoxSpaceQuery();

    /** See RaySpaceQuery. */
    void execute(SpaceQueryCmd* cmd);
};


}

#endif


