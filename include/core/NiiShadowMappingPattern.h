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

#ifndef NII_SHADOWMAPPING_RENDERPATTERN_H
#define NII_SHADOWMAPPING_RENDERPATTERN_H

#include "NiiPreInclude.h"
#include "NiiRenderPattern.h"
#include "NiiCamera.h"
#include "NiiPixelFormat.h"
#include "NiiRenderQueue.h"
#include "NiiTextureManager.h"

namespace NII
{
    class ShadowMappingData;

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
        void setCullingMode(CullingMode mode);

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
        inline void setExtentOffsetFactor(NIIf oft)     { mExtentOffsetFactor = oft; }

        /** �������������Ӱ����ı���
        @version NIIEngine 3.0.0
        */
        inline NIIf getExtentOffsetFactor() const       { return mExtentOffsetFactor; }

        /** ������Ӱ��ʼ������������.
        @remark The default is 0.7
        */
        inline void setFadeBegin(NIIf start)            { mFadeBegin = start; }

        /** ������Ӱ���������ı�������
        @remark The default is 0.9.
        */
        inline void setFadeEnd(NIIf end)                { mFadeEnd = end; }

        /** �����Ƿ�Ͷ����Ӱ���Լ�
        @version NIIEngine 3.0.0
        */
        void setSelfShadow(bool b);

        /** ��ȡ�Ƿ�Ͷ����Ӱ���Լ�
        @version NIIEngine 3.0.0
        */
        inline bool isSelfShadow()                      { return mSelfShadow; }

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
        void setTexture(Nidx idx, const TextureDefine & config);

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
        inline void setTextureCount(LightType type, NCount count)   { mTextureCount[type] = count; }

        /** ��ȡ��Ӱ��������
        @version NIIEngine 3.0.0 �߼�api
        */
        inline NCount getTextureCount(LightType type) const         { return mTextureCount[type]; }

        /** ��Ӱ���������
        @version NIIEngine 3.0.0
        */
        void setShadowSetup(ShadowSetup * obj);

        /** ��Ӱ���������
        @version NIIEngine 3.0.0
        */
        const ShadowSetup * getShadowSetup() const;

        /** ���ü���
        @version NIIEngine
        */
        inline void setListener(Listener * lis)     { mListener = lis; }
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
        TextureDefineList mTexConfList;
        vector<NCount>::type mTextureIndexList;
        NIIf mFadeBegin;
        NIIf mFadeEnd;
        NIIf mExtentOffsetFactor;
        NCount mTextureCount[3];
        bool mSelfShadow;
        bool mTextureValid;
    };
}
#endif