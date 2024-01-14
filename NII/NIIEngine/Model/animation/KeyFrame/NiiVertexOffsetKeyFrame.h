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

#ifndef _NII_VERTEX_OFFSET_KEYFRAME_H_
#define _NII_VERTEX_OFFSET_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"

namespace NII
{
    /** 顶点偏移关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetKeyFrame : public KeyFrame
    {
        friend class VertexOffsetKeyFrameTrack;
    public:
        /** 偏移引用
        @version NIIEngine 3.0.0
        */
        struct VertexOffset
        {
            Nui16 mTargetIndex;
            NIIf mWeight;
        };
        typedef vector<VertexOffset>::type Offsets;
    public:
        VertexOffsetKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~VertexOffsetKeyFrame();

        /** 添加偏移集
        @version NIIEngine 3.0.0
        */
        void add(Nui16 index, NIIf weight);

        /** 设置偏移集全重
        @version NIIEngine 3.0.0
        */
        void set(Nui16 index, NIIf weight);

        /** 移去偏移集
        @param[in] index mTargetIndex属性
        @version NIIEngine 3.0.0
        */
        void remove(Nui16 index);

        /** 移去所有偏移集
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取偏移集
        @version NIIEngine 3.0.0 高级api
        */
        const Offsets & getValue() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Offsets mOffsets;
    };
}
#endif