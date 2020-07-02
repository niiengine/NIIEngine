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

#ifndef _NII_UNIT_FUSION_H_
#define _NII_UNIT_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 单元混合实例
    @remark
        可以用于混合 Vector2 Vector3 等简单的数字型(线性)动画,如UI窗体/单元从大到小,从小
        到大的缩放过程动画
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitFusion : public AnimationFusion
    {
    public:
        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani,
            NIIf weight = 1.0, bool enable = false);

        UnitFusion(FusionData * dst, AnimationObj * obj, Animation * ani, TimeDurMS pos,
            TimeDurMS dur, NIIf weight = 1.0, bool enable = false);

        UnitFusion(AnimationFusionObj * own, const UnitFusion & o);

        ~UnitFusion();

        /** 应用
        @remark
        @param[in] factor 混合影响权重
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0f);
    public:
        /// @copydetails AnimationFusion::reset
        void reset();
        
        /// @copydetails AnimationFusion::
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        FusionData * mDst;
    };
}
#endif