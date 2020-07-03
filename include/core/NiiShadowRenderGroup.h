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

#ifndef _NII_SHADOW_RENDER_Group_H_
#define _NII_SHADOW_RENDER_Group_H_

#include "NiiPreInclude.h"
#include "NiiRenderGroup.h"

namespace NII
{
    /** ��Ӱ��Ⱦר�õȼ���
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderLevelGroup : public RenderLevelGroup
    {
    public:
        ShadowRenderLevelGroup(RenderGroup * parent, bool splitRenderStep,
            bool splitNoShadow, bool castNonReceive);
        ~ShadowRenderLevelGroup();

        /** Sets whether or not the queue will split passes by their lighting type,
            ie ambient, per-light and decal.
        */
        inline void setSplitPassesByLightingType(bool b)
        {
            mSplitPassesByLightingType = b;
        }

        /** Sets whether or not passes which have shadow receive disabled should
            be separated.
        */
        inline void setSplitNoShadowPasses(bool b)
        {
            mSplitNoShadowPasses = b;
        }

        /** Sets whether or not objects which cast shadows should be treated as
            never receiving shadows.
        */
        inline void setShadowCastersCannotBeReceivers(bool b)
        {
            mShadowCastersNotReceivers = b;
        }

        /** Get the collection of solids currently queued per light (only applicable in
             additive shadow modes).
        */
        inline const RenderSort & getSolidsDiffuseSpecular() const
        { return mSolidsDiffuseSpecular; }

        /** Get the collection of solids currently queued for decal passes (only
            applicable in additive shadow modes).
        */
        inline const RenderSort & getSolidsDecal() const
        { return mSolidsDecal; }

        /** Get the collection of solids for which shadow receipt is disabled (only
            applicable when shadows are enabled).
        */
        inline const RenderSort & getSolidsNoShadowReceive() const
        { return mSolidsNoShadowReceive; }

        /** �������ģʽ
        @version NIIEngine 3.0.0
        */
        virtual void addSort(RenderSortMode om);

        /** ��ȥ����ģʽ
        @version NIIEngine 3.0.0
        */
        virtual void removeSort(RenderSortMode om);

        /** �����Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void add(GeometryObj * obj, ShaderFusion * sf);

        /** ��ȥ��Ⱦ
        @version NIIEngine 3.0.0
        */
        virtual void remove(ShaderCh * ch);

        /** ����
        @version NIIEngine 3.0.0
        */
        virtual void sort(const Camera * cam);

        /** ����������Ⱦ��Ⱥ��
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** �ϲ�����Ⱦ��Ⱥ��
        @version NIIEngine 3.0.0
        */
        virtual void merge(const RenderLevelGroup * o);
    protected:
        RenderSort mSolidsDiffuseSpecular;
        RenderSort mSolidsDecal;
        RenderSort mSolidsNoShadowReceive;
        bool mSplitPassesByLightingType;
        bool mSplitNoShadowPasses;
        bool mShadowCastersNotReceivers;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderGroup : public RenderGroup
    {
    public:
        ShadowRenderGroup(RenderQueue * parent, bool splitRenderStep,
            bool splitNoShadow, bool castNonReceive);
        ~ShadowRenderGroup();

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitNoShadowPasses(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        void setSplitPassesByLightingType(bool b);

        /**
        @version NIIEngine 3.0.0
        */
        void setShadowCastersCannotBeReceivers(bool b);
    protected:
        /** ������Ⱦ�ȼ���ʵ��
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderLevelGroup *& out);
    protected:
        bool mSplitNoShadowPasses;
        bool mSplitPassesByLightingType;
        bool mShadowCastersNotReceivers;
    };
}
#endif