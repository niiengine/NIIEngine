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

#ifndef _NII_VERTEX_UNIT_KEYFRAME_H_
#define _NII_VERTEX_UNIT_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiGpuBuffer.h"

namespace NII
{
    /** 顶点单元关键帧(全).
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexUnitKeyFrame : public KeyFrame
    {
    public:
        VertexUnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        ~VertexUnitKeyFrame();

        /** 设置顶点缓存包含为这个关键针的位置
        @param[in] buf 顶点缓存
        @version NIIEngine 3.0.0
        */
        void setValue(VertexBuffer * buf);

        /** 获取顶点缓存包含为这个关键针的位置
        @version NIIEngine 3.0.0
        */
        VertexBuffer * getValue() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        VertexBuffer * mBuffer;
    };
}
#endif