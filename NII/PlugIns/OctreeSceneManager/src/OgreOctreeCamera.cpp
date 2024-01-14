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
/***************************************************************************
octreecamera.cpp  -  description
-------------------
begin                : Fri Sep 27 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team

***************************************************************************/
#include <OgreMath.h>
#include <OgreAABox.h>
#include <OgreRoot.h>

#include <OgreOctreeCamera.h>

namespace Ogre
{
OctreeCamera::OctreeCamera(const String & name, SpaceManager * sm) :
    Camera(name, sm)
{
                                                                      
}

OctreeCamera::~OctreeCamera()
{
}

OctreeCamera::Visibility OctreeCamera::getVisibility(const AABox & bound)
{
    // Null boxes always invisible
    if(bound.isNull())
        return NONE;

    bool all_inside = true;
    for(int plane = 0; plane < 6; ++plane)
    {
        // Skip far plane if infinite view frustum
        if(plane == Frustum::PT_Far && mFarExtent == 0)
            continue;

		// This updates frustum planes and deals with cull frustum
        Plane::PositionType side = getClipPlane(plane).getSide(bound);
        if(side == Plane::PT_Negative) 
            return NONE;
        // We can't return now as the box could be later on the negative side of a plane.
        if(side == Plane::PT_Merge) 
            all_inside = false;
    }

    if(all_inside)
        return FULL;
    else
        return PARTIAL;
}

}