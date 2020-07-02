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

#ifndef _NII_SOUND_KEYFRAME_H_
#define _NII_SOUND_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"

namespace NII
{
    /** 节点专用关键帧
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundKeyFrame : public KeyFrame
    {
    public:
        SoundKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~SoundKeyFrame();

        SoundKeyFrame & operator = (const SoundKeyFrame & o);
        
        /** 添加资源
        @version NIIEngine 3.0.0
        */
        void addResource(ResourceID rid);

        /** 移去资源
        @version NIIEngine 3.0.0
        */
        void removeResource(ResourceID rid);

        /** 获取使用的资源
        @version NIIEngine 3.0.0
        */
        const ResourceIDList & getList() const;
        
        /** 播放
        @version NIIEngine 3.0.0
        */
        void play(SpaceNode * node);
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        ResourceIDList mResourceList;
    };
}
#endif