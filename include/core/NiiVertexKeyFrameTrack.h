/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-7-22

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
            VertexData * target, CalcMode mode = M_CPU);
            
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