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

#ifndef _NII_Particle_H_
#define _NII_Particle_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 单独粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Particle : public ParticleAlloc
    {
        friend class ParticleSpace;
    public:
        /** 粒子类型
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_Alone,
            T_Glint,
            T_Spread
        };

        Particle();

        /** 设置效果控制
        @version NIIEngine 3.0.0 高级api
        */
        void setEffect(ParticleEffect * eff);

        /** 获取效果控制器
        @version NIIEngine 3.0.0 高级api
        */
        ParticleEffect * getEffect() const;

        /** 设置是否自动产生的
        @version NIIEngine 3.0.0
        */
        void setAuto(bool b);

        /** 获取是否自动产生的
        @version NIIEngine 3.0.0
        */
        bool isAuto() const;
        
        /** 设置大小
        @version NIIEngine 3.0.0
        */
        void setSize(NIIf width, NIIf height);

        /** 获取宽
        @version NIIEngine 3.0.0
        */
        NIIf getWidth() const;

        /** 获取高
        @version NIIEngine 3.0.0
        */
        NIIf getHeight() const;

        /** 是否继承大小
        @version NIIEngine 3.0.0
        */
        void inheritSize(bool b);

        /** 是否继承大小
        @version NIIEngine 3.0.0
        */
        bool isInheritSize() const;

        /** 获取控制器的宽度
        @version NIIEngine 3.0.0
        */
        NIIf getCtrlWidth() const;
        
        /** 获取控制器的高度
        @version NIIEngine 3.0.0
        */
        NIIf getCtrlHeight() const;
        
        /** 设置颜色
        @version NIIEngine 3.0.0
        */
        void setColour(const Colour & c);

        /** 获取颜色
        @version NIIEngine 3.0.0
        */
        const Colour & getColour() const;

        /** 设置旋转
        @version NIIEngine 3.0.0
        */
        void setRotation(const Radian & r);

        /** 获取旋转
        @version NIIEngine 3.0.0
        */
        const Radian & getRotation() const;

        /** 设置旋转因子
        @version NIIEngine 3.0.0
        */
        void setRotationFactor(NIIf f);

        /** 获取旋转因子
        @version NIIEngine 3.0.0
        */
        NIIf getRotationFactor() const;

        /** 设置控制系统
        @version NIIEngine 3.0.0 高级api
        */
        void setCtrl(ParticleSpace * sys);
    public:
        Colour mColour;
        Vector3f mPosition;
        Vector3f mDirection;
        Radian mRotation;
        TimeDurMS mTotalTTL;
        TimeDurMS mRemainTTL;
    protected:
        ParticleSpace * mCtrl;
        ParticleEffect * mEffect;
        Type mType;
        NIIf mWidth;
        NIIf mHeight;
        Radian mRotFactor;
        bool mInheritSize;
        bool mAuto;
    };
    typedef list<Particle *>::type ParticleList;
}
#endif