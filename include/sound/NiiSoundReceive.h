/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-7

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

#ifndef NII_SoundReceive_H
#define NII_SoundReceive_H

#include "NiiPreInclude.h"

namespace NII
{
    /**
    @version NIIEngine 4.0.0
    */
    class SoundReceive
    {
    public:
        virtual	~SoundReceive();

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setPos(const Vector3f & pos) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual const Vector3f & getPos() const = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void setVelocity(const Vector3f & vel) = 0;

        /** 
        @version NIIEngine 4.0.0
        */
        virtual void getVelocity(Vector3f & vel) const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setOrientation(const Vector3f & front, const Vector3f & up) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void getOrientation(Vector3f & front, Vector3f & up) const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setGain(NIIf gain) = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getGain() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setDopplerFactor(NIIf f) = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getDopplerFactor() const = 0;

        /**
        @version NIIEngine 4.0.0
        */
        virtual void setRolloffFactor(NIIf f) = 0;
        
        /**
        @version NIIEngine 4.0.0
        */
        virtual NIIf getRolloffFactor() const = 0;

        /** 
        @VERSION niieNGINE 4.0.0
        */
        virtual void setOption(Nui32 opt, void * value, NCount size) = 0;
    protected:
        SoundReceive();
    };
}
#endif