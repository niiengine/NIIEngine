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

#include "NiiPreProcess.h"
#include "NiiRenderObj.h"
#include "NiiMaterialManager.h"
#include "NiiMaterial.h"
#include "NiiException.h"
#include "NiiSpaceNode.h"
#include "NiiGeometryObj.h"

namespace NII
{
    //-----------------------------------------------------------------------
    RenderObj::RenderObj() :
        SpaceObj(),
        mMatrix(Matrix4f::IDENTITY),
        mMaterialID(MaterialManager::WhiteLight->getOriginID()),
        mMaterial(const_cast<Material *>(MaterialManager::WhiteLight)),
        mCamera(NULL)
    {
        if(mMaterial != 0)
            mMaterial->ref(false);
    }
    //-----------------------------------------------------------------------
    RenderObj::RenderObj(SpaceID sid) :
        SpaceObj(sid),
        mMatrix(Matrix4f::IDENTITY),
        mMaterialID(BaseMaterialWhite),
        mMaterial(const_cast<Material *>(MaterialManager::WhiteLight)),
        mCamera(NULL)
    {
        if(mMaterial != 0)
            mMaterial->ref(false);
    }
    //----------------------------------------------------------------------
    RenderObj::~RenderObj()
    {
        if(mMaterial != 0)
            mMaterial->unref();
    }
    //-----------------------------------------------------------------------
    void RenderObj::setMaterial(ResourceID rid)
    {
        if(mMaterialID != rid)
        {
            mMaterialID = rid;
            if(mMaterial != 0)
                mMaterial->unref();
            mMaterial = static_cast<Material *>(N_Only(Material).get(mMaterialID));
            if(mMaterial == 0)
                N_EXCEPT(NotExist, _I18n("Could not find material ") + mMaterialID);

            mMaterial->ref();
        }
    }
    //----------------------------------------------------------------------
    const Material * RenderObj::getMaterial() const
    {
        return mMaterial;
    }
    //----------------------------------------------------------------------
    void RenderObj::getGeometry(GeometryRaw & op) const
    {
        op = mRenderOp;
    }
    //----------------------------------------------------------------------
    void RenderObj::setGeometry(const GeometryRaw & op)
    {
        mRenderOp = op;
    }
    //----------------------------------------------------------------------
    void RenderObj::setMatrix(const Matrix4f & local)
    {
        mMatrix = local;
    }
    //----------------------------------------------------------------------
    void RenderObj::getMatrix(Matrix4f * out, NCount & count) const
    {
        *out = mMatrix * mSpaceNode->getSpaceMatrix();
        count = 1;
    }
    //----------------------------------------------------------------------
    void RenderObj::_notify(Camera * cam)
    {
        SpaceObj::_notify(cam);
        mCamera = cam;
    }
    //----------------------------------------------------------------------
    void RenderObj::setAABB(const AABox & box)
    {
        mBox = box;
    }
    //----------------------------------------------------------------------
    const AABox & RenderObj::getAABB() const
    {
        return mBox;
    }
    //-----------------------------------------------------------------------
    NIIf RenderObj::getCenterRange() const
    {
        return mRadius;
    }
    //----------------------------------------------------------------------
    void RenderObj::queue(RenderQueue * queue)
    {
        queue->add(this, mRenderGroup, N_Geo_Default_Level);
    }
    //----------------------------------------------------------------------
    void RenderObj::query(GeometryQuery * query)
    {
        query->query(this);
    }
    //----------------------------------------------------------------------
    FactoryID RenderObj::getFactoryID() const
    {
        return N_FACTORY_RenderObj;
    }
    //----------------------------------------------------------------------
    void RenderObj::getLight(LightList & ll) const
    {
        query(ll);
    }
    //----------------------------------------------------------------------
}