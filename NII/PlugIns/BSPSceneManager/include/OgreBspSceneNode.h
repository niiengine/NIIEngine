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
#ifndef __BSPSCENENODE_H__
#define __BSPSCENENODE_H__

#include "OgreBspPrerequisites.h"
#include "OgreSceneNode.h"

namespace Ogre {

    /** Specialisation of SpaceNode for the BspSceneManager.
    @remarks
        This specialisation of SpaceNode is to enable information about the
        leaf node in which any attached objects are held is stored for
        use in the visibility determination.
    @par
        Do not confuse this class with BspNode, which reflects nodes in the
        BSP tree itself. This class is just like a regular SpaceNode, except that
        it should be locating BspNode leaf elements which objects should be included
        in. Note that because objects are movable, and thus may very well be overlapping
        the boundaries of more than one leaf, that it is possible that an object attached
        to one BspSceneNode may actually be associated with more than one BspNode.
    */
    class BspSceneNode : public SpaceNode
    {
    public:
        BspSceneNode(SpaceManager* creator) : SpaceNode(creator) {}
        BspSceneNode(SpaceManager* creator, const String& name)
            : SpaceNode(creator, name) {}

        /// @copydetails PosNode::update
        void update(bool own, bool child);

        /// @copydetails SpaceNode::detach
        SpaceObj * detach(Nindex index);

        /// @copydetails SpaceNode::detach
        SpaceObj * detach(SpaceID sid);

        /// @copydetails SpaceNode::detachAll
        void detachAll(void);

		/// @copydetails SpaceNode::setEnable
		void setEnable(bool b);
    };
}
#endif