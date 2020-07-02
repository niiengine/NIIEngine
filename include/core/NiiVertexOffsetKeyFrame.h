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
    /** ����ƫ�ƹؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetKeyFrame : public KeyFrame
    {
        friend class VertexOffsetKeyFrameTrack;
    public:
        /** ƫ������
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

        /** ���ƫ�Ƽ�
        @version NIIEngine 3.0.0
        */
        void add(Nui16 index, NIIf weight);

        /** ����ƫ�Ƽ�ȫ��
        @version NIIEngine 3.0.0
        */
        void set(Nui16 index, NIIf weight);

        /** ��ȥƫ�Ƽ�
        @param[in] index mTargetIndex����
        @version NIIEngine 3.0.0
        */
        void remove(Nui16 index);

        /** ��ȥ����ƫ�Ƽ�
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��ȡƫ�Ƽ�
        @version NIIEngine 3.0.0 �߼�api
        */
        const Offsets & getValue() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Offsets mOffsets;
    };
}
#endif