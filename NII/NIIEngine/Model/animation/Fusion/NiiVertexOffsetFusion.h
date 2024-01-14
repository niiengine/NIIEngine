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

#ifndef _NII_VERTEX_OFFSET_FUSION_H_
#define _NII_VERTEX_OFFSET_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiVertexFusion.h"

namespace NII
{
    /** 顶点偏移(集)混合
    @remark 一般指实例模型中的顶点
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetFusion : public VertexFusion
    {
    public:
        VertexOffsetFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos, TimeDurMS dur,
            NIIf weight = 1.0, bool enable = false);

        VertexOffsetFusion(AnimationFusionObj * own, const VertexOffsetFusion & o);

        /** 获取顶点偏移列表
        @version NIIEngine 3.0.0
        */
        const VertexOffsetList & getOffsetList() const;
        
        /// @copydetails VertexFusion::clone
        AnimationFusion * clone(AnimationFusionObj * own) const;
        
        /// @copydetails VertexFusion::apply
        void apply(GeometrySpace * dst, bool sw, bool hw);
        
        /// @copydetails VertexFusion::apply
        void apply(VertexData * data);
    protected:
        Mesh * mMesh;
    };
}
#endif