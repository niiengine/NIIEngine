/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-5

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

#ifndef _NII_VIDEO_SKY_H_
#define _NII_VIDEO_SKY_H_

#include "NiiPreInclude.h"
#include "NiiSky.h"

namespace NII
{
    /** 一般模拟极限天空
    @remark 一般用于粒子系统无法表达,或效率很差的粒子系统的代替方式
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VideoSky : public Sky
    {
    public:
        VideoSky();
        VideoSky(const SpaceManager * gm);
        virtual ~VideoSky();

        /** 设置视频资源
        @version NIIEngine 3.0.0
        */
        void setVideo(ResourceID rid);

		/** 获取视频资源
		@version NIIEngine 3.0.0
		*/
		ResourceID getVideo() const;

		/** 获取平面的定义
		@version NIIEngine 3.0.0
		*/
		const Plane & getPlane() const;

        /** 设置时间因子
        @version NIIEngine 3.0.0
        */
        void setTimeFactor(NIIf f);

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        NIIf getTimeFactor() const;

		/** 构建几何
		@version NIIEngine 3.0.0
		*/
		void buildGeo(const Plane & plane, NIIf scale = 1000, NIIf width = 1000, NIIf height = 1000);
    protected:
        /// @copydetails Sky::renderImpl
        virtual void renderImpl(RenderQueue * rq);
    protected:
		Plane mPlane;
        NIIf mTimeFactor;
        Texture * mGeo;
    };
}
#endif