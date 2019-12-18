/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_PLANESPACE_H_
#define _NII_PLANESPACE_H_

#include "NiiPreInclude.h"
#include "NiiPlane.h"
#include "NiiAABox.h"
#include "NiiSphere.h"

namespace NII
{
    /** 平面限制空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSpace
    {
    public:
        PlaneSpace();
        PlaneSpace(Plane::PositionType outside);

        /** 添加限制面
        @version NIIEngine 3.0.0
        */
        void add(const Plane & o);
        
        /** 移去限制面
        @version NIIEngine 3.0.0
        */
        void remove(const Plane & o);
        
        /** 移去所有限制面
        @version NIIEngine 3.0.0
        */
        void clear();
        
        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & box) const;
        
        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Sphere & sphere) const;
    public:
        PlaneList mPlanes;
        Plane::PositionType mOuterType;
    };
    typedef vector<PlaneSpace>::type PlaneSpaceList;
}
#endif