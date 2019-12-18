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

#ifndef _NII_BOX_H_
#define _NII_BOX_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 3D空间盒
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Box
    {
    public:
        Box() : 
            mLeft(0), 
            mTop(0), 
            mRight(1), 
            mBottom(1), 
            mFront(0), 
            mBack(1)
        {
        }
        
        /** 定义盒子
        @remark (mFront = 0 and mBack = 1)
        @param[in] l 左边
        @param[in] t 上边
        @param[in] r 右边
        @param[in] b 下边
        */
        Box(NCount l, NCount t, NCount r, NCount b):
            mLeft(l),
            mTop(t),
            mRight(r),
            mBottom(b),
            mFront(0),
            mBack(1)
        {
        }
        
        /** 定义盒子
        @param[in] l 左边
        @param[in] t 上边
        @param[in] f 前边
        @param[in] r 右边
        @param[in] bt 下边
        @param[in] bb 后边
        */
        Box(NCount l, NCount t, NCount f, NCount r, NCount bt, NCount bb):
            mLeft(l),
            mTop(t),
            mRight(r),
            mBottom(bt),
            mFront(f),
            mBack(bb)
        {
        }

        /** 是否包含
        @verison NIIEngine 3.0.0
        */
        bool contains(const Box & o) const
        {
            return (o.mLeft >= mLeft && o.mTop >= mTop && o.mFront >= mFront &&
                o.mRight <= mRight && o.mBottom <= mBottom && o.mBack <= mBack);
        }

        /** 获取这个盒子的宽
        @version NIIEngine 3.0.0
        */
        NCount getWidth() const { return mRight - mLeft; }

        /** 获取这个盒子的高
        @version NIIEngine 3.0.0
        */
        NCount getHeight() const { return mBottom - mTop; }

        /** 获取这个盒子的深
        @version NIIEngine 3.0.0
        */
        NCount getDepth() const { return mBack - mFront; }
    public:
        NCount mLeft, mTop, mRight, mBottom, mFront, mBack;
    };
}
#endif