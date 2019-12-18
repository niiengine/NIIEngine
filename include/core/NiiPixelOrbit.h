/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-5-8

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

#ifndef _NII_PixelOrbit_H_
#define _NII_PixelOrbit_H_

#include "NiiPreInclude.h"
#include "NiiPixelChain.h"
#include "NiiPosListener.h"

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
        NIIf getOrbitLength() const;

        /** 设置原色
        @version NIIEngine 3.0.0
        */
        void setColour(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a = 1.0);

        /** 获取原色
        @version NIIEngine 3.0.0
        */
        const Colour & getColour(Nidx index) const;

        /** 设置退色
        @version NIIEngine 3.0.0
        */
        void setColourFade(Nidx index, NIIf r, NIIf g, NIIf b, NIIf a);

        /** 获取退色
        @version NIIEngine 3.0.0
        */
        const Colour & getColourFade(Nidx index) const;

        /** 设置尺寸
        @version NIIEngine 3.0.0
        */
        void setDim(Nidx index, NIIf width);

        /** 获取尺寸
        @version NIIEngine 3.0.0
        */
        NIIf getDim(Nidx index) const;

        /** 设置尺寸销减
        @version NIIEngine 3.0.0
        */
        void setDimReduce(Nidx index, NIIf reduce);

        /** 获取尺寸销减
        @version NIIEngine
        */
        NIIf getDimReduce(Nidx index) const;

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

}
#endif
