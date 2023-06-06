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

#ifndef _NII_SHADERCH_TEXTURE_H_
#define _NII_SHADERCH_TEXTURE_H_

#include "NiiPreInclude.h"
#include "NiiGpuProgram.h"
#include "NiiMatrix4.h"
#include "NiiColour.h"

namespace NII
{
    /** ����Ѱַģʽ
    @version NIIEngine 3.0.0
    */
    enum TextureAddressingMode
    {
        TAM_REPEAT = 0,                 ///< ����1.0,�򷵻�(1.0 - x)
        TAM_MIRROR = 1,                 ///< ����1.0,�����ظ�
        TAM_CLAMP = 2,                  ///< ������[0.0-1.0]��
        TAM_BORDER = 3,                 ///< ����0.0-1.0��Χ,����Ϊ��Ե��ɫ
        TAM_Count = 4                   ///< ����ʹ�����е�,��Ϊ���ײ�APIԼ��,����ֻ�ܴ�ͼƬ��������
    };

    /** �����ַģʽ,����ÿ����������
    @version NIIEngine 3.0.0
    */
    struct TextureAddressing
    {
        TextureAddressingMode mS_U;     ///< x����
        TextureAddressingMode mT_V;     ///< y����
        TextureAddressingMode mR_W;     ///< w����
    };

    /** �����������
    @version NIIEngine 3.0.0
    */
    enum TextureFilterType
    {
        TFT_MIN = 0,                    ///< ����ʱ
        TFT_MAG = 1,                    ///< �Ŵ�ʱ
        TFT_MIP = 2,                    ///< mipmapʱ
        TFT_Count = 3                   ///< ����ʹ�����е�,��Ϊ����ȾϵͳԼ��,����ֻ�ܴ�ͼƬ��������
    };

    /** ������˲���
    @version NIIEngine 3.0.0
    */
    enum TextureFilterOP
    {
        TFOP_NONE = 0,                  ///< ������
        TFOP_POINT = 1,                 ///< λ����ӽ�����
        TFOP_LINEAR = 2,                ///< ����ȡֵ
        TFOP_ANISOTROPIC = 3,           ///< ����ȡֵ + �����Ƕ�
        TFOP_Count = 4                  ///< ����ʹ�����е�,��Ϊ����ȾϵͳԼ��,����ֻ�ܴ�ͼƬ��������
    };

    /** �������ģʽ
    @remark �������Ԥ���
    @version NIIEngine 3.0.0
    */
    enum TextureFilterMode
    {
        TFM_BILINEAR = 0,               ///< LINEAR, LINEAR, POINT
        TFM_TRILINEAR = 1,              ///< LINEAR, LINEAR, LINEAR
        TFM_ANISOTROPIC = 2,            ///< ANISOTROPIC, ANISOTROPIC, LINEAR
        TFM_NONE = 3,                   ///< POINT, POINT, NONE (С����Ϸ����)
        TFM_Count = 4                   ///< ����ʹ�����е�,��Ϊ���ײ�APIԼ��,����ֻ�ܴ�ͼƬ��������
    };

    /** �����ϲ���������
    @version NIIEngine 3.0.0
    */
    enum TextureColourType
    {
        TCT_RGB,        ///< (Rs��Gs, Bs)
        TCT_AAA         ///< (As, As, As)
    };

    /** ����������
    @remark
    @version NIIEngine 3.0.0
    */
    enum TextureBlendMode
    {
        TBM_Src_Replace_Dst,    ///< ���������
        TBM_Src_Add_Dst,        ///< �������ǰ������
        TBM_Src_Sub_Dst,        ///< �������ǰ������
        TBM_Src_Modulate_Dst,   ///< �������ǰ������
        TBM_Src_Alpha_Dst,      ///< �������ǰ�����͸���ȼ�Ȩ
        TBM_Count               ///< ����ʹ�����е�,��Ϊ����ȾϵͳԼ��,�������ʹ��Ƭ����ɫ����
    };

    /** �����һ/�ڶ�����������
    @version NIIEngine 3.0.0
    */
    enum TextureBlendSource
    {
        TBS_PRE_TEXTURE,        ///< ǰһ�������/�׶������ɫ.�統ǰ���ײ����,��Ϊ������������ɫ.
        TBS_TEXTURE,            ///< ��ǰ������������ɫ
        TBS_DIFFUSE,            ///< �����������ֵ(GOURAUD),��Ⱦϵͳ����Ϊ���ⷴ�����ɫ
        TBS_SPECULAR,           ///< ���㾵�淴���ֵ(GOURAUD),��Ⱦϵͳ����Ϊ���ⷴ�����ɫ
        TBS_CONSTANT,           ///< �Զ��峣��
        TBS_Count               ///< ����ʹ�����е�,��Ϊ����ȾϵͳԼ��,�������ʹ��Ƭ����ɫ����
    };

    /** �����һ/�ڶ���������������
    @note �������������ڲ�����
    @version NIIEngine 3.0.0
    */
    enum TextureBlendOp
    {
        TBO_OPERAND1,       ///< ����һ��,fn(src1, src2) = src1
        TBO_OPERAND2,       ///< ���ض���,fn(src1, src2) = src2
        TBO_OPERAND3,       ///< ��������,fn(src1, src2, src3) = src3
        TBO_MODULATE,       ///< ������˺����,fn(src1, src2) = src1 * src2
        TBO_MODULATE_2X,    ///< ������˺��ٳ���2,fn(src1, src2) = src1 * src2 * 2
        TBO_MODULATE_4X,    ///< ������˺��ٳ���4,fn(src1, src2) = src1 * src2 * 4
        TBO_SUB,            ///< �������,fn(src1, src2) = src1 - src2
        TBO_ADD,            ///< �������,fn(src1, src2) = src1 + src2
        TBO_ADD_SPECULAR,   ///< ��TBO_ADD, fn(src1, src2, TBS_PRE_TEXTURE) = (src1 * TBS_PRE_TEXTURE) + (src2 * (1-TBS_PRE_TEXTURE))
        TBO_ADD_DIFFUSE,    ///< ��TBO_ADD, fn(src1, src2, TBS_DIFFUSE) = (src1 * TBS_DIFFUSE) + (src2 * (1-TBS_DIFFUSE))
        TBO_ADD_TEXTURE,    ///< ��TBO_ADD, fn(src1, src2, GL_TEXTURE) = (src1 * GL_TEXTURE) + (src2 * (1-GL_TEXTURE))
        TBO_ADD_PRE_TEXTURE,///< ��TBO_ADD, fn(src1, src2, TBS_PRE_TEXTURE) = (src1 * TBS_PRE_TEXTURE) + (src2 * (1-TBS_PRE_TEXTURE))
        /** ������Ӻ��ȥ0.5���.(�䷶Χ������[-0.5, 0.5]��.������Ӻ�,�Ƚϰ��Ĳ���,
            ����������,����ԭ�����������˥��,�������������ԭ����������ǿ.������ϸ��
            ����.����е��Կ���֧�� D3DTOP_ADDSIGNED��ʽ,�ɿ��ǲ��� TBO_MODULATE_2X
            ��ģ������Ч��). fn(src1, src2) = src1 + src2 �C 0.5
        */
        TBO_ADD_SIGNED,
        TBO_ADD_SIGNED_2X,  ///< ͬ��,����ٳ���2,fn(src1, src2) = (src1 + src2 �C 0.5) * 2
        TBO_ADD_SMOOTH,     ///< ���κͼ�ȥ���εĻ�,fn(src1, src2, src3) = (src1 * src3) + (src2 * (1-src3))
        TBO_DOT_PRODUCT,    ///< ��LBX_MODULATE, fn(src1, src2) = dot(src1, src2)
        TBO_CONSTANT_FACTOR,///< fn(src1) = (Rs * src1, Gs * src1, Bs * src1, As * src1)
        TBO_Count           ///< ����ʹ�����е�,��Ϊ����ȾϵͳԼ��,�������ʹ��Ƭ����ɫ����
    };
    
    /** ������״̬��������
    @note �ɱ�̹�������Ч
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureBlend : public ShaderAlloc
    {
    public:
        TextureBlend();
        TextureBlend(TextureColourType type);
        TextureBlend(TextureColourType type, TextureBlendOp op, TextureBlendSource op1, TextureBlendSource op2);
        ~TextureBlend();

        void operator =(const TextureBlend & o);
        
        bool operator==(const TextureBlend & o) const;

        bool operator!=(const TextureBlend & o) const;

        /** ������ɫ���ģʽ
        @note �ɱ�̹�������Ч
        @param[in] op ģʽ
        @version NIIEngine 3.0.0
        */
        void setColourBlend(TextureBlendMode mode);

        /** ����͸���Ȼ��ģʽ
        @note �ɱ�̹�������Ч
        @param[in] op ģʽ
        @version NIIEngine 3.0.0
        */
        void setAlphaBlend(TextureBlendMode mode);

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = op2;
        }

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendSource op1, TextureBlendSource op2, NIIf factor)
        {
            mOP = TBO_CONSTANT_FACTOR;
            mColourSrc1 = op1;
            mColourSrc2 = op2;
            mConstantFactor = factor;
        }

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, const Colour & c1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = TBS_CONSTANT;
            mColourSrc2 = op2;
            mConstant1 = c1;
        }

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, const Colour & c2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = TBS_CONSTANT;
            mConstant2 = c2;
        }

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, NIIf f1, TextureBlendSource op2)
        {
            mOP = op;
            mColourSrc1 = TBS_CONSTANT;
            mColourSrc2 = op2;
            mConstant1.a = f1;
        }

        /** �����
        @version NIIEngine 3.0.0
        */
        inline void setBlend(TextureBlendOp op, TextureBlendSource op1, NIIf f2)
        {
            mOP = op;
            mColourSrc1 = op1;
            mColourSrc2 = TBS_CONSTANT;
            mConstant2.a = f2;
        }

        /** ���û�ϲ���
        @version NIIEngine 3.0.0
        */
        inline void setBlendOp(TextureBlendOp op)              { mOP = op; }

        /** ������ɫ����
        @version NIIEngine 3.0.0
        */
        inline void setBlendType(TextureColourType type)       { mColourType = type; }

        /** ���õ�һ������
        @param[in] tbs ������
        @version NIIEngine 3.0.0
        */
        inline void setBlendSource1(TextureBlendSource tbs)    { mColourSrc1 = tbs; }

        /** ���õڶ�������
        @param[in] tbs ������
        @version NIIEngine 3.0.0
        */
        inline void setBlendSource2(TextureBlendSource tbs)    { mColourSrc2 = tbs; }

        /** ���ó�����һ������
        @note TextureBlendSource Ϊ TBS_CONSTANT �� TextureColourType Ϊ TCT_RGB ʱʹ��
        @version NIIEngine 3.0.0
        */
        inline void setConstant1(const Colour & c1)            { mConstant1 = c1; }

        /** ���ó����ڶ�������
        @note TextureBlendSource Ϊ TBS_CONSTANT �� TextureColourType Ϊ TCT_RGB ʱʹ��
        @version NIIEngine 3.0.0
        */
        inline void setConstant2(const Colour & c2)            { mConstant2 = c2; }

        /** ���ó�����һ������
        @note TextureBlendSource Ϊ TBS_CONSTANT �� TextureColourType Ϊ TCT_AAA ʱʹ��
        @version NIIEngine 3.0.0
        */
        inline void setConstant1(NIIf f1)                      { mConstant1.a = f1; }

        /** ���ó�����һ������
        @note TextureBlendSource Ϊ TBS_CONSTANT �� TextureColourType Ϊ TCT_AAA ʱʹ��
        @version NIIEngine 3.0.0
        */
        inline void setConstant2(NIIf f2)                      { mConstant2.a = f2; }

        /** ���ó����������
        @note TextureBlendOp Ϊ TBO_CONSTANT_FACTOR ʱʹ��
        @version NIIEngine 3.0.0
        */
        inline void setConstantFactor(NIIf f)                  { mConstantFactor = f; }
    public:
        TextureBlendOp mOP;
        TextureColourType mColourType;
        TextureBlendSource mColourSrc1;
        TextureBlendSource mColourSrc2;
        NIIf mConstantFactor;
        Colour mConstant1;
        Colour mConstant2;
    };
    
    /** ��������ģʽ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureSampleUnit : public ShaderAlloc
    {
        friend ShaderChTextureUnit;
    public:
        TextureSampleUnit();
        TextureSampleUnit(TextureFilterOP min, TextureFilterOP mag, TextureFilterOP mip,
            TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w);
        virtual ~TextureSampleUnit();

        TextureSampleUnit & operator =(const TextureSampleUnit & o);
        bool operator !=(const TextureSampleUnit & o) const;
        bool operator ==(const TextureSampleUnit & o) const;
        bool operator <(const TextureSampleUnit & o) const;

        /** ����Ѱַģʽ
        @note Ĭ���� TAM_REPEAT,�ڿɱ�̹��߻�̶���������Ч
        @version NIIEngine 3.0.0
        */
        inline void setMode(const TextureAddressing & uvw)  { mAddressMode = uvw; }

        /** ����Ѱַģʽ
        @note Ĭ���� TAM_REPEAT,�ڿɱ�̹��߻�̶���������Ч
        @version NIIEngine 3.0.0
        */
        void setMode(TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w);

        /** ��ȡѰַģʽ
        @note Ĭ���� TAM_REPEAT,�ڿɱ�̹��߻�̶���������Ч
        @version NIIEngine 3.0.0
        */
        inline const TextureAddressing & getMode() const { return mAddressMode; }

        /** ���ù���ģʽ
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterMode type);

        /** ���ù���ģʽ
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterType type, TextureFilterOP op);

        /** ���ù���ģʽ
        @version NIIEngine 3.0.0
        */
        void setFiltering(TextureFilterOP minop, TextureFilterOP magop, TextureFilterOP mipop);

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        TextureFilterOP getFiltering(TextureFilterType type) const;

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMinFiltering() const { return mMinOP; }

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMagFiltering() const { return mMagOP; }

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        inline TextureFilterOP getMipFiltering() const { return mMipOP; }

        /** ���ÿ���ݵȼ�
        @param[in] max Ĭ��Ϊ1
        @note �����Ⱦ����Ӧ���ڹ̶����ߺͿɱ�̹�����
        @version NIIEngine 3.0.0
        */
        inline void setAnisotropy(Nui32 aniso)      { mMaxAniso = aniso; }

        /** ���ÿ���ݵȼ�
        @param[in] max Ĭ��Ϊ1
        @note �����Ⱦ����Ӧ���ڹ̶����ߺͿɱ�̹�����
        @version NIIEngine 3.0.0
        */
        inline Nui32 getAnisotropy() const          { return mMaxAniso; }

        /** ����Ӧ�õ�mipmap�����ƫ��ֵ
        @remark Mipmap ���������LOD����
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngine 
        */
        inline void setMipmapBias(NIIf bias)        { mMipmapBias = bias; }

        /** ��ȡӦ�õ�mipmap�����ƫ��ֵ
        @remark Mipmap ���������LOD����
        @param[in] bias fn(x) = mip-; fn(-x) = mip+;
        @version NIIEngien 3.0.0
        */
        inline NIIf getMipmapBias() const           { return mMipmapBias; }

        /** ���ñ�Ե��ɫ
        @remark TAM_BORDERģʽ����
        @version NIIEngien 3.0.0
        */
        inline void setBorder(const Colour& colour) { mBorder = colour; }

        /** ��ȡ��Ե��ɫ
        @remark TAM_BORDERģʽ����
        @version NIIEngien 3.0.0
        */
        inline const Colour & getBorder() const     { return mBorder; }

        /** ���ñȽϺ���
        @version NIIEngine 3.0.0
        */
        inline void setCompareFunc(CmpMode function) { mCompareFunc = function; }

        /** ��ȡ�ȽϺ���
        @version NIIEngien 3.0.0
        */
        inline CmpMode getCompareFunc() const       { return mCompareFunc; }
    protected:
        TextureFilterOP mMinOP;
        TextureFilterOP mMagOP;
        TextureFilterOP mMipOP;
        TextureAddressing mAddressMode;
        CmpMode mCompareFunc;
        Colour mBorder;
        Nui32 mMaxAniso;
        NIIf mMipmapBias;
        NIIf mMinLod;
        NIIf mMaxLod;
    };
    
    inline void TextureSampleUnit::setMode(TextureAddressingMode u, TextureAddressingMode v, TextureAddressingMode w)
    {
        mAddressMode.mS_U = u;
        mAddressMode.mT_V = v;
        mAddressMode.mR_W = w;
    }

    inline void TextureSampleUnit::setFiltering(TextureFilterOP minop, TextureFilterOP magop, TextureFilterOP mipop)
    {
        mMinOP = minop;
        mMagOP = magop;
        mMipOP = mipop;
    }
	
	typedef vector<TextureSampleUnit *>::type TextureSampleUnitList;
    
    /** ��������ģʽ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TextureSample : public ShaderAlloc
    {
        friend ShaderChTextureUnit;
    public:
        typedef map<VString, TextureSampleUnit>::type UnitList;
    public:
        TextureSample();
        virtual ~TextureSample();
        
        TextureSample & operator = (const TextureSample & o) const;
        
        bool operator == (const TextureSample & o) const;
        
        bool operator != (const TextureSample & o) const;
        
        bool operator < (const TextureSample & o) const;
        
        /** ���
        @version NIIEngine 3.0.0
        */
        void add(const VString & name, const TextureSampleUnit & unit);
        
        /** ��ȥ
        @version NIIEngine 3.0.0
        */
        void remove(const VString & name);
        
        /** ����
        @version NIIEngine 3.0.0
        */
        void set(Nidx slot, const VString & name);
        
        /** ��ȡ
        @version NIIEngine 3.0.0
        */
        TextureSampleUnit * get(const VString & name) const;
        
        /** ��ȡ
        @version NIIEngine 3.0.0
        */
        TextureSampleUnit * get(Nidx slot) const;
        
        /** ��ȡ�б�
        @version NIIEngine 3.0.0
        */
        inline const TextureSampleUnitList & getList() const         		    { return mSetList; }

        /** ����ʹ������
        @version NIIEngine 3.0.0
        */
        inline void setUsedCount(GpuProgram::ShaderType type, NCountb cnt) const{ N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type] = cnt; }
        
        /** ��ȡʹ������
        @version NIIEngine 3.0.0
        */
        inline NCountb getUsedCount(GpuProgram::ShaderType type) const          { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type]; }
    protected:
        UnitList mUnitList;
        TextureSampleUnitList mSetList;
        NCountb mUsed[GpuProgram::ST_Cnt];
    };
    typedef vector<TextureSample *>::type TextureSampleList;
    
    /** �������ݼ�������
    @remark ���������Ĳ�������,����������ɫ����ʹ�õ���Ϣ
    @version NIIEngine 3.0.0 �߼�api
    */
    enum TextureDataFetch
    {
        TDF_FS      = 0,            ///< ����Ƭ�δ���Ԫ - Ĭ��.
        TDF_TS      = 1,            ///< ϸ����
        TDF_DS      = 2,            ///< ϸ��������
        TDF_VS      = 3,            ///< ���㴦��Ԫ
        TDF_GS      = 4,            ///< ���δ���
        TDF_CS      = 5,            ///< ���㴦��
        TDF_Count   = 6             ///< ������ɫ���򷶳�,��û�кܶ�ѡ��
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

    class ShaderChTexture;
    typedef vector<ShaderChTexture *>::type ShaderChTextureList;
    
    /** ��������Ԫ
    @remark
        ��Texture�಻ͬ,������л���״̬.�漰���������/����ӳ����ڿɱ�̹�����û��Ч��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTextureUnit : public ShaderAlloc
    {
        friend class RenderSys;
    public:
        /** ����ԪӦ������
        @remark ������ɫ�����Զ�����
        @version NIIEngine 3.0.0
        */
        enum Type
        {
            T_NORMAL = 0,       ///< ��ͨ��������/����(Ĭ��)
            T_PHOTO = 1,        ///< ������Ƭ,��speedtree��Զ����/�����(Զ���Ķ�����������ģ����)
            T_SHADOW = 2,       ///< ��Ӱ����(������Ӱ������)
            T_INV_PHOTO = 3,    ///< ����/ˮ��Ӱ��Ƭ(renderFrameObject)
            T_FUSION = 4,       ///< ֡�ϳ�
            T_HALO = 5,         ///< ����
            T_NORMALSMAP = 6,   ///< ��������
            T_HIGHLIGHT = 7,    ///< �߹���Ƭ(�����Թ��������Ƭ����ɫ����,Ҳ������ͼƬģ��)
            T_Count = 8         ///< ��������
        };
    
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
        
        struct TextureView
        {
            NCount mMipmap;
            NCount mMipmapCount;
            NCount mLayer;
            NCount mLayerCount;
            PixelFormat mPixelFormat;
            bool mAs2DArray;
        };
        
        struct BufferData
        {
            TextureBuffer * mBuffer;
            NCount mOffet;
            NCount mCount;
        };
    public:
        ShaderChTextureUnit();
        ShaderChTextureUnit(ResourceID rid);
        ShaderChTextureUnit(const ShaderChTextureUnit & o);
        virtual ~ShaderChTextureUnit();

        ShaderChTextureUnit & operator = (const ShaderChTextureUnit & o);
        
        /** ������Դ��
        @version NIIEngine 3.0.0
        */
        inline void setResourceGroup(GroupID gid)              { mResGroupID = gid; }
        
        /** ��ȡ��Դ��
        @version NIIEngine 3.0.0
        */
        inline GroupID getResourceGroup() const                { return mResGroupID; }

        /** ��������Ԫ������
        @remark ��������
        @version NIIEngine 3.0.0
        */
        inline void setName(const String & name)               {mName = name; }

        /** ��ȡ��Ԫ����
        @remark ��������
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const                  { return mName; }

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const ResourceIDList & rid, Texture::Type type, Type ctype = T_NORMAL);

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(ResourceID rid, const StringList & src, Texture::Type type, Type ctype = T_NORMAL);

        /** ������Դ
        @note T_CUBE ������Ҫ���� setEnvMap(true) �� setEnvMapType(ENM_Sphere).
        @version NIIEngine 3.0.0
        */
        void setSrc(const Texture * tex, Texture::Type type, Type ctype = T_NORMAL);

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
        inline const Texture * getTexture() const              { return getTexture(mCurrentFrame); }

        /** ��ȡ֡����
        @version NIIEngine 3.0.0
        */
        const Texture * getTexture(NCount frame) const;

        /** ���õ�ǰ֡������ָ��
        @version NIIEngine 3.0.0
        */
        inline void setTexture(const Texture * tex)            { setTexture(tex, mCurrentFrame); }

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
        inline ResourceID getFrameFusion() const               { return mFrameFusion; }

        /** ��ȡ�ϳ�����
        @version NIIEngine 3.0.0
        */
        inline const String & getFusionTexture() const         { return mFusionTex; }

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
        inline const EffectMap & getEffects() const                { return mEffects; }

        /** ����ʱ��
        @remark ��������ר��
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getAniTime() const                        { return mAnimDuration; }
        
        /** �����Ƿ��Զ�����
        @version NIIEngine 3.0.0
        */
        void setAutoLoad(bool b);
        
        /** �����Ƿ��Զ�����
        @version NIIEngine 3.0.0
        */
        bool isAutoLoad() const;

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
        inline void setEnvMapType(EnvMapMode type)                 { mEnvMapType = type; }

        /** ��ȡ����ӳ������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        inline EnvMapMode getEnvMapType() const                    { return mEnvMapType; }

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
        inline void setProjFrustum(Frustum * proj)                 { mProjFrustum = proj; }

        /** ��ȡͶӰ�����ͶӰ�ռ�
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        inline const Frustum * getProjFrustum() const              { return mProjFrustum; }

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
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        void setFrame(ResourceID rid, NCount frame);
        
        /** ��ȡ����һ��֡��������������. �׳�һ���쳣,���frameNumber�����˴洢֡������.
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        inline ResourceID getFrame(NCount frame) const     { N_assert (frame >= mTexIDList.size(), ""); return mTexIDList[frame]; }

        /** Ϊһ�����ȡ֡����.
        @note �̶����ߺͿɱ�̹�������Ч��
        */
        inline NCount getFrameCount() const                { return mTexIDList.size(); }

        /** ���û֡
        @remark ���û֡
        @note �̶����ߺͿɱ�̹�������Ч��
        */
        inline void setCurrentFrame(NCount frame)          { N_assert (frame < mTexIDList.size(), ""); mCurrentFrame = frame; }

        /** ��ȡ�֡
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        inline NCount getCurrentFrame() const              { return mCurrentFrame; }

        /** ���������ʹ���������꼯������
        @note Ĭ��0,����ʹ�ö��������������
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        inline void setCoordSet(Nidx16 index)              { mCoordSet = index; }

        /** ��ȡ�����ʹ���������꼯������
        @note �̶����ߺͿɱ�̹�������Ч��
        @version NIIEngine 3.0.0
        */
        inline Nidx16 getCoordSet() const                  { return mCoordSet; }

        /** ������ɫ����Ӧ������
        @note ��ɫ����ר������
        @version NIIEngine 3.0.0 �߼�api
        */
        inline void setFetchType(TextureDataFetch bt)      { mFetchType = bt; }

        /** ��ȡ��ɫ����Ӧ������
        @note ��ɫ����ר������
        @verison NIIEngien 3.0.0 �߼�api
        */
        inline TextureDataFetch getFetchType() const       { return mFetchType; }

        /** �������ݵ�����
        @remark
        @version NIIEngine 3.0.0
        */
        void setContentType(Type type);

        /** ��ȡ���ݵ�����
        @version NIIEngine 3.0.0
        */
        inline Type getContentType() const                 { return mContentType; }

        /** ��ȡ�Ƿ���6����Ԫ���
        @note �̶����ߺͿɱ�̹����ж���Ч
        */
        bool isCubic() const;

        /** ����������������.
        @note Ӧ�õ��̶����ߺͿɱ�̹���
        @version NIIEngine 3.0.0
        */
        inline Texture::Type getType() const               { return mTextureType; }

        /** �������ظ�ʽ
        @remark ����
        @version NIIEngine 3.0.0
        */
        inline void setFormat(PixelFormat pf)              { mFormat = pf; }

        /** ��ȡ���ظ�ʽ
        @remark ����
        @version NIIEngine 3.0.0
        */
        inline PixelFormat getFormat() const               { return mFormat; }

        /** ����
        @remark ����
        @version NIIEngine 3.0.0
        */
        inline void setMipmapCount(NIIi count)             { mMipmapCount = count; }

        /** ��ȡ
        @remark ����
        @version NIIEngine 3.0.0
        */
        inline NIIi getMipmapCount() const                 { return mMipmapCount; }

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
        void setLocalIndex(ShaderChTexture * sct, Nidx16 index);

        /** ��ȡ(����)�е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        Nidx16 getLocalIndex(ShaderChTexture * sct) const;

        /** ���ø����е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        inline void setMultiIndex(Nidx16 index)            { mMultiIndex = index; }

        /** ��ȡ�����е�����
        @version NIIEngine 3.0.0 �߼�api
        */
        inline Nidx16 getMultiIndex() const                { return mMultiIndex; }

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
        inline NIIf getUOft() const                        { return mUOft; }

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
        inline NIIf getVOft() const                        { return mVOft; }

        /** ����U��������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setUScale(NIIf value);

        /** ��ȡU��������
        @version NIIEngine 3.0.0
        */
        inline NIIf getUScale() const                      { return mUScale; }

        /** ����V��������
        @note �ڿɱ�̹�����û��Ч��
        @version NIIEngine 3.0.0
        */
        void setVScale(NIIf value);

        /** ��ȡV��������
        @version NIIEngine 3.0.0
        */
        inline NIIf getVScale() const                      {return mVScale;}

        /** ����Ӧ���������������ʱ����ת����
        @note �ڿɱ�̹�����û��Ч��
        @param[in] angle ��ת�Ƕ�(��ʱ��)
        */
        void setRotate(const Radian & r);

        /** ��ȡ������תЧ��angleֵ
        @version NIIEngine 3.0.0
        */
        inline const Radian & getRotate() const            {return mRotate;}

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
        inline TextureBlend * getColourBlend() const        { return const_cast<TextureBlend *>(&mColourBlend); }

        /** ��ȡ������͸���Ȼ��
        @version NIIEngine 3.0.0 �߼�api
        */
        inline TextureBlend * getAlphaBlend() const         { return const_cast<TextureBlend *>(&mAlphaBlend); }

        /** ��ȡ��������ģʽ
        @version NIIEngien 3.0.0 �߼�api
        */
        inline TextureSampleUnit * getSample() const        { return const_cast<TextureSampleUnit *>(&mSample); }

        /** ״̬/���ʱ仯��
        @version NIIEngine 3.0.0
        */
        void notify();

        /** �������
        @version NIIEngine 3.0.0
        */
        void addRef(const ShaderChTexture * sct);
        
        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void removeRef(const ShaderChTexture * sct);

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
        
        /** ��ʽ����
        @remark ��������Ϊ0,���������Ϊ�Զ�ɾ��,������潫������
        @version NIIEngine 3.0.0
        */
        NCount ref();

        /** �������
        @remark ��������Ϊ0,���������Ϊ�Զ�ɾ��,������潫������
        @version NIIEngine 3.0.0
        */
        NCount unref();

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        inline NCount getRefCount() const                   { return mRefCount;  }

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
        typedef map<ShaderChTexture *, Nidx16>::type LocalIndex;
    protected:
        String mName;
        ShaderChTextureList mApplyList;
        LocalIndex mLocalIndexList;
        DataEquation<TimeDurMS, NIIf> * mTimeFunc;
        GroupID mResGroupID;
        Texture::Type mTextureType;         ///< ��������
        Type mContentType;                  ///< ������������
        ResourceID mFrameFusion;            ///< ֡�ϳ�
        String mFusionTex;                  ///< �ϳ�����
        TimeDurMS mAnimDuration;            ///< ��������(��λ:����)
        NCount mCurrentFrame;               ///< ��ǰ����֡
        TextureDataFetch mFetchType;        ///< ���ݼ���ģʽ
        TextureAddressing mAddressMode;     ///< �����ַģʽ
        EnvMapMode mEnvMapType;             ///< ����ӳ������
        Frustum * mProjFrustum;             ///< ����ͶӰ�ռ�
        PixelFormat mFormat;                ///< ���ظ�ʽ
        NIIi mMipmapCount;                  ///< �������
        EffectMap mEffects;
        Nidx16 mCoordSet;
        Nidx16 mMultiIndex;
        TextureSampleUnit mSample;
        TextureBlend mColourBlend;
        TextureBlend mAlphaBlend;
        NCount mRefCount;

        NIIf mUOft, mVOft, mUScale, mVScale;
        Radian mRotate;
        mutable Matrix4f mMatrix;
        ResourceIDList mTexIDList;
        mutable TextureList mTextures;
        mutable Nmark mMark;
    };
        
    /** Ӧ������
    @remark
        �����ʹ�úͶ������й�ϵ,�����ͨ���Բ���ǿ,�����񶥵�/����������ֱ���й�
    @note 16���󶨿�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ShaderChTexture : public ShaderAlloc
    {
        friend class ShaderCh;
    public:
        /** ����Ԫ��������
        @version NIIEngine 5.0.0
        */
        enum SamplerType
        {
            ST_Storage,
            ST_Sampler
        };

        /** �����
        @version NIIEngine 5.0.0
        */
        struct StorageUnit
        {
            StorageUnit()            { memset(this, 0, sizeof(StorageUnit)); }

            StorageUnit(TextureBuffer * buf, NCount oft, NCount size) : mBuffer(buf), mOffset(oft), mSize(size)
            {
                if(mBuffer)
                    mBuffer->ref();
            }
            
            ~StorageUnit()
            {
                if(mBuffer)
                {
                    mBuffer->unref();
                    mBuffer = 0;
                }
            }
            
            void setBuffer(TextureBuffer * buf)         
            {
                if(mBuffer)
                    mBuffer->unref();
                mBuffer = buf;
                if(mBuffer)
                    mBuffer->ref();
            }
            
            inline TextureBuffer * getBuffer() const           { return mBuffer; }
            
            StorageUnit & operator =(const StorageUnit & o)
            {
                if(mBuffer)
                    mBuffer->unref();
                mBuffer = o.mBuffer;
                if(mBuffer)
                    mBuffer->ref();
                mOffset = o.mOffset;
                mSize = o.mSize;
                return *this;
            }
            
            inline bool operator == (const StorageUnit & o) const
            {
                return mBuffer == o.mBuffer && mOffset == o.mOffset && mSize == o.mSize;
            }
            
            inline bool operator != (const StorageUnit & o) const
            {
                return mBuffer != o.mBuffer || mOffset != o.mOffset || mSize != o.mSize;
            }

            inline bool operator < (const StorageUnit & o) const
            {
                if(mBuffer != o.mBuffer)
                    return mBuffer < o.mBuffer;
                if(mOffset != o.mOffset)
                    return mOffset < o.mOffset;
                if(mSize != o.mSize)
                    return mSize < o.mSize;

                return false;
            }
        protected:
            TextureBuffer * mBuffer;
            PixelFormat mPadding;       ///< no using
        public:
            Nui32 mOffset;
            Nui32 mSize;
        };

        /** �����
        @version NIIEngine 5.0.0
        */
        struct SamplerUnit
        {
            SamplerUnit()           { memset(this, 0, sizeof(SamplerUnit)); }
            
            SamplerUnit(ShaderChTextureUnit * buf, PixelFormat pf, Nui16 mipmaps, Nui16 arrayidx, Nui16 mipmap, bool force2d) :
                mTexture(buf), mFormat(pf), mMipmapCount(mipmaps), mArrayIndex(arrayidx), mMipmap(mipmap), mForce2DArray(force2d)
            {
                if(mTexture)
                {
                    mTexture->ref();
                    if(mFormat == PF_UNKNOWN)
                        mFormat = mTexture->getFormat();
                }
            }
            
            ~SamplerUnit()
            {
                if(mTexture)
                {
                    mTexture->unref();
                    mTexture = 0;
                }
            }
            
            void setBuffer(ShaderChTextureUnit * buf)         
            {
                if(mTexture)
                    mTexture->unref();
                mTexture = buf;
                if(mTexture)
                    mTexture->ref();
            }
            
            inline ShaderChTextureUnit * getBuffer() const      { return mTexture; }
            
            SamplerUnit & operator =(const SamplerUnit & o)
            {
                if(mTexture)
                    mTexture->unref();
                mTexture = o.mTexture;
                if(mTexture)
                    mTexture->ref();
                mFormat = o.mFormat;
                mMipmapCount = o.mMipmapCount;
                mArrayIndex = o.mArrayIndex;
                mForce2DArray = o.mForce2DArray;
                mMipmap = o.mMipmap;
                return *this;
            }

            inline bool operator == (const SamplerUnit & o) const
            {
                return mTexture == o.mTexture && mMipmap == o.mMipmap && mMipmapCount == o.mMipmapCount &&
                    mArrayIndex == o.mArrayIndex && mFormat == o.mFormat;
            }

            inline bool operator != (const SamplerUnit & o) const
            {
                return mTexture != o.mTexture || mMipmap != o.mMipmap || mMipmapCount != o.mMipmapCount || 
                    mArrayIndex != o.mArrayIndex || mFormat != o.mFormat;
            }

            bool operator < (const SamplerUnit & o) const
            {
                if(mTexture != o.mTexture)
                    return mTexture < o.mTexture;
                if(mMipmap != o.mMipmap)
                    return mMipmap < o.mMipmap;
                if(mMipmapCount != o.mMipmapCount)
                    return mMipmapCount < o.mMipmapCount;
                if(mArrayIndex != o.mArrayIndex)
                    return mArrayIndex < o.mArrayIndex;
                if(mFormat != o.mFormat)
                    return mFormat < o.mFormat;

                return false;
            }

            inline bool isValidView() const
            {
                return mFormat == mTexture->getFormat() && mMipmap == 0 && mArrayIndex == 0 && mMipmapCount == 0 &&
                    !(mForce2DArray && (mTexture->getType() == Texture::T_CUBE || mTexture->getType() == Texture::T_CUBEArray));
            }
            
            ShaderChTextureUnit * mTexture;
            PixelFormat mFormat;
            Nui16 mMipmapCount;
            Nui16 mArrayIndex;
            Nui16 mMipmap;
            Nui16 mForce2DArray;
        };

        struct Slot
        {
        public:
            Slot() : mPType(ST_Storage)
            {
                memset(this, 0, sizeof(Slot));
            }

            Slot(SamplerType type) : mPType(type)
            {
                memset(this, 0, sizeof(Slot));
            }
            
            void operator =(const Slot & o)
            {
                mPType = o.mPType;
                if(mPType == ST_Storage)
                {
                    return mBuffer = o.mBuffer;
                }
                else
                {
                    return mTexture = o.mTexture;
                }
            }

            inline bool empty() const
            {
                return mBuffer.mBuffer != 0 || mTexture.mTexture != 0;
            }

            inline bool isBuffer() const
            {
                return mPType == ST_Storage;
            }
            
            inline bool isTexture() const
            {
                return mPType == ST_Sampler;
            }

            inline StorageUnit & getBuffer()
            {
                N_assert1(mPType == ST_Storage);
                return mBuffer;
            }

            inline const StorageUnit & getBuffer() const
            {
                N_assert1(mPType == ST_Storage);
                return mBuffer;
            }

            inline SamplerUnit & getTexture()
            {
                N_assert1(mPType == ST_Sampler);
                return mTexture;
            }

            inline const SamplerUnit & getTexture() const
            {
                N_assert1(mPType == ST_Sampler);
                return mTexture;
            }

            bool operator == (const Slot & o) const
            {
                if(mPType == o.mPType)
                {
                    if(mPType == ST_Storage)
                    {
                        return mBuffer == o.mBuffer;
                    }
                    else
                    {
                        return mTexture == o.mTexture;
                    }
                }
                return false;
            }
            
            bool operator != (const Slot & o) const
            {
                if(mPType == o.mPType)
                {
                    if(mPType == ST_Storage)
                    {
                        return mBuffer != o.mBuffer;
                    }
                    else
                    {
                        return mTexture != o.mTexture;
                    }
                }
                return true;
            }

            bool operator < (const Slot & o) const
            {
                if(mPType != o.mPType)
                    return mPType < o.mPType;

                if(mPType == ST_Storage)
                {
                    return mBuffer < o.mBuffer;
                }
                else
                {
                    return mTexture < o.mTexture;
                }
            }
        protected:
            SamplerType mPType;
            union
            {
                StorageUnit mBuffer;
                SamplerUnit mTexture;
            };
        };
        
        typedef vector<Slot>::type SlotList;
    public:
        ShaderChTexture();
        ShaderChTexture(const ShaderChTexture & o);
        ~ShaderChTexture();

        ShaderChTexture & operator =(const ShaderChTexture & o);
        
        bool operator == (const ShaderChTexture & o) const;
        
        bool operator != (const ShaderChTexture & o) const;

        bool operator < (const ShaderChTexture & o) const;
        
        /** ״̬/���ʱ仯��
        @version NIIEngine 3.0.0
        */
        void notify();
        
        /** �������
        @version NIIEngine 3.0.0
        */
        void addRef(const ShaderCh * sch);
        
        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void removeRef(const ShaderCh * sch);

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
        
        /** ״̬/���ʱ仯��
        @version NIIEngine 3.0.0
        */
        void notify();

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
        ShaderChTextureUnit * create(GpuProgram::ShaderType type, Type type = T_NORMAL);

        /** ����һ��Ӧ������
        @param[in] rid ��ԴID(Ӧ�ó�����Ψһ)
        @param[in] slot
            �����׿�,������Ⱦϵͳһ��֧��ͬһʱ����Ⱦ�������,Ĭ��ֻ��һ��,����Ĭ��Ϊ0,
            �˴��������׿ڲ�����ֱ�������� Type ����������, Type �����岢���������׿�
            ��ͬ
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(ResourceID rid, GpuProgram::ShaderType type, Nidx slot = 0, GpuProgram::ShaderType type, Type type = T_NORMAL);

        /** ����һ��Ӧ������
        @param[in] tex �������
        @param[in] slot
            �����׿�,������Ⱦϵͳһ��֧��ͬһʱ����Ⱦ�������,Ĭ��ֻ��һ��,����Ĭ��Ϊ0,
            �˴��������׿ڲ�����ֱ�������� Type ����������, Type �����岢���������׿�
            ��ͬ
        @param[in] type ��������
        @version NIIEngine 3.0.0
        */
        ShaderChTextureUnit * create(const Texture * tex, GpuProgram::ShaderType type, Nidx slot = 0, Type type = T_NORMAL);

        /** ��ӵ�Ӧ������Ԫ
        @param[in] unit
        @version NIIEngine 3.0.0
        */
        void add(GpuProgram::ShaderType type, ShaderChTextureUnit * unit, Nidx idx = -1);

        /** ��ȡӦ������Ԫ
        @param[in] index
        @version NIIEngine 3.0.0
        */
        Slot & get(Nidx index) const;

        /** ��ȡӦ������
        @param[in] name ����Ԫ����
        @version NIIEngine 3.0.0
        */
        Slot & get(const String & name) const;

        /** ��ȡ���͵�N������.
        @param[in] type ����
        @param[in] index ����
        @version NIIEngine 3.0.0
        */
        Slot & get(GpuProgram::ShaderType type, Nidx index) const;

        /** ��ȡ���������������
        @version NIIEngine 3.0.0 �߼�api
        */
        Slot & get(GpuProgram::ShaderType type, SlotList::const_iterator & begin, SlotList::const_iterator & end) const;

        /** �ƶ�Ӧ��������һ��Ӧ��������
        @remark Type([begin, end))�������ƶ�������o��
        @param[in] type ����
        @param[in] begin ��ʼ����
        @param[in] end ��������
        @param[in] o ��һ��Ӧ������
        @note ����ָ���Ǽ���ճ��,���Ǹ���
        @version NIIEngine 3.0.0 �߼�api
        */
        void move(GpuProgram::ShaderType type, Nidx begin, Nidx end, ShaderChTexture * o);

        /** ��ȥӦ������
        @param[in] type
        @param[in] index
        @version NIIEngine 3.0.0
        */
        void remove(GpuProgram::ShaderType type, Nidx index);

        /** ��ȥ����Ӧ������
        @version NIIEngine 3.0.0
        */
        void removeAll();
        
        /** �Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** ��ȡӦ�������б�
        @remark ���������ֱ��Ӱ������Ӧ������,ע���������mUsed[x]
        @version NIIEngien 3.0.0 �߼�api
        */
        inline const SlotList & getSlots() const                            { return mSlotList; }
        
        /** ����ʹ������
        @version NIIEngine 3.0.0
        */
        void setUsedCount(GpuProgram::ShaderType type, NCountb cnt) const   { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type] = cnt; }
        
        /** ��ȡʹ������
        @version NIIEngine 3.0.0
        */
        NCountb getUsedCount(GpuProgram::ShaderType type) const             { N_assert1(type < GpuProgram::ST_Cnt); return mUsed[type]; }
    private:
        ShaderChList mApplyList;
        SlotList mSlotList;
        NCountb mUsed[GpuProgram::ST_Cnt];
        static Slot NullSlot;
    };
}
#endif