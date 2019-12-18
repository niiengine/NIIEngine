/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-9-21

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