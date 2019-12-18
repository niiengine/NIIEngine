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

#ifndef _NII_SpreadParticle_H_
#define _NII_SpreadParticle_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiParticle.h"
#include "NiiString.h"
#include "NiiVector3.h"
#include "NiiColour.h"
#include "NiiRadian.h"

namespace NII
{
    /** 扩散粒子
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpreadParticle : public Particle, public PropertyCmdObj
    {
        friend class ParticleSpace;
    public:
        SpreadParticle(ParticleSpace * sys, LangID lid = N_PrimaryLang);
        virtual ~SpreadParticle();

        /** 基础类型
        @version NIIEngine 3.0.0
        */
        const String & getType() const;

        /** 设置名字
        @version NIEngine 3.0.0
        */
        virtual void setName(const String & name);

        /** 获取名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 设置扩散名字
        @version NIIEngine 3.0.0
        */
        virtual void setSpreadName(const String & name);

        /** 获取扩散名字
        @version NIIEngine 3.0.0
        */
        const String & getSpreadName() const;

        /** 设置是否扩散
        @version NIIEngine 3.0.0
        */
        virtual void setEnable(bool b);

        /** 设置记时启动
        @version NIIEngine 3.0.0
        */
        virtual void setEnable(TimeDurMS ms);

        /** 获取是否扩散
        @version NIIEngine 3.0.0
        */
        virtual bool getEnable() const;

        /** 设置扩散中心
        @remark 一般就是粒子位置
        @version NIIEngine 3.0.0
        */
        virtual void setPos(const Vector3f & pos);

        /** 获取扩散中心
        @remark 一般就是粒子位置
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getPos() const;

        /** 设置扩散方向
        @remark 一般就是粒子方向
        @version NIIEngine 3.0.0
        */
        virtual void setDirection(const Vector3f & dir);

        /** 设置扩散方向
        @version 一般就是粒子方向
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getDirection() const;

        /** 设置扩散角度
        @version NIIEngine 3.0.0
        */
        virtual void setAngle(const Radian & angle);

        /** 获取扩散角度
        @version NIIEngine 3.0.0
        */
        virtual const Radian & getAngle() const;

        /** 设置扩散方向辅助
        @note 一般垂直于方向
        @version NIIEngine 3.0.0
        */
        virtual void setUp(const Vector3f & up);

        /** 设置扩散方向辅助
        @note 一般垂直于方向
        @version NIIEngine 3.0.0
        */
        virtual const Vector3f & getUp() const;

        /** 设置扩散颜色
        @version NIIEngine 3.0.0
        */
        virtual void setColour(const Colour & c);

        /** 获取扩散颜色
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getColour() const;

        /** 设置扩散最小颜色
        @version NIIEngine 3.0.0
        */
        virtual void setMinColour(const Colour & c);

        /** 获取扩散最小颜色
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getMinColour() const;

        /** 设置扩散最大颜色
        @version NIIEngine 3.0.0
        */
        virtual void setMaxColour(const Colour & c);

        /** 获取扩散最大颜色
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getMaxColour() const;

        /** 设置扩散速度
        @version NIIEngine 3.0.0
        */
        virtual void setSpreadSpeed(NIIf speed);

        /** 获取设置扩散速度
        @version NIIEngine 3.0.0
        */
        virtual void setMinSpreadSpeed(NIIf min);

        /** 获取设置扩散速度
        @version NIIEngine 3.0.0
        */
        virtual void setMaxSpreadSpeed(NIIf max);

        /** 获取扩散速度
        @version NIIEngine 3.0.0
        */
        virtual NIIf getSpreadSpeed() const;

        /** 获取最小扩散速度
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMinSpreadSpeed() const;

        /** 获取最大扩散速度
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMaxSpreadSpeed() const;

        /** 设置扩散率
        @version NIIEngine 3.0.0
        */
        virtual void setSpreadRate(NIIf rate);

        /** 获取扩散率
        @version NIIEngine 3.0.0
        */
        virtual NIIf getSpreadRate() const;

        /** 设置最小扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual void setMinTTL(TimeDurMS min);

        /** 获取最小扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMinTTL() const;

        /** 设置最大扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual void setMaxTTL(TimeDurMS max);

        /** 获取最大扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMaxTTL() const;

        /** 设置扩散持续时间
        @version NIIEngine 3.0.0
        */
        virtual void setDuration(TimeDurMS ms);

        /** 获取扩散持续时间
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getDuration() const;

        /** 设置扩散最小持续时间
        @version NIIEngine 3.0.0
        */
        virtual void setMinDuration(TimeDurMS min);

        /** 获取扩散最小持续时间
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMinDuration() const;

        /** 设置扩散最大持续时间
        @version NIIEngine 3.0.0
        */
        virtual void setMaxDuration(TimeDurMS max);

        /** 获取扩散最大持续时间
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMaxDuration() const;

        /** 设置重复扩散延时
        @version NIIEngine 3.0.0
        */
        virtual void setRepeatDelay(TimeDurMS duration);

        /** 获取重复扩散延时
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getRepeatDelay() const;

        /** 设置重复扩散最小延时
        @version NIIEngine 3.0.0
        */
        virtual void setMinRepeatDelay(TimeDurMS min);

        /** 获取重复扩散最小延时
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMinRepeatDelay() const;

        /** 设置重复扩散最大延时
        @version NIIEngine 3.0.0
        */
        virtual void setMaxRepeatDelay(TimeDurMS max);

        /** 获取重复扩散最大延时
        @version NIIEngine 3.0.0
        */
        virtual TimeDurMS getMaxRepeatDelay() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /** 更新渡过的时间
        @version NIIEngine 3.0.0
        */
        void updateCostTime();

        /** 获取一定时间内的扩散数量
        @version NIIEngine 3.0.0
        */
        virtual NCount getSpreadImpl(TimeDurMS cost);

        /** 初始化扩散出来的粒子
        @version NIIEngine 3.0.0
        */
        virtual void initImpl(Particle * spread);

        /** 初始化扩散出来的粒子方向
        @version NIIEngine 3.0.0
        */
        virtual void initDirectionImpl(Particle * spread);

        /** 初始化扩散出来的粒子颜色
        @version NIIEngine 3.0.0
        */
        virtual void initColourImpl(Particle * spread);

        /** 初始化扩散出来的粒子速度
        @version NIIEngine 3.0.0
        */
        virtual void initSpeedImpl(Particle * spread);

        /** 初始化扩散出来的粒子存在时间
        @version NIIEngine 3.0.0
        */
        virtual void initTTLImpl(Particle * spread);
    protected:
        String mType;
        String mName;
        String mSpreadName;
        Vector3f mUp;
        Radian mAngle;
        NIIf mSpreadRate;
        TimeDurMS mMinDur;
        TimeDurMS mMaxDur;        
        NIIf mMinSpeed;
        NIIf mMaxSpeed;
        TimeDurMS mMinTTL;
        TimeDurMS mMaxTTL;
        Colour mMinColour;
        Colour mMaxColour;        
        TimeDurMS mMinRepeatDelay;
        TimeDurMS mMaxRepeatDelay;
        TimeDurMS mRemainDur;
        TimeDurMS mRemainDelay;
        TimeDurMS mEnableTime;        
        bool mEnabled;
    };
}
#endif