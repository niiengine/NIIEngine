/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-8

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

#ifndef _NII_MultiTextureFrame_H_
#define _NII_MultiTextureFrame_H_

#include "NiiPreInclude.h"
#include "NiiFrameObj.h"

namespace NII
{
    /** 帧输出到多个纹理
    @note 纹理基础格式必须相同
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiTextureFrame : public FrameObj
    {
    public:
        typedef vector<TextureFrame *>::type AttachList;
    public:
        MultiTextureFrame(const String & name);

        /** 附加
        @param[in] index 索引
        @param[in] obj 对象
        @version NIIEngine 3.0.0
        */
        virtual void attach(Nidx index, TextureFrame * obj);

        /** 解除
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        virtual void detach(Nidx index);

        /** 获取
        @param[in] index 索引
        @version NIIEngine 3.0.0
        */
        TextureFrame * getAttach(NCount index);

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const AttachList & getAttachList() const;

        /// @copydetails FrameObj::fill
        virtual void fill(FrameObj::FaceType buffer, PixelFormat pf, PixelBlock & dst);

        /// @copydetails FrameObj::getFormat
        PixelFormat getFormat() const;
    protected:
        /** 实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void attachImpl(Nidx index, TextureFrame * obj) = 0;

        /** 实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void detachImpl(Nidx index) = 0;
    protected:
        AttachList mAttachList;
    };
}

#endif