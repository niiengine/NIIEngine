/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef _NII_SpreadParticle_H_
#define _NII_SpreadParticle_H_

#include "NiiPreInclude.h"
#include "NiiPropertyObj.h"
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
        inline Nid getType() const                      { return mType; }

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
        inline bool getEnable() const                   { return mEnabled; }

        /** 设置扩散中心
        @remark 一般就是粒子位置
        @version NIIEngine 3.0.0
        */
        virtual void setPos(const Vector3f & pos);

        /** 获取扩散中心
        @remark 一般就是粒子位置
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getPos() const          { return mPosition; }

        /** 设置扩散方向
        @remark 一般就是粒子方向
        @version NIIEngine 3.0.0
        */
        virtual void setDirection(const Vector3f & dir);

        /** 设置扩散方向
        @version 一般就是粒子方向
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getDirection() const    { return mDirection; }

        /** 设置扩散角度
        @version NIIEngine 3.0.0
        */
        virtual void setAngle(const Radian & angle);

        /** 获取扩散角度
        @version NIIEngine 3.0.0
        */
        inline const Radian & getAngle() const          { return mAngle; }

        /** 设置扩散方向辅助
        @note 一般垂直于方向
        @version NIIEngine 3.0.0
        */
        virtual void setUp(const Vector3f & up);

        /** 设置扩散方向辅助
        @note 一般垂直于方向
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getUp() const           { return mUp; }

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
        inline const Colour & getMinColour() const      { return mMinColour; }

        /** 设置扩散最大颜色
        @version NIIEngine 3.0.0
        */
        virtual void setMaxColour(const Colour & c);

        /** 获取扩散最大颜色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getMaxColour() const      {return mMaxColour; }

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
        virtual NIIf getMinSpreadSpeed() const          { return mMinSpeed; }

        /** 获取最大扩散速度
        @version NIIEngine 3.0.0
        */
        virtual NIIf getMaxSpreadSpeed() const          { return mMaxSpeed; }

        /** 设置扩散率
        @version NIIEngine 3.0.0
        */
        virtual void setSpreadRate(NIIf rate);

        /** 获取扩散率
        @version NIIEngine 3.0.0
        */
        inline NIIf getSpreadRate() const               { return mSpreadRate; }

        /** 设置最小扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual void setMinTTL(TimeDurMS min);

        /** 获取最小扩散生命期
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMinTTL() const              { return mMinTTL; }

        /** 设置最大扩散生命期
        @version NIIEngine 3.0.0
        */
        virtual void setMaxTTL(TimeDurMS max);

        /** 获取最大扩散生命期
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMaxTTL() const              { return mMaxTTL; }

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
        inline TimeDurMS getMinDuration() const         { return mMinDur;}

        /** 设置扩散最大持续时间
        @version NIIEngine 3.0.0
        */
        virtual void setMaxDuration(TimeDurMS max);

        /** 获取扩散最大持续时间
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMaxDuration() const         { return mMaxDur; }

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
        inline TimeDurMS getMinRepeatDelay() const      { return mMinRepeatDelay; }

        /** 设置重复扩散最大延时
        @version NIIEngine 3.0.0
        */
        virtual void setMaxRepeatDelay(TimeDurMS max);

        /** 获取重复扩散最大延时
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getMaxRepeatDelay() const      { return mMaxRepeatDelay;}
    protected:
        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

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
        Nid mType;
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
    
    /** SpreadParticle 工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpreadParticleFactory : public ParticleAlloc
    {
    public:
        SpreadParticleFactory() {}
        virtual ~SpreadParticleFactory();

        /** 获取工厂ID
        @version NIIEngine 3.0.0
        */
        virtual Nid getID() const = 0;

        /** 创建
        @version NIIEngine 3.0.0
        */
        virtual SpreadParticle * create(ParticleSpace * psys) = 0;

        /** 删除
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpreadParticle * e);
    protected:
        vector<SpreadParticle *>::type mList;
    };
}
#endif