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

#ifndef _NII_ViewRectGeo_H_
#define _NII_ViewRectGeo_H_

#include "NiiPreInclude.h"
#include "NiiRenderObj.h"

namespace NII
{
    /** 视图面矩形
    @note 可以用于获取帧缓存
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RectGeo : public RenderObj
    {
    public:
        RectGeo(bool texcoord = false, Nmark bufmark = Buffer::M_WRITE);
        RectGeo(SpaceID sid, bool texcoord = false, Nmark bufmark = Buffer::M_WRITE);

        ~RectGeo();

        /** 设置矩形角落相对坐标
        @note 原点在 left(0), top(0), right(0), bottom(0)
        @param[in] left [-1, 1.0]
        @param[in] top [-1, 1.0]
        @param[in] right [-1, 1.0]
        @param[in] bottom [-1, 1.0]
        @version NIIEngine 3.0.0
        */
        void setRect(NIIf left, NIIf top, NIIf right, NIIf bottom, bool syncbounds = true);

        /** 设置矩形的法线
        @param[in] tl 左上角法线
        @param[in] bl 左下角法线
        @param[in] tr 右上角法线
        @param[in] br 右下角法线
        @version NIIEngine 3.0.0
        */
        void setNormals(const Vector3f & tl, const Vector3f & bl, const Vector3f & tr, const Vector3f & br);

        /** 设置纹理坐标
        @param[in] tl 左上角纹理坐标
        @param[in] bl 左下角纹理坐标
        @param[in] tr 右上角纹理坐标
        @param[in] br 右下角纹理坐标
        @version NIIEngine 3.0.0
        */
        void setTexCoord(const Vector2f & tl, const Vector2f & bl, const Vector2f & tr, const Vector2f & br);

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails SpaceObj::getCenterRange
        NIIf getCenterRange() const;
    protected:
        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;
    private:
        void buildGeo(bool texcoord, Nmark bufmark);
    };
}
#endif