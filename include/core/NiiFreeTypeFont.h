/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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
#ifndef _NII_FreeTypeFont_H_
#define _NII_FreeTypeFont_H_

#include "NiiPreInclude.h"
#include "NiiFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace NII
{
    /** ttf ����
    @version NIIEngine 3.0.0
    */
    class FreeTypeFont : public Font
    {
    public:
        FreeTypeFont(FontID fid, NIIf size, NCount pdi, bool antialias,
            const String & src, ResourceID rid, GroupID gid = GroupUnknow,
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, ScaleMode sm = SM_Unknow,
			const PlaneSizef & nativeRes = PlaneSizef(640.0f, 480.0f), NIIf lineSpace = 0.0f, 
				LangID lid = N_PrimaryLang);

        ~FreeTypeFont();

        /** ���÷ֱ���
        @version NIIEngine 3.0.0
        */
        void setDPI(NCount c);

        /** ��ȡ�ֱ���
        @version NIIEngine 3.0.0
        */
        NCount getDPI() const;

        /** ���õ��С
        @version NIIEngine 3.0.0
        */
        void setPointSize(NIIf size);

        /** ��ȡ���С
        @version NIIEngine 3.0.0
        */
        NIIf getPointSize() const;

        /** �����Ƿ񿹾��
        @version NIIEngine 3.0.0
        */
        void setAntialias(bool b);

        /** ��ȡ�Ƿ񿹾��
        @version NIIEngine 3.0.0
        */
        bool isAntialias() const;
        
        /** �����о���
        @version NIIEngine 3.0.0
        */
		void setLineSpace(NIIf space);

        /** ��ȡ�о���
        @version NIIEngine 3.0.0
        */
        NIIf getLineSpace() const;
        
        /** ���ô˿�����������ɫ.
        @version NIIEngine 3.0.0
        */
        void setAlphaAsColour(bool b);

        /** ��ȡ�˿�����������ɫ.
        @version NIIEngine 3.0.0
        */
        bool isAlphaAsColour() const;
    protected:
        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Font::rasterCodePointImpl
        void rasterCodePointImpl(CodePoint start, CodePoint end);

        /// @copydetails Font::reflushCodePointImpl
        void reflushCodePointImpl();

        /// @copydetails Font::getCodePointWidth
        NIIf getCodePointWidth(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointHeight
        NIIf getCodePointHeight(const FontGlyph * value) const;

        /// @copydetails Font::getCodePointExtentImpl
        NIIf getCodePointExtentImpl(const FontGlyph * value) const;

        /** ��������
        @param[in] ������
        @param[in] w ������Ŀ�
        @param[in] h ������ĸ�
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void genCodePointGlyph(Nui16 * out, NCount w, NCount h) const;

        /** ɾ����������
        @version NIIEngine 3.0.0 �ڲ�api
        */
        void destroyCodePointGlyph();

        /** ��ȡ�����С
        @param out ��Ҫ�������С
        @param start ��ʼ
        @param end ����
        @return ʵ�ʼ����ַ�����
        @version NIIEngine 3.0.0 �ڲ�api
        */
        NCount getTextureSize(Vector2i & out, CodePoint start, CodePoint end) const;
    protected:
        /// @copydetails Resource::planImpl
        void planImpl();

        /// @copydetails Resource::loadImpl
        void loadImpl();

        /// @copydetails Resource::unloadImpl
        void unloadImpl();

        /// @copydetails Resource::calcSize
        NCount calcSize() const;
    protected:
        NCount mDIP;
        NIIf mPointSize;
        NIIf mLineSpace;
        FT_Face mFontFace;
        DataStream * mFontStream;
        bool mAntialias;
        bool mAlphaAsColour;
    };
}
#endif