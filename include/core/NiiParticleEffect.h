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
    /** ����Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleEffect : public PropertyCmdObj, public ParticleAlloc
    {
    public:
        ParticleEffect(ParticleSpace * space, LangID lid = N_PrimaryLang);
        virtual ~ParticleEffect();

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        inline Nid getType() const              { return mType;}

        /** ��ʼ������
        @version NIIEngine 3.0.0
        */
        virtual void initImpl(Particle * obj);

        /** Ӧ������Ч��(����)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(Particle * obj, TimeDurMS cost);
        
        /** Ӧ������Ч��(��Ⱥ)
        @version NIIEngine 3.0.0
        */
        virtual void effectImpl(ParticleList & list, TimeDurMS cost);
    protected:
        /** λ�ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onPos(const Vector3f & o);
        
        /** ��С�仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onSize(const Vector2f & s);
        
        /** ��ת�仯ʱ
        @version NIIEngine 3.0.0
        */
        virtual void onRotation(const Radian & r);
    protected:
        Nid mType;
        ParticleSpace * mParent;
    };
    
    /** ParticleEffect ������
    @version NIIEngine 3.0.0.
    */
    class _EngineAPI ParticleEffectFactory : public ParticleAlloc
    {
    public:
        ParticleEffectFactory();
        virtual ~ParticleEffectFactory();

        /** ��ȡ����Ч��������
        @version NIIEngine 3.0.0
        */
        virtual Nid getID() const = 0;

        /** ����Ч����ʵ��
        @version NIIEngine 3.0.0
        */
        virtual ParticleEffect * create(ParticleSpace * psys) = 0;

        /** ɾ��ָ��Ч����
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ParticleEffect * psys);
    protected:
        vector<ParticleEffect *>::type mList;
    };
}

#endif