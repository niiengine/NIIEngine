/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-1

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

#ifndef _NII_SHADERCH_TEXTURE_H_
#define _NII_SHADERCH_TEXTURE_H_

#include "NiiPreInclude.h"
#include "NiiTextureBlend.h"
#include "NiiTextureSample.h"
#include "NiiTexture.h"
#include "NiiMatrix4.h"

namespace NII
{
    /** �������ݼ�������
    @remark ���������Ĳ�������,������������ʹ�õ���Ϣ
    @version NIIEngine 3.0.0 �߼�api
    */
    enum TextureDataFetch
    {
        TDF_FS = 0,             ///< ����Ƭ�δ���Ԫ - Ĭ��.
        TDF_VS = 1,             ///< ���㴦��Ԫ
        TDF_Count = 2           ///< ������ɫ���򷶳�,��û�кܶ�ѡ��
    };

    /** �������괦��
    @note ���Ƶ������������� �� gltexcoord ����ʧЧ,һ��߹ⷴ͸�������Ҫ,��ǹ,��,ˮ��
    @version NIIEngine 3.0.0 �߼�api
    */
    enum TextureCoordPrc
    {
        TCP_NONE                = 0,    ///< ��������������
        TCP_ENV_SPHERE          = 1,    ///< ���ڳ�������
        TCP_ENV_NORMAL          = 2,    ///< ���ڶ��㷨��
        TCP_ENV_REFLECTION      = 3,    ///< ���ڶ���λ��
        TCP_ENV_REFLECTION_INV  = 4,    ///< ���ڷ���ƽ��
        TCP_ENV_PROJECTIVE      = 5,    ///< ����ͶӰ(�ӿ�)
        TCP_Count               = 6,    ///< ����ʹ�����е�,��Ϊ���ײ�APIԼ��,�ڲ���ʵ���и�����ϵ��
    };

    /** Ӧ������
    @remark
        �����ʹ�úͶ������й�ϵ,�����ͨ���Բ���ǿ,�����񶥵�/����������ֱ���й�
    @note ÿ������֧��255������,���ܳ���. ��������,������������Ⱦͨ·���л�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTexture : public ShaderAlloc
    {
        friend class ShaderCh;
    public:
        /** ����Ԫ��������
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_NORMAL = 0,       ///< ��ͨ��������(�Զ�)
            T_PHOTO = 1,        ///< ������Ƭ,��speedtree��Զ����/�����(Զ���Ķ�����������ģ����)
            T_SHADOW = 2,       ///< ��Ӱ����(������Ӱ������)
            T_INV_PHOTO = 3,    ///< ����/ˮ��Ӱ��Ƭ
            T_FUSION = 4,       ///< ֡�ϳ�
            T_HALO = 5,         ///< ����
            T_NORMALMAP = 6,    ///< ��������
            T_HIGHLIGHT = 7,    ///< �߹���Ƭ(�����Թ��������Ƭ����ɫ����,Ҳ������ͼƬģ��)
            T_Count = 8         ///< ��������
        };

        typedef vector<ShaderChTextureUnit *>::type Textures;
    public:
        ShaderChTexture(ShaderCh * ch);
        ShaderChTexture(ShaderCh * ch, const ShaderChTexture & o);
        ~ShaderChTexture();

        ShaderChTexture & operator =(const ShaderChTexture & o);

        /** �ֲ������߳�
        @see Resource::plan
        @version NIIEngine 3.0.0
        */
        void plan();

        /** �ֲ������߳�
        @see Resource::unplan
        @version NIIEngine 3.0.0
        */
        void unplan();

        /** �ֲ������߳�
        @see Resource::load
        @version NIIEngine 3.0.0
        */
        void load();

        /** �ֲ������߳�
        @see Resource::unload
        @version NIIEngine 3.0.0
        */
        void unload();

        /** ����ÿ������Ԫ���������
        @param[in] mode �������ģʽ
        @version NIIEngine 3.0.0
        */
        void setFilterMode(TextureFilterMode mode);

        /** ������������ʹ�õĸ������Եȼ�
        @param[in] value ֵ
        @version NIIEngine 3.0.0
        */
        void setAnisotropy(Nui32 value);

        /** ����һ��Ӧ������
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(Type type = T_NORMAL);

        /** ����һ��Ӧ������
        @param[in] rid ��ԴID(Ӧ�ó�����Ψһ)
        @param[in] slot
            �����׿�,������Ⱦϵͳһ��֧��ͬһʱ����Ⱦ�������,Ĭ��ֻ��һ��,����Ĭ��Ϊ0,
            �˴��������׿ڲ�����ֱ�������� Type ����������, Type �����岢���������׿�
            ��ͬ
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(ResourceID rid, Nidx slot = 0, Type type = T_NORMAL);

        /** ����һ��Ӧ������
        @param[in] tex �������
        @param[in] slot
            �����׿�,������Ⱦϵͳһ��֧��ͬһʱ����Ⱦ�������,Ĭ��ֻ��һ��,����Ĭ��Ϊ0,
            �˴��������׿ڲ�����ֱ�������� Type ����������, Type �����岢���������׿�
            ��ͬ
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(const Texture * tex, Nidx slot = 0, Type type = T_NORMAL);

        /** ��ӵ�Ӧ������Ԫ
        @param[in] unit
        @version NIIEngine 3.0.0
        */
        void add(ShaderChTextureUnit * unit);

        /** ��ȡӦ������Ԫ
        @param[in] index
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(Nidx index) const;

        /** ��ȡӦ������
        @param[in] name ����Ԫ����
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(const String & name) const;

        /** ��ȡ���͵�N������.
        @param[in] type ����
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * get(Type type, Nidx index) const;

        /** ��ȡ���������������
        @version NIIEngine 3.0.0 �߼�api
        */
        ShaderChTextureUnit * get(Type type, Textures::const_iterator & begin, Textures::const_iterator & end) const;

        /** �ƶ�Ӧ��������һ��Ӧ��������
        @remark Type([begin, end])�������ƶ�������o��
        @param[in] type ����
        @param[in] begin ��ʼ����
        @param[in] end ��������
        @param[in] o ��һ��Ӧ������
        @note ����ָ���Ǽ���ճ��,���Ǹ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void move(Type type, Nidx begin, Nidx end, ShaderChTexture * o);

        /** ��ȥӦ������
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** ��ȥӦ������
        @param[in] type
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(Type type, Nidx index);

        /** ��ȥ����Ӧ������
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ָ��������������
        @version NIIEngine 3.0.0
        */
        NCount getCount(Type type) const;

        /** Ӧ����������
        @remark ������������
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** ��ȡӦ�������б�
        @remark ���������ֱ��Ӱ������Ӧ������,ע���������
        @version NIIEngien 3.0.0 �߼�api
        */
        const Textures & getTextures() const;
    protected:
        ShaderChTexture();
    private:
        ShaderCh * mParent;
        Textures mTextures;
        Nui8 mMark[T_Count];
    };

    /** ��������Ԫ
    @remark
        ��Texture�಻ͬ,������л���״̬.�漰���������/����ӳ����ڿɱ�̹�����û��Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTextureUnit : public ShaderAlloc
    {
        friend class RenderSys;
    public:
        /** ����ӳ��ģʽ
        @version NIIEngine 3.0.0
        */
        enum EnvMapMode
        {
            ENM_Planar,     ///< ���������������λ�õ�ӳ��
            ENM_Sphere,     ///< ���������������λ�úͷ��ߵ�ӳ��
            ENM_Reflection, ///< ���������������λ�õķ�תӳ��
            ENM_Normal      ///< ����������ռ�ķ���
        };

        /** ����任
        @version NIIEngine 3.0.0
        */
        enum TextureTransformType
        {
            TTT_U_OFT,      ///< U��������ƫ��
            TTT_V_OFT,      ///< V��������ƫ��
            TTT_UV_OFT,     ///< UV��������ƫ��
            TTT_U_SCALE,    ///< U������������
            TTT_V_SCALE,    ///< V������������
            TTT_UV_SCALE,   ///< UV������������
            TTT_ROTATE,     ///< ������ת
            TTT_FUNCTION    ///< ��������
        };

        /** ����Ч��
        @version NIIEngine 3.0.0
        */
        struct TextureEffect
        {
            TextureTransformType mType;             ///< ��������
            TextureTransformType mEffect;           ///< ʵ��Ч������
            DataValue<NIIf> * mIn;                  ///< ������������
            NIIf mFactor;                           ///< ���������������
            DataFunc<TimeDurMS, NIIf> * mOutFunc;   ///< ���������������
            DataEquation<TimeDurMS, NIIf> * mTimer; ///< ֡������
        };
        typedef multimap<TextureTransformType, TextureEffect>::type EffectMap;
    public:
        ShaderChTextureUnit(ShaderCh * ch);
        ShaderChTextureUnit(ShaderCh * ch, ResourceID rid);
        ShaderChTextureUnit(ShaderCh * ch, const ShaderChTextureUnit & o);
        virtual ~ShaderChTextureUnit();

        ShaderChTextureUnit & operator = (const ShaderChTextureUnit & o);

        /** ��ȡͨ��
        @version NIIEngine 3.0.0
        */
        ShaderCh * getParent() const;

        /** ��������Ԫ������
        @remark ��������
        @version NIIEngine 3.0.0
        */
        void setName(const String & name);

        /** ��ȡ��Ԫ����
        @remark ��������
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const ResourceIDList & rid, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(ResourceID rid, const StringList & src, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const Texture * tex, Texture::Type type, ShaderChTexture::Type ctype = ShaderChTexture::T_NORMAL);

        /** ���úϳ�
        @param[in] framefusion ֡�ϳ�
        @param[in] tex �ϳ�����
        @version NIIEngine 3.0.0
        */
        void setFusion(ResourceID fusion, const String & tex);

        /** ���ö���
        @param rid ��ԴID�� [rid[0], rid[count - 1]] ����
        @param count ��Ƭ����
        @param ms ������ʱ��
        @version NIIEngine 3.0.0
        */
        void setAni(const ResourceIDList & rid, TimeDurMS ms = 0);

        /** ��ȡ��ǰ����
        @version NIIEngine 3.0.0
        */
        const Texture * getTexture() const;

        /** ��ȡ֡����
        @version NIIEngine 3.0.0
        */
        const Texture * getTexture(NCount frame) const;

        /** ���õ�ǰ֡������ָ��
        @version NIIEngine 3.0.0
        */
        void setTexture(const Texture * tex);

        /** ����һ��֡,��������ָ��
        @version NIIEngine 3.0.0
        */
        void setTexture(const Texture * tex, NCount frame);

        /** ��ȡ��ǰ����ͼ���
        @version NIIEngine 3.0.0
        */
        ResourceID getResourceID() const;

        /** ��ȡ�ϳ���
        @version NIIEngine 3.0.0
        */
        ResourceID getFrameFusion() const;

        /** ��ȡ�ϳ�����
        @version NIIEngine 3.0.0
        */
        const String & getFusionTexture() const;

        /** ��������.
        @remark
        @param[in] u fn(u) = right, fn(-u) = left
        @param[in] v fn(v) = up, fn(-v) = down
        @version NIIEngine 3.0.0
        */
        void setScrollAnimation(NIIf u, NIIf v);

        /** ���Ŷ���
        @param[in] u ϵ��
        @param[in] v ϵ��
        @version NIIEngine 3.0.0
        */
        void setScaleAnimation(NIIf u, NIIf v);

        /** ��ת����
        @param[in] f ϵ��
        @version NIIEngine 3.0.0
        */
        void setRotateAnimation(NIIf f);

        /** ���ú��Ӷ���
        @note ʹ�� [0.0, 1.0] ���귶Χ,���Ծ�Ϊ NIIf ����
        @param[in] in �����������
        @param[in] out ����������
        @version NIIEngine 3.0.0
        */
        void addAnimation(TextureTransformType ttt, DataValue<NIIf> * in, DataFunc<TimeDurMS, NIIf> * out);

        /** ��ȥЧ��
        @param[in] type Ч������
        @version NIIEngine 3.0.0
        */
        void remove(TextureTransformType type);

        /** ��ȥ���Ӧ�õ��������������Ч��
        @version NIIEngine 3.0.0
        */
        void removeAllEffects();

        /** ��ȡ��һ������ӳ����Ե�����Ч������
        @version NIIEngine 3.0.0
        */
        const EffectMap & getEffects() const;

        /** ����ʱ��
        @remark ��������ר��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getAniTime() const;

        /** ���û���ӳ��
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setEnvMap(bool b);

        /** �Ƿ񻷾�ӳ��
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        bool isEnvMap() const;

        /** ���û���ӳ������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setEnvMapType(EnvMapMode type);

        /** ��ȡ����ӳ������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        EnvMapMode getEnvMapType() const;

        /** �����Ƿ�ͶӰ����
        @note �ڿɱ�̹�����û��Ч��
        @param[in] b ���ʹ��,����Ҫ���� setProjFrustum
        @version NIIEngine 3.0.0
        */
        void setProjTexture(bool b);

        /** ��ȡ�Ƿ�ͶӰ����
        @note �ڿɱ�̹�����û��Ч��,���ʹ��,����Ҫ����setProjFrustum
        @version NIIEngine 3.0.0
        */
        bool isProjTexture() const;

        /** ����ͶӰ�����ͶӰ�ռ�
        @note �ڿɱ�̹�����û��Ч��
        @param[in] proj ͶӰ�ռ�
        @version NIIEngine 3.0.0
        */
        void setProjFrustum(Frustum * proj);

        /** ��ȡͶӰ�����ͶӰ�ռ�
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        const Frustum * getProjFrustum() const;

        /** ���һ���������ֵ�֡������ĩβ
        @param[in] name ��������
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        void addFrame(ResourceID rid);

        /** ɾ��һ��ָ������֡.�������û�б�ɾ��,�������������������Ԫ��
            ʹ��.����һ���쳣,���֡����������ʵ�ʵ�֡��
        @param[in] frame
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        void removeFrame(NCount frame);

        /** ���ù���һ��֡����������
        @param[in] name ��������
        @param[in] frame The frame the texture name is to be placed in
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        void setFrame(ResourceID rid, NCount frame);

        /** ��ȡ����һ��֡��������������. �׳�һ���쳣,���frameNumber�����˴洢֡������.
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        ResourceID getFrame(NCount frame) const;

        /** Ϊһ�����ȡ֡����.
        @note �̶����ߺͿɱ�̹�������Ч��
        */
        NCount getFrameCount() const;

        /** ���û֡
        @remark ���û֡
        @note �̶����ߺͿɱ�̹�������Ч��
        */
        void setCurrentFrame(NCount frame);

        /** ��ȡ�֡
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        NCount getCurrentFrame() const;

        /** ���������ʹ���������꼯������
        @note Ĭ��0,����ʹ�ö��������������
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        void setCoordSet(Nidx index);

        /** ��ȡ�����ʹ���������꼯������
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        Nidx getCoordSet() const;

        /** �����������ݼ�������
        @version NIIEngine 3.0.0 �߼�api
        */
        void setFetchType(TextureDataFetch bt);

        /** ��ȡ�������ݼ�������
        @verison NIIEngien 3.0.0 �߼�api
        */
        TextureDataFetch getFetchType() const;

        /** �������ݵ�����
        @remark
        @version NIIEngine 3.0.0
        */
        void setContentType(ShaderChTexture::Type type);

        /** ��ȡ���ݵ�����
        @version NIIEngine 3.0.0
        */
        ShaderChTexture::Type getContentType() const;

        /** ��ȡ�Ƿ���6����Ԫ���
        @note �̶����ߺͿɱ�̹����ж���Ч
        */
        bool isCubic() const;

        /** ����������������.
        @note Ӧ�õ��̶����ߺͿɱ�̹���
        @version NIIEngine 3.0.0
        */
        Texture::Type getType() const;

        /** �������ظ�ʽ
        @remark ����
        @version NIIEngine 3.0.0
        */
        void setFormat(PixelFormat pf);

        /** ��ȡ���ظ�ʽ
        @remark ����
        @version NIIEngine 3.0.0
        */
        PixelFormat getFormat() const;

        /** ����
        @remark ����
        @version NIIEngine 3.0.0
        */
        void setMipmapCount(NIIi count);

        /** ��ȡ
        @remark ����
        @version NIIEngine 3.0.0
        */
        NIIi getMipmapCount() const;

        /** �����Ƿ�Alphaͨ��
        @version NIIEngine 3.0.0
        */
        void setIsAlpha(bool b);

        /** ��ȡ�Ƿ�Alphaͨ��
        @version NIIEngine 3.0.0
        */
        bool getIsAlpha() const;

        /// @copydetails Texture::setHardwareSRGB
        void setHardwareSRGB(bool b);

        /// @copydetails Texture::isHardwareSRGB
        bool isHardwareSRGB() const;

        /** ����(����)�е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setLocalIndex(Nidx index);

        /** ��ȡ(����)�е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        Nidx getLocalIndex() const;

        /** ���ø����е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        void setMultiIndex(Nidx index);

        /** ��ȡ�����е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        Nidx getMultiIndex() const;

        /** ����U�������ֵ
        @param[in] value
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngien 3.0.0
        */
        void setUOft(NIIf value);

        /** ��ȡU�������ֵ
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngien 3.0.0
        */
        NIIf getUOft() const;

        /** ����V�������ֵ
        @param[in] value
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngien 3.0.0
        */
        void setVOft(NIIf value);

        /** ��ȡV�������ֵ
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngien 3.0.0
        */
        NIIf getVOft() const;

        /** ����U��������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setUScale(NIIf value);

        /** ��ȡU��������
        @version NIIEngine 3.0.0
        */
        NIIf getUScale() const;

        /** ����V��������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setVScale(NIIf value);

        /** ��ȡV��������
        @version NIIEngine 3.0.0
        */
        NIIf getVScale() const;

        /** ����Ӧ���������������ʱ����ת����
        @note �ڿɱ�̹�����û��Ч��
        @param[in] angle ��ת�Ƕ�(��ʱ��)
        */
        void setRotate(const Radian & r);

        /** ��ȡ������תЧ��angleֵ
        @version NIIEngine 3.0.0
        */
        const Radian & getRotate() const;

        /** ��ȡ���ջ���
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngien 3.0.0
        */
        void setMatrix(const Matrix4f & x);

        /** ��ȡ���ջ���
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        const Matrix4f & getMatrix() const;

        /** ��ȡ��������ɫ���
        @version NIIEngine 3.0.0 �߼�api
        */
        inline TextureBlend * getColourBlend() const { return const_cast<TextureBlend *>(&mColourBlend); }

        /** ��ȡ������͸���Ȼ��
        @version NIIEngine 3.0.0 �߼�api
        */
        inline TextureBlend * getAlphaBlend() const { return const_cast<TextureBlend *>(&mAlphaBlend); }

        /** ��ȡ��������ģʽ
        @version NIIEngien 3.0.0 �߼�api
        */
        inline TextureSample * getSample() const { return const_cast<TextureSample *>(&mSample); }

        /** ״̬/���ʱ仯��
        @version NIIEngine 3.0.0
        */
        void notify();

        /** ������ı���
        @version NIIEngine 3.0.0
        */
        void notify(ShaderCh * ch);

        /** ������ز���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void plan();

        /** �Ƴ����ز���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void unplan();

        /** ����
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void load();

        /** ж��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void unload();

        /** �Ƿ��Ѿ�����
        @version NIIEngien 3.0.0
        */
        bool isLoad() const;

        /** �����Ƿ���ռ����
        @remark һ���漰���任��,�ͺͿռ��й�ϵ
        @version NIIEngine 3.0.0
        */
        virtual bool isMatrixRelation() const;

        /** �����Ƿ�����������
        @remark
            ����漰����ӳ��������Ǻ�������й�ϵ��,��ͬ�������������,��Ҫ
            һ����λ/��λ�����
        @version NIIEngien 3.0.0
        */
        virtual bool isCameraRelation() const;
    protected:
        /// ���ռ��ص���Դ
        void recover();
    protected:
        ShaderCh * mParent;
        DataEquation<TimeDurMS, NIIf> * mTimeFunc;
        String mName;
        ResourceID mFrameFusion;            ///< ֡�ϳ�
        String mFusionTex;                  ///< �ϳ�����
        TimeDurMS mAnimDuration;            ///< ��������(��λ:����)
        Texture::Type mTextureType;         ///< ��������
        NCount mCurrentFrame;               ///< ��ǰ����֡
        ShaderChTexture::Type mContentType; ///< ������������
        TextureDataFetch mFetchType;        ///< ���ݼ���ģʽ
        TextureAddressing mAddressMode;     ///< �����ַģʽ
        EnvMapMode mEnvMapType;             ///< ����ӳ������
        Frustum * mProjFrustum;             ///< ����ͶӰ�ռ�
        PixelFormat mFormat;                ///< ���ظ�ʽ
        NIIi mMipmapCount;                  ///< �������
        EffectMap mEffects;
        Nui16 mCoordSet;
        Nui16 mLocalIndex;
        Nui16 mMultiIndex;
        TextureSample mSample;
        TextureBlend mColourBlend;
        TextureBlend mAlphaBlend;

        NIIf mUOft, mVOft, mUScale, mVScale;
        Radian mRotate;
        mutable Matrix4f mMatrix;
        ResourceIDList mTexIDList;
        mutable TextureList mTextures;
        mutable Nmark mMark;
    };
}
#endif