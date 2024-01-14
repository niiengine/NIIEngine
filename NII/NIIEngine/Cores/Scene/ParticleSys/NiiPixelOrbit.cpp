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
#include "NiiPixelOrbit.h"
#include "NiiRenderFrameListener.h"
#include "NiiStrConv.h"
#include "NiiEngine.h"
#include "NiiMath.h"
#include "NiiEquationManager.h"
#include "NiiAnimationManager.h"

namespace NII
{
    namespace
    {
        class _EngineInner TimeControllerValue : public DataValue<TimeDurMS>
        {
        public:
            TimeControllerValue(PixelOrbit * r, bool autoDsy):
                DataValue<TimeDurMS>(autoDsy){ mTrail = r; }

            const TimeDurMS getValue() const { return 0; }

            void setValue(const TimeDurMS & t) { mTrail->update(t); }
        protected:
            PixelOrbit * mTrail;
        };
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelOrbit
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PixelOrbit::PixelOrbit(SpaceID sid, NCount max, NCount count,
        bool texcoord, bool texcolour):
            PixelChain(sid, max, 0, texcoord, texcolour),
            mFadeFunctor(0)
    {
        setOrbitLength(100);
        setChainCount(count);
        mTimeFactor = N_new TimeControllerValue(this, true);
        // use V as varying texture coord, so we can use 1D textures to 'smear'
        setTexCoordU(false);
    }
    //-----------------------------------------------------------------------
    PixelOrbit::~PixelOrbit()
    {
        PosNodeList::iterator i, iend = mNodeList.end();
        for(i = mNodeList.begin(); i != iend; ++i)
        {
            (*i)->set((PosListener *)0);
        }
        mNodeList.clear();

        if(mFadeFunctor)
        {
            N_Only(Equation).destroy(mFadeFunctor);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::attachNode(PosNode * n, Nidx idx)
    {
        if(mNodeList.size() == mChainCount)
        {
            N_EXCEPT(InvalidParam, N_conv(mID) + _I18n(" cannot monitor any more nodes, chain count exceeded"));
        }

        if(n->getListener())
        {
            N_EXCEPT(InvalidParam, N_conv(mID) + _I18n(" cannot monitor node ") + N_conv(n->getID()) + _I18n(" since it already has a listener."));
        }

        ChainIndexList::iterator i = std::find(mDetachChainList.begin(), mDetachChainList.end(), idx);
        if(i != mDetachChainList.end())
        {
            mDetachChainList.erase(i);
            mAttachChainList.push_back(idx);
            mNodeChainList[n] = idx;

            attachOribit(idx, n);

            mNodeList.push_back(n);
            n->set(this);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::detachNode(Nidx idx)
    {
        ChainIndexList::iterator i = std::find(mAttachChainList.begin(), mAttachChainList.end(), idx);
        if(i != mAttachChainList.end())
        {
            PosNodeList::iterator mi = mNodeList.begin();
            NCount index = std::distance(mAttachChainList.begin(), i);
            std::advance(mi, index);
            PosNode * n = *mi;
            PixelChain::removeAll(idx);
            // mark as free now
            mDetachChainList.push_back(idx);
            n->set(0);
            mNodeList.erase(mi);
            mAttachChainList.erase(i);
            mNodeChainList.erase(mNodeChainList.find(n));
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::detachNode(PosNode * n)
    {
        PosNodeList::iterator i = std::find(mNodeList.begin(), mNodeList.end(), n);
        if(i != mNodeList.end())
        {
            ChainIndexList::iterator mi = mAttachChainList.begin();
            NCount index = std::distance(mNodeList.begin(), i);
            std::advance(mi, index);
            NCount chainIndex = *mi;
            PixelChain::removeAll(chainIndex);
            // mark as free now
            mDetachChainList.push_back(chainIndex);
            n->set(0);
            mNodeList.erase(i);
            mAttachChainList.erase(mi);
            mNodeChainList.erase(mNodeChainList.find(n));
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::detachAllNode()
    {
        PosNodeList::iterator i, iend = mNodeList.end();
        for(i = mNodeList.begin(); i != iend; ++i)
        {
            (*i)->set((PosListener *)0);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setOrbitLength(NIIf len)
    {
        mOrbitLength = len;
        mPixelLength = mOrbitLength / mPixelCount;
        mSquaredElemLength = mPixelLength * mPixelLength;
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setMaxPixelCount(NCount max)
    {
        PixelChain::setMaxPixelCount(max);
        mPixelLength = mOrbitLength / mPixelCount;
        mSquaredElemLength = mPixelLength * mPixelLength;

        for(NCount i = 0; i < mNodeList.size(); ++i)
        {
            attachOribit(i, mNodeList[i]);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setChainCount(NCount count)
    {
        if(count < mNodeList.size())
        {
            N_EXCEPT(InvalidParam, _I18n("Can't shrink the number of chains less than number of tracking nodes"));
        }

        NCount oldChains = getChainCount();

        PixelChain::setChainCount(count);

        mColour.resize(count, Colour::White);
        mColourFade.resize(count, Colour::ZERO);
        mDim.resize(count, 10);
        mDimReduce.resize(count, 0);

        if(oldChains > count)
        {
            // remove free chains
            ChainIndexList::iterator i = mDetachChainList.begin();
            for(; i != mDetachChainList.end();)
            {
                if(*i >= count)
                    i = mDetachChainList.erase(i);
                else
                    ++i;
            }
        }
        else if(oldChains < count)
        {
            // add new chains, at front to preserve previous ordering (pop_back)
            for(NCount i = oldChains; i < count; ++i)
                mDetachChainList.insert(mDetachChainList.begin(), i);
        }
        for(NCount i = 0; i < mNodeList.size(); ++i)
        {
            attachOribit(i, mNodeList[i]);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::removeAll(Nidx index)
    {
        PixelChain::removeAll(index);

        // Reset if we are tracking for this chain
        ChainIndexList::iterator i = std::find(mAttachChainList.begin(), mAttachChainList.end(), index);
        if(i != mAttachChainList.end())
        {
            NCount nodeIndex = std::distance(mAttachChainList.begin(), i);
            attachOribit(*i, mNodeList[nodeIndex]);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setColour(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a)
    {
        N_assert(index < mChainCount, "error index");

        mColour[index].r = r;
        mColour[index].g = g;
        mColour[index].b = b;
        mColour[index].a = a;
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setColourFade(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a)
    {
        N_assert(index < mChainCount, "error index");

        mColourFade[index].r = r;
        mColourFade[index].g = g;
        mColourFade[index].b = b;
        mColourFade[index].a = a;

        createTimeCtl();
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::setDimReduce(Nidx index, NIIf reduce)
    {
        N_assert(index < mChainCount, "error index");
        mDimReduce[index] = reduce;
        createTimeCtl();
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::createTimeCtl()
    {
        bool needController = false;
        for(NCount i = 0; i < mChainCount; ++i)
        {
            if (mDimReduce[i] != 0 || mColourFade[i] != Colour::ZERO)
            {
                needController = true;
                break;
            }
        }
        if(!mFadeFunctor && needController)
        {
            mFadeFunctor = N_Only(Equation).create(N_Only(Animation).getLastTimeValue(),
                mTimeFactor, N_Only(Animation).getNormalTimeFunc());
        }
        else if (mFadeFunctor && !needController)
        {
            N_Only(Equation).destroy(mFadeFunctor);
            mFadeFunctor = 0;
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::update(PosNode * node)
    {
        NodeChainList::const_iterator i = mNodeChainList.find(node);
        if(i != mNodeChainList.end())
        {
            updateOribit(i->second, node);
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::destroy(PosNode * node)
    {
        detachNode(node);
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::updateOribit(Nidx index, const PosNode * node)
    {
        // Repeat this entire process if chain is stretched beyond its natural length
        bool done = false;
        while(!done)
        {
            // PosNode has changed somehow, we're only interested in the derived position
            Chain & seg = mChainList[index];
            Pixel & headElem = mPixelList[seg.mBegin + seg.mStart];
            NCount nextElemIdx = seg.mStart + 1;
            // wrap
            if(nextElemIdx == mPixelCount)
                nextElemIdx = 0;
            Pixel & nextElem = mPixelList[seg.mBegin + nextElemIdx];

            // Vary the mStart elem, but bake new version if that exceeds element len
            Vector3f newPos = node->getSpacePos();
            if(mSpaceNode)
            {
                // Transform position to ourself space
                newPos = mSpaceNode->getSpaceOri().inverseUnit() *
                    (newPos - mSpaceNode->getSpacePos()) / mSpaceNode->getSpaceScale();
            }
            Vector3f diff = newPos - nextElem.mPos;
            NIIf sqlen = diff.lengthSquared();
            if(sqlen >= mSquaredElemLength)
            {
                // Move existing mStart to mPixelLength
                Vector3f scaledDiff = diff * (mPixelLength / Math::sqrt(sqlen));
                headElem.mPos = nextElem.mPos + scaledDiff;
                // Add a new element to be the new mStart
                Pixel newElem;
                newElem.mPos = newPos;
                newElem.mDim = mDim[index];
                newElem.mCoord = 0.0f;
                newElem.mColour = mColour[index];
                newElem.mOri = node->getSpaceOri();
                add(index, newElem);
                // alter diff to represent new mStart size
                diff = newPos - headElem.mPos;
                // check whether another step is needed or not
                if(diff.lengthSquared() <= mSquaredElemLength)
                    done = true;
            }
            else
            {
                // Extend existing mStart
                headElem.mPos = newPos;
                done = true;
            }

            // Is this segment full?
            if((seg.mEnd + 1) % mPixelCount == seg.mStart)
            {
                // If so, shrink mEnd gradually to match mStart extension
                Pixel & tailElem = mPixelList[seg.mBegin + seg.mEnd];
                NCount preTailIdx;
                if(seg.mEnd == 0)
                    preTailIdx = mPixelCount - 1;
                else
                    preTailIdx = seg.mEnd - 1;
                Pixel & preTailElem = mPixelList[seg.mBegin + preTailIdx];

                // Measure mEnd diff from pretail to mEnd
                Vector3f taildiff = tailElem.mPos - preTailElem.mPos;
                NIIf taillen = taildiff.length();
                if(taillen > 1e-06)
                {
                    NIIf tailsize = mPixelLength - diff.length();
                    taildiff *= tailsize / taillen;
                    tailElem.mPos = preTailElem.mPos + taildiff;
                }
            }
        } // end while

        mAABBValid = false;
        // Need to dirty the parent node, but can't do it using notify() here
        // since we're in the middle of the scene graph update (node listener),
        // so re-entrant calls don't work. Queue.
        if(mSpaceNode)
        {
            N_Engine().addUpdate(getSceneNode());
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::update(TimeDurMS time)
    {
        // Apply all segment effects
        for(NCount s = 0; s < mChainList.size(); ++s)
        {
            Chain & seg = mChainList[s];
            if(seg.mStart != N_Max_Num_Value && seg.mStart != seg.mEnd)
            {

                for(NCount e = seg.mStart + 1;; ++e) // until break
                {
                    e = e % mPixelCount;

                    Pixel & elem = mPixelList[seg.mBegin + e];
                    elem.mDim = elem.mDim - (mDimReduce[s] * (time / 1000));
                    elem.mDim = std::max(NIIf(0.0f), elem.mDim);
                    elem.mColour = elem.mColour - (mColourFade[s] * (time / 1000));
                    elem.mColour.clamp();

                    if(e == seg.mEnd)
                        break;

                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void PixelOrbit::attachOribit(Nidx index, const PosNode * node)
    {
        N_assert(index < mChainCount, "error");

        Chain & seg = mChainList[index];
        // set up this segment
        seg.mStart = seg.mEnd = N_Max_Num_Value;
        // Create new element, v coord is always 0.0f
        Vector3f position = node->getSpacePos();
        if(mSpaceNode)
        {
            position = mSpaceNode->getSpaceOri().inverse() *
                (position - mSpaceNode->getSpacePos()) / mSpaceNode->getSpaceScale();
        }
        Pixel e;
        e.mPos = position;
        e.mDim = mDim[index];
        e.mCoord = 0.0f;
        e.mColour = mColour[index];
        e.mOri = node->getSpaceOri();
        // Add the mBegin position
        add(index, e);
        // Add another on the same spot, this will extend
        add(index, e);
    }
    //-----------------------------------------------------------------------
    FactoryID PixelOrbit::getFactoryID() const
    {
        return N_FACTORY_PixelOrbit;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // PixelOrbitFactory
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    PixelOrbitFactory::PixelOrbitFactory()
    {
    }
    //-----------------------------------------------------------------------
    PixelOrbitFactory::~PixelOrbitFactory()
    {
    }
    //-----------------------------------------------------------------------
    FactoryID PixelOrbitFactory::getID() const
    {
        return N_FACTORY_PixelOrbit;
    }
    //-----------------------------------------------------------------------
    SpaceObj * PixelOrbitFactory::create(SpaceID sid, SpaceManager * mag, const PropertyData * params)
    {
        Nui32 maxElements;
        Nui32 numberOfChains;
        bool useTex;
        bool useCol;

        if(params != 0)
        {
            params->get(N_PropertyPixelOrbit_MaxElement, maxElements, 20);
            params->get(N_PropertyPixelOrbit_NumOfChain, numberOfChains, 1);
            params->get(N_PropertyPixelOrbit_UseTexCoord, useTex, true);
            params->get(N_PropertyPixelOrbit_UseVerColour, useCol, true);
        }
        SpaceObj * re = N_new PixelOrbit(sid, maxElements, numberOfChains, useTex, useCol);
        re->setFactory(this);
        re->setManager(mag);
        return re;
    }
    //-----------------------------------------------------------------------
    void PixelOrbitFactory::destroy(void * obj)
    {
        SpaceObj * temp = static_cast<SpaceObj *>(obj);
        N_delete temp;
    }
    //-----------------------------------------------------------------------
}