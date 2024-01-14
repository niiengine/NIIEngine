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

#include "NiiPreProcess.h"
#include "NiiParticleSpace.h"
#include "NiiParticleSpaceManager.h"
#include "NiiRenderQueue.h"
#include "NiiSpreadParticle.h"
#include "NiiParticleEffect.h"
#include "NiiStrConv.h"
#include "NiiLogManager.h"
#include "NiiAnimationManager.h"
#include "NiiResourceSchemeManager.h"
#include "NiiParticleGeo.h"
#include "NiiRadixSort.h"
#include "NiiMaterialManager.h"
#include "NiiSpaceManager.h"
#include "NiiEquationManager.h"
#include "NiiEngine.h"

namespace NII
{
    //-----------------------------------------------------------------------
    class _EngineInner CmdQuota : public PropertyCmd
    {
    public:
        CmdQuota() : PropertyCmd(N_PropertyParticleSpace_PoolSize, _T("quota"), 
            _T("The maximum number of particle allowed at once in this system."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getPoolSize(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setPoolSize(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getPoolSize();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<ParticleSpace *>(own)->setPoolSize(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdQuota();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)10, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdEmittedEmitterQuota : public PropertyCmd
    {
    public:
        CmdEmittedEmitterQuota() : PropertyCmd(N_PropertyParticleSpace_SpreadMax, _T("emit_emitter_quota"),
            _T("The maximum number of emitters to be emitted at once in this system."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getSpreadMax(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setSpreadMax(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getSpreadMax();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<ParticleSpace *>(own)->setSpreadMax(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdEmittedEmitterQuota();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NCount)3, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdMaterial : public PropertyCmd
    {
    public:
        CmdMaterial() : PropertyCmd(N_PropertyParticleSpace_MaterialID, _T("material"),
            _T("The name of the material to be used to render all particles in this system."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getMaterialID(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setMaterial(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getMaterialID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<ParticleSpace *>(own)->setMaterial(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaterial();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdCull : public PropertyCmd
    {
    public:
        CmdCull(): PropertyCmd(N_PropertyParticleSpace_CullItself, _T("cull_each"),
            _T("If true, each particle is culled in it's own right. If false, the entire system is culled as a whole."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getCullItself(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setCullItself(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getCullItself();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ParticleSpace *>(own)->setCullItself(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCull();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdWidth : public PropertyCmd
    {
    public:
        CmdWidth() : PropertyCmd(N_PropertyParticleSpace_ParticleWidth, _T("particle_width"),
            _T("The width of particles in world units."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getParticleWidth(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setParticleWidth(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getParticleWidth();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ParticleSpace *>(own)->setParticleWidth(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdWidth();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdHeight : public PropertyCmd
    {
     public:
        CmdHeight() : PropertyCmd(N_PropertyParticleSpace_ParticleHeight, _T("particle_height"), _T("The height of particles in world units."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getParticleHeight(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace*>(own)->setParticleHeight(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getParticleHeight();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<ParticleSpace*>(own)->setParticleHeight(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdHeight();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdRenderer : public PropertyCmd
    {
    public:
        CmdRenderer() : PropertyCmd(N_PropertyParticleSpace_Geo, _T("renderer"),
            _T("Sets the particle system renderer to use (default 'billboard')."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            ParticleGeo * obj = static_cast<const ParticleSpace *>(own)->getGeo();
            if(obj)
                out = obj->getType();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nid temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setGeo(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            FactoryID * temp = (FactoryID *)out;
            ParticleGeo * obj = static_cast<const ParticleSpace *>(own)->getGeo();
            if(obj)
                *temp = obj->getType();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nid * temp = (Nid *)in;
            static_cast<ParticleSpace *>(own)->setGeo(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdRenderer();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = StrConv::conv(N_FACTORY_ParticleCtrl, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdSorted : public PropertyCmd
    {
    public:
        CmdSorted() : PropertyCmd(N_PropertyParticleSpace_SortEnable, _T("sorted"),
            _T("Sets whether particles should be sorted relative to the camera. "))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->isSortEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace*>(own)->setSortEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ParticleSpace *>(own)->isSortEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ParticleSpace*>(own)->setSortEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSorted();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdLocalSpace : public PropertyCmd
    {
    public:
        CmdLocalSpace() : PropertyCmd(N_PropertyParticleSpace_LocalSpace, _T("local_space"),
            _T("Sets whether particles should be kept in local space rather than "
                "emitted into world space. "))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->isLocalSpace(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace*>(own)->setLocalSpace(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const ParticleSpace *>(own)->isLocalSpace();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            bool * temp = (bool *)in;
            static_cast<ParticleSpace*>(own)->setLocalSpace(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdLocalSpace();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdIterationInterval : public PropertyCmd
    {
    public:
        CmdIterationInterval() : PropertyCmd(N_PropertyParticleSpace_TimeInterval, _T("iteration_interval"),
            _T("Sets a fixed update interval for the system, or 0 for the frame rate. ")){}

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getTimeInterval(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TimeDurMS temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setTimeInterval(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TimeDurMS * temp = (TimeDurMS *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getTimeInterval();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            TimeDurMS * temp = (TimeDurMS *)in;
            static_cast<ParticleSpace *>(own)->setTimeInterval(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdIterationInterval();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //-----------------------------------------------------------------------
    class _EngineInner CmdNonvisibleTimeout : public PropertyCmd
    {
    public:
        CmdNonvisibleTimeout() : PropertyCmd(N_PropertyParticleSpace_TimeThreshold, _T("nonvisible_update_timeout, "
            "Sets a timeout on updates to the system if the system is not visible for the given number of seconds (0 to always update)")){}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const ParticleSpace *>(own)->getTimeThreshold(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TimeDurMS temp;
            StrConv::conv(val, temp);
            static_cast<ParticleSpace *>(own)->setTimeThreshold(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TimeDurMS * temp = (TimeDurMS *)out;
            *temp = static_cast<const ParticleSpace *>(own)->getTimeThreshold();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            TimeDurMS * temp = (TimeDurMS *)in;
            static_cast<ParticleSpace *>(own)->setTimeThreshold(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNonvisibleTimeout();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };
    //-----------------------------------------------------------------------
    struct ParticleSpaceDirSortFunctor
    {
        Vector3f sortDir;

        ParticleSpaceDirSortFunctor(const Vector3f & dir) : sortDir(dir){}
        NIIf operator()(Particle * p) const
        {
            return sortDir.dotProduct(p->mPosition);
        }
    };

    struct ParticleSpaceDisSortFunctor
    {
        Vector3f sortPos;

        ParticleSpaceDisSortFunctor(const Vector3f & pos) : sortPos(pos){}
        NIIf operator()(Particle * p) const
        {
            // Sort descending by squared distance
            return - (sortPos - p->mPosition).lengthSquared();
        }
    };
    //---------------------------------------------------------------------------
    static RadixSort<ParticleList, Particle *, NIIf> __ParticleSpaceRadixSorter;
    //-----------------------------------------------------------------------
    // Local class for updating based on time
    class ParticleSpaceUpdateValue : public DataValue<TimeDurMS>
    {
    public:
        ParticleSpaceUpdateValue(ParticleSpace * target, bool autoDsy) :
            DataValue<TimeDurMS>(autoDsy),
            mTarget(target) {}

        const TimeDurMS getValue() const { return 0; } // N/A

        void setValue(const TimeDurMS & value) { mTarget->_update(value); }
    protected:
        ParticleSpace * mTarget;
    };
    //-----------------------------------------------------------------------
    ParticleSpace::ParticleSpace(LangID lid):
        PropertyCmdObj(N_CmdObj_ParticleSpace, lid),
        mAABB(),
        mBoundRange(1.0f),
        mAutoBoundInterval(500),
        mCurrentIntervalBound(500),
        mTimeToUpdate(0),
        mSpaceAABB(),
        mMaterialGroup(GroupDefault),
        mTimeFactor(1.0f),
        mTimeInterval(0),
        mTimeThreshold(0),
        mCurrentTimeThreshold(0),
        mFrameMark(0),
        mTimeCtrl(0),
        mGeo(0),
        mPoolSize(0),
        mSpreadMax(0),
        mSpreadInit(false),
        mGeoUpdate(false),
        mCullIndividual(false),
        mIntervalBound(false),
        mLocalSpace(false),
        mAutoBound(true),
        mSorted(false),
        mSpread(true)
    {
        setupCmd(N_CmdObj_ParticleSpace);

        setGeo(N_FACTORY_ParticleCtrl);
    }
    //-----------------------------------------------------------------------
    ParticleSpace::ParticleSpace(SpaceID sid, GroupID gid, LangID lid):
        PropertyCmdObj(N_CmdObj_ParticleSpace, lid),
        SpaceObj(sid),
        mAABB(),
        mBoundRange(1.0f),
        mAutoBoundInterval(500),
        mCurrentIntervalBound(500),
        mTimeToUpdate(0),
        mSpaceAABB(),
        mMaterialGroup(gid),
        mTimeFactor(1.0f),
        mTimeInterval(0),
        mTimeThreshold(0),
        mCurrentTimeThreshold(0),
        mFrameMark(N_Engine().getCurrentFrame()),
        mTimeCtrl(0),
        mGeo(0),
        mPoolSize(0),
        mSpreadMax(0),
        mSpreadInit(false),
        mGeoUpdate(false),
        mCullIndividual(false),
        mIntervalBound(false),
        mLocalSpace(false),
        mAutoBound(true),
        mSpread(true),
        mSorted(false)
    {
        setupCmd(N_CmdObj_ParticleSpace);

        setParticleWidth(100);
        setParticleHeight(100);
        setMaterial(MaterialManager::WhiteLight->getOriginID());
        // Default to 10 particles, expect app to specify (will only be increased, not decreased)
        setPoolSize(10);
        setSpreadMax(3);

        setGeo(N_FACTORY_ParticleCtrl);
    }
    //-----------------------------------------------------------------------
    ParticleSpace::~ParticleSpace()
    {
        if(mTimeCtrl)
        {
            // Destroy controller
            N_Only(Equation).destroy(mTimeCtrl);
            mTimeCtrl = 0;
        }

        // Arrange for the deletion of emitters & affectors
        removeAllSpread();
        destorySpreadPool();
        removeAllEffect();

        ParticlePool::iterator i, iend = mParticlePool.end();
        for(i = mParticlePool.begin(); i != iend; ++i)
        {
            N_delete *i;
        }

        if(mGeo)
        {
            N_Only(ParticleSpace).destroyGeo(mGeo);
            mGeo = 0;
        }
    }
    //-----------------------------------------------------------------------
    SpreadParticle * ParticleSpace::addSpread(Nid type)
    {
        SpreadParticle * em = N_Only(ParticleSpace).createSpread(type, this);
        mSpreadList.push_back(em);
        return em;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::removeSpread(Nidx index)
    {
        N_assert(index < mSpreadList.size(), "Emitter index out of bounds!");
        SpreadVector::iterator ei = mSpreadList.begin() + index;
        N_Only(ParticleSpace).destroySpread(*ei);
        mSpreadList.erase(ei);
    }
    //-----------------------------------------------------------------------
    SpreadParticle * ParticleSpace::getSpread(Nidx index) const
    {
        N_assert(index < mSpreadList.size(), "Emitter index out of bounds!");
        return mSpreadList[index];
    }
    //-----------------------------------------------------------------------
    NCount ParticleSpace::getSpreadCount() const
    {
        return mSpreadList.size();
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::removeAllSpread()
    {
        // DON'T delete directly, we don't know what heap these have been created on
        SpreadVector::iterator i, iend = mSpreadList.end();
        for(i = mSpreadList.begin(); i != iend; ++i)
        {
            N_Only(ParticleSpace).destroySpread(*i);
        }
        mSpreadList.clear();
    }
    //-----------------------------------------------------------------------
    ParticleEffect * ParticleSpace::addEffect(Nid name)
    {
        ParticleEffect * re = N_Only(ParticleSpace).createEffect(name, this);
        mEffectList.push_back(re);
        return re;
    }
    //-----------------------------------------------------------------------
    ParticleEffect* ParticleSpace::getEffect(Nidx index) const
    {
        N_assert(index < mEffectList.size(), "Affector index out of bounds!");
        return mEffectList[index];
    }
    //-----------------------------------------------------------------------
    NCount ParticleSpace::getEffectCount() const
    {
        return mEffectList.size();
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::removeEffect(Nidx index)
    {
        N_assert(index < mEffectList.size(), "Affector index out of bounds!");
        ParticleEffectList::iterator ai = mEffectList.begin() + index;
        N_Only(ParticleSpace).destroyEffect(*ai);
        mEffectList.erase(ai);
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::removeAllEffect()
    {
        // DON'T delete directly, we don't know what heap these have been created on
        ParticleEffectList::iterator i, iend = mEffectList.end();
        for(i = mEffectList.begin(); i != iend; ++i)
        {
            N_Only(ParticleSpace).destroyEffect(*i);
        }
        mEffectList.clear();
    }
    //-----------------------------------------------------------------------
    ParticleSpace & ParticleSpace::operator=(const ParticleSpace & o)
    {
        removeAllSpread();
        destorySpreadPool();
        removeAllEffect();

        // Copy emitters
        for(NCount i = 0; i < o.getSpreadCount(); ++i)
        {
            SpreadParticle * rhsEm = o.getSpread(i);
            SpreadParticle * newEm = addSpread(rhsEm->getType());
            newEm->copyProperty(rhsEm);
        }
        // Copy affectors
        for(NCount i = 0; i < o.getEffectCount(); ++i)
        {
            ParticleEffect * rhsAf = o.getEffect(i);
            ParticleEffect * newAf = addEffect(rhsAf->getType());
            newAf->copyProperty(rhsAf);
        }
        setPoolSize(o.getPoolSize());
        setSpreadMax(o.getSpreadMax());
        setMaterial(o.mMaterialID);
        setParticleWidth(o.mWidth);
        setParticleHeight(o.mHeight);
        mCullIndividual = o.mCullIndividual;
        mLocalSpace = o.mLocalSpace;
        mSorted = o.mSorted;
        mTimeInterval = o.mTimeInterval;
        mTimeThreshold = o.mTimeThreshold;
        // last frame visible and time since last visible should be left default

        ParticleGeo * temp = o.getGeo();
        if(temp)
        {
            setGeo(temp->getType());
        }
        else
        {
            setGeo(0);
        }
        // Copy settings
        if(mGeo && o.mGeo)
        {
            mGeo->copyProperty(o.mGeo);
        }
        mGeoUpdate = false;
        return *this;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setPoolSize(NCount size)
    {
        // Never shrink below size()
        NCount currSize = mParticlePool.size();
        if(currSize < size)
        {
            // Will allocate particles on demand
            mPoolSize = size;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setSpreadMax(NCount size)
    {
        // Never shrink below size()
        NCount currSize = 0;
        SpreadPoolMap::iterator i, iend = mSpreadPool.end();
        for(i = mSpreadPool.begin(); i != iend; ++i)
        {
            currSize += i->second.size();
        }

        if(currSize < size)
        {
            // Will allocate emitted emitters on demand
            mSpreadMax = size;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::_update(TimeDurMS cost)
    {
        if(!mSpaceNode)
            return;

        TimeDurMS nonvisibleTimeout = mTimeThreshold ? mTimeThreshold : ParticleSpaceManager::mTimeThreshold;
        if(nonvisibleTimeout > 0)
        {
            // Check whether it's been more than one frame (update is ahead of
            // camera notification by one frame because of the ordering)
            NIIl frameDiff = N_Engine().getCurrentFrame() - mFrameMark;
            if(frameDiff > 1 || frameDiff < 0) // < 0 if wrap only
            {
                mCurrentTimeThreshold += cost;
                if(mCurrentTimeThreshold >= nonvisibleTimeout)
                {
                    // No update
                    return;
                }
            }
        }

        // Scale incoming speed for the rest of the calculation
        cost *= mTimeFactor;

        // Init renderer if not done already
        syncGeo();

        // Initialise emitted emitters list if not done already
        initSpreadPool();

        TimeDurMS timeInterval = mTimeInterval ? mTimeInterval : ParticleSpaceManager::mTimeInterval;
        if(timeInterval > 0)
        {
            mTimeToUpdate += cost;

            while(mTimeToUpdate >= timeInterval)
            {
                // Update existing particles
                costImpl(timeInterval);
                effectImpl(timeInterval);
                moveImpl(timeInterval);

                if(mSpread)
                {
                    spreadImpl(timeInterval);
                }

                mTimeToUpdate -= timeInterval;
            }
        }
        else
        {
            // Update existing particles
            costImpl(cost);
            effectImpl(cost);
            moveImpl(cost);

            if(mSpread)
            {
                // Emit new particles
                spreadImpl(cost);
            }
        }

        if(mAutoBound)
        {
            if(mCurrentIntervalBound < cost)
            {
                mCurrentIntervalBound = mAutoBoundInterval;
                mIntervalBound = true;
            }
            else
            {
                mCurrentIntervalBound -= cost;
            }
        }
        updateSpaceAABB();
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::_forward(TimeDurMS time, TimeDurMS interval)
    {
        for(TimeDurMS ftime = 0; ftime < time; ftime += interval)
        {
            _update(interval);
        }
    }
    //-----------------------------------------------------------------------
    bool ParticleSpace::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdCull());
        dest->add(N_new CmdHeight());
        dest->add(N_new CmdMaterial());
        dest->add(N_new CmdQuota());
        dest->add(N_new CmdEmittedEmitterQuota());
        dest->add(N_new CmdWidth());
        dest->add(N_new CmdRenderer());
        dest->add(N_new CmdSorted());
        dest->add(N_new CmdLocalSpace());
        dest->add(N_new CmdIterationInterval());
        dest->add(N_new CmdNonvisibleTimeout());
        return true;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::costImpl(TimeDurMS cost)
    {
        Particle * pParticle;
        SpreadParticle * pSpreadParticle;

        ParticleList::iterator i, iend = mActiveList.end();
        for(i = mActiveList.begin(); i != iend;)
        {
            pParticle = static_cast<Particle *>(*i);
            if(pParticle->mRemainTTL < cost)
            {
                // Identify the particle type
                if(pParticle->mType == Particle::T_Alone)
                {
                    // Destroy this one
                    mFreeList.splice(mFreeList.end(), mActiveList, i++);
                }
                else
                {
                    // For now, it can only be an emitted emitter
                    pSpreadParticle = static_cast<SpreadParticle*>(*i);
                    FreeSpreadMap::iterator feei = mFreeSpreadMap.find(pSpreadParticle->getName());
                    if(feei != mFreeSpreadMap.end())
                    {
                        // Found feei
                        return (feei->second).push_back(pSpreadParticle);
                    }

                    // Also erase from mActiveSpreadList
                    N_assert(pSpreadParticle, "Emitter to be removed is 0!");
                    SpreadList::iterator si, siend = mActiveSpreadList.end();
                    for(si = mActiveSpreadList.begin(); si != siend; ++si)
                    {
                        if(pSpreadParticle == (*si))
                        {
                            mActiveSpreadList.erase(si);
                            break;
                        }
                    }

                    // And erase from mActiveList
                    i = mActiveList.erase(i);
                }
            }
            else
            {
                // Decrement TTL
                pParticle->mRemainTTL -= cost;
                ++i;
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::spreadImpl(TimeDurMS cost)
    {
        // Add up requests for emission
        static vector<NCount>::type requested;
        if(requested.size() != mSpreadList.size())
            requested.resize(mSpreadList.size());

        NCount totalRequested, emitterCount, i, emissionAllowed;
        SpreadList::iterator itActiveEmit;

        emitterCount = mSpreadList.size();
        emissionAllowed = mFreeList.size();
        totalRequested = 0;

        // Count up total requested emissions for regular emitters (and exclude the ones that are used as
        // a template for emitted emitters)
        SpreadVector::iterator itEmit, iEmitEnd = mSpreadList.end();
        for(itEmit = mSpreadList.begin(), i = 0; itEmit != iEmitEnd; ++itEmit, ++i)
        {
            if(!(*itEmit)->isAuto())
            {
                requested[i] = (*itEmit)->getSpreadImpl(cost);
                totalRequested += requested[i];
            }
        }

        // Add up total requested emissions for (active) emitted emitters
        for (itActiveEmit = mActiveSpreadList.begin(); itActiveEmit != mActiveSpreadList.end(); ++itActiveEmit)
        {
            totalRequested += (*itActiveEmit)->getSpreadImpl(cost);
        }

        // Check if the quota will be exceeded, if so reduce demand
        NIIf ratio = 1.0f;
        if(totalRequested > emissionAllowed)
        {
            // Apportion down requested values to allotted values
            ratio =  (NIIf)emissionAllowed / (NIIf)totalRequested;
            for(i = 0; i < emitterCount; ++i)
            {
                requested[i] = requested[i] * ratio;
            }
        }

        // Emit
        // For each emission, apply a subset of the motion for the frame
        // this ensures an even distribution of particles when many are
        // emitted in a single frame
        for(itEmit = mSpreadList.begin(), i = 0; itEmit != iEmitEnd; ++itEmit, ++i)
        {
            // Trigger the emitters, but exclude the emitters that are already in the emitted emitters list;
            // they are handled in a separate loop
            if(!(*itEmit)->isAuto())
                spreadParticle(*itEmit, cost, requested[i]);
        }

        // Do the same with all active emitted emitters
        for(itActiveEmit = mActiveSpreadList.begin(), i = 0; itActiveEmit != mActiveSpreadList.end(); ++itActiveEmit, ++i)
            spreadParticle(*itActiveEmit, cost, (*itActiveEmit)->getSpreadImpl(cost) * ratio);
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::spreadParticle(SpreadParticle * emitter, TimeDurMS cost,
        NCount requested)
    {
        ParticleEffectList::iterator itAff, itAffEnd;
        NIIf timePoint = 0.0f;

        // avoid any divide by zero conditions
        if(!requested)
            return;

        NIIf timeInc = cost / (requested * 1000);

        for(NCount j = 0; j < requested; ++j)
        {
            // Create a new particle & init using emitter
            // The particle is a visual particle if the emit_emitter property of the emitter isn't set
            Particle * p = 0;
            String emitterName = emitter->getSpreadName();
            if(emitterName == StrUtil::WBLANK)
                p = create();
            else
                p = createSpread(emitterName);

            // Only continue if the particle was really created (not null)
            if(!p)
                return;

            emitter->initImpl(p);

            // Translate mPosition & direction into world space
            if(!mLocalSpace)
            {
                p->mPosition  = (mSpaceNode->getSpaceOri() *
                    (mSpaceNode->getSpaceScale() * p->mPosition)) + mSpaceNode->getSpacePos();

                p->mDirection = (mSpaceNode->getSpaceOri() * p->mDirection);
            }

            // apply partial frame motion to this particle
            p->mPosition += (p->mDirection * timePoint);

            // apply particle initialization by the affectors
            itAffEnd = mEffectList.end();
            for(itAff = mEffectList.begin(); itAff != itAffEnd; ++itAff)
                (*itAff)->initImpl(p);

            // Increment time fragment
            timePoint += timeInc;

            if(p->mType == Particle::T_Spread)
            {
                // If the particle is an emitter, the mPosition on the emitter side must also be initialised
                // Note, that mPosition of the emitter becomes a mPosition in worldspace if mLocalSpace is set
                // to false (will this become a problem?)
                SpreadParticle * pSpreadParticle = static_cast<SpreadParticle*>(p);
                pSpreadParticle->setPos(p->mPosition);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::moveImpl(TimeDurMS cost)
    {
        Particle * p;
        SpreadParticle * pSpread;

        ParticleList::iterator i, iend = mActiveList.end();
        for(i = mActiveList.begin(); i != iend; ++i)
        {
            p = static_cast<Particle *>(*i);
            p->mPosition += (p->mDirection * (cost / 1000));

            if(p->mType == Particle::T_Spread)
            {
                // If it is an emitter, the emitter mPosition must also be updated
                // Note, that mPosition of the emitter becomes a mPosition in worldspace if mLocalSpace is set
                // to false (will this become a problem?)
                pSpread = static_cast<SpreadParticle *>(*i);
                pSpread->setPos(p->mPosition);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::effectImpl(TimeDurMS cost)
    {
        ParticleEffectList::iterator i, iend = mEffectList.end();
        for(i = mEffectList.begin(); i != iend; ++i)
        {
            (*i)->effectImpl(mActiveList, cost);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::resizePool(NCount size)
    {
        NCount oldSize = mParticlePool.size();

        // Increase size
        mParticlePool.reserve(size);
        mParticlePool.resize(size);

        // Create new particles
        for(NCount i = oldSize; i < size; i++ )
        {
            mParticlePool[i] = N_new Particle();
        }
    }
    //-----------------------------------------------------------------------
    Particle* ParticleSpace::getParticle(Nidx index)
    {
        N_assert (index < mActiveList.size(), "Index out of bounds!");
        ParticleList::iterator i = mActiveList.begin();
        std::advance(i, index);
        return *i;
    }
    //-----------------------------------------------------------------------
    Particle * ParticleSpace::create()
    {
        Particle * p = 0;
        if(!mFreeList.empty())
        {
            // Fast creation (don't use superclass since emitter will init)
            p = mFreeList.front();
            mActiveList.splice(mActiveList.end(), mFreeList, mFreeList.begin());

            p->setCtrl(this);
        }
        return p;
    }
    //-----------------------------------------------------------------------
    Particle * ParticleSpace::createSpread(const String & name)
    {
        // Get the appropriate list and retrieve an emitter
        Particle * p = 0;
        FreeSpreadMap::iterator feei = mFreeSpreadMap.find(name);
        if(feei != mFreeSpreadMap.end())
        {
            list<SpreadParticle *>::type & fee = feei->second;
            if(!fee.empty())
            {
                p = fee.front();
                p->mType = Particle::T_Spread;
                fee.pop_front();
                mActiveList.push_back(p);

                // Also add to mActiveSpreadList. This is needed to traverse through all active emitters
                // that are emitted. Don't use mActiveList for that (although they are added to
                // mActiveList also), because it would take too NIIl to traverse.
                mActiveSpreadList.push_back(static_cast<SpreadParticle*>(p));

                p->setCtrl(this);
            }
        }
        return p;
    }
    //-----------------------------------------------------------------------
    Particle * ParticleSpace::createGlint()
    {
        Particle * p = 0;
        if(!mFreeList.empty())
        {
            // Fast creation (don't use superclass since emitter will init)
            p = mFreeList.front();
            p->mType = Particle::T_Glint;
            mActiveList.splice(mActiveList.end(), mFreeList, mFreeList.begin());

            p->setCtrl(this);
        }
        return p;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::queue(RenderQueue * queue)
    {
        if(mGeo)
        {
            mGeo->queue(queue, mActiveList);
        }
    }
    //---------------------------------------------------------------------
    void ParticleSpace::query(GeometryQuery * query)
    {
        if(mGeo)
        {
            mGeo->query(query);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::updateSpaceAABB()
    {
        if(mSpaceNode && mAutoBound && mIntervalBound)
        {
            if(mActiveList.empty())
            {
                // No particles, reset to null if auto update bounds
                if(mAutoBound)
                {
                    mSpaceAABB.setNull();
                }
            }
            else
            {
                Vector3f min;
                Vector3f max;
                if(!mAutoBound)
                {
                    // We're on a limit, grow rather than reset each time
                    // so that we pick up the worst case scenario
                    min = mSpaceAABB.getMin();
                    max = mSpaceAABB.getMax();
                }
                else
                {
                    min.x = min.y = min.z = Math::POS_INFINITY;
                    max.x = max.y = max.z = Math::NEG_INFINITY;
                }
                ParticleList::iterator p;
                Vector3f halfScale = Vector3f::UNIT * 0.5;
                Vector3f defaultPadding = halfScale * std::max(mHeight, mWidth);
                for(p = mActiveList.begin(); p != mActiveList.end(); ++p)
                {
                    if((*p)->mInheritSize)
                    {
                        min = min.floor((*p)->mPosition - defaultPadding);
                        max = max.ceil((*p)->mPosition + defaultPadding);
                    }
                    else
                    {
                        Vector3f padding = halfScale * std::max((*p)->mWidth, (*p)->mHeight);
                        min = min.floor((*p)->mPosition - padding);
                        max = max.ceil((*p)->mPosition + padding);
                    }
                }
                mSpaceAABB.setMin(min);
                mSpaceAABB.setMax(max);
            }

            if(mLocalSpace)
            {
                // Merge calculated box with current AABB to preserve any user-set AABB
                mAABB.merge(mSpaceAABB);
            }
            else
            {
                // We've already put particles in world space to decouple them from the
                // node transform, so reverse transform back since we're expected to
                // provide a local AABB
                AABox newAABB(mSpaceAABB);
                newAABB.affine(mSpaceNode->getSpaceMatrix().inverseAffine());

                // Merge calculated box with current AABB to preserve any user-set AABB
                mAABB.merge(newAABB);
            }

            mSpaceNode->notify();
            mIntervalBound = false;
        }
    }
    //-----------------------------------------------------------------------
    FactoryID ParticleSpace::getFactoryID() const
    {
        return N_FACTORY_ParticleSpace;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::onParticleResize()
    {
        if(mGeo)
        {
            mGeo->onParticleResize();
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::onParticleRotate()
    {
        if(mGeo)
        {
            mGeo->onParticleRotate();
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setParticleWidth(NIIf w)
    {
        mWidth = w;
        if(mGeo)
        {
            mGeo->onCtrlResize(mWidth, mHeight);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setParticleHeight(NIIf h)
    {
        mHeight = h;
        if(mGeo)
        {
            mGeo->onCtrlResize(mWidth, mHeight);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::_notify(Camera * cam)
    {
        SpaceObj::_notify(cam);

        // Record visible
        if(isVisible())
        {
            mFrameMark = N_Engine().getCurrentFrame();
            mCurrentTimeThreshold = 0;

            if(mSorted)
            {
                sortImpl(cam);
            }

            if(mGeo)
            {
                if(!mGeoUpdate)
                    syncGeo();

                mGeo->_notify(cam);
            }
        }
    }
    //-----------------------------------------------------------------------
    const AABox & ParticleSpace::getAABB() const
    {
        return mAABB;
    }
    //-----------------------------------------------------------------------
    NIIf ParticleSpace::getCenterRange() const
    {
        return mBoundRange;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::attach(PosNode * parent, bool isTagBone)
    {
        SpaceObj::attach(parent, isTagBone);
        if(mGeo && mGeoUpdate)
        {
            mGeo->attach(parent, isTagBone);
        }

        if(parent && !mTimeCtrl)
        {
            mCurrentTimeThreshold = 0;
            mFrameMark = N_Engine().getCurrentFrame();

            DataValue<TimeDurMS> * updValue = N_new ParticleSpaceUpdateValue(this, true);
            mTimeCtrl = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(), updValue, N_Only(Animation).getNormalTimeFunc());
        }
        else if(!parent && mTimeCtrl)
        {
            N_Only(Equation).destroy(mTimeCtrl);
            mTimeCtrl = 0;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setMaterial(ResourceID rid, GroupID gid)
    {
        if(rid != mMaterialID)
        {
            mMaterialID = rid;

            if(mGeoUpdate)
            {
                mGeo->setMaterial(rid, mMaterialGroup);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::clear()
    {
        // Move actives to free list
        mFreeList.splice(mFreeList.end(), mActiveList);
        // Add active emitted emitters to free list
        SpreadList::iterator i, iend = mActiveSpreadList.end();
        for(i = mActiveSpreadList.begin(); i != iend; ++i)
        {
            FreeSpreadMap::iterator feei = mFreeSpreadMap.find((*i)->getName());
            if(feei != mFreeSpreadMap.end())
            {
                feei->second.push_back(*i);
            }
        }
        // Remove all active emitted emitter instances
        mActiveSpreadList.clear();
        // Reset update remain time
        mTimeToUpdate = 0;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setGeo(Nid name)
    {
        if(mGeo)
        {
            N_Only(ParticleSpace).destroyGeo(mGeo);
            mGeo = 0;
        }

        if(name)
        {
            mGeo = N_Only(ParticleSpace).createGeo(name);
            mGeoUpdate = false;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::syncGeo()
    {
        // Actual allocate particles
        NCount currSize = mParticlePool.size();
        NCount size = mPoolSize;
        if(currSize < size)
        {
            resizePool(size);

            for(NCount i = currSize; i < size; ++i)
            {
                // Add new items to the queue
                mFreeList.push_back(mParticlePool[i]);
            }

            // Tell the renderer, if already configured
            if(mGeo && mGeoUpdate)
            {
                mGeo->onPoolSizeChange(size);
            }
        }

        if(mGeo && !mGeoUpdate)
        {
            mGeo->onPoolSizeChange(mParticlePool.size());
            mGeo->attach(mSpaceNode, mTagNode);
            mGeo->onCtrlResize(mWidth, mHeight);
            mGeo->setMaterial(mMaterialID, mMaterialGroup);
            if(mRenderGroup != RQG_Unknow)
                mGeo->setRenderGroup(mRenderGroup);
            mGeo->setCullItself(mCullIndividual);
            mGeo->setLocalSpace(mLocalSpace);
            mGeoUpdate = true;
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setCullItself(bool b)
    {
        mCullIndividual = b;
        if(mGeo)
            mGeo->setCullItself(mCullIndividual);
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setAutoBound(bool b, TimeDurMS interval)
    {
        mAutoBound = b;
        mAutoBoundInterval = interval;
        mCurrentIntervalBound = interval;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setRenderGroup(Nui16 rqid, Nui16 level)
    {
        SpaceObj::setRenderGroup(rqid, level);
        if(mGeo)
        {
            mGeo->setRenderGroup(rqid);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::setLocalSpace(bool b)
    {
        mLocalSpace = b;
        if(mGeo)
        {
            mGeo->setLocalSpace(b);
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::sortImpl(Camera * cam)
    {
        if(mGeo)
        {
            ParticleCtrl::SortMode sortMode = mGeo->getSortMode();
            if(sortMode == ParticleCtrl::SM_DIRECTION)
            {
                Vector3f camDir = cam->getSpaceDirection();
                if (mLocalSpace)
                {
                    // transform the camera direction into local space
                    camDir = mSpaceNode->getSpaceOri().inverseUnit() * camDir;
                }
                __ParticleSpaceRadixSorter.sort(mActiveList, ParticleSpaceDirSortFunctor(- camDir));
            }
            else if (sortMode == ParticleCtrl::SM_DISTANCE)
            {
                Vector3f camPos = cam->getSpacePos();
                if (mLocalSpace)
                {
                    // transform the camera mPosition into local space
                    camPos = mSpaceNode->getSpaceOri().inverseUnit() *
                        (camPos - mSpaceNode->getSpacePos()) / mSpaceNode->getSpaceScale();
                }
                __ParticleSpaceRadixSorter.sort(mActiveList, ParticleSpaceDisSortFunctor(camPos));
            }
        }
    }
    //-----------------------------------------------------------------------
    Nui32 ParticleSpace::getTypeMark() const
    {
        return SpaceManager::ParticleSpaceMark;
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::initSpreadPool()
    {
        // Initialise the pool if needed
        NCount currSize = 0;
        if(mSpreadPool.empty())
        {
            if(mSpreadInit)
            {
                // It was already initialised, but apparently no emitted emitters were used
                return;
            }
            else
            {
                if(mSpreadInit)
                    return;

                // Run through mSpreadList and add keys to the pool
                SpreadVector::iterator emitterIterator;
                SpreadVector::iterator emitterIteratorInner;
                SpreadParticle * emitter = 0;
                SpreadParticle * emitterInner = 0;
                for(emitterIterator = mSpreadList.begin(); emitterIterator != mSpreadList.end(); ++emitterIterator)
                {
                    // Determine the names of all emitters that are emitted
                    emitter = *emitterIterator ;
                    if(emitter && emitter->getSpreadName() != StrUtil::WBLANK)
                    {
                        // This one will be emitted, register its name and leave the vector empty!
                        SpreadVector empty;
                        mSpreadPool.insert(Npair(emitter->getSpreadName(), empty));
                    }

                    // Determine whether the emitter itself will be emitted and set the 'mAuto' attribute
                    for(emitterIteratorInner = mSpreadList.begin(); emitterIteratorInner != mSpreadList.end(); ++emitterIteratorInner)
                    {
                        emitterInner = *emitterIteratorInner;
                        if(emitter && emitterInner && emitter->getName() != StrUtil::WBLANK &&
                            emitter->getName() == emitterInner->getSpreadName())
                        {
                            emitter->setAuto(true);
                            break;
                        }
                        else
                        {
                            // Set explicitly to 'false' although the default value is already 'false'
                            emitter->setAuto(false);
                        }
                    }
                }
                mSpreadInit = true;
            }
        }
        else
        {
            SpreadPoolMap::iterator i, iend = mSpreadPool.end();
            for(i = mSpreadPool.begin(); i != iend; ++i)
            {
                currSize += i->second.size();
            }
        }

        NCount size = mSpreadMax;
        if(currSize < size && !mSpreadPool.empty())
        {
            // Increase the pool. Equally distribute over all vectors in the map
            resizeSpreadPool(size);

            SpreadVector::iterator svi, sviend;
            String fsmname = StrUtil::WBLANK;

            list<SpreadParticle *>::type * spfee = 0;
            SpreadVector * spreadlist = 0;
            SpreadPoolMap::iterator c, cend = mSpreadPool.end();
            for(c = mSpreadPool.begin(); c != cend; ++c)
            {
                fsmname = c->first;
                spreadlist = &c->second;
                FreeSpreadMap::iterator feei = mFreeSpreadMap.find(fsmname);
                if(feei == mFreeSpreadMap.end())
                {
                    feei = mFreeSpreadMap.insert(Npair(fsmname, SpreadList())).first;
                }
                spfee = &feei->second;
                sviend = spreadlist->end();
                for(svi = spreadlist->begin(); svi != sviend; ++svi)
                {
                    spfee->push_back(*svi);
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::resizeSpreadPool(NCount size)
    {
        // Don't proceed if the pool doesn't contain any keys of emitted emitters
        if(mSpreadPool.empty())
            return;

        SpreadVector::iterator emitterIterator;
        SpreadParticle* emitter = 0;
        SpreadParticle* clonedEmitter = 0;
        String name = StrUtil::WBLANK;
        SpreadVector* e = 0;
        NCount maxNumberOfEmitters = size / mSpreadPool.size(); // equally distribute the number for each emitted emitter list
        NCount oldSize = 0;

        SpreadPoolMap::iterator i, iend = mSpreadPool.end();
        // Run through mSpreadPool and search for every key (=name) its corresponding emitter in mSpreadList
        for(i = mSpreadPool.begin(); i != iend; ++i)
        {
            name = i->first;
            e = &i->second;

            // Search the correct emitter in the mSpreadList vector
            emitter = 0;
            for(emitterIterator = mSpreadList.begin(); emitterIterator != mSpreadList.end(); ++emitterIterator)
            {
                emitter = *emitterIterator;
                if(emitter && name != StrUtil::WBLANK && name == emitter->getName())
                {
                    // Found the right emitter, clone each emitter a number of times
                    oldSize = e->size();
                    for(NCount t = oldSize; t < maxNumberOfEmitters; ++t)
                    {
                        clonedEmitter = N_Only(ParticleSpace).createSpread(emitter->getType(), this);
                        clonedEmitter->copyProperty(emitter);
                        clonedEmitter->setAuto(emitter->isAuto()); // is always 'true' by the way, but just in case

                        // Initially deactivate the emitted emitter if duration/repeat_delay are set
                        if(clonedEmitter->getDuration() > 0.0f &&
                            (clonedEmitter->getRepeatDelay() > 0.0f || clonedEmitter->getMinRepeatDelay() > 0.0f || clonedEmitter->getMinRepeatDelay() > 0.0f))
                            clonedEmitter->setEnable(false);

                        // Add cloned emitters to the pool
                        e->push_back(clonedEmitter);
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ParticleSpace::destorySpreadPool()
    {
        SpreadVector * e = 0;
        SpreadVector::iterator j, jend;
        SpreadPoolMap::iterator i, iend = mSpreadPool.end();
        for(i = mSpreadPool.begin(); i != iend; ++i)
        {
            e = &i->second;
            jend = e->end();
            for (j = e->begin(); j != jend; ++j)
            {
                N_Only(ParticleSpace).destroySpread(*j);
            }
            e->clear();
        }
        // Dont leave any references behind
        mSpreadPool.clear();
        mFreeSpreadMap.clear();
        mActiveSpreadList.clear();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ParticleEffectFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ParticleEffectFactory::ParticleEffectFactory() {}
    //-----------------------------------------------------------------------
    ParticleEffectFactory::~ParticleEffectFactory()
    {
        // Destroy all affectors
        vector<ParticleEffect *>::type::iterator i, iend = mList.end();
        for (i = mList.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mList.clear();
    }
    //-----------------------------------------------------------------------
    void ParticleEffectFactory::destroy(ParticleEffect * e)
    {
        vector<ParticleEffect*>::type::iterator i, iend = mList.end();
        for (i = mList.begin(); i != iend; ++i)
        {
            if ((*i) == e)
            {
                mList.erase(i);
                N_delete e;
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
}