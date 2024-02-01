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
#include "NiiRenderQueue.h"
#include "NiiGeometryObj.h"
#include "NiiMaterialManager.h"
#include "NiiSpaceTypeManager.h"
#include "NiiSpaceObj.h"
#include "NiiShaderFusion.h"
#include "NiiRadixSort.h"
#include "NiiException.h"
#include "NiiRenderPattern.h"

namespace NII
{
    //-----------------------------------------------------------------------
    struct RadixSortCh
    {
        Nui32 _EngineInner operator()(const RenderItem & p) const
        {
            return p.mCh->getIndex();
        }
    };
    //-----------------------------------------------------------------------
    struct RadixSortDistance
    {
        const Camera * mCamera;

        RadixSortDistance(const Camera * cam) : mCamera(cam) {}

        NIIf _EngineInner operator()(const RenderItem & p) const
        {
            return static_cast<NIIf>(-p.mGeometryObj->distanceSquared(mCamera));
        }
    };
    //-----------------------------------------------------------------------
    struct DistanceDescendingLess
    {
        const Camera * mCamera;

        DistanceDescendingLess(const Camera * cam) : mCamera(cam) {}

        bool _EngineInner operator()(const RenderItem & a, const RenderItem & b) const
        {
            if (a.mGeometryObj == b.mGeometryObj)
            {
                return a.mCh->getIndex() < b.mCh->getIndex();
            }
            else
            {
                NIIf adepth = a.mGeometryObj->distanceSquared(mCamera);
                NIIf bdepth = b.mGeometryObj->distanceSquared(mCamera);
                if (Math::isEqual(adepth, bdepth))
                {
                    return a.mCh < b.mCh;
                }
                else
                {
                    return (adepth > bdepth);
                }
            }
        }
    };
    //-----------------------------------------------------------------------
    static RadixSort<RenderItemList, RenderItem, Nui32> _rlgRadixSort1;
    static RadixSort<RenderItemList, RenderItem, NIIf> _rlgRadixSort2;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Group
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    RenderLevelGroup::Group::Group() :
        mSortMark(0)
    {
    }
    //-----------------------------------------------------------------------
    RenderLevelGroup::Group::~Group()
    {
		RenderItemChList::iterator i, iend = mChRenderList.end();
		for(i = mChRenderList.begin(); i != iend, ++i)
		{
			N_delete_t(i->second, RenderItemList);
		}
        mChRenderList.clear();
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::add(SpaceObj * sobj, GeometryObj * gobj, ShaderCh * ch, bool alpha)
    {
        if((mSortMark & RSM_View) && alpha)
        {
            mRenderList.push_back(RenderItem(sobj, gobj, ch));
        }

        if(mSortMark & RSM_Material)
        {
            RenderItemChList::iterator i = mChRenderList.find(ch);
            if(i == mChRenderList.end())
            {
                std::pair<RenderItemChList::iterator, bool> re = 
                    mChRenderList.insert(RenderItemChList::value_type(ch, N_new_t(RenderItemList)()));
                N_assert1(re.second);
                i = re.first;
            }

            i->second->push_back(RenderItem(sobj, gobj, i->first));
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf)
    {
        if((mSortMark & RSM_View) && sf->isDepthSort())
        {
            ShaderChList::iterator i, iend = sf->getShaderChList().end();
            for (i = sf->getShaderChList().begin(); i != iend; ++i)
            {
                mRenderList.push_back(RenderItem(sobj, gobj, *i));
            }
        }
        if(mSortMark & RSM_Material)
        {
            ShaderChList::iterator i, iend = sf->getShaderChList().end();
            for(i = sf->getShaderChList().begin(); i != iend; ++i)
            {
                RenderItemChList::iterator j = mChRenderList.find(*i);
                if(j == mChRenderList.end())
                {
                    std::pair<RenderItemChList::iterator, bool> re = 
                        mChRenderList.insert(RenderItemChList::value_type(*i, N_new_t(RenderItemList)()));
                    N_assert1(re.second);
                    j = re.first;
                }
                j->second->push_back(RenderItem(sobj, gobj, *i));
            }
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::remove(ShaderCh * sch)
    {
        RenderItemChList::iterator i = mChRenderList.find(sch);
        if (i != mChRenderList.end())
        {
            N_delete_t(i->second, RenderItemList);
            mChRenderList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::clear()
    {
        RenderItemChList::iterator i, iend = mChRenderList.end();
        for (i = mChRenderList.begin(); i != iend; ++i)
        {
            N_delete_t(i->second, RenderItemList);
        }
        mChRenderList.clear();
        mRenderList.clear();
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::sort(const Camera * cam)
    {
		if (mSortMark & RSM_View)
		{
			if (mRenderList.size() > 2000)
			{
				_rlgRadixSort1.sort(mRenderList, RadixSortCh());
				_rlgRadixSort2.sort(mRenderList, RadixSortDistance(cam));
			}
			else if(mSortMark & RSM_Stable_View)
			{
				std::stable_sort(mRenderList.begin(), mRenderList.end(), DistanceDescendingLess(cam));
			}
			else
			{
				std::sort(mRenderList.begin(), mRenderList.end(), DistanceDescendingLess(cam));
			}
		}
        if(mSortMark & RSM_Material_View)
        {
            RenderItemChList::iterator i, iend = mChRenderList.end();
            for(i = mChRenderList.begin(); i != iend; ++i)
            {
                RenderItemList & temp = *i->second;

                if(temp.size() > 2000)
                {
                    _rlgRadixSort1.sort(temp, RadixSortCh());
                    _rlgRadixSort2.sort(temp, RadixSortDistance(cam));
                }
                else if(mSortMark & RSM_Stable_Material_View)
                {
                    std::stable_sort(temp.begin(), temp.end(), DistanceDescendingLess(cam));
                }
                else
                {
                    std::sort(temp.begin(), temp.end(), DistanceDescendingLess(cam));
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::render(GroupSortType gst, RenderFilter * filter) const
    {
        if(gst & mSortMark)
        {
            switch(gst)
            {
            case GST_Material:
            case GST_MaterialDescend:
            {
                RenderItemChList::const_iterator i, iend = mChRenderList.end();
                for (i = mChRenderList.begin(); i != iend; ++i)
                {
                    const RenderItemList & temp = *(i->second);
                    if (temp.size())
                    {
                        filter->render(temp, false);
                    }
                }
            }
            break;
            case GST_MaterialAscend:
            {
                RenderItemChList::const_iterator i, iend = mChRenderList.end();
                for (i = mChRenderList.begin(); i != iend; ++i)
                {
                    const RenderItemList & temp = *(i->second);
                    if (temp.size())
                    {
                        filter->render(temp, true);
                    }
                }
            }
            break;
            case GST_View:
            case GST_ViewDescend:
            {
                RenderItemList::const_iterator i, iend = mRenderList.end();
                for (i = mRenderList.begin(); i != iend; ++i)
                {
                    filter->render(i->mSpaceObj, i->mGeometryObj, i->mCh);
                }
            }
            break;
            case GST_ViewAscend:
            {
                RenderItemList::const_reverse_iterator i, iend = mRenderList.rend();
                for (i = mRenderList.rbegin(); i != iend; ++i)
                {
                    filter->render(i->mSpaceObj, i->mGeometryObj, i->mCh);
                }
            }
            break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::Group::merge(const Group & o)
    {
        mRenderList.insert(mRenderList.end(), o.mRenderList.begin(), o.mRenderList.end());

        RenderItemChList::const_iterator srcG, endG = o.mChRenderList.end();
        for (srcG = o.mChRenderList.begin(); srcG != endG; ++srcG)
        {
            RenderItemChList::iterator dstG = mChRenderList.find(srcG->first);
            if (dstG == mChRenderList.end())
            {
                std::pair<RenderItemChList::iterator, bool> re = 
                    mChRenderList.insert(RenderItemChList::value_type(srcG->first, N_new_t(RenderItemList)()));

                N_assert1(re.second);
                dstG = re.first;
            }
            RenderItemList * dst = dstG->second;
            RenderItemList * src = srcG->second;
            dst->insert(dst->end(), src->begin(), src->end());
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderLevelGroup
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    RenderLevelGroup::RenderLevelGroup(RenderGroup * parent, RenderCommandGroup * dcg) :
        mParent(parent),
        mDrawCallList(dcg)
    {
        mBasic.mSortMark = RSM_Material;
        mAlpha.mSortMark = RSM_Material | RSM_View;
    }
    //-----------------------------------------------------------------------
    RenderLevelGroup::~RenderLevelGroup()
    {
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::render(SortType st, RenderFilter * filter) const
    {
        switch (st)
        {
        case RST_BasicMaterial:
            mBasic.render(GST_Material, filter);
            break;
        case RST_BasicMaterialDescend:
            mBasic.render(GST_MaterialDescend, filter);
            break;
        case RST_BasicMaterialAscend:
            mBasic.render(GST_MaterialAscend, filter);
            break;
        case RST_BasicView:
            mBasic.render(GST_View, filter);
            break;
        case RST_BasicViewDescend:
            mBasic.render(GST_ViewDescend, filter);
            break;
        case RST_BasicViewAscend:
            mBasic.render(GST_ViewAscend, filter);
            break;
		case RST_AlphaMaterial:
            mAlpha.render(GST_Material, filter);
            break;
        case RST_AlphaMaterialDescend:
            mAlpha.render(GST_MaterialDescend, filter);
            break;
        case RST_AlphaMaterialAscend:
            mAlpha.render(GST_MaterialAscend, filter);
            break;
        case RST_AlphaView:
            mAlpha.render(GST_View, filter);
            break;
		case RST_AlphaViewDescend:
            mAlpha.render(GST_ViewDescend, filter);
            break;
        case RST_AlphaViewAscend
            mAlpha.render(GST_ViewAscend, filter);
            break;
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::addSort(RenderSortMode om)
    {
		mBasic.addSort(om);
		mAlpha.addSort(om);
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::removeSort(RenderSortMode om)
    {
		mBasic.removeSort(om);
		mAlpha.removeSort(om);
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::resetSort()
    {
        removeSort(RSM_All);
        addSort(RSM_Material);
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf)
    {
        if (sf->isForceDepthSort() || (sf->isTransparent() &&
            (!sf->isDepthWrite() || !sf->isDepthEnable() || (sf->getColourWrite() != 0))))
        {
            mAlpha.add(sobj, gobj, sf);
        }
        else
        {
            mBasic.add(sobj, gobj, sf);
        }
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::remove(ShaderCh * ch)
    {
        mBasic.remove(ch);
        mAlpha.remove(ch);
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::clear()
    {
        mBasic.clear();
        mAlpha.clear();
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::sort(const Camera * cam)
    {
		mBasic.sort(cam);
		mAlpha.sort(cam);
    }
    //-----------------------------------------------------------------------
    void RenderLevelGroup::merge(const RenderLevelGroup * o)
    {
        mBasic.merge(o->mBasic);
        mAlpha.merge(o->mAlpha);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderGroup
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    RenderGroup::RenderGroup(RenderQueue * parent) :
        mParent(parent),
        mSortMark(0),
		mRenderType(RT_QueueArray),
        mShadowsEnable(true)
    {
    }
    //-----------------------------------------------------------------------
    RenderGroup::~RenderGroup()
    {
        LevelList::iterator i, iend = mLevelList.end();
        for(i = mLevelList.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf, Nui16 level)
    {
        RenderLevelGroup * rlg;
        LevelList::iterator i = mLevelList.find(level);
        if (i == mLevelList.end())
        {
            createImpl(rlg);
            if (mSortMark)
            {
                rlg->removeSort(RSM_All);
                rlg->addSort((RenderSortMode)mSortMark);
            }

            mLevelList.insert(LevelList::value_type(level, rlg));
        }
        else
        {
            rlg = i->second;
        }

        rlg->add(sobj, gobj, sf);
    }
    //-----------------------------------------------------------------------
    void RenderGroup::clear(bool destroy)
    {
        LevelList::iterator i, iend = mLevelList.end();
        for (i = mLevelList.begin(); i != iend; ++i)
        {
            if(destroy)
                N_delete i->second;
            else
                i->second->clear();
        }
        mSortMark &= ~(RSM_Sort_View_OK | RSM_Sort_Material_OK | RSM_Sort_Material_View_OK);

        if (destroy)
            mLevelList.clear();
    }
    //-----------------------------------------------------------------------
    void RenderGroup::setLevel(NCount cnt)
    {
        mLevelList.rsize(cnt);
    }
    //-----------------------------------------------------------------------
    void RenderGroup::addSort(RenderSortMode om)
    {
        mSortMark |= om;

        LevelList::iterator i, iend = mLevelList.end();
        for (i = mLevelList.begin(); i != iend; ++i)
        {
            i->second->addSort(om);
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::addSort(Nui16 level, RenderSortMode om)
    {
        mSortMark |= om;

        LevelList::iterator i = mLevelList.find(level);
        if (i != mLevelList.end())
        {
            i->second->addSort(om);
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::removeSort(RenderSortMode om)
    {
        mSortMark = 0;

        LevelList::iterator i, iend = mLevelList.end();
        for (i = mLevelList.begin(); i != iend; ++i)
        {
            i->second->removeSort(om);
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::removeSort(Nui16 level, RenderSortMode om)
    {
        LevelList::iterator i = mLevelList.find(level);
        if (i != mLevelList.end())
        {
            i->second->removeSort(om);
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::resetSort()
    {
        mSortMark = 0;

        LevelList::iterator i, iend = mLevelList.end();
        for (i = mLevelList.begin(); i != iend; ++i)
        {
            i->second->resetSort();
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::merge(const RenderGroup * o)
    {
        LevelList::const_iterator j, jend = o->mLevelList.end();
        for (j = o->mLevelList.begin(); j != jend; ++j)
        {
            Nui16 priority = j->first;
            RenderLevelGroup * src = j->second;
            RenderLevelGroup * dst;

            LevelList::iterator i = mLevelList.find(priority);
            if (i == mLevelList.end())
            {
                createImpl(dst);

                if(mSortMark)
                {
                    dst->removeSort(RSM_All);
                    dst->addSort((RenderSortMode)mSortMark);
                }

                mLevelList.insert(LevelList::value_type(priority, dst));
            }
            else
            {
                dst = i->second;
            }

            dst->merge(src);
        }
    }
    //-----------------------------------------------------------------------
    void RenderGroup::createImpl(RenderLevelGroup *& out)
    {
        out = N_new RenderLevelGroup(this);
    }
	//---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // RenderQueue
    //---------------------------------------------------------------------
	//---------------------------------------------------------------------
    class RqBits
    {
    public:
        static const int SubRqIdBits		    = 3;
        static const int TransparencyBits	    = 1;
        static const int MacroblockBits		    = 10;
        static const int ShaderBits			    = 10; //The higher 3 bits contain RenderPatternType
        static const int MeshBits			    = 14;
        static const int TextureBits		    = 11;
        static const int DepthBits			    = 15;

        static const int SubRqIdShift           = 64 - SubRqIdBits; 				    //61
        static const int TransparencyShift      = SubRqIdShift - TransparencyBits; 	    //60
        static const int MacroblockShift        = TransparencyShift - MacroblockBits;   //50
        static const int ShaderShift            = MacroblockShift   - ShaderBits; 	    //40
        static const int MeshShift              = ShaderShift       - MeshBits; 	    //26
        static const int TextureShift           = MeshShift         - TextureBits; 	    //15
        static const int DepthShift             = TextureShift      - DepthBits; 	    //0

        static const int DepthShiftTransp       = TransparencyShift - DepthBits; 	    //45
        static const int MacroblockShiftTransp  = DepthShiftTransp  - MacroblockBits;   //35
        static const int ShaderShiftTransp      = MacroblockShiftTransp - ShaderBits;   //25
        static const int MeshShiftTransp	    = ShaderShiftTransp - MeshBits;		    //11
        static const int TextureShiftTransp	    = MeshShiftTransp   - TextureBits;	    //0
    };
	//---------------------------------------------------------------------
    AtomicScalar<uint32> GeometryRaw::MeshIndexId(0);
	//---------------------------------------------------------------------
    const RenderStateCache c_dummyCache(RenderStateObject(), 0, 0);
    //---------------------------------------------------------------------
    RenderQueue::RenderQueue(RenderCommandGroup * dcg):
        mDrawCallList(dcg),
        mDefaultGroup(RQG_User),
        mDefaultLevel(N_Geo_Default_Level),
        mVaoId(0),
        mVertexData(0),
        mIndexData(0),
        mLastTextureHash(0),
        mRenderMark(0),
        mShadowType(false),
        mRemoveGroupOnClear(false)
    {
		mStateList.resize(RPT_Count);
    }
    //---------------------------------------------------------------------
    RenderQueue::~RenderQueue()
    {
        N_Only(Material).recover();

        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            N_delete i->second;
        }
        mGroups.clear();

        N_assert1(mUsedBufferList.empty());

        IndirectBufferList::const_iterator itor = mFreeBufferList.begin();
        IndirectBufferList::const_iterator end  = mFreeBufferList.end();
        while(itor != end)
        {
            IndirectBuffer * ib = *itor;
            if(ib->isMapping())
                ib->unlock(UT_UnlockMap);
            N_Only(GBuffer)->destroy(ib);
            ++itor;
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::prepareState(ShadowType type)
    {
        ++mRenderMark;
        //N_EnginePtr()->beginFrame();

        //for(size_t i = 0; i < RPT_Count; ++i)
        //{
        RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, mRenderPatternType);
        if(rp)
        {
            mStateList[i] = rp->createState(type);
        }
        //}
    }
    //-----------------------------------------------------------------------
    void RenderQueue::setRenderType(RenderGroupType rgt, RenderType rtype)
    {
        GroupList::iterator i = mGroups.find(rgt);
        if(i != mGroups.end())
        {
            i->second->setRenderType(rtype);
        }
    }
    //-----------------------------------------------------------------------
    RenderType RenderQueue::getRenderType(RenderGroupType rgt) const
    {
        GroupList::iterator i = mGroups.find(rgt);
        if(i != mGroups.end())
        {
            return i->second->getRenderType();
        }
        return RT_Unknow; 
    }
    //-----------------------------------------------------------------------
    void RenderQueue::removeSort(RenderSortMode rsm)
    {
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            i->second->removeSort(rsm);
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::removeSort(RenderGroupType rgt, RenderSortMode rsm)
    {
        GroupList::iterator i = mGroups.find(rgt);
        if(i != mGroups.end())
        {
            i->second->removeSort(rsm);
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::addSort(RenderSortMode rsm)
    {
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            i->second->addSort(rsm);
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::addSort(RenderGroupType rgt, RenderSortMode rsm)
    {
        GroupList::iterator i = mGroups.find(rgt);
        if(i != mGroups.end())
        {
            i->second->addSort(rsm);
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::resetSort()
    {
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            i->second->resetSort();
        }
    }
    //-----------------------------------------------------------------------
    void RenderQueue::clear()
    {
        SpaceTypeManager::SpaceList::iterator i, iend = N_Only(SpaceType).getSpaceList().end();

        for(i = N_Only(SpaceType).getSpaceList().begin(); i != iend; ++i)
        {
            RenderQueue * queue = i->second->getRenderQueue();

            GroupList::iterator j, iend = queue->mGroups.end();
            for(j = queue->mGroups.begin(); j != iend; ++j)
            {
                if(j->second != 0)
                    j->second->clear(mRemoveGroupOnClear);
            }
        }

        N_Only(Material).recover();
    }
    //-----------------------------------------------------------------------
    void RenderQueue::clearState()
    {
        mShadowType = false;
        mVaoId = 0;
        mVertexData = 0;
        mIndexData  = 0;
        mLastTextureHash = 0;
    }
    //-----------------------------------------------------------------------
    void RenderQueue::add(SpaceObj * sobj, GeometryObj * gobj, RenderGroupType rgt, Nui16 rlg, ShadowType stype)
    {
        RenderGroup * rg = getGroup(rgt);

        ShaderFusion * sf;

        if(gobj->getMaterial() != 0)
            const_cast<Material *>(gobj->getMaterial())->ref();

        if((gobj->getMaterial() == 0) || !gobj->getShaderFusion())
        {
            sf = const_cast<Material *>(MaterialManager::WhiteLight)->get(0);
        }
        else
        {
            sf = gobj->getShaderFusion();
        }

        rg->add(sobj, gobj, sf, rlg);
    }
    //---------------------------------------------------------------------
    void RenderQueue::add(SpaceObj * sobj, GeometryObj * gobj, RenderGroupType rgt, Nui16 rlg, ShadowType stype)
    {
        assert(rgt == sobj->getRenderQueueGroup());

        uint8 subId = gobj->getRenderQueueSubGroup();
        RealAsUint depth = sobj->getCachedDistanceToCamera();

        assert(!(mGroups[rgt].mSortMark & RSM_Sort_OK) && "Called add after render and before clear" );

        assert(subId < N_Mask(RqBits::SubRqIdBits));

        uint32 hlmsHash = stype ? gobj->getCasterHash() : gobj->getNormalHash();
        const ShaderChMaterial * mat = gobj->getMaterial();

        const bool transparent = mat->mBlend[stype]->isTransparent();

        uint16 stencil = mat->mHash[stype];
        uint16 texturehash = mat->mHash[3];

        //Flip the float to deal with negative & positive numbers
#if OGRE_DOUBLE_PRECISION == 0
        RealAsUint mask = -int(depth >> 31) | 0x80000000; ///???
        depth = (depth ^ mask);
#else
        RealAsUint mask = -int64(depth >> 63) | 0x8000000000000000; ///??? 
        depth = (depth ^ mask) >> 32;
#endif
        uint32 quantizedDepth = static_cast<uint32>(depth) >> (32 - RqBits::DepthBits);

        uint32 meshHash;

        if(isV1)
        {
            GeometryRaw op;
            gobj->getGeometry(op, stype & (mat->getAlphaTest() == CMPF_ALWAYS_PASS));
            meshHash = op.meshIndex;
        }
        else
        {
            uint8 meshLod = sobj->getCurrentMeshLod();
            const GeometryRawList & vaos = gobj->getVaos(static_cast<VertexPass>(stype));

            assert( meshLod < vaos.size() && "Vaos meshLod/shadowLod not set. "
                    "Note: If this is a v1 object, it is in the wrong RenderQueue ID "
                    "(or the queue incorrectly set)." );

            GeometryRaw * vao = vaos[meshLod];
            meshHash = vao->getRenderID();
        }
        //TODO: Account for skeletal animation in any of the hashes (preferently on the material side)
        //TODO: Account for auto instancing animation in any of the hashes

        uint64 hash;
        if(transparent)
        {
            //Transparent objects are sorted by depth back to front, then by material, then by mesh.
            quantizedDepth = quantizedDepth ^ 0xffffffff;
            hash =
                N_Mask_Num( subId,            RqBits::SubRqIdBits,        RqBits::SubRqIdShift )          |
                N_Mask_Num( transparent,      RqBits::TransparencyBits,   RqBits::TransparencyShift )     |
                N_Mask_Num( quantizedDepth,   RqBits::DepthBits,          RqBits::DepthShiftTransp )      |
                N_Mask_Num( stencil,          RqBits::MacroblockBits,     RqBits::MacroblockShiftTransp ) |
                N_Mask_Num( hlmsHash,         RqBits::ShaderBits,         RqBits::ShaderShiftTransp )     |
                N_Mask_Num( meshHash,         RqBits::MeshBits,           RqBits::MeshShiftTransp );
        }
        else
        {
            //Opaque objects are first sorted by material, then by mesh, then by depth front to back.
            hash =
                N_Mask_Num(subId,            RqBits::SubRqIdBits,        RqBits::SubRqIdShift)      |
                N_Mask_Num(transparent,      RqBits::TransparencyBits,   RqBits::TransparencyShift) |
                N_Mask_Num(stencil,          RqBits::MacroblockBits,     RqBits::MacroblockShift)   |
                N_Mask_Num(hlmsHash,         RqBits::ShaderBits,         RqBits::ShaderShift)       |
                N_Mask_Num(meshHash,         RqBits::MeshBits,           RqBits::MeshShift)         |
                N_Mask_Num(texturehash,      RqBits::TextureBits,        RqBits::TextureShift)      |
                N_Mask_Num(quantizedDepth,   RqBits::DepthBits,          RqBits::DepthShift);
        }

        mGroups[rgt].mLevelList[rlg].mBasic.mRenderList.push_back(RenderItem(hash, gobj, sobj));
    }
    //-----------------------------------------------------------------------
    RenderGroup * RenderQueue::getGroup(RenderGroupType gid)
    {
        RenderGroup * grp;
        if(mGroups[gid].second == 0)
        {
            createImpl(grp);
            mGroups[gid].second = grp;
        }
        else
        {
            grp = mGroups[gid].second;
        }
        return grp;
    }
    //-----------------------------------------------------------------------
    void RenderQueue::frameBegin()
    {
        
    }
    //-----------------------------------------------------------------------
    void RenderQueue::frameEnded()
    {
        N_assert1(mRenderMark == 0u);

        mFreeBufferList.insert(mFreeBufferList.end(), mUsedBufferList.begin(), mUsedBufferList.end());
        mUsedBufferList.clear();
    }
    //-----------------------------------------------------------------------
    void RenderQueue::merge(const RenderQueue * o)
    {
        GroupList::const_iterator i, iend = o->mGroups.end();
        for(i = o->mGroups.begin(); i != iend; ++i)
        {
            if(i->second != 0)
            {
                getGroup(i->first)->merge(i->second);
            }
        }
    }
    //-----------------------------------------------------------------------
    IndirectBuffer * RenderQueue::getIndirectBuffer(NCount cnt)
    {
        size_t rsize = cnt * sizeof(IndexIndirect);

        IndirectBufferList::iterator itor = mFreeBufferList.begin();
        IndirectBufferList::iterator end  = mFreeBufferList.end();

        size_t sbufsize = std::numeric_limits<size_t>::max();
        IndirectBufferList::iterator siter = end;

        while(itor != end)
        {
            size_t bsize = (*itor)->getSize();
            if(rsize <= bsize && sbufsize > bsize)
            {
                siter = itor;
                sbufsize = bsize;
            }

            ++itor;
        }

        if(siter == end)
        {
			IndirectBuffer * renew;
			N_Only(GBuffer)->create(renew, IndirectBuffer::T_Index, rsize, M_Normal | M_PersistentMap, 0, false);
            mFreeBufferList.push_back(renew);
            siter = mFreeBufferList.end() - 1;
        }

        IndirectBuffer * re = *siter;

        mUsedBufferList.push_back(*siter);
        remove(mFreeBufferList, siter);

        return re;
    }
    //--------------------------------------------------------------------
    void RenderQueue::createImpl(RenderGroup *& out)
    {
        out = N_new RenderGroup(this);
    }
    //--------------------------------------------------------------------
    void RenderQueue::init()
    {
        if(mGroups[RQG_User].second == 0)
        {
            mGroups[RQG_User].second = N_new RenderGroup(this);
            mGroups[RQG_User].setLevel(mag->getLevelCount());
            mGroups[RQG_User].setRenderType(RT_Queue);
        }
    }
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    // RenderGroupFusion
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    RenderGroupFusion::RenderGroupFusion(RenderGroupType gid, const String & name) :
        mName(name),
        mRenderGroup(gid),
        mShadowFeture(SF_Additive_Texture),
        mShaderCh(0)
    {
    }
    //--------------------------------------------------------------------
    //inline void setShadowEnable(bool b)             { mShadowEnable = b; }
    //inline bool isShadowEnable() const              { return mShadowEnable; }
    //--------------------------------------------------------------------
    void RenderGroupFusion::render(RenderGroup * group, RenderPattern * rp, SpaceManager * sm) const
    {
        bool oldShadows = rp->isShadowEnable();
        bool oldRSChanges = sm->isLockShaderCh();

        rp->setShadowEnable(mShadowFeture != SF_None);
        sm->setLockShaderCh(mShaderCh == 0);

        renderImpl(group, rp);

        rp->setShadowEnable(oldShadows);
        sm->setLockShaderCh(oldRSChanges);
    }
    //--------------------------------------------------------------------
    void RenderGroupFusion::renderImpl(RenderGroup * group, RenderPattern * rp) const
    {
        rp->render(group);
    }
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    // RenderQueueFusion
    //--------------------------------------------------------------------
    //--------------------------------------------------------------------
    RenderQueueFusion::RenderQueueFusion(Nid id) :
        mID(id)
    {
    }
    //--------------------------------------------------------------------
    RenderQueueFusion::~RenderQueueFusion()
    {
        removeAll();
    }
    //--------------------------------------------------------------------
    RenderGroupFusion * RenderQueueFusion::add(RenderGroupType qid, const String & name)
    {
        RenderGroupFusion * re = N_new RenderGroupFusion(qid, name);
        mList.push_back(re);
        return re;
    }
    //---------------------------------------------------------------------
    void RenderQueueFusion::removeAll()
    {
        RenderGroupFusionList::iterator i, iend = mList.end();
        for (i = mList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mList.clear();
    }
    //---------------------------------------------------------------------
    void RenderQueueFusion::remove(Nidx index)
    {
        N_assert(index < getCount(), "Index out of bounds");

        RenderGroupFusionList::iterator i = mList.begin();
        std::advance(i, index);
        N_delete *i;
        mList.erase(i);
    }
    //-----------------------------------------------------------------------
    void RenderQueue::render(RenderGroupType firstRq, RenderGroupType lastRq, ShadowType stype)
    {
        if(mShadowType != stype)
        {
            clearState();
            mShadowType = stype;
        }

        //OgreProfileBeginGroup("Command Preparation", OGREPROF_RENDERING);

        mRenderSys->setRenderCount(1);

        size_t numNeededDraws = 0;
        for(size_t i = firstRq; i < lastRq; ++i)
        {
            RenderGroup & temp = mGroups[i];
            if(temp.mRenderType == RT_QueueArray)
            {
                LevelList::const_iterator itor = temp.mLevelList.begin();
                LevelList::const_iterator end  = temp.mLevelList.end();

                while(itor != end)
                {
                    numNeededDraws += itor->mBasic.mRenderList.size();
                    ++itor;
                }
            }
        }

        mDrawCallList->setRender(mRenderSys);

        bool indirectValid = N_Only(GBuffer)->supportsIndirectBuffers();

        IndirectBuffer * ibuffer = 0;
        unsigned char * indirectDraw = 0;
        unsigned char * startIndirectDraw = 0;

        if(numNeededDraws > 0)
        {
            ibuffer = getIndirectBuffer(numNeededDraws);

            if(indirectValid)
            {
                indirectDraw = static_cast<unsigned char *>(ibuffer->map(0, ibuffer->getUnitCount()));
            }
            else
            {
                indirectDraw = ibuffer->getSwBufferPtr();
            }

            startIndirectDraw = indirectDraw;
        }

        for(size_t i = firstRq; i < lastRq; ++i)
        {
            RenderGroup & temp = mGroups[i];
            RenderItemList & queuedRenderables = temp.mLevelList[0];
            LevelList & perThreadQueue = temp.mLevelList;

            if(!(temp.mSortMark & RSM_Sort_OK))
            {
                //OgreProfileGroupAggregate("Sorting", OGREPROF_RENDERING);

                size_t numRenderables = 0;
                LevelList::const_iterator itor, end  = perThreadQueue.end();
                itor = perThreadQueue.begin();

                while(itor != end)
                {
                    numRenderables += itor->mBasic.mRenderList.size();
                    ++itor;
                }

                queuedRenderables.reserve(numRenderables);

                itor = perThreadQueue.begin();
                while(itor != end)
                {
                    queuedRenderables.append(itor->mBasic.mRenderList.begin(), itor->mBasic.mRenderList.end());
                    ++itor;
                }

                //TODO: Exploit temporal coherence across frames then use insertion sorts.
                //As explained by L. Spiro in
                //http://www.gamedev.net/topic/661114-temporal-coherence-and-render-queue-sorting/?view=findpost&p=5181408
                //Keep a list of sorted indices from the previous frame (one per camera).
                //If we have the sorted list "5, 1, 4, 3, 2, 0":
                //  * If it grew from last frame, append: 5, 1, 4, 3, 2, 0, 6, 7 and use insertion sort.
                //  * If it's the same, leave it as is, and use insertion sort just in case.
                //  * If it's shorter, reset the indices 0, 1, 2, 3, 4; probably use quicksort or other generic sort
                //
                //TODO2: Explore sorting first on multiple threads, then merge sort into one.
                if(temp.mSortMark & RSM_Stable_View)
                {
					std::stable_sort(queuedRenderables.begin(), queuedRenderables.end());
                }
                else
                {
                    std::sort(queuedRenderables.begin(), queuedRenderables.end());
                }
                temp.mSortMark |= RSM_Sort_OK;
            }

            if(temp.mRenderType == RT_Single)
            {
                if(mVaoId)
                {
                    //mRenderSys->_startLegacyV1Rendering();
                    mVaoId = 0;
                }
                VertexData const * lastVertexData = mVertexData;
                IndexData const * lastIndexData = mIndexData;
                RenderStateCache const * lastRSC = &c_dummyCache;
                uint32 lastHlmsCacheHash = 0;
                uint32 lastTextureHash = mLastTextureHash;
                //uint32 lastVertexDataId = ~0;

                const RenderItemList & queuedRenderables = temp.mLevelList[0];

                RenderItemList::const_iterator itor = queuedRenderables.begin();
                RenderItemList::const_iterator end  = queuedRenderables.end();

                while(itor != end)
                {
                    const RenderItem & renderItem = *itor;
                    /*uint32 hlmsHash = stype ? renderItem.mGeometryObj->getCasterHash() :
                                                   renderItem.mGeometryObj->getNormalHash();*/
                    const ShaderChMaterial * mat = renderItem.mGeometryObj->getMaterial();
                    GeometryRaw op;
                    renderItem.mGeometryObj->getGeometry(op, stype & (mat->getAlphaTest() == CMPF_ALWAYS_PASS));

                    if(lastVertexData != op.mVertex)
                    {
                        lastVertexData = op.mVertex;
                    }
                    if(lastIndexData != op.mIndex)
                    {
                        lastIndexData = op.mIndex;
                    }

                    RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, static_cast<RenderPatternType>(mat->mType));

                    lastHlmsCacheHash = lastRSC->mHash;
                    const RenderStateCache * rsc = rp->getCache(lastRSC->mHash, mStateList[mat->mType], renderItem, stype);

                    if(lastHlmsCacheHash != rsc->mHash)
                    {
                        mRenderSys->setRSO(&rsc->mRSO);
                        lastRSC = rsc;
                    }

                    lastTextureHash = rp->fillBuffersFor(rsc, renderItem, stype, lastHlmsCacheHash, lastTextureHash);

                    const v1::GeometryRaw cmd(op);
                    mRenderSys->_setRenderOperation(&cmd);

                    mRenderSys->_render(op);

                    ++itor;
                }

                mVertexData = lastVertexData;
                mIndexData = lastIndexData;
                mLastTextureHash = lastTextureHash;
            }
            else if(temp.mRenderType == RT_Queue)
            {
                if(mVaoId)
                {
                    *mDrawCallList->addCommand<v1::CbStartV1LegacyRendering>() = v1::CbStartV1LegacyRendering();
                    mVaoId = 0;
                }
                GeometryRaw lastRenderOp;
                RenderStateCache const * lastRSC = &c_dummyCache;
                uint32 lastHlmsCacheHash = 0;

                const bool supportsBaseInstance = N_Only(GBuffer)->supportsBaseInstance();

                const bool instancedStereo = mSceneManager->isUsingInstancedStereo();
                const uint32 instancesPerDraw = instancedStereo ? 2u : 1u;
                const uint32 baseInstanceShift = instancedStereo ? 1u : 0u;

                uint32 insCnt = instancesPerDraw;

                v1::CbDrawCall * drawCmd = 0;

                RenderSys::DrawRecord stats;

                const RenderItemList & queuedRenderables = temp.mLevelList[0];

                RenderItemList::const_iterator itor = queuedRenderables.begin();
                RenderItemList::const_iterator end  = queuedRenderables.end();

                while( itor != end )
                {
                    const RenderItem & renderItem = *itor;

                    const ShaderChMaterial * mat = renderItem.mGeometryObj->getMaterial();

                    GeometryRaw renderOp;
                    renderItem.mGeometryObj->getGeometry(renderOp, stype & (mat->getAlphaTest() == CMPF_ALWAYS_PASS));

                    RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, static_cast<RenderPatternType>(mat->mType));

                    lastHlmsCacheHash = lastRSC->mHash;
                    const RenderStateCache * rsc = rp->getCache(lastRSC->mHash, mStateList[mat->mType], renderItem, stype);

                    if(lastRSC != rsc)
                    {
                        CbPipelineStateObject * psoCmd = mDrawCallList->addCommand<CbPipelineStateObject>();
                        *psoCmd = CbPipelineStateObject(&rsc->mRSO);
                        lastRSC = rsc;

                        //Flush the RenderOp when changing shaders. Needed by D3D11/12 & possibly Vulkan
                        lastRenderOp.mVertex = 0;
                        lastRenderOp.mIndex  = 0;
                    }

                    uint32 baseInstance = rp->fillBuffersForV1(rsc, renderItem, stype, lastHlmsCacheHash, mDrawCallList);

                    bool differentRenderOp = lastRenderOp.mVertex != renderOp.mVertex ||
                        lastRenderOp.mIndex != renderOp.mIndex ||
                        lastRenderOp.mType != renderOp.mType ||
                        lastRenderOp.useGlobalInstancingVertexBufferIsAvailable !=
                        renderOp.useGlobalInstancingVertexBufferIsAvailable;

                    if(drawCmd != mDrawCallList->getLastCommand() || differentRenderOp || renderOp.mInstanceCount != 1)
                    {
                        //Different mesh, vertex buffers or layout. If instanced, entities
                        //likely use their own low level materials. Make a new draw call.
                        //(or also the the RenderPattern made a batch-breaking command)

                        if(differentRenderOp)
                        {
                            *mDrawCallList->addCommand<v1::GeometryRaw>() = v1::GeometryRaw(renderOp);
                            lastRenderOp = renderOp;
                        }

                        if(renderOp.useIndexes)
                        {
                            v1::CbDrawCallIndexed * drawCall = mDrawCallList->addCommand<v1::CbDrawCallIndexed>();
                            *drawCall = v1::CbDrawCallIndexed(supportsBaseInstance);

                            /*drawCall->useGlobalInstancingVertexBufferIsAvailable =
                                    renderOp.useGlobalInstancingVertexBufferIsAvailable;*/
                            drawCall->mCount         = renderOp.mIndex->mCount;
                            drawCall->mInstanceCount = instancesPerDraw;
                            drawCall->mOft           = renderOp.mIndex->mOffset;
                            drawCall->mInstanceOft   = mInstanceOft << baseInstanceShift;

                            insCnt = instancesPerDraw;

                            drawCmd = drawCall;
                        }
                        else
                        {
                            v1::CbDrawCallStrip * drawCall = mDrawCallList->addCommand<v1::CbDrawCallStrip>();
                            *drawCall = v1::CbDrawCallStrip(supportsBaseInstance);

                            /*drawCall->useGlobalInstancingVertexBufferIsAvailable =
                                    renderOp.useGlobalInstancingVertexBufferIsAvailable;*/
                            drawCall->mCount         = renderOp.mVertex->mCount;
                            drawCall->mInstanceCount = renderOp.mInstanceCount;
                            drawCall->mOft           = renderOp.mVertex->mOffset;
                            drawCall->mInstanceOft   = mInstanceOft << baseInstanceShift;

                            insCnt = instancesPerDraw;

                            drawCmd = drawCall;
                        }

                        stats.mDrawCount += 1u;
                        stats.mInstanceCount += instancesPerDraw;
                    }
                    else
                    {
                        //Same mesh. Just go with instancing. Keep the counter in
                        //an external variable, as the region can be write-combined
                        insCnt += instancesPerDraw;
                        drawCmd->instanceCount = insCnt;
                        stats.mInstanceCount += instancesPerDraw;
                    }

                    if(renderOp.mType == OT_TRIANGLE_STRIP || renderOp.mType == OT_TRIANGLE_LIST)
                    {
                        stats.mFaceCount += (renderOp.mVertex->mCount / 3u) * instancesPerDraw;
                    }

                    stats.mVertexCount += renderOp.mVertex->mCount * instancesPerDraw;

                    ++itor;
                }

                mRenderSys->_addRecord(stats);

                mVaoId = 0;
                mVertexData = 0;
                mIndexData = 0;
                mLastTextureHash = 0;
            }
            else if(numNeededDraws > 0)
            {
                GeometryRaw * lastVao = 0;
                uint32 lastVaoName = mVaoId;
                RenderStateCache const * lastRSC = &c_dummyCache;
                uint32 lastHlmsCacheHash = 0;

                int baseInstanceAndIndirectBuffers = 0;
                if(N_Only(GBuffer)->supportsIndirectBuffers())
                    baseInstanceAndIndirectBuffers = 2;
                else if(N_Only(GBuffer)->supportsBaseInstance())
                    baseInstanceAndIndirectBuffers = 1;

                const bool instancedStereo = mSceneManager->isUsingInstancedStereo();
                const uint32 instancesPerDraw = instancedStereo ? 2u : 1u;
                const uint32 baseInstanceShift = instancedStereo ? 1u : 0u;
                uint32 insCnt = instancesPerDraw;

                CbDrawCall * drawCmd = 0;
                CbSharedDraw * drawCountPtr = 0;

                RenderSys::DrawRecord stats;

                const RenderItemList & queuedRenderables = temp.mLevelList[0];

                RenderItemList::const_iterator itor = queuedRenderables.begin();
                RenderItemList::const_iterator end  = queuedRenderables.end();

                while( itor != end )
                {
                    const RenderItem & renderItem = *itor;
                    uint8 meshLod = renderItem.mSpaceObj->getCurrentMeshLod();
                    const GeometryRawList & vaos = renderItem.mGeometryObj->getVaos(static_cast<VertexPass>(stype));

                    GeometryRaw * vao = vaos[meshLod];
                    const ShaderChMaterial * mat = renderItem.mGeometryObj->getMaterial();

                    RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, static_cast<RenderPatternType>(mat->mType));

                    lastHlmsCacheHash = lastRSC->mHash;
                    const RenderStateCache * rsc = rp->getCache(lastRSC->mHash, mStateList[mat->mType], renderItem, stype);

                    if(lastHlmsCacheHash != rsc->mHash)
                    {
                        CbPipelineStateObject * psoCmd = mDrawCallList->addCommand<CbPipelineStateObject>();
                        *psoCmd = CbPipelineStateObject(&rsc->mRSO);
                        lastRSC = rsc;

                        //Flush the Vao when changing shaders. Needed by D3D11/12 & possibly Vulkan
                        lastVaoName = 0;
                    }

                    uint32 baseInstance = rp->queue(mDrawCallList, rsc, renderItem, stype, lastHlmsCacheHash);

                    if(drawCmd != mDrawCallList->getLastCommand() || lastVaoName != vao->mVaoId)
                    {
                        //Different mesh, vertex buffers or layout. Make a new draw call.
                        //(or also the the RenderPattern made a batch-breaking command)

                        if(lastVaoName != vao->mVaoId)
                        {
                            *mDrawCallList->addCommand<CbVao>() = CbVao(vao);
                            *mDrawCallList->addCommand<CbIndirectBuffer>() = CbIndirectBuffer(ibuffer);
                            lastVaoName = vao->mVaoId;
                        }

                        void *offset = reinterpret_cast<void *>(ibuffer->getCurrentOffset() + (indirectDraw - startIndirectDraw) );

                        if(vao->getIndexBuffer())
                        {
                            CbDrawCallIndexed * drawCall = mDrawCallList->addCommand<CbDrawCallIndexed>();
                            *drawCall = CbDrawCallIndexed(baseInstanceAndIndirectBuffers, vao, offset);
                            drawCmd = drawCall;
                        }
                        else
                        {
                            CbDrawCallStrip * drawCall = mDrawCallList->addCommand<CbDrawCallStrip>();
                            *drawCall = CbDrawCallStrip(baseInstanceAndIndirectBuffers, vao, offset);
                            drawCmd = drawCall;
                        }

                        lastVao = 0;
                        stats.mDrawCount += 1u;
                    }

                    if(lastVao != vao)
                    {
                        //Different mesh, but same vertex buffers & layouts. Advance indirection buffer.
                        ++drawCmd->numDraws;

                        if(vao->mIndexBuffer)
                        {
                            IndexIndirect * iIPtr = reinterpret_cast<IndexIndirect *>(indirectDraw);
                            indirectDraw += sizeof(IndexIndirect);

                            drawCountPtr = iIPtr;
                            iIPtr->mCount       = vao->mCount;
                            iIPtr->mInstanceCount= instancesPerDraw;
                            iIPtr->mOft         = vao->mIndexBuffer->getCurrentOffset() + vao->mOffset;
                            iIPtr->mVertexOft   = vao->mVertexBuffer->getCurrentOffset();
                            iIPtr->mInstanceOft = mInstanceOft << baseInstanceShift;

                            insCnt = instancesPerDraw;
                        }
                        else
                        {
                            VertexIndirect * vIPtr = reinterpret_cast<VertexIndirect *>(indirectDraw);
                            indirectDraw += sizeof(VertexIndirect);

                            drawCountPtr = vIPtr;
                            vIPtr->mCount        = vao->mCount;
                            vIPtr->mInstanceCount= instancesPerDraw;
                            vIPtr->mOft          = vao->mVertexBuffer->getCurrentOffset() + vao->mOffset;
                            vIPtr->mInstanceOft  = mInstanceOft << baseInstanceShift;

                            insCnt = instancesPerDraw;
                        }

                        lastVao = vao;
                        stats.mInstanceCount += instancesPerDraw;
                    }
                    else
                    {
                        //Same mesh. Just go with instancing. Keep the counter in
                        //an external variable, as the region can be write-combined
                        insCnt += instancesPerDraw;
                        drawCountPtr->mInstanceCount = insCnt;
                        stats.mInstanceCount += instancesPerDraw;
                    }

                    const Ogre::OperationType type = vao->getType();
                    if(type == OT_TRIANGLE_STRIP || type == OT_TRIANGLE_LIST)
                        stats.mFaceCount += (vao->mCount / 3u) * instancesPerDraw;

                    stats.mVertexCount += vao->mCount * instancesPerDraw;

                    ++itor;
                }

                mRenderSys->_addRecord(stats);

                mVaoId = lastVaoName;
                mVertexData = 0;
                mIndexData = 0;
                mLastTextureHash = 0;
            }
        }

        if(indirectValid && ibuffer)
            ibuffer->unlock(UT_PersistentMap);

        //OgreProfileEndGroup("Command Preparation", OGREPROF_RENDERING);
        //OgreProfileBeginGroup("Command Execution", OGREPROF_RENDERING);
        //OgreProfileGpuBegin("Command Execution" );

        //for(size_t i = 0; i < RPT_Count; ++i)
        //{
        RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, mRenderPatternType);
        if(rp)
            rp->executeBegin(mDrawCallList);
        //}

        mDrawCallList->execute();

        //for(size_t i = 0; i < RPT_Count; ++i)
        //{
        RenderPattern * rp = mRenderSys->getRenderPattern(mSceneManager, mRenderPatternType);
        if(rp)
            rp->executeEnd(mDrawCallList);
        //}

        --mRenderMark;

        //OgreProfileGpuEnd("Command Execution");
        //OgreProfileEndGroup("Command Execution", OGREPROF_RENDERING);
    }
    //-----------------------------------------------------------------------
    void RenderQueue::renderSingleObject(GeometryObj * gobj, const SpaceObj * sobj, ShadowType stype)
    {
        if(mVaoId)
        {
            //mRenderSys->_startLegacyV1Rendering();
            mVaoId = 0;
        }

        if(mShadowType != stype)
        {
            clearState();
            mShadowType = stype;
        }

        const ShaderChMaterial * mat = gobj->getMaterial();

        ++mRenderMark;
        N_EnginePtr()->beginFrame();

        RenderPattern * rp = mat->getParent();
		rp->setSpaceManager(mSceneManager);
        rp->clearAll();
        RenderStateCache * rsc = rp->createState(stype);

        const RenderItem renderItem(0, gobj, sobj);
        GeometryRaw op;
        renderItem.mGeometryObj->getGeometry(op, stype & (mat->getAlphaTest() == CMPF_ALWAYS_PASS));
        /*uint32 hlmsHash = stype ? renderItem.mGeometryObj->getCasterHash() :
                                       renderItem.mGeometryObj->getNormalHash();*/

        if(mVertexData != op.mVertex)
        {
            mVertexData = op.mVertex;
        }
        if(mIndexData != op.mIndex)
        {
            mIndexData = op.mIndex;
        }

        const RenderStateCache * rsc = rp->getCache(0, rsc, renderItem, stype);

        mRenderSys->setRSO(&rsc->mRSO);

        mLastTextureHash = rp->fillBuffersFor(renderItem, rsc, stype, 0, mLastTextureHash);

        mRenderSys->setGeometry(&op);
        mRenderSys->_render();

        mVaoId = 0;
        --mRenderMark;
    }
    //-----------------------------------------------------------------------
}