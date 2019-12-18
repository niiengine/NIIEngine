/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-1

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

#ifndef _NII_LINEARSPLINE_H_
#define _NII_LINEARSPLINE_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiMatrix4.h"

namespace NII
{
    /** 实现线性样条曲线
    @remark Catmull-Rom
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LinearSpline
    {
    public:
        LinearSpline();
        ~LinearSpline();

        /** 添加控制点
        @param ctl 控制点
        @version NIIEngine 3.0.0
        */
        void add(const Vector3f & ctl);

        /** 获取控制点
        @param index 索引
        @version NIIEngine 3.0.0
        */
        const Vector3f & get(Nidx index) const;

        /** 更新控制点
        @param[in] index 索引
        @param[in] ctl 控制点
        @remark 这个点必须存在于样条中
        */
        void set(Nidx index, const Vector3f & ctl);

        /** 控制点数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 构建切线
        @remark 控制点变化后要调用
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** 清除所有控制点
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 插值
        @param[in] t 插值系数
        */
        Vector3f interpolate(NIIf t) const;

        /** 插值
        @param[in] first 第一值
        @param[in] t 第一值与下一值插值系数
        */
        Vector3f interpolate(Nidx first, NIIf t) const;
    protected:
        vector<Vector3f >::type mPoints;
        vector<Vector3f >::type mTangents;
        Matrix4f mHermite;
    };
}
#endif