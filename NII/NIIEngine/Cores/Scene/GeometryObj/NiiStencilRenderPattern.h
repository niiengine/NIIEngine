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

#ifndef _NII_STENCIL_RENDERPATTERN_H
#define _NII_STENCIL_RENDERPATTERN_H

#include "NiiPreInclude.h"
#include "NiiRenderPattern.h"

namespace NII
{
    class StencilShaderData;
    /** �ɰ���Ⱦ������
    @version NIIEngine 3.0.0
    */
    class StencilRenderPattern : public RenderPattern
    {
    public :
        StencilRenderPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~StencilRenderPattern();

        /// @copydetails RenderPattern::init
        void init();

        /// @copydetails RenderPattern::isValid
        bool isValid();

        /// @copydetails RenderPattern::createQueue
        RenderQueue * createQueue() const;
        
        /// @copydetails RenderPattern::renderShadow
        virtual void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /// @copydetails RenderPattern::isStencilShadow
        bool isStencilShadow() const;

        /// @copydetails RenderPattern::setShadowColour
        void setShadowColour(const Colour & c);

        /// @copydetails RenderPattern::sort
        void sortLight(LightList & dest);
    public:
        /** ������������
        @param[in] size ������������
        @version NIIEngine 3.0.0
        */
        void setIndexSize(NCount size);

        /** ��ȡ������������
        @version NIIEngine 3.0.0
        */
        inline NCount getIndexSize() const      { return mIndexBufferSize; }

        /** �Ƿ������Ӱ
        @version NIIEngine 3.0.0
        */
        inline void setDebugShadow(bool b)      { mDebugShadows = b; }

        /** �Ƿ������Ӱ
        @version NIIEngine 3.0.0
        */
        inline bool isDebugShadow() const       { return mDebugShadows; }
    protected:
        /** ȷ����Ͷ����Ӱ���б�
        @version NIIEngine 3.0.0
        */
        const StencilObjList & findCastList(const Light * light, const Camera * camera);

        /** ������͵ƹ�Ľ��ü�
        @version NIIEngine 3.0.0
        */
        const PlaneSpace & getNearClip(const Light * light, const Camera * cam) const;

        /** ������͵ƹ�Ĳü��ݻ�
        @version NIIEngine 3.0.0
        */
        const PlaneSpaceList & getFrustumClip(const Light * light, const Camera * cam) const;

        /** Ϊ��Ӱ�ݻ������ɰ�״̬���ڲ�ʵ�÷���.
        @param[in] two ���ǵڶ���
        @param[in] zfail ʹ�� zfail
        */
        void applyStencil(bool two, bool zfail);

        /** ��Ⱦ�ɰ�
        @param[in] light �ƹ���Դ
        @param[in] cam ���ڹۿ��������
        @param[in] clip �����ü�״̬
        */
        void renderStencil(const Light * light, const Camera * cam, bool clip);

        /** Ӧ�ð���
        @version NIIEngine 3.0.0
        */
        virtual void applyDarkCap(const Light * light, const Camera * camera,
            ShadowObj * caster, NIIf extrude, Nmark & mark);

        /** ��Ⱦ��Ӱ
        @version NIIEngine 3.0.0
        */
        void renderShadows(const ShadowVolumeList & src, ShaderCh * ch, const LightList * lights, 
            bool secondpass, bool zfail, bool twosided, Nmark mark);
    private:
        StencilShaderData * mShaderData;
    protected:
        IndexBuffer * mIndexBuffer;
        RectGeo * mScreen;
        ShaderCh * mDebugCh;
        ShaderCh * mStencilCh;
        ShaderCh * mModulateCh;
        StencilObjList mCasterList;
        mutable PlaneSpace mNearClipSpace;
        mutable PlaneSpaceList mFrustumClipSpace;
        NCount mIndexBufferSize;
        ResourceID mDebugChRID;
        ResourceID mStencilChRID;
        ResourceID mModulationChRID;
        bool mDebugShadows;
        bool mBothSideStencil;
    };
}
#endif