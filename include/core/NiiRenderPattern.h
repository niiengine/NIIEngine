/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

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

#ifndef _NII_RENDERPATTERN_H_
#define _NII_RENDERPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"

namespace NII
{
    class ShadowRenderTest;

    /** ��Ӱ��������,�����Ժ��ϸ���Ż����޸�
    @remark
        ��Ӱ��������Ⱦ�����ǱȽϸ��ӵ�,�������ܶ�Ч������,�е�ʱ����ῴ��������Ϸ
        ��Ӱ�Ӷ����Ե���,�ƹ�����Խ���������ӰԽ��,Ч��Խ��,��Ӱʵ�ʾ��ǵƹ������
        �Ĺ���
    @note ����ʹ������Ĭ���ṩ����Ӱ������
    @par ���¼�������Ż���ӰЧ��
        (1)���һ����Ӱ��ȫ����һ������,ֱ�Ӱ���ӰͶ��ͨ�����뵽������Ⱦͨ����
        (2)����ƹ��Ǿ�̬,�������㹻Զ,�������ֻҪ����һ����Ӱ��
        (3)���������ڵƹ�(��̬,�㹻Զ)��,������Ӱ���ʹ�� PlanarShadow
        (4)����ƹ���,��������ȴ���32������ı�Ե,Ȼ��ͨ���ƹ�λ��ѡ����ӽ�����ı�Ե
        (5)�������Ͳ�Ҫ��Ӱ����
    @version NIIEngine 3.0.0 ����api
    */
    class _EngineAPI RenderPattern : public Pattern
    {
        friend class SpaceManager;
    public:
        /** �ⲿ����
        @version NIIEngine 3.0.0
        */
        class Listener
        {
        public:
            Listener();
            virtual ~Listener();

            /** Ѱ�ҿɼ�����
            @version NIIEngine 3.0.0
            */
            virtual void onFindGeometry(SpaceManager * sm, Viewport * v);

            /** Ѱ��Ͷ����Ӱ����
            @version NIIEngine 3.0.0
            */
            virtual void onFindCastGeometry(SpaceManager * sm, Viewport * v);

            /** Ѱ�ҽ�����Ӱ����
            @version NIIEngine 3.0.0
            */
            virtual void onFindReceiveGeometry(SpaceManager * sm, Viewport * v);
        };

        typedef vector<Listener *>::type Listeners;
    public :
        RenderPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~RenderPattern();

        /** ��ʼ��
        @version NIIEngine 3.0.0
        */
        virtual void init();

        /** ��ǰϵͳ��/Ӳ��֧���Ƿ�֧�������Ӱ����
        @version NIIEngine 3.0.0
        */
        virtual bool isValid();

        /** ����ר�ö���
        @version NIIEngine 3.0.0
        */
        virtual RenderQueue * createQueue() const;

        /** �ָ���Ⱦ���еĽṹ
        @param[in] queue ��Ⱦ����
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderQueue * queue) = 0;

        /** �ָ���Ⱦ���еĽṹ
        @param[in] group ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderGroup * group) = 0;

        /** ��Ⱦ����Ⱥ
        @param[in] grp ��Ҫ��Ⱦ�ļ�����
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * grp);

        /** ��Ⱦ��Ӱ
        @remark ׼����Ⱦ��Ӱ
        @note ������Ӱ����Ҫ,�ɰ���Ӱ���ҪԤ�����ɻ�ժѡ��ԵҲ��Ҫ�������
        @version NIIEngine 3.0.0
        */
        virtual void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /** Ӧ������
        @param[in] ch ʹ�õ�ͨ·
        @version NIIEngine 3.0.0
        */
        virtual void applyTexture(const ShaderCh * ch);

        /** Ӧ�õƹ�
        @remark ����Ӧ��
        @param[in] ch ������Ⱦͨ��
        @param[in] src Դ�ƹ�
        @param[in] oft Դ�ƹ⿪ʼ����,ָ src �����Ŀ�ʼ
        @param[in] app Ŀ��ƹ�(ʵ��Ӧ��)
        @note �����޸� src �������,�����Ҫ�޸� dst ���鷵�س�Ա����(src����)
        @version NIIEngine 3.0.0
        */
        virtual void applyLight(const ShaderCh * ch, const LightList & src, NCount & oft, LightList & app);

        /** ��������������ģʽ
        @param[in] ch ʹ�õ�ͨ·
        @version NIIEngine 3.0.0
        */
        virtual CullingMode setCullingMode(CullingMode mode);

        /** ��ȡ����������ģʽ
        @version NIIEngine 3.0.0
        */
        virtual CullingMode getCullingMode() const;

        /** ������Ⱦ�ƹ�
        @remark ������Ӱʱ,�ȶԵƹ�����
        @param[in][out] dst ��Ч�ƹ�
        @version NIIEngine 3.0.0
        */
        virtual void sortLight(LightList & dst);

        /** ȷ����Ч�ƹ�
        @param[in][out] dest �ƹ�
        @version NIIEngine 3.0.0
        */
        virtual void findLight(LightList & dst);

        /** �Ƶ���Ӱ��Ⱦͨ·
        @remark ����Ⱦ�����׶�ת��ͨ·
        @param[in] dst Ŀ��ͨ·
        @param[in] src Դͨ·
        @version NIIEngine 3.0.0
        */
        virtual void deriveShadow(const ShaderCh *& dst, const ShaderCh * src);

        /** �Ƿ�ͨ����Ӱ����
        @remark ��Ӱ����
        @param[in] ch ��ǰӦ�õ�ͨ��
        @param[in] obj ��ǰ��Ⱦ�Ķ���
        @version NIIEngine 3.0.0
        */
        virtual bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /** �Ƿ�ͨ����Ӱ����
        @remark ��Ӱ����
        @param[in] ch ��ǰӦ�õ�ͨ��
        */
        virtual bool testRender(const ShaderCh * ch);

        /** �Ƿ�����Ⱦ��Ӱ�׶�
        @version NIIEngine 3.0.0
        */
        virtual bool isRenderShadow() const;

        /** ����������Ӱ
        @version NIIEngine 3.0.0
        */
        virtual bool isTextureShadow() const;

        /** ����ģ����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual bool isStencilShadow() const;

        /** ������Ӱ��ɫ.
        @remark �����ʹ�ø�����ȥ������Ӱ,Խ��Խ��,���Բ�����,�����ʵ����ͻ�����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual void setShadowColour(const Colour & c);

        /** ��ȡ��Ӱ��ɫ.
        @remark �����ʹ�ø�����ȥ������Ӱ,Խ��Խ��,���Բ�����,�����ʵ����ͻ�����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getShadowColour() const;

        /** ������ӰͶ���Ƿ�ʹ�ú����,������������.
        @version NIIEngine 3.0.0
        */
        void setBackFaceInCast(bool b);

        /** �����ӰͶ���Ƿ�ʹ�ú����,������������.
        @version nIIEngine 3.0.0
        */
        bool isBackFaceInCast() const;

        /** �Ƿ�����Զ�ü���
        @remark �ɰ���Ӱר��
        @version NIIEngine 3.0.0
        */
        void setInfiniteFar(bool b);

        /** �Ƿ�����Զ�ü���
        @remark �ɰ���Ӱר��
        @version NIIEngine 3.0.0
        */
        bool isInfiniteFar()const;

        /** �����Ƿ���еƹ���ü�.
        @version NIIEngine 3.0.0
        */
        void setLightClipPlane(bool b);

        /** ��ȡ�Ƿ���еƹ���ü�.
        @version NIIEngine 3.0.0
        */
        bool isLightClipPlane() const;

        /** �����Ƿ��ֹ��Ӱ
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b);

        /** ��ȡ�Ƿ��ֹ��Ӱ
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const;

        /** ������Ӱ�����쳤��
        @param[in] ext ���쳤��
        @version NIIEngine 3.0.0
        */
        void setShadowExtent(NIIf ext);

        /** ��ȡ��Ӱ�����쳤��
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtent() const;

        /** ������Ӱ������쳤��
        @param[in] ext ������쳤��
        @version NIIEngine 3.0.0
        */
        void setShadowExtentMax(NIIf ext);

        /** ��ȡ��Ӱ������쳤��
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtentMax() const;

        /** ��Ӽ���
        @version NIIEngine 3.0.0
        */
        void add(Listener * s);

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void remove(Listener * s);
    protected:
        /** Ӧ����Ⱦϵͳ
        @version NIIEngine 3.0.0
        */
        void applyRender(RenderSys * sys);

        /** ��Ӱͨ·
        @version NIIEngine ����api
        */
        virtual void renderImpl(RenderGroup * rg) = 0;

        /** ����ͨ·
        @version NIIEngine ����api
        */
        virtual void renderBasic(RenderGroup * rg);

        /** ���ҿ�������ʱ����
        @version NIIEngine 3.0.0
        */
        void onFindGeometry(Viewport * v);

        /** ���ҿ�Ͷ����Ӱ������ʱ����
        @version NIIEngine 3.0.0
        */
        void onFindCastGeometry(Viewport * v);

        /** ���ҿɽ�����Ӱ������ʱ����
        @version NIIEngine 3.0.0
        */
        void onFindReceiveGeometry(Viewport * v);
    protected:
        SpaceManager * mParent;             ///< ��ǰ����
        RenderSys * mRenderSys;             ///< ��ǰ��Ⱦϵͳ
        RenderQueue * mRenderQueue;         ///< ��ǰ��Ⱦ��
        Camera * mCamera;                   ///< ��ǰ�����
        const Viewport * mViewPort;         ///< ��ǰ�ӿ�
        CullingMode mCullingMode;           ///< ��ǰ��ѡģʽ
        SysSyncParam * mSyncParams;         ///< ��Ⱦ�Զ�����
        ShadowRenderTest * mTest;           ///< ��Ӱ������
        ShaderCh * mCast;
        ShaderCh * mReceive;
        ResourceID mCastRID;
        ResourceID mReceiveRID;
        Colour mShadowColour;
        NIIf mShadowExtent;                 ///< ��Ӱ����
        NIIf mShadowExtentMax;
        Nui32 mLightHash;                   ///< ��ǰ�ƹ��ϣ
        Listeners mListeners;               ///< �����б�
        bool mInit;
        bool mShadowEnable;
        bool mBackFaceInCast;
        bool mInfiniteFar;                  ///< ����Զ�Ľ���
        bool mLightClipPlane;               ///< �ƹ���ü�
        static LightList mNullLight;
    };
}
#endif