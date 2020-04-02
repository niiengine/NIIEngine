/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-1

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

#ifndef _NII_NODE_KEYFRAME_H_
#define _NII_NODE_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiQuaternion.h"
#include "NiiVector3.h"

namespace NII
{
    /** 节点专用关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeKeyFrame : public KeyFrame
    {
    public:
        NodeKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~NodeKeyFrame();

        /** 设置关联这个关键针的平移
        @remark 平移因子,影响关键帧它的动画对象在它的时间索引中的平移(移动)数量
        @param[in] trans 通过该向量平移
        @version NIIEngine 3.0.0
        */
        void setTranslate(const Vector3f & trans);

        /** 获取这个关键针应用的平移
        @version NIIEngine 3.0.0
        */
        const Vector3f & getTranslate() const;

        /** 设置这个关键针应用到动画对象在它的时间索引中的缩放因子
        @param[in] scale
            通过该向量缩放(任何成分为0的值,它将缩放对象到0尺寸)
        @version NIIEngine 3.0.0
        */
        void setScale(const Vector3f & scale);

        /** 获取这个关键针应用的缩放因子
        @version NIIEngine 3.0.0
        */
        const Vector3f & getScale() const;

        /** 设置这个关键针应用的旋转
        @param[in] rot
            应用的旋转;如果你不喜欢直接使用四元数的话,使用元数的方法从angle/axis
            或Matrix3中转换.
        @version NIIEngine 3.0.0
        */
        void setRotation(const Quaternion & rot);

        /** 获取这个关键针应用的旋转
        @version NIIEngine 3.0.0
        */
        const Quaternion & getRotation() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Vector3f mTranslate;
        Vector3f mScale;
        Quaternion mRotate;
    };
}
#endif