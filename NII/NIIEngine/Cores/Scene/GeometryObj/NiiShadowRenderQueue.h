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

#ifndef _NII_SHADOW_RENDER_QUEUE_H_
#define _NII_SHADOW_RENDER_QUEUE_H_

#include "NiiPreInclude.h"
#include "NiiRenderQueue.h"

namespace NII
{
    /** 阴影渲染专用等级组
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderLevelGroup : public RenderLevelGroup
    {
    public:
        ShadowRenderLevelGroup(RenderGroup * parent, RenderCommandGroup * dcg,
            bool splitLightingType, bool splitNoShadow, bool castNoReceive);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSplitLightingType(bool b)                    { mSplitLightingType = b; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setSplitNoShadow(bool b)                        { mSplitNoShadow = b; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setCastersNotReceivers(bool b)                  { mCastersNotReceivers = b; }

        /**
        @version NIIEngine 3.0.0
        */
        inline const Group & getSolidsDiffuseSpecular() const       { return mSolidsDiffuseSpecular; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline const Group & getSolidsDecal() const                 { return mSolidsDecal; }

        /** 
        @version NIIEngine 3.0.0
        */
        inline const Group & getSolidsNoShadowReceive() const       { return mSolidsNoShadowReceive; }

        /** 添加排序模式
        @version NIIEngine 3.0.0
        */
        virtual void addSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        virtual void removeSort(RenderSortMode om);

        /** 添加渲染
        @version NIIEngine 3.0.0
        */
        virtual void add(SpaceObj * sobj, GeometryObj * obj, ShaderFusion * sf);

        /** 移去渲染
        @version NIIEngine 3.0.0
        */
        virtual void remove(ShaderCh * ch);

        /** 排序
        @version NIIEngine 3.0.0
        */
        virtual void sort(const Camera * cam);

        /** 清除这个可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** 合并可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void merge(const RenderLevelGroup * o);
    protected:
        Group mSolidsDiffuseSpecular;
        Group mSolidsDecal;
        Group mSolidsNoShadowReceive;
        bool mSplitNoShadow;
        bool mSplitLightingType;
        bool mCastersNotReceivers;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderGroup : public RenderGroup
    {
    public:
        ShadowRenderGroup(RenderQueue * parent, bool lightingType, bool splitNoShadow, bool castNoReceive);

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitNoShadow(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitLightingType(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        void setCastersNotReceivers(bool b);
    protected:
        /** 创建渲染等级组实现
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderLevelGroup *& out);
    protected:
        bool mSplitNoShadow;
        bool mSplitLightingType;
        bool mCastersNotReceivers;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderQueue : public RenderQueue
    {
    public:
        ShadowRenderQueue(RenderCommandGroup * dcg);
        
        /// @copydetails RenderQueue::init
        void init();

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitNoShadow(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSplitNoShadow() const                     { return mSplitNoShadow; }

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitLightingType(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isSplitLightingType() const                 { return mSplitLightingType; }

        /**
        @version NIIEngine 3.0.0
        */
        void setCastersNotReceivers(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isCastersNotReceivers() const               { return mCastersNotReceivers; }
    protected:
        /// @copydetails RenderQueue::createImpl
        void createImpl(RenderGroup *& out);
    protected:
        bool mSplitNoShadow;
        bool mSplitLightingType;
        bool mCastersNotReceivers;
    };
}
#endif