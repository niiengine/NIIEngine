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

#ifndef _NII_VERTEX_KEYFRAME_TRACK_H_
#define _NII_VERTEX_KEYFRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** 顶点混合
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexKeyFrameTrack : public KeyFrameTrack
    {
    public:
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id);
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            VertexData * target, CalcMode mode = KeyFrameTrack::M_HOST);
            
        virtual ~VertexKeyFrameTrack();
    
        /** 应用混合
        @version NIIEngine 3.0.0
        */
        virtual void apply(AnimationFusion * dst, VertexData * target, TrackTime * tt, NIIf weight = 1.0) const = 0;
        
        /** 是否混合顶点法线
        @version NIIEngine 3.0.0
        */
        virtual bool isNromalsFusion() const = 0;
        
        /** 设置目标混合实例
        @param[in] data
        @version NIIEngine 3.0.0
        */
        void setTarget(VertexData * obj);

        /** 获取目标混合实例
        @return
        @version NIIEngine 3.0.0
        */
        VertexData * getTarget() const;
        
        /** 设置目标模式
        @param[in] m 模式
        @version NIIEngine 3.0.0
        */
        void setMode(CalcMode m);

        /** 设获取目标模式
        @version NIIEngine 3.0.0
        */
        CalcMode getMode() const;
    protected:
        VertexData * mTarget;
        CalcMode mMode;
    };
}
#endif