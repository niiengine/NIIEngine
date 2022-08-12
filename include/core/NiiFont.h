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
#ifndef _NII_FONT_H_
#define _NII_FONT_H_

#include "NiiPreInclude.h"
#include "NiiFontGlyph.h"
#include "NiiColour.h"
#include "NiiEvent.h"

namespace NII
{
    /** �����¼�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI FontEventArgs : public EventArgs
    {
    public:
        FontEventArgs(Font * font) :
            mFont(font) {}

        Font * mFont;
    };

    /** ��������
    @version NIIEngine 3.0.0
    */
    enum FontType
    {
        FT_TrueType = 1,    ///< ttf����
        FT_Pixel = 2        ///< ����
    };

    /** ����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Font : public Resource
    {
    public:
        typedef std::pair<CodePoint, CodePoint> CodePointRange;
        typedef vector<CodePointRange>::type CodePointRangeList;
        typedef map<CodePoint, FontGlyph>::type CodePointList;
        typedef vector<std::pair<Texture *, CodePointList *> >::type TextureGlyphList;
    public:
        virtual ~Font();
        
        /** ��ȡ����ID
        @note �� ResourceID ���һ��
        @version NIIEngine 3.0.0�߼�api
        */
        void setID(FontID fid);
        
        /** ��ȡ����ID
        @note �� ResourceID ���һ��
        @version NIIEngine 3.0.0
        */
        FontID getID() const;

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        FontType getFontType() const;

        /** ������Դ��Դ
        @note �����������ļ���ͼƬ�ļ�
        @version NIIEngine 3.0.0
        */
        void setSource(const String & src);
        
        /** ��ȡ��Դ��Դ
        @note ���������ͼƬ
        @version NIIEngine 3.0.0
        */
        const String & getSource() const;

        /** �����ַ���Χ
        @param[in] range
        @version NIIEngine 3.0.0
        */
        void addDefineRange(const CodePointRange & range);

        /** �������ж����ַ�
        @version NIIEngine 3.0.0
        */
        void clearAllDefineRange();

        /** ��ȡ�ַ���Χ
        @version NIIEngine 3.0.0
        */
        const CodePointRangeList & getDefineRangeList() const;

        /** ��ȡ���ص��ַ���Χ�б�
        @note �� getDefineRangeList ���,�������ҳ����ṹ
        @version NIIEngine 3.0.0 �߼�api
        */
        const CodePointRangeList & getCodePointList() const;

        /** ��ȡ��Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setNativeRes(const PlaneSizef & size);

        /** ������ͼ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        void setViewRes(const PlaneSizei & size);

        /** ������Դԭ�ֱ���
        @note (���ؼ�)
        @version NIIEngine 3.0.0
        */
        const PlaneSizef & getNativeRes() const;

        /** ��������ģʽ
        @version NIIEngine 3.0.0
        */
        void setScaleMode(ScaleMode sm);

        /** ��ȡ����ģʽ
        @version NIIEngine 3.0.0
        */
        ScaleMode getScaleMode() const;

        /** ��Ⱦ���嵽���ؼ��λ�������
        @param dest ��������
        @param text �ı�
        @param pos
        @param xScale
        @param yScale
        @return
        */
        NIIf drawText(GeometryPixel * dest, const String & text,
            const Vector2f & pos, const Rectf * clipper, const ColourArea & colour,
                NIIf spaceExtent = 0.0f, NIIf xScale = 1.0f, NIIf yScale = 1.0f) const;

        /** �ַ�ͼ�νṹ�Ƿ����
        @version NIIEngine 3.0.0
        */
        bool isCodePointExist(CodePoint c) const;

        /** ��ȡ�ַ�ͼ�νṹ
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        const FontGlyph * getCodePoint(CodePoint c, Nidx & pageidx) const;

        /** ��ȡ����
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        Material * getMaterial(Nidx pageidx) const;

        /** ��ȡ����
        @param[in] pageidx
        @version NIIEngine 3.0.0
        */
        Texture * getTexture(Nidx pageidx) const;
        
        /** ����߶�
        @version NIIEngine 3.0.0
        */
        NIIf getFontHeight() const;

        /** ������
        @version NIIEngine 3.0.0
        */
        NIIf getBaseline() const;

        /** �о�
        @version NIIEngine 3.0.0
        */
        NIIf getLineSpacing() const;

        /** ��ȡ�ַ���߱�
        @version NIIEngine 3.0.0
        */
        NIIf getCharAspect(CodePoint c) const;
        
        /** ��ȡ�ַ���߱�
        @version NIIEngine 3.0.0
        */
        NIIf getCharAspect(const FontGlyph * glyph) const;
        
        /** ��ȡ�ַ���λƫ����
        @version NIIEngine 3.0.0
        */
        NIIf getTextExtent(const String & text) const;

        /** ��ȡ�ַ���ˮƽƫ��
        @version NIIEngine 3.0.0
        */
        NIIf getTextAdvance(const String & text) const;

        /** ��ȡָ�������е��ַ�
        @version NIIEngine 3.0.0
        */
        NCount getCharAtPixel(const String & txt, NIIf pixel, NIIf xScale = 1.0f) const;

        /** ��ȡָ�������е��ַ�
        @version NIIEngine 3.0.0
        */
        NCount getCharAtPixel(const String & txt, NCount start, NIIf pixel, NIIf xScale = 1.0f) const;

        /** ��ȡ��ͼX��������
        @version NIIEngine 3.0.0 �߼�api
        */
        NIIf getViewXScale() const;

        /** ��ȡ��ͼY��������
        @version NIIEngine 3.0.0 �߼�api
        */
        NIIf getViewYScale() const;

        /** ��ȡ���������б�
        @version NIIEngine 3.0.0 �߼�api
        */
        const TextureGlyphList & getGlyphList() const;
    public:
        static const ARGB DefaultColour;
        static const NIIf DefaultNativeHorzRes;
        static const NIIf DefaultNativeVertRes;
    protected:
        Font(ScriptTypeID stid, FontID id, FontType type, const String & src, ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode mode = SM_Unknow,
                const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), LangID lid = N_PrimaryLang);

        /// @copydetails PropertyCmdObj::init
        bool initCmd(PropertyCmdSet * dest);

        /** ��ȡ�ַ�ʵ��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual const FontGlyph * getCodePointImpl(CodePoint c, Nidx & page) const;

        /** ��դ���ַ�
        @note һ����ָ����������һ������
        @param start ��ʼ�ַ�
        @param end �����ַ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void rasterCodePointImpl(CodePoint start, CodePoint end) = 0;

        /** �ַ���̬�仯,ˢ���ѹ�դ�ַ�
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void reflushCodePointImpl() = 0;

        /** ��ȡ�ַ���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual NIIf getCodePointWidth(const FontGlyph * glyph) const = 0;

        /** ��ȡ�ַ���
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual NIIf getCodePointHeight(const FontGlyph * glyph) const = 0;

        /** ��ȡ�ַ�λƫ
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual NIIf getCodePointExtentImpl(const FontGlyph * glyph) const = 0;
    protected:
        FontID mFontID;
        FontType mType;
        String mSource;
        TextureGlyphList mTextureGlyphList;
        CodePointRangeList mDefineRangeList;
        CodePointRangeList mLoadRangeList;
        MaterialList mMaterialList;
        CodePoint mMinCodePoint;
        CodePoint mMaxCodePoint;
        ScaleMode mScaleMode;
        NIIf mHeight;
        PlaneSizef mNativeRes;
        NIIf mXScale;
        NIIf mYScale;
        NIIf mAscLine;
        NIIf mDesLine;
    };
}
#endif