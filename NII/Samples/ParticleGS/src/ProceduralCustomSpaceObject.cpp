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

#include "ProceduralCustomSpaceObj.h"

namespace Ogre
{
	//-----------------------------------------------------------------------
	void ProceduralCustomSpaceObj::queue(RenderQueue * queue)
	{
		mR2vbObject->update(mParentSceneManager);
		queue->add(this);
	}
	//-----------------------------------------------------------------------
	void ProceduralCustomSpaceObj::getGeometry(GeometryRaw & op) const
	{
		mR2vbObject->getGeometry(op);
	}
	//-----------------------------------------------------------------------
	void ProceduralCustomSpaceObj::setCustomSpaceObj(Ogre::CustomSpaceObj * obj)
	{
		mCustomSpaceObj = obj;
		mParentSceneManager = obj->getManager();
		if(mR2vbObject != 0)
		{
			mR2vbObject->setGeo(obj->getGeo(0));
		}
	}
    //-----------------------------------------------------------------------
    CustomSpaceObj * ProceduralCustomSpaceObj::getCustomSpaceObj() const
    {
        return mCustomSpaceObj;
    }
    //-----------------------------------------------------------------------
    void ProceduralCustomSpaceObj::setRenderToVertexBuffer(DisplayBuffer * obj)
    {
        mR2vbObject = obj;
    }
    //-----------------------------------------------------------------------
    const DisplayBuffer * ProceduralCustomSpaceObj::getRenderToVertexBuffer()
    {
        return mR2vbObject;
    }
    //-----------------------------------------------------------------------
	FactoryID ProceduralCustomSpaceObj::getFactoryID() const
	{
		return "ProceduralCustomSpaceObj";
	}
    //-----------------------------------------------------------------------
    ProceduralCustomSpaceObjFactory::ProceduralCustomSpaceObjFactory()
    {
    }
    //-----------------------------------------------------------------------
    ProceduralCustomSpaceObjFactory::~ProceduralCustomSpaceObjFactory()
    {
    }
	//-----------------------------------------------------------------------
	FactoryID ProceduralCustomSpaceObjFactory::getID() const
	{
		return "ProceduralCustomSpaceObj";
	}
	//-----------------------------------------------------------------------
	SpaceObj * ProceduralCustomSpaceObjFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
	{
		SpaceObj * re = N_new ProceduralCustomSpaceObj();
        re->setFactory(this);
        re->setManager(mag);
        return re;
	}
	//-----------------------------------------------------------------------
	void ProceduralCustomSpaceObjFactory::destroy(SpaceObj * obj)
	{
		N_delete obj;
	}
    //-----------------------------------------------------------------------
}
