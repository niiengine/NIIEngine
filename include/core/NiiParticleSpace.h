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

#ifndef _NII_ParticleSpace_H_
#define _NII_ParticleSpace_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiString.h"
#include "NiiPropertyCmdObj.h"
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
        void setTimeFactor(NIIf f);

        /** 获取时间因子
        @version NIIEngine 3.0.0
        */
        NIIf getTimeFactor() const;

        /** 设置时间间隔
        @version NIIEngine 3.0.0
        */
        void setTimeInterval(TimeDurMS ms);

        /** 获取时间间隔
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeInterval() const;

        /** 设置时间阀值
        @version NIIEngine 3.0.0
        */
        void setTimeThreshold(TimeDurMS ms);

        /** 获取时间阀值
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimeThreshold() const;

        /** 设置是否排序
        @param 视图距离
        @version NIIEngine 3.0.0
        */
        void setSortEnable(bool b);

        /** 设置是否排序
        @param 视图距离
        @version NIIEngine 3.0.0
        */
        bool isSortEnable() const;

        /** 设置纹理
        @version NIIEngine 3.0.0
        */
        virtual void setMaterial(ResourceID rid, GroupID gid = GroupUnknow);

        /** 获取纹理ID
        @version NIIEngine 3.0.0
        */
        virtual ResourceID getMaterialID() const;

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        virtual GroupID getResourceGroup() const;

        /** 设置粒子视图
        @version NIIEngine 3.0.0
        */
        void setGeo(Nid type);

        /** 获取粒子几何控制
        @version NIIEngine 3.0.0
        */
        ParticleGeo * getGeo() const;

        /** 设置粒子最大数量
        @version NIIEngine 3.0.0
        */
        void setPoolSize(NCount size);

        /** 获取最大数量
        @version NIIEngine 3.0.0
        */
        NCount getPoolSize() const;

        /** 设置是否启用独立拣选
        @version NIIEngine 3.0.0
        */
        void setCullItself(bool b);

        /** 获取是否启用独立拣选
        @version NIIEngine 3.0.0
        */
        bool getCullItself() const;

        /** 设置是否本地空间模式
        @note 默认是全局空间,不是本地空间
        @version NIIEngine 3.0.0
        */
        void setLocalSpace(bool b);

        /** 获取是否本地空间模式
        @note 默认是全局空间,不是本地空间
        @version NIIEngine 3.0.0
        */
        bool isLocalSpace() const;

        /** 设置粒子宽
        @version NIIEngine 3.0.0
        */
        void setParticleWidth(NIIf w);

        /** 获取粒子宽
        @version NIIEngine 3.0.0
        */
        NIIf getParticleWidth() const;

        /** 设置粒子高
        @version NIIEngine 3.0.0
         */
        void setParticleHeight(NIIf h);

        /** 获取粒子高
        @version NIIEngine 3.0.0
        */
        NIIf getParticleHeight() const;

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
        NCount getCount() const;

        /** 设置是否扩散
        @version NIIEngine 3.0.0
        */
        void setSpread(bool b);

        /** 获取是否扩散
        @version NIIEngine 3.0.0
        */
        bool isSpread() const;

        /** 设置扩散粒子最大数量
        @version NIIEngine 3.0.0
        */
        void setSpreadMax(NCount size);

        /** 获取扩散粒子最大数量
        @version NIIEngine 3.0.0
        */
        NCount getSpreadMax() const;

        /** 添加扩散粒子
        @version NIIEngine 3.0.0
        */
        SpreadParticle * addSpread(const String & type);

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
        const ParticleList & getList() const;

        /** 清理所有粒子
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 创建粒子效果
        @version NIIEngine 3.0.0
        */
        ParticleEffect * addEffect(const String & name);

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
        void setBounds(const AABox & aabb);

        /** 更新边界
        @note isLocalSpace 结果有关系,如果是全局空间,则这个设置是全局空间所在的边界,如果是本
            地空间,则是本地空间所在的边界
        @version NIIEngine 3.0.0 高级api
        */
        void updateSpaceAABB();

        /** 设置资源数据来源
        @version NIIEngine 3.0.0
        */
        void setSrc(const String & src);

        /** 获取资源数据来源
        @version NIIEngine 3.0.0
        */
        const String & getSrc() const;

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
        bool init(PropertyCmdSet * dest);

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

        String mOrigin;
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