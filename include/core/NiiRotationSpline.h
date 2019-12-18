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

#ifndef _NII_ROTATIONSPLINE_H_
#define _NII_ROTATIONSPLINE_H_

#include "NiiPreInclude.h"
#include "NiiQuaternion.h"

namespace NII
{
    /** 方向专用样条
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RotationSpline
    {
    public:
        RotationSpline();
        ~RotationSpline();

        /** 添加控制点
        @param[in] ctl 控制点
        @version NIIEngine 3.0.0
        */
        void add(const Quaternion & ctl);

        /** 获取控制点
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        const Quaternion & get(Nidx index) const;

        /** 设置获取控制点
        @param[in] index 索引
        @param[in] ctl 控制点
        @verison NIIEngine 3.0.0
        */
        void set(Nidx index, const Quaternion & ctl);

        /** 获取控制点数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 清除这个样条所有点
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 构建切线
        @remark 控制点变化后要调用
        @version NIIEngine 3.0.0
        */
        void rebuild();

        /** 插值
        @param[in] t 插值系数
        @param[in] shortpath 是否最短距离
        @version NIIEngine 3.0.0
        */
        Quaternion interpolate(NIIf t, bool shortpath = true);

        /** 插值
        @param[in] first 第一值
        @param[in] t 第一值与下一值插值系数
        @param[in] shortpath 是否最短距离
        @version NIIEngine 3.0.0
        */
        Quaternion interpolate(Nidx first, NIIf t, bool shortpath = true);
    protected:
        vector<Quaternion>::type mPoints;
        vector<Quaternion>::type mTangents;
    };
}
#endif