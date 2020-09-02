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
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexKeyFrameTrack : public KeyFrameTrack
    {
    public:
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id);
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            VertexData * target, CalcMode mode = KeyFrameTrack::M_HOST);
            
        virtual ~VertexKeyFrameTrack();
    
        /** Ӧ�û��
        @version NIIEngine 3.0.0
        */
        virtual void apply(AnimationFusion * dst, VertexData * target, TrackTime * tt, NIIf weight = 1.0) const = 0;
        
        /** �Ƿ��϶��㷨��
        @version NIIEngine 3.0.0
        */
        virtual bool isNromalsFusion() const = 0;
        
        /** ����Ŀ����ʵ��
        @param[in] data
        @version NIIEngine 3.0.0
        */
        void setTarget(VertexData * obj);

        /** ��ȡĿ����ʵ��
        @return
        @version NIIEngine 3.0.0
        */
        VertexData * getTarget() const;
        
        /** ����Ŀ��ģʽ
        @param[in] m ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(CalcMode m);

        /** ���ȡĿ��ģʽ
        @version NIIEngine 3.0.0
        */
        CalcMode getMode() const;
    protected:
        VertexData * mTarget;
        CalcMode mMode;
    };
}
#endif