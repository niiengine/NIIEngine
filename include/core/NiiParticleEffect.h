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

#ifndef _NII_ParticleEffect_H_
#define _NII_ParticleEffect_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiString.h"
#include "NiiParticle.h"
#include "NiiVector3.h"
#include "NiiVector2.h"

namespace NII
{
    /** 粒子效果
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleEffect : public PropertyCmdObj, public ParticleAlloc
    {
    public:
        ParticleEffect(ParticleSpace * space, LangID lid = N_PrimaryLang);
        virtual ~ParticleEffect();

        /** 获取类型名字
        @version NIIEngine 3.0.0
        */
        const String & getType() const;

        /** 初始化粒子
        @version NIIEngine 3.0.0
        */
        virtual void initImpl(Particle * obj);

        /** 应用粒子效果(单个)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(Particle * obj, TimeDurMS cost);
        
        /** 应用粒子效果(集群)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(ParticleList & list, TimeDurMS cost);
    protected:
        /** 位置变化时
        @version NIIEngine 3.0.0
        */
        virtual void onPos(const Vector3f & o);
        
        /** 大小变化时
        @version NIIEngine 3.0.0
        */
        virtual void onSize(const Vector2f & s);
        
        /** 旋转变化时
        @version NIIEngine 3.0.0
        */
        virtual void onRotation(const Radian & r);
    protected:
        ParticleSpace * mParent;
        String mType;
    };
}

#endif