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

#ifndef _NII_ParticleSpace_H_
#define _NII_ParticleSpace_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiString.h"
#include "NiiPropertyObj.h"
#include "NiiSpaceObj.h"
#include "NiiDataEquation.h"
#include "NiiParticle.h"

namespace NII
{
    /** 粒子空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ParticleSpace : public SpaceObj, public PropertyCmdObj
    {
    public:
        ParticleSpace(LangID lid = N_PrimaryLang);
        ParticleSpace(SpaceID sid, GroupID gid, LangID lid = N_PrimaryLang);
        virtual ~ParticleSpace();

        ParticleSpace & operator=(const ParticleSpace & o);

        /** 更新
        @param[in] cost 上次与这次调用渡过时间
        @version NIIEngine 3.0.0 高级api
        */
        void _update(TimeDurMS cost);

        /** 快进更新
        @version NIIEngine 3.0.0
        */
        void _forward(TimeDurMS time, TimeDurMS interval = 200);

        /** 设置时间因子
        @version NIIEngine 3.0.0
        */
        inline void setTimeFactor(NIIf f)           { mTimeFactor = f; }

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        inline NIIf getTimeFactor() const           { return mTimeFactor; }

        /** 设置时间间隔
        @version NIIEngine 3.0.0
        */
        inline void setTimeInterval(TimeDurMS ms)   { mTimeInterval = ms; }

        /** 获取时间间隔
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getTimeInterval() const    { return mTimeInterval; }

        /** 设置时间阀值
        @version NIIEngine 3.0.0
        */
        inline void setTimeThreshold(TimeDurMS ms)  { mTimeThreshold = ms; }

        /** 获取时间阀值
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getTimeThreshold() const   { return mTimeThreshold; }

        /** 设置是否排序
        @param 视图距离
        @version NIIEngine 3.0.0
        */
        inline void setSortEnable(bool b)           { mSorted = b; }

        /** 设置是否排序
        @param 视图距离
        @version NIIEngine 3.0.0
        */
        inline bool isSortEnable() const            { return mSorted; }

        /** 设置纹理
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 获取纹理ID
        @version NIIEngine 3.0.0
        */
        inline ResourceID getMaterialID() const     { return mMaterialID; }

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        inline GroupID getResourceGroup() const     { return mMaterialGroup; }

        /** 设置粒子视图
        @version NIIEngine 3.0.0
        */
        void setGeo(Nid type);

        /** 获取粒子几何控制
        @version NIIEngine 3.0.0
        */
        inline ParticleGeo * getGeo() const         { return mGeo; }

        /** 设置粒子最大数量
        @version NIIEngine 3.0.0
        */
        void setPoolSize(NCount size);

        /** 获取最大数量
        @version NIIEngine 3.0.0
        */
        inline NCount getPoolSize() const           { return mPoolSize; }

        /** 设置是否启用独立拣选
        @version NIIEngine 3.0.0
        */
        void setCullItself(bool b);

        /** 获取是否启用独立拣选
        @version NIIEngine 3.0.0
        */
        inline bool getCullItself() const           { return mCullIndividual; }

        /** 设置是否本地空间模式
        @note 默认是全局空间,不是本地空间
        @version NIIEngine 3.0.0
        */
        void setLocalSpace(bool b);

        /** 获取是否本地空间模式
        @note 默认是全局空间,不是本地空间
        @version NIIEngine 3.0.0
        */
        inline bool isLocalSpace() const            { return mLocalSpace; }

        /** 设置粒子宽
        @version NIIEngine 3.0.0
        */
        void setParticleWidth(NIIf w);

        /** 获取粒子宽
        @version NIIEngine 3.0.0
        */
        inline NIIf getParticleWidth() const        { return mWidth; }

        /** 设置粒子高
        @version NIIEngine 3.0.0
         */
        void setParticleHeight(NIIf h);

        /** 获取粒子高
        @version NIIEngine 3.0.0
        */
        inline NIIf getParticleHeight() const       { return mHeight; }

        /** 创建粒子
        @version NIIEngine 3.0.0
        */
        Particle * create();

        /** 创建闪烁粒子
        @version NIIEngine 3.0.0
        */
        Particle * createGlint();

        /** 创建扩散粒子
        @version NIIEngine 3.0.0
        */
        Particle * createSpread(const String & name);

        /** 获取粒子
        @version NIIEngine 3.0.0
        */
        Particle * getParticle(Nidx index);

        /** 获取粒子数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const              { return mActiveList.size();}

        /** 设置是否扩散
        @version NIIEngine 3.0.0
        */
        inline void setSpread(bool b)               { mSpread = b; }

        /** 获取是否扩散
        @version NIIEngine 3.0.0
        */
        inline bool isSpread() const                { return mSpread; }

        /** 设置扩散粒子最大数量
        @version NIIEngine 3.0.0
        */
        void setSpreadMax(NCount size);

        /** 获取扩散粒子最大数量
        @version NIIEngine 3.0.0
        */
        inline NCount getSpreadMax() const          { return mSpreadMax; }

        /** 添加扩散粒子
        @version NIIEngine 3.0.0
        */
        SpreadParticle * addSpread(Nid type);

        /** 获取扩散粒子
        @version NIIEngine 3.0.0
        */
        SpreadParticle * getSpread(Nidx index) const;

        /** 移去扩散粒子
        @version NIIEngine 3.0.0
        */
        void removeSpread(Nidx index);

        /** 移去所有扩散粒子
        @version NIIEngine 3.0.0
        */
        void removeAllSpread();

        /** 获取扩散粒子数量
        @version NIIEngine 3.0.0
        */
        NCount getSpreadCount() const;

        /** 获取活动粒子列表
        @version NIIEngine 3.0.0 高级api
        */
        inline const ParticleList & getList() const { return mActiveList; }

        /** 清理所有粒子
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 创建粒子效果
        @version NIIEngine 3.0.0
        */
        ParticleEffect * addEffect(Nid name);

        /** 获取效果
        @version NIIEngine 3.0.0
        */
        ParticleEffect * getEffect(Nidx index) const;

        /** 移去效果
        @version NIIEngine 3.0.0
        */
        void removeEffect(Nidx index);

        /** 移去所有效果
        @version NIIEngine 3.0.0
        */
        void removeAllEffect();

        /** 获取效果数量
        @version NIIEngine 3.0.0
        */
        NCount getEffectCount() const;

        /** 设置自动更新边界
        @param[in] b 是否自动
        @param[in] interval 更新间隔,如果参数是500,第一参数是true,则是每500毫秒更新一次
        @version NIIEngine 3.0.0 高级api
        */
        void setAutoBound(bool b, TimeDurMS interval = 500);

        /** 设置边界
        @note isLocalSpace 结果有关系,如果是全局空间,则这个设置是全局空间所在的边界,如果是本
            地空间,则是本地空间所在的边界
        @version NIIEngine 3.0.0
        */
        inline void setBounds(const AABox & aabb)   { mAABB = aabb; mBoundRange = mAABB.calcRadius(); }

        /** 更新边界
        @note isLocalSpace 结果有关系,如果是全局空间,则这个设置是全局空间所在的边界,如果是本
            地空间,则是本地空间所在的边界
        @version NIIEngine 3.0.0 高级api
        */
        void updateSpaceAABB();

        /** 设置资源数据来源
        @version NIIEngine 3.0.0
        */
        inline void setSrc(const String & src)      { mSrc = src; }

        /** 获取资源数据来源
        @version NIIEngine 3.0.0
        */
        inline const String & getSrc() const        { return mSrc; }

        /// @copydetails SpaceObj::_notify
        virtual void _notify(Camera * cam);

        /// @copydetails SpaceObj::attach
        void attach(PosNode * parent, bool tag = false);

        /// @copydetails SpaceObj::getAABB
        virtual const AABox & getAABB() const;

        /// @copydetails SpaceObj::getCenterRange
        virtual NIIf getCenterRange() const;

        /// @copydetails SpaceObj::queue
        virtual void queue(RenderQueue * queue);

        /// @copydetails SpaceObj::query
        void query(GeometryQuery * query);

        /// @copydetails SpaceObj::setRenderGroup
        void setRenderGroup(Nui16 rqid, Nui16 level);

        /// @copydetails SpaceObj::getTypeMark
        Nui32 getTypeMark() const;

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;
    public:
        /** 粒子大小已经改变
        @version NIIEngine 3.0.0
        */
        virtual void onParticleResize();

        /** 粒子
        @version NIIEngine 3.0.0
        */
        virtual void onParticleRotate();
    protected:
        /// @copydetails PropertyCmdObj
        bool initCmd(PropertyCmdSet * dest);

        /** 排序实现
        @version NIIEngine 3.0.0 高级api
        */
        void sortImpl(Camera * cam);

        /** 时渡实现
        @version NIIEngine 3.0.0 高级api
        */
        void costImpl(TimeDurMS cost);

        /** 移动实现
        @version NIIEngine 3.0.0 高级api
        */
        void moveImpl(TimeDurMS cost);

        /** 扩展实现
        @version NIIEngine 3.0.0 高级api
        */
        void spreadImpl(TimeDurMS cost);

        /** 效果实现
        @version NIIEngine 3.0.0 高级api
        */
        void effectImpl(TimeDurMS cost);

        /** 扩展池
        @version NIIEngine 3.0.0
        */
        void resizePool(NCount size);

        /** 扩展扩散池
        @version NIIEngine 3.0.0
        */
        void resizeSpreadPool(NCount size);

        /** 同步几何
        @version NIIEngine 3.0.0
        */
        void syncGeo();

        void initSpreadPool();
        void destorySpreadPool();
        void spreadParticle(SpreadParticle * p, TimeDurMS cost, NCount count);
    protected:
        typedef vector<Particle *>::type ParticlePool;
        typedef list<SpreadParticle *>::type SpreadList;
        typedef vector<SpreadParticle *>::type SpreadVector;
        typedef map<String, SpreadList>::type FreeSpreadMap;
        typedef map<String, SpreadVector>::type SpreadPoolMap;
        typedef vector<ParticleEffect *>::type ParticleEffectList;
    protected:
        ParticleList mActiveList;
        ParticleList mFreeList;
        ParticlePool mParticlePool;
        SpreadList mActiveSpreadList;
        SpreadVector mSpreadList;
        FreeSpreadMap mFreeSpreadMap;
        SpreadPoolMap mSpreadPool;
        ParticleEffectList mEffectList;
        DataEquation<TimeDurMS, TimeDurMS> * mTimeCtrl;

        AABox mAABB;
        AABox mSpaceAABB;
        NIIf mBoundRange;
        NCount mPoolSize;
        NCount mSpreadMax;
        NIIf mTimeFactor;

        String mSrc;
        ResourceID mMaterialID;
        GroupID mMaterialGroup;
        ParticleGeo * mGeo;
        NIIf mWidth;
        NIIf mHeight;
        Nmark mFrameMark;
        
        TimeDurMS mTimeToUpdate;
        TimeDurMS mTimeInterval;
        TimeDurMS mTimeThreshold;
        TimeDurMS mAutoBoundInterval;
        TimeDurMS mCurrentIntervalBound;
        TimeDurMS mCurrentTimeThreshold;
        
        bool mSorted;
        bool mSpread;
        bool mLocalSpace;
        bool mGeoUpdate;
        bool mSpreadInit;
        bool mCullIndividual;
        bool mAutoBound;
        bool mIntervalBound;
    };
}
#endif