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

#ifndef _NII_ParticleSpaceManager_H_
#define _NII_ParticleSpaceManager_H_

#include "NiiPreInclude.h"
#include "NiiParticleSpace.h"
#include "NiiRenderFrameListener.h"
#include "NiiScriptParserSys.h"
#include "NiiResourceSchemeManager.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    class ParticleSpaceFactory;

    /** 粒子管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleSpaceManager : public ScriptParserSys, public Singleton<ParticleSpaceManager>, public FXAlloc
    {
        friend class Engine;
        friend class ParticleSpaceFactory;
    public:
        typedef map<SpaceID, ParticleSpace *>::type SpaceList;
        typedef map<Nid, ParticleGeoFactory *>::type GeoFactoryList;
        typedef map<Nid, ParticleEffectFactory *>::type EffectFactoryList;
        typedef map<Nid, SpreadParticleFactory *>::type SpreadFactoryList;
    public:
        ParticleSpaceManager();
        virtual ~ParticleSpaceManager();

        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(SpreadParticleFactory * factory);

        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(ParticleEffectFactory * factory);

        /** 添加工厂
        @version NIIEngine 3.0.0
        */
        void add(ParticleGeoFactory * factory);

        /** 获取工厂列表
        @version NIIEngine 3.0.0
        */
        const EffectFactoryList & getEffectFactoryList() const;

        /** 获取工厂列表
        @version NIIEngine 3.0.0
        */
        const SpreadFactoryList & getSpreadFactoryList() const;

        /** 获取工厂列表
        @version NIIEngine 3.0.0
        */
        const GeoFactoryList & getGeoFactoryList() const;

        /** 创建模板
        @version NIIEngine 3.0.0
        */
        ParticleSpace * createTemplate(SpaceID sid, GroupID rgid);

        /** 添加粒子模板
        @version NIIEngine 3.0.0
        */
        void addTemplate(SpaceID sid, ParticleSpace * psys);

        /** 获取粒子模板
        @version NIIEngine 3.0.0
        */
        ParticleSpace * getTemplate(SpaceID sid);

        /** 移去粒子模板
        @version NIIEngine 3.0.0
        */
        void removeTemplate(SpaceID sid, bool destroy = true);

        /** 移去所有粒子模板
        @version NIIEngine 3.0.0
        */
        void removeAllTemplate(bool destroy = true);

        /** 移去指定群组的粒子模板
        @version NIIEngine 3.0.0
        */
        void removeAllTemplate(GroupID gid);

        /** 获取模板列表
        @version NIIEngine 3.0.0
        */
        const SpaceList & getSpaceList() const;

        /** 创建粒子
        @version NIIEngine 3.0.0
        */
        SpreadParticle * createSpread(Nid type, ParticleSpace * psys);

        /** 删除粒子
        @version NIIEngine 3.0.0
        */
        void destroySpread(SpreadParticle * spread);

        /** 创建效果
        @version NIIEngine 3.0.0
        */
        ParticleEffect * createEffect(Nid type, ParticleSpace * psys);

        /** 删除效果
        @version NIIEngine 3.0.0
        */
        void destroyEffect(ParticleEffect * affector);

        /** 创建粒子图形
        @version NIIEngine 3.0.0
        */
        ParticleGeo * createGeo(Nid rendererType);

        /** 删除粒子图形
        @version NIIEngine 3.0.0
        */
        void destroyGeo(ParticleGeo * renderer);

        /** 设置时间频率
        @version NIIEngine 3.0.0
        */
        static inline void setTimeInterval(TimeDurMS ms) 
        { 
            mTimeInterval = ms; 
        }

        /** 获取时间频率
        @version NIIEngine 3.0.0
        */
        static inline TimeDurMS getTimeInterval() 
        { 
            return mTimeInterval; 
        }

        /** 设置不可见
        @version NIIEngine 3.0.0
        */
        static inline void setTimeThreshold(TimeDurMS ms)
        {
            mTimeThreshold = ms;
        }

        /** 
        @version NIIEngine 3.0.0
        */
        static inline TimeDurMS getTimeThreshold()
        {
            return mTimeThreshold;
        }

        /// 获取默认工厂类
        inline ParticleSpaceFactory * getFactory() { return mFactory; }

        /// @copydetails ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        /// @copydetails ScriptParserSys::getScriptLevel
        NIIf getScriptLevel() const;
        
        static TimeDurMS mTimeInterval;
        static TimeDurMS mTimeThreshold;
    protected:
        /** 初始化系统
        @version NIIEngine 3.0.0
        */
        void init();
        
        /// 解析
        void parse(Nid type, DataStream * chunk, ParticleSpace * sys);

        /// 创建粒子实现
        ParticleSpace * createImpl(SpaceID name, NCount max, GroupID gid);

        /// 创建粒子实现
        ParticleSpace * createImpl(SpaceID name, SpaceID templateid);

        /// 删除粒子实现
        void destroyImpl(ParticleSpace * sys);
    protected:
        N_mutex1
        SpaceList mSpaceList;
        GeoFactoryList mGeoFactoryList;
        SpreadFactoryList mEmitterFactories;
        EffectFactoryList mAffectorFactories;
        ParticleSpaceFactory * mFactory;
    };

    /// ParticleSpace 工厂类
    class _EngineAPI ParticleSpaceFactory : public SpaceObjFactory
    {
    public:
        ParticleSpaceFactory() {}
        ~ParticleSpaceFactory() {}

        /// @copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        /// @copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);        

        /// @copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}
#endif