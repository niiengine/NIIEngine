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

#ifndef _NII_VERTEX_UNIT_KEYFRAME_H_
#define _NII_VERTEX_UNIT_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiVertexBuffer.h"

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