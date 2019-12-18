/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-17

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

#ifndef _NII_NODE_FUSION_H_
#define _NII_NODE_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 节点混合实例
    @remark 设置插值类型配合LOD策略使用能有效的控制运行效率
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NodeFusion : public AnimationFusion
    {
    public:
        /** 平面插值类型.
        @note 配合LOD策略一起使用
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_LINEAR,       ///< 线性插值.快,不准确
            M_SPLINE        ///< 曲折插值,准确,低性能.
        };

        /** 空间插值的类型.
        @note 配合LOD策略一起使用
        @version NIIEngine 3.0.0
        */
        enum RotationMode
        {
            RM_LINEAR,      ///< 线性插值.快,不准确.
            RM_SPHERICAL    ///< 球形插值,准确,低性能.
        };
    public:
        NodeFusion(AnimationObj * obj, Animation * ani,  TimeDurMS pos, TimeDurMS dur, 
            NIIf scale = 1.0, NIIf weight = 1.0, bool enable = false);

        NodeFusion(AnimationFusionObj * own, const NodeFusion & o);

        ~NodeFusion();

        /** 执行应用
        @param[in] factor 混合影响权重(0.0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** 设置缩放
        @version NIIEngine 3.0.0
        */
        void setScale(NIIf scale);

        /** 获取缩放
        @version NIIEngine 3.0.0
        */
        NIIf getScale() const;
        
        /** 设置插值模式.
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void setMode(Mode m);

        /** 获取插值模式.
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** 设置旋转插值模式.
        @param[in] m
        @version NIIEngine 3.0.0
        */
        void setRotationMode(RotationMode m);

        /** 获取旋转插值模式.
        @version NIIEngine 3.0.0
        */
        RotationMode getRotationMode() const;

        /** 设置节点权重
        @param[in] boneidx 骨头索引
        @param[in] weight 权重
        @version NIIEngine 3.0.0
        */
        void setFactor(Nidx boneidx, NIIf weight);

        /** 获取节点权重
        @param[in] boneidx 不要超过
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(Nidx boneidx) const;

        /** 设置混合权重
        @remark 有多少节点,就有多大的数组,
        @param[in] in 权重数组
        @note 不检测大小错误,in指针数据将变直接引用(内存将由这个NodeFusion对象管理)
        @version NIIEngine 3.0.0
        */
        void setFactor(NIIf * in);

        /** 获取混合权重
        @remark 有多少节点,就有多大的数组
        @param[in] out 权重数组
        @note 将直接引用数据,任何的改变都直接影响结果
        @version NIIEngine 3.0.0
        */
        void getFactor(NIIf *& out) const;
    public:
        /// @copydetails NodeFusion::reset
        void reset();

        /// @copydetails NodeFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        SkeletonFusion * mDst;  ///< 骨骼混合实例
        NIIf mScale;            ///< 骨骼缩放
        NIIf * mBlendMask;      ///< 节点的权重
        Mode mMode;
        RotationMode mRotationMode;

    };
}
#endif