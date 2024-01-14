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
#include "OgreBspSceneNode.h"
#include "OgreBspSceneManager.h"

namespace Ogre 
{
    void BspSceneNode::update(bool parentHasChanged, bool updateChildren)
    {
        bool checkMovables = false;

        if(mPosUnvalid || parentHasChanged)
        {
            // This means we've moved
            checkMovables = true;
        }

        // Call superclass
        SpaceNode::update(parentHasChanged, updateChildren);

        if(checkMovables)
        {
            // Check membership of attached objects
            AttachList::const_iterator it, itend = mAttachList.end();
            for (it = mAttachList.begin(); it != itend; ++it)
            {
                SpaceObj* mov = it->second;

                static_cast<BspSceneManager*>(mCreator)->_notifyObjectMoved(
                    mov, this->getSpacePos());

            }
        }

    }
	//-------------------------------------------------------------------------
	SpaceObj* BspSceneNode::detach(Nindex index)
	{
		SpaceObj* ret = SpaceNode::detach(index);
		static_cast<BspSceneManager*>(mCreator)->_notifyObjectDetached(ret);
		return ret;
		
	}
	//-------------------------------------------------------------------------
	SpaceObj* BspSceneNode::detach(SpaceID sid)
	{
		SpaceObj* ret = SpaceNode::detach(sid);
		static_cast<BspSceneManager*>(mCreator)->_notifyObjectDetached(ret);
		return ret;
	}
	//-------------------------------------------------------------------------
	void BspSceneNode::detachAll()
	{
		AttachList::const_iterator i, iend = mAttachList.end();
		for(i = mAttachList.begin(); i != iend; ++i)
		{
			static_cast<BspSceneManager*>(mCreator)->_notifyObjectDetached(i->second);
		}
		SpaceNode::detachAll();
	}
	//-------------------------------------------------------------------------
	void BspSceneNode::setEnable(bool inGraph)
	{
		if(mEnable != inGraph)
		{
			AttachList::const_iterator i, iend = mAttachList.end();
			for(i = mAttachList.begin(); i != iend; ++i)
			{
				if(!inGraph)
				{
					// Equivalent to detaching
					static_cast<BspSceneManager*>(mCreator)->_notifyObjectDetached(i->second);
				}
				else
				{
					// move deals with re-adding
	                static_cast<BspSceneManager*>(mCreator)->_notifyObjectMoved(
    	                i->second, this->getSpacePos());
				}
			}
		}
		mEnable = inGraph;
	}
}