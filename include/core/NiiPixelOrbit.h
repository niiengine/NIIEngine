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

#ifndef _NII_PixelOrbit_H_
#define _NII_PixelOrbit_H_

#include "NiiPreInclude.h"
#include "NiiPixelChain.h"
#include "NiiPosListener.h"
#include "NiiSpaceObjFactory.h"

namespace NII
{
    /** 像素轨迹
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelOrbit : public PixelChain, public PosListener
    {
    public:
        PixelOrbit(SpaceID sid, NCount max = 18, NCount count = 1,
            bool texcoord = true, bool texcolour = true);
        virtual ~PixelOrbit();

        using PixelChain::add;
        using PosListener::add;

        /** 更新轨迹状态
        @param[in] cost 上次和这次调用渡过的时间,单位:毫秒
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 附加到节点
        @param[in] idx 链索引,必须是空闲的
        @version NIIEngine 3.0.0
        */
        virtual void attachNode(PosNode * n, Nidx idx = 0);

        /** 链中解除
        @version NIIEngine 3.0.0
        */
        virtual void detachNode(Nidx idx);

        /** 节点中解除
        @version NIIEngine 3.0.0
        */
        virtual void detachNode(PosNode * n);

        /** 解除所有节点
        @version NIIEngine 3.0.0
        */
        virtual void detachAllNode();

        /** 设置轨迹长度
        @param[in] len
        @version NIIEngine 3.0.0
        */
        void setOrbitLength(NIIf len);

        /** 获取轨迹长度
        @version NIIEngine 3.0.0
        */
        inline NIIf getOrbitLength() const                      { return mOrbitLength; }

        /** 设置原色
        @version NIIEngine 3.0.0
        */
        void setColour(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a = 1.0);

        /** 获取原色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getColour(Nidx index) const       { N_assert(index < mChainCount, "error index"); return mColour[index]; }

        /** 设置退色
        @version NIIEngine 3.0.0
        */
        void setColourFade(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a);

        /** 获取退色
        @version NIIEngine 3.0.0
        */
        inline const Colour & getColourFade(Nidx index) const   { N_assert(index < mChainCount, "error index"); return mColourFade[index]; }

        /** 设置尺寸
        @version NIIEngine 3.0.0
        */
        inline void setDim(Nidx index, NIIf width)              { N_assert(index < mChainCount, "error index"); mDim[index] = width; }

        /** 获取尺寸
        @version NIIEngine 3.0.0
        */
        inline NIIf getDim(Nidx index) const                    { N_assert(index < mChainCount, "error index"); return mDim[index]; }

        /** 设置尺寸销减
        @version NIIEngine 3.0.0
        */
        void setDimReduce(Nidx index, NIIf reduce);

        /** 获取尺寸销减
        @version NIIEngine
        */
        inline NIIf getDimReduce(Nidx index) const              { N_assert(index < mChainCount, "error index"); return mDimReduce[index]; }

        /// @copydetails PixelChain::setChainCount
        void setChainCount(NCount count);

        /// @copydetails PixelChain::removeAll
        void removeAll(Nidx index);

        /// @copydetails PixelChain::setMaxPixelCount
        void setMaxPixelCount(NCount max);

        /// @copydetails PosListener::update
        void update(PosNode * node);

        /// @copydetails PosListener::destroy
        void destroy(PosNode * node);

        /// @copydetails SpaceObj::getFactoryID
        FactoryID getFactoryID() const;
    protected:
        void createTimeCtl();

        void updateOribit(Nidx index, const PosNode * node);

        void attachOribit(Nidx index, const PosNode * node);
    protected:
        typedef map<PosNode *, Nidx>::type NodeChainList;
        typedef vector<Colour>::type ChainColourList;
        typedef vector<NIIf>::type ChainDimList;
        typedef vector<Nidx>::type ChainIndexList;
    protected:
        PosNodeList mNodeList;
        NodeChainList mNodeChainList;
        ChainIndexList mAttachChainList;
        ChainIndexList mDetachChainList;
        NIIf mOrbitLength;
        NIIf mPixelLength;
        NIIf mSquaredElemLength;
        ChainColourList mColour;
        ChainColourList mColourFade;
        ChainDimList mDim;
        ChainDimList mDimReduce;
        DataValue<TimeDurMS> * mTimeFactor;
        DataEquation<TimeDurMS, TimeDurMS> * mFadeFunctor;
    };

    /** PixelOrbit专用工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PixelOrbitFactory : public SpaceObjFactory
    {
    public:
        PixelOrbitFactory();
        ~PixelOrbitFactory();

        ///@copydetails SpaceObjFactory::getID
        FactoryID getID() const;

        ///@copydetails SpaceObjFactory::create
        SpaceObj * create(SpaceID sid, SpaceManager * mag, const PropertyData * params = 0);

        ///@copydetails SpaceObjFactory::destroy
        void destroy(void * obj);
    };
}
#endif
