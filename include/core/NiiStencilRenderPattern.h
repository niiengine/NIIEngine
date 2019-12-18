/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-3-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
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
        NCount getIndexSize() const;

        /** �Ƿ������Ӱ
        @version NIIEngine 3.0.0
        */
        void setDebugShadow(bool b);

        /** �Ƿ������Ӱ
        @version NIIEngine 3.0.0
        */
        bool isDebugShadow() const;
    protected:
        /** ȷ����Ͷ����Ӱ���б�
        @version NIIEngine 3.0.0
        */
        const ShadowVolumeGenList & findCastList(const Light * light, const Camera * camera);

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
            ShadowVolumeObj * caster, NIIf extrude, Nmark & mark);

        /** ��Ⱦ��Ӱ
        @version NIIEngine 3.0.0
        */
        void renderShadows(const ShadowVolumeList & src, ShaderCh * ch, 
            const LightList * lights, bool secondpass, bool zfail, bool twosided, Nmark mark);
    private:
        StencilShaderData * mShaderData;
    protected:
        IndexBuffer * mIndexBuffer;
        ViewRectGeo * mScreen;
        ShaderCh * mDebugCh;
        ShaderCh * mStencilCh;
        ShaderCh * mModulateCh;
        ShadowVolumeGenList mCasterList;
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