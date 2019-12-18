/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-8

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

#ifndef _NII_EXT_FRAMEBUFFER_H_
#define _NII_EXT_FRAMEBUFFER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 帧的扩展缓存
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI ExtFrameBuffer : public BufferAlloc
    {
    public:
        ExtFrameBuffer(GroupID gid, NCount w, NCount h, NCount bit, Nui32 fsaa,
            const String & hint, bool manual);

        virtual ~ExtFrameBuffer();

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getWidth() const;

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getHeight() const;

        /** 获取宽度
        @version NIIEngine 3.0.0
        */
        virtual NCount getBitDepth() const;

        /** 获取各异向因子
        @version NIIEngine 3.0.0
        */
        virtual NCount getFsaa() const;

        /** 获取各异向因子提示
        @version NIIEngine 3.0.0
        */
        virtual const String & getFsaaHint() const;

        /** 是否合适这个帧缓存对象
        @version NIIEngine 3.0.0
        */
        virtual bool isSuitable(FrameObj * fo) const;

        /** 附加到帧缓存对象中
        @version NIIEngine 3.0.0
        */
        virtual void attach(FrameObj * fo);

        /** 冲帧缓存对象中解除
        @version NIIEngine 3.0.0
        */
        virtual void detach(FrameObj * fo);
        
        /** 设置组
        @version NIIEngine 3.0.0 高级api
        */
        void setGroup(GroupID gid);

        /** 获取组
        @version NIIEngine 3.0.0 高级api
        */
        GroupID getGroup() const;

        /** 设置属于
        @version NIIEngine 3.0.0 高级api
        */
        void setManual(bool b);
        
        /** 设置属于
        @version NIIEngine 3.0.0 高级api
        */
        bool isManual() const;
    protected:
        typedef set<FrameObj *>::type UserList;
    protected:
        GroupID mPoolId;
        NCount mWidth;
        NCount mHeight;
        NCount mBitDepth;
        NCount mFsaa;
        String mFsaaHint;
        UserList mUserList;
        bool mManual;
    };
    typedef vector<ExtFrameBuffer *>::type ExtFrameBufferList;
}
#endif
