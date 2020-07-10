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
        inline Nid getType() const              { return mType;}

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
        Nid mType;
        ParticleSpace * mParent;
    };
    
    /** ParticleEffect 工厂类
    @version NIIEngine 3.0.0.
    */
    class _EngineAPI ParticleEffectFactory : public ParticleAlloc
    {
    public:
        ParticleEffectFactory();
        virtual ~ParticleEffectFactory();

        /** 获取粒子效果器类型
        @version NIIEngine 3.0.0
        */
        virtual Nid getID() const = 0;

        /** 创建效果器实例
        @version NIIEngine 3.0.0
        */
        virtual ParticleEffect * create(ParticleSpace * psys) = 0;

        /** 删除指定效果器
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ParticleEffect * psys);
    protected:
        vector<ParticleEffect *>::type mList;
    };
}

#endif