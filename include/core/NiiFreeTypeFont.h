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
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, ScaleMode sm = SM_Unknow,
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
        bool initCmd(PropertyCmdSet * dest);

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
        NCount getTextureSize(Vector2ui & out, CodePoint start, CodePoint end) const;
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