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

#ifndef _NII_SOUND_FUSION_H_
#define _NII_SOUND_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiAnimationFusion.h"

namespace NII
{
    /** 节点混合实例
    @remark 设置插值类型配合LOD策略使用能有效的控制运行效率
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundFusion : public AnimationFusion
    {
    public:
        SoundFusion(AnimationObj * obj, Animation * ani,  TimeDurMS pos, TimeDurMS dur, 
            NIIf scale = 1.0, NIIf weight = 1.0, bool enable = false);

        SoundFusion(AnimationFusionObj * own, const SoundFusion & o);

        ~SoundFusion();

        /** 执行应用
        @param[in] factor 混合影响权重(0.0 - 1.0]
        @version NIIEngine 3.0.0
        */
        void apply(NIIf factor = 1.0);

        /** 
        @version NIIEngine 4.0.0
        */
        inline void setDirty(bool b) { mDirty = b; }

        /** 
        @version NIIEngine 4.0.0
        */
        inline bool isDirty() const { return mDirty; }
    public:
        /// @copydetails AnimationFusion::reset
        void reset();

        /// @copydetails AnimationFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
    protected:
        SpaceNode * mDst;
        bool mDirty;
    };
}
#endif