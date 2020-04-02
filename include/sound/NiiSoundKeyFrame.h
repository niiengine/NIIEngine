/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-1

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
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