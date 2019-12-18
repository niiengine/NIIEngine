/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-1-7

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

#ifndef NII_SHADOWMAPPING_RENDERPATTERN_H
#define NII_SHADOWMAPPING_RENDERPATTERN_H

#include "NiiPreInclude.h"
#include "NiiRenderPattern.h"
#include "NiiShadowGenCamera.h"
#include "NiiPixelFormat.h"
#include "NiiRenderSort.h"

namespace NII
{
    class ShadowMappingData;
    /** ��������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShadowTextureConfig : public ShadowAlloc
    {
    public:
        ShadowTextureConfig();

        bool operator== (const ShadowTextureConfig & o2);
        bool operator!= (const ShadowTextureConfig & o2);
    public:
        Nui32 mWidth;           ///< ���
        Nui32 mHeight;          ///< �߶�
        Nui32 mFSAA;            ///< ����ݵȼ�
        PixelFormat mFormat;    ///< ��ʽ
        GroupID mGroup;         ///< Ⱥ��id
    };

    typedef vector<ShadowTextureConfig>::type ShadowTexCfgList;

    /** ��Ӱӳ����Ⱦ������
    @version NIIEngine 3.0.0
    */
    class ShadowMappingPattern : public RenderPattern
    {
    public:
        class _EngineAPI Listener
        {
        public:
            void onCast(Light * light, Camera * camera) {}
        };

        /** ����׶�
        @version NIIEngine 3.0.0
        */
        enum StageType
        {
            ST_Normal,
            ST_Texture,
            ST_Receive
        };

        /** ��Ӱ����
        @version NIIEngine 3.0.0
        */
        struct LightShadowLess
        {
            _EngineAPI bool operator()(const Light * l1, const Light * l2) const;
        };

        typedef map<const Camera *, const Light *>::type CameraLightList;
    public:
        ShadowMappingPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~ShadowMappingPattern();
    public:
        /// @copydetails RenderPattern::init
        void init();

        /// @copydetails RenderPattern::isValid
        bool isValid();

        /// @copydetails RenderPattern::createQueue
        RenderQueue * createQueue() const;

        /// @copydetails RenderPattern::setCullingMode
        CullingMode setCullingMode(CullingMode mode);

        /// @copydetails RenderPattern::applyTexture
        void applyTexture(const ShaderCh * ch);

        /// @copydetails RenderPattern::test
        virtual bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /// @copydetails RenderPattern::test
        virtual bool testRender(const ShaderCh * ch);

        /// @copydetails RenderPattern::renderShadow
        void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /// @copydetails RenderPattern::apply
        void applyLight(const ShaderCh * ch, const LightList & src, NCount & oft, LightList & app);

        /// @copydetails RenderPattern::sort
        void sortLight(LightList & dest);

        /// @copydetails RenderPattern::find
        void findLight(LightList & dest);

        /// @copydetails RenderPattern::derive
        void deriveShadow(const ShaderCh *& out, const ShaderCh * src);

        /// @copydetails RenderPattern::isRenderShadow
        bool isRenderShadow() const;

        /// @copydetails RenderPattern::isTextureShadow
        bool isTextureShadow() const;
    public:
        /** ����Ͷ�����
        @version NIIEngine 3.0.0
        */
        void setTextureCasterMaterial(ResourceID rid);

        /** ���ý��ղ���
        @version NIIEngine 3.0.0
        */
        void setTextureReceiverMaterial(ResourceID rid);

        /** ���������С
        @param[in] w power of 2.
        @param[in] h power of 2.
        @version NIIEngine 3.0.0
        */
        void setTextureSize(NCount w, NCount h);

        /** ������Ӱ�����ʽ
        @version NIIEngine 3.0.0
        */
        void setTexturePixelFormat(PixelFormat pf);

        /** ������Ӱ�����������
        @version NIIEngine 3.0.0
        */
        void setTextureFSAA(Nui16 fsaa);

        /** �������������Ӱ����ı���.
        @remark the default is 0.6.
        */
        void setExtentOffsetFactor(NIIf oft);

        /** �������������Ӱ����ı���
        @version NIIEngine 3.0.0
        */
        NIIf getExtentOffsetFactor() const;

        /** ������Ӱ��ʼ������������.
        @remark The default is 0.7
        */
        void setFadeBegin(NIIf start);

        /** ������Ӱ���������ı�������
        @remark The default is 0.9.
        */
        void setFadeEnd(NIIf end);

        /** �����Ƿ�Ͷ����Ӱ���Լ�
        @version NIIEngine 3.0.0
        */
        void setSelfShadow(bool b);

        /** ��ȡ�Ƿ�Ͷ����Ӱ���Լ�
        @version NIIEngine 3.0.0
        */
        bool isSelfShadow();

        /** ������������
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void setTexture(Nidx idx, NCount w, NCount h, PixelFormat pf, Nui16 fsaa = 0,
            GroupID rasterGroup = 1);

        /** ������������.
        @param[in] idx
        @version NIIEngine 3.0.0
        */
        void setTexture(Nidx idx, const ShadowTextureConfig & config);

        /** ͳ����������
        @version NIIEngine 3.0.0
        */
        void setTextureConfig(NCount w, NCount h, NCount count, PixelFormat fmt = PF_X8R8G8B8,
            Nui16 fsaa = 0, GroupID rasterGroup = 1);

        /** ����������õ�ǰ��������Ӱ����.
        @version NIIEngine 3.0.0
        */
        Texture * getTexture(Nidx index) const;

        /** ������������
        @version NIIEngine 3.0.0
        */
        void setTextureCount(NCount count);

        /** ������Ӱ��������.
        @version NIIEngine 3.0.0 �߼�api
        */
        void setTextureCount(LightType type, NCount count);

        /** ��ȡ��Ӱ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        NCount getTextureCount(LightType type) const;

        /** ��Ӱ���������
        @version NIIEngine 3.0.0
        */
        void setShadowGenCamera(ShadowGenCamera * obj);

        /** ��Ӱ���������
        @version NIIEngine 3.0.0
        */
        const ShadowGenCamera * getShadowGenCamera() const;

        /** ���ü���
        @version NIIEngine
        */
        void setListener(Listener * lis);
    protected:
        /** ������Ӱ�����
        @version NIIEngine 3.0.0 �߼�api
        */
        Camera * createCamera(SpaceID sid);

        /** ������Ӱ����
        @version NIIEngine 3.0.0
        */
        void createTexture();

        /** ɾ����Ӱ����
        @version NIIEngine 3.0.0
        */
        void destroyTexture();

        /** ��Ⱦ��Ӱ�������
        @version NIIEngine 3.0.0 �߼�api
        */
        void renderTexture(RenderGroup * rg, RenderSortMode om);

        /** �Ƶ�����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual ShaderCh * deriveReceiver(const ShaderCh * src);

        /** �Ƶ�Ͷ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual ShaderCh * deriveCaster(const ShaderCh * src);

        /** ����Ⱦ������ʱ�����⴦��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void setTextureFade(NIIf start, NIIf end);

        /** ����Ⱦ������ʱ�����⴦��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void setTextureAmbient();

        /** ����Ⱦ������ʱ�����⴦��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void setColourBlend(ShaderChTextureUnit * src);
    private:
        ShadowMappingData * mShaderData;
    protected:
        ShaderCh * mSTCCP;
        ShaderCh * mSTCRP;
        Listener * mListener;
        Texture * mCurrentTexture;
        StageType mRenderStage;
        TextureList mTextures;
        CameraList mCameras;
        MaterialList mMaterials;
        LightList mShadowLights;
        CameraLightList mCamLightList;
        ShadowTexCfgList mTexConfList;
        vector<NCount>::type mTextureIndexList;
        NIIf mFadeBegin;
        NIIf mFadeEnd;
        NIIf mExtentOffsetFactor;
        NCount mTextureCount[3];
        bool mSelfShadow;
        bool mShadowTexValid;
    };
}
#endif