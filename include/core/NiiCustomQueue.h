/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_CUSTOM_QUEUE_H_
#define _NII_CUSTOM_QUEUE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 自定义队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomQueue : public RenderAlloc
    {
    public:
        CustomQueue(GroupID gid, const String & name = StrUtil::BLANK);

        virtual ~CustomQueue();

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup() const;

		/** 设置渲染通道
		@version NIIEngine 3.0.0
		*/
		void setShaderCh(ShaderCh * ch);

		/** 获取渲染通道
		@version NIIEngine 3.0.0
		*/
		ShaderCh * getShaderCh() const;

        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b);

        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const;

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * group, RenderPattern * pattern, SpaceManager * dst);

        /** 名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;
    protected:
        /** 执行渲染实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderGroup * group, RenderPattern * pattern);
    protected:
        String mName;
        GroupID mRenderGroup;
		ShaderCh * mShaderCh;
        bool mShadowEnable;
    };
    typedef vector<CustomQueue *>::type CustomQueueList;
}
#endif