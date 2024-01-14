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
OgrePCZSpaceQuery.h  -  description
-----------------------------------------------------------------------------
begin                : Wed Feb 21, 2007
author               : Eric Cha
email                : ericc@xenopi.com
Code Style Update	 :
-----------------------------------------------------------------------------
*/

#ifndef PCZSCENEQUERY_H
#define PCZSCENEQUERY_H

#include <OgreSceneManager.h>
#include "OgrePCZPrerequisites.h"
#include "OgrePCZone.h"


namespace Ogre
{
    /** PCZ implementation of AreaSpaceQuery. */
    class _OgrePCZPluginExport PCZIntersectionSpaceQuery :  public AreaSpaceQuery
    {
    public:
        PCZIntersectionSpaceQuery(SpaceManager* creator);
        ~PCZIntersectionSpaceQuery();

        /** See AreaSpaceQuery. */
        void execute(SpaceQueryCmd * cmd);
    };
    /** PCZ implementation of AABSpaceQuery. */
    class _OgrePCZPluginExport PCZAxisAlignedBoxSpaceQuery : public AABSpaceQuery
    {
    public:
        PCZAxisAlignedBoxSpaceQuery(SpaceManager* creator);
        ~PCZAxisAlignedBoxSpaceQuery();

        /** See RaySpaceQuery. */
        void execute(SpaceQueryCmd * cmd);

		/** set the zone to start the scene query */
        void setStartZone(PCZone * startZone) {mStartZone = startZone;}
        /** set node to exclude from query */
        void setExcludeNode(SpaceNode * excludeNode) {mExcludeNode = excludeNode;}
	protected:
		PCZone * mStartZone;
        SpaceNode * mExcludeNode;
	};
    /** PCZ implementation of RaySpaceQuery. */
    class _OgrePCZPluginExport PCZRaySpaceQuery : public RaySpaceQuery
    {
    public:
        PCZRaySpaceQuery(SpaceManager* creator);
        ~PCZRaySpaceQuery();

        /** See RayScenQuery. */
        void execute(SpaceQueryCmd * cmd);

		/** set the zone to start the scene query */
        void setStartZone(PCZone * startZone) {mStartZone = startZone;}
        /** set node to exclude from query */
        void setExcludeNode(SpaceNode * excludeNode) {mExcludeNode = excludeNode;}
	protected:
		PCZone * mStartZone;
        SpaceNode * mExcludeNode;
    };
    /** PCZ implementation of SphereSpaceQuery. */
    class _OgrePCZPluginExport PCZSphereSpaceQuery : public SphereSpaceQuery
    {
    public:
        PCZSphereSpaceQuery(SpaceManager* creator);
        ~PCZSphereSpaceQuery();

        /** See SpaceQuery. */
        void execute(SpaceQueryCmd* cmd);

		/** set the zone to start the scene query */
        void setStartZone(PCZone * startZone) {mStartZone = startZone;}
        /** set node to exclude from query */
        void setExcludeNode(SpaceNode * excludeNode) {mExcludeNode = excludeNode;}
	protected:
		PCZone * mStartZone;
        SpaceNode * mExcludeNode;
    };
    /** PCZ implementation of PlaneSpaceQuery. */
    class _OgrePCZPluginExport PCZPlaneSpaceQuery : public PlaneSpaceQuery
    {
    public:
        PCZPlaneSpaceQuery(SpaceManager* creator);
        ~PCZPlaneSpaceQuery();

        /** See SpaceQuery. */
        void execute(SpaceQueryCmd* cmd);

		/** set the zone to start the scene query */
        void setStartZone(PCZone * startZone) {mStartZone = startZone;}
        /** set node to exclude from query */
        void setExcludeNode(SpaceNode * excludeNode) {mExcludeNode = excludeNode;}
	protected:
		PCZone * mStartZone;
        SpaceNode * mExcludeNode;
	};


}

#endif


