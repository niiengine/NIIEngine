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

#ifndef _NII_PlaneSpaceObj_H_
#define _NII_PlaneSpaceObj_H_

#include "NiiPreInclude.h"
#include "NiiPlane.h"
#include "NiiSpaceObj.h"

namespace NII
{
    /** 定义一个平面空间
    @vesion NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSpaceObj : public Plane, public SpaceObj
    {
    public:
        PlaneSpaceObj(SpaceID sid);
        PlaneSpaceObj(const Plane & o);
        PlaneSpaceObj(const Vector3f & normals, NIIf constant);
        PlaneSpaceObj(const Vector3f & normals, const Vector3f & p);
        PlaneSpaceObj(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3);
        ~PlaneSpaceObj();

        /** 获取空间平面
        @version NIIEngine 3.0.0
        */
        const Plane & getSpacePlane() const;

        /// @copydetails SpaceObj::_notify
        void _notify(Camera * cam);

        /// @copydetails SpaceObj::getAABB
        const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;

        /// @copydetails SpaceObj::queue
        void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);
    protected:
        mutable Plane mSpacePlane;
        mutable Vector3f mSpacePos;
        mutable Quaternion mSpaceRot;
        mutable bool mDirty;
    };
}
#endif