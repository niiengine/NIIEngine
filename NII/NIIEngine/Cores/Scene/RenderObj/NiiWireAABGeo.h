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

#ifndef _NII_WireAABBGEO_H_
#define _NII_WireAABBGEO_H_

#include "NiiPreInclude.h"
#include "NiiRenderObj.h"

namespace NII
{
    /** 线型aabb
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WireAABGeo : public RenderObj
    {
    public:
        WireAABGeo();
        WireAABGeo(SpaceID sid);
        ~WireAABGeo();

        /** 设置aabb几何
        @version NIIEngine 3.0.0
        */
        void setAABBGeo(const AABox & aabb);

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;
    protected:
        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;
    };
}
#endif