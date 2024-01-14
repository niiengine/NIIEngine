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