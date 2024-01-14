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

#ifndef _NII_RENDEROBJ_H_
#define _NII_RENDEROBJ_H_

#include "NiiPreInclude.h"
#include "NiiSpaceObj.h"
#include "NiiGeometryObj.h"
#include "NiiAABox.h"

namespace NII
{
    /** 渲染基础对象
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderObj : public SpaceObj, public GeometryObj
    {
    public:
        RenderObj();
        RenderObj(SpaceID sid);
        virtual ~RenderObj();

        /** 设置材质
        @version NIIEngine 3.0.0
        */
        void setMaterial(ResourceID rid);

        /** 设置渲染操作
        @version NIIEngine 3.0.0
        */
        virtual void setGeometry(const GeometryRaw & raw);

        /** 设置边界盒子
        @note 局部级
        @version NIIEngine 3.0.0
        */
        void setAABB(const AABox & box);

        /** 设置变换
        @note 局部级
        @version NIIEngine 3.0.0
        */
        void setMatrix(const Matrix4f & local);

        using SpaceObj::query;

        /// @copydetails GeometryObj::getGeometry
        virtual void getGeometry(GeometryRaw & raw) const;

        /// @copydetails GeometryObj:: getMaterial
        virtual const Material * getMaterial() const;

        /// @copydetails GeometryObj::getMatrix
        virtual void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::_notify
        virtual void _notify(Camera * cam);

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @copydetails SpaceObj::getAABB
        virtual const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;
        
        /// @copydetails SpaceObj::_update
        virtual void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::getFactoryID
        virtual FactoryID getFactoryID() const;
    protected:
        AABox mBox;
        NIIf mRadius;
        Matrix4f mMatrix;
        GeometryRaw mRenderOp;
        ResourceID mMaterialID;
        Material * mMaterial;
        Camera * mCamera;
    };
}
#endif