/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-20

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

#ifndef _NII_KEYFRAME_TRACK_H_
#define _NII_KEYFRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiTrackTime.h"

namespace NII
{
    /** 关键帧轨迹
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyFrameTrack : public AnimableAlloc
    {
        friend class Animation;
    public:
        /** 混算模式
        @version NIIEngine 3.0.0
        */
        enum CalcMode
        {
            M_CPU,  ///< cpu完成计算
            M_GPU,  ///< gpu完成计算
            M_SIMD, ///< cpu的并行计算
            M_CL    ///< 第三方公共计算opencl    
        };
        typedef vector<KeyFrame *>::type KeyFrames;
    public:
        /// 构造函数
        KeyFrameTrack(const Animation * o, KeyFrameTrackID id);

        virtual ~KeyFrameTrack();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        KeyFrameTrackID getID() const;

        /** 所属的动画
        @version NIIEngine 3.0.0
        */
        Animation * getParent() const;

        /** 创建指定时间的关键帧
        @param[in] pos 这个关键帧应用的时间
        @version NIIEngine 3.0.0
        */
        virtual KeyFrame * create(TimeDurMS pos);

        /** 给定的索引,获取关键帧
        @param[in] index
        @version NIIEngine 3.0.0
        */
        virtual KeyFrame * get(Nidx index) const;

        /** 修改关键帧所在的时间位
        @param[in] obj 关键帧
        @param[in] pos 期望的时间位
        @version NIIEngine 3.0.0
        */
        void modify(KeyFrame * obj, TimeDurMS pos);

        /** 移去关键帧
        @version NIIEngine 3.0.0
        */
        virtual void destroy(Nidx index);

        /** 移去所有关键帧
        @version NIIEngine 3.0.0
        */
        virtual void removeAll();

        /** 返回关键帧数量.
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取轨迹列表
        @version NIIEngine 3.0.0 高级api
        */
        const KeyFrames & getList() const;

        /** 指定时间,获取2个关键帧对象
        @param[in] tt 时间(相对于整个动画序列)
        @param[out] out1 前关键帧
        @param[out] out2 后关键帧
        @return 间距参数浮点型[0, 1]
        */
        virtual NIIf get(TrackTime * tt, KeyFrame *& out1, KeyFrame *& out2) const;

        /** 获取插值关键帧对象
        @param[in] dst 动画混合实例
        @param[in] tt 时间(相对于整个动画序列)
        @param[out] out 插值关键帧
        @version NIIEngine 3.0.0
        */
        virtual void getFusion(AnimationFusion * dst, TrackTime * tt, KeyFrame * out) const = 0;

        /** 应用到指定动画混合中
        @param[in] dst 动画混合实例
        @param[in] tt 时间位置
        @param[in] weight [0, 1]
        @param[in] scale 涉及仿射变换时使用
        @version NIIEngine 3.0.0
        */
        virtual void apply(AnimationFusion * dst, TrackTime * tt, NIIf weight = 1.0, NIIf scale = 1.0f) const = 0;

        /** 设置基础关键针
        @param[in] in 基础关键帧
        @version NIIEngine 3.0.0
        */
        virtual void setOrigin(const KeyFrameTrack * org, TrackTime * time);

        /** 关键帧轨迹是否有实际效果
        @version NIIEngine 3.0.0
        */
        virtual bool isValid() const;

        /** 优化关键帧结构
        @remark
        @version NIIEngine 3.0.0
        */
        virtual void optimise();

        /** 创建相应得动画混合对象
        @version NIIEngine 3.0.0
        */
        virtual AnimationFusion * createFusion(AnimationObj * obj, Animation * ani, NIIf weight, bool enable) const = 0;

        /** 复制关键帧
        @param[in] 新父对象
        @version NIIEngine 3.0.0
        */
        virtual KeyFrameTrack * clone(const Animation * o) const = 0;

        /** 内容变化实现
        @version NIIEngine 3.0.0
        */
        virtual void notifyImpl() const;
    protected:
        /** 创建关键帧实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual KeyFrame * createImpl(TimeDurMS pos) = 0;

        /** 复制关键帧实现
        @param[in] in 关键帧来源
        @version NIIEngine 3.0.0 高级api
        */
        virtual void copyImpl(const KeyFrameTrack * in);
    protected:
        Animation * mParent;    ///< 父对象
        KeyFrameTrackID mID;    ///<
        KeyFrames mKeyFrames;   ///<
        CalcMode mMode;         ///< 混算模式
    };
    typedef vector<KeyFrameTrack *>::type KeyFrameTrackList; 
}
#endif