/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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