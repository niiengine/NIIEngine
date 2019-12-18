/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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
    class _EngineAPI ViewRectGeo : public RenderObj
    {
    public:
        ViewRectGeo(bool texcoord = false, Nmark bufmark = Buffer::M_WRITE);
        ViewRectGeo(SpaceID sid, bool texcoord = false, Nmark bufmark = Buffer::M_WRITE);

        ~ViewRectGeo();

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