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

#include "NiiPreProcess.h"
#include "NiiFontXmlAnalyze.h"
#include "NiiXMLSerializer.h"
#include "NiiLogManager.h"
#include "NiiFontManager.h"
#include "NiiPixmapFont.h"
#include "NiiFreeTypeFont.h"

namespace NII
{
    //------------------------------------------------------------------------
    FontXmlAnalyze::FontXmlAnalyze(LangID lid, FontType type):
        XmlAnalyze(3, N_Script_Font, _T("Font.xsd"), lid),
        mFontType(type),
        mCurrentFont(0)
    {
        setBeginPrc(N_PropertyFont_CharDefine, (PrcCB)(&FontXmlAnalyze::PrcDefineCB));
    }
    //------------------------------------------------------------------------
    FontXmlAnalyze::~FontXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    const FontList & FontXmlAnalyze::getObjectList() const
    {
        return mFontList;
    }
    //------------------------------------------------------------------------
    const String & FontXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    XmlUnitID FontXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_FontUnit;
    }
    //------------------------------------------------------------------------
    GroupID FontXmlAnalyze::getGroup() const
    {
        return FontManager::getGroup();
    }
    //------------------------------------------------------------------------
    void FontXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {
        Nui32 version;
        pdc->get(VersionProperty, version, 0);
        if(version != mVersion)
            N_EXCEPT(InvalidRequest, _I18n("字体版本问题!"));

        // get type of font being created
        String ftype = pdc->get(N_PropertyFont_Type);
        if(ftype == _T(""))
            N_EXCEPT(InvalidRequest, _I18n("字体类型问题!"));

        // log the start of font creation.
        N_Only(Log).log(_I18n("Started creation of Font from XML specification:"));

        ScaleMode smode;
        Nui32 id;
        pdc->get(N_PropertyFont_ID, id);
        if(N_Only(Font).isFontExist(id))
        {
            mCurrentFont = 0;
            return;
        }

        FontList::iterator f, fend = mFontList.end();
        for(f = mFontList.begin(); f != fend; ++f)
        {
            if((*f)->getID() == id)
            {
                mCurrentFont = 0;
                return;
            }
        }

        if(ftype == _T("freetype"))
        {
            mFontType = FT_TrueType;
            String src;
            String defsize;
            Nui32 gip;
            NIIf psize;
            pdc->get(GroupProperty, gip, GroupUnknow);
            pdc->get(N_PropertyFont_Size, psize, 12.0f);
            pdc->get(N_PropertyFont_Src, src);
            pdc->get(N_PropertyFont_Size, defsize, _T("12"));

            N_Only(Log).log(_I18n("---- NIIEngine font id: ") + id);
            N_Only(Log).log(_I18n("----       Font type: FreeType"));
            N_Only(Log).log(_I18n("----     Source file: ") + src +
                _I18n(" in resource group: ") + (gip == GroupUnknow ? _I18n("GroupUnknow") : N_conv(gip)));
            N_Only(Log).log(_I18n("---- Real point size: ") + defsize);

            bool antialias;
            NIIf nativRes[2];
            nativRes[0] = 640.0f;
            nativRes[1] = 480.0f;
            NIIf size, tf4;
            pdc->get(N_PropertyFont_Size, size, 12.0f);
            pdc->get(N_PropertyFont_AntiAlias, antialias, true);
            pdc->get(N_PropertyFont_NativeRes, nativRes, 2);
            pdc->get(N_PropertyFont_LineSpace, tf4, 0.0f);
            StrConv::conv(pdc->get(N_PropertyFont_AutoRes), smode);

            mCurrentFont = N_new FreeTypeFont(id, size, 96, antialias, src, N_Only(Font).genValidID(), gip,
                0, 0, smode, PlaneSizef(nativRes[0], nativRes[1]), tf4);

            mFontList.push_back(mCurrentFont);
        }
        else if(ftype == _T("pixmap"))
        {
            mFontType = FT_Pixel;
            const String & src(pdc->get(N_PropertyFont_Src));
            Nui32 gip;
            pdc->get(GroupProperty, gip, GroupUnknow);

            N_Only(Log).log(_I18n("---- NIIEngine font id: ") + id);
            N_Only(Log).log(_I18n("----    Font type: Pixmap"));
            N_Only(Log).log(_I18n("----    Source file: ") + src +
                _I18n(" in resource group: ") + (gip == GroupUnknow ? _I18n("(Default)") : N_conv(gip)));

            NIIf nativRes[2];
            nativRes[0] = 640.0f;
            nativRes[1] = 480.0f;
            pdc->get(N_PropertyFont_NativeRes, nativRes, 2);
            StrConv::conv(pdc->get(N_PropertyFont_AutoRes), smode);

            mCurrentFont = N_new PixmapFont(id, src, N_Only(Font).genValidID(), gip,
                0, 0, smode, PlaneSizef(nativRes[0], nativRes[1]));

            mFontList.push_back(mCurrentFont);
        }
    }
    //------------------------------------------------------------------------
    void FontXmlAnalyze::PrcDefineCB(const PropertyData * pdc)
    {
        if(!mCurrentFont)
            N_EXCEPT(InvalidRequest, _I18n("Attempt to access null object."));

        // double-check font type just in case - report issues as 'soft' errors
        if(mCurrentFont->getFontType() != FT_Pixel)
            N_Only(Log).log(_I18n("Imageset_xmlHandler::PrcDefineCB: <Define> element is only valid for Pixmap type fonts."));
        else
        {
            Nui32 texidx;
            NIIi codepoint;
            Vector2f pos, oft;
            NIIf tf;
            NIIf size[2];
            pdc->get(N_PropertyFont_CharTextureIndx, texidx, 0);
            pdc->get(N_PropertyFont_CharCodeValue, codepoint);
            pdc->get(N_PropertyFont_CharSize, size, 2);
            pdc->get(N_PropertyFont_CharPosOft, oft.data(), 2);
            pdc->get(N_PropertyFont_CharPos, pos.data(), 2);
            pdc->get(N_PropertyFont_CharXOft, tf, -1.0f);

            static_cast<PixmapFont *>(mCurrentFont)->defineCodePoint(texidx, codepoint,
                Rectf(pos, PlaneSizef(size[0], size[1])), oft, tf);
        }
    }
    //------------------------------------------------------------------------
    void FontXmlAnalyze::write(XmlSerializer * out, const Font * obj) const
    {
        // output starting <Font ... > element
        out->begin(_T("Font"))
            .attribute(N_PropertyFont_ID, N_conv(obj->getID()))
            .attribute(N_PropertyFont_Src, obj->getSource());

        if(obj->getGroup() != 0)
            out->attribute(GroupProperty, N_conv(obj->getGroup()));

        if(obj->getNativeRes().mWidth != Font::DefaultNativeHorzRes ||
            obj->getNativeRes().mHeight != Font::DefaultNativeVertRes)
            out->attribute(N_PropertyFont_NativeRes, N_conv(Vector2f(obj->getNativeRes().mWidth, obj->getNativeRes().mHeight)));

        if(obj->getScaleMode() != SM_Unknow)
        {
            String temp;
            StrConv::conv(obj->getScaleMode(), temp);
            out->attribute(N_PropertyFont_AutoRes, temp);
        }
        if(mFontType == FT_TrueType)
            writeTrueType(out, obj);
        else
            writePixel(out, obj);
        // output closing </Font> element.
        out->end();
    }
    //------------------------------------------------------------------------
    void FontXmlAnalyze::writeTrueType(XmlSerializer * out, const Font * src) const
    {
        const FreeTypeFont * temp = static_cast<const FreeTypeFont *>(src);
        out->attribute(N_PropertyFont_Size, temp->getPointSize());

        if(!temp->isAntialias())
            out->attribute(N_PropertyFont_AntiAlias, N_conv(temp->isAntialias()));

        if(temp->getLineSpacing() > 0.0f)
            out->attribute(N_PropertyFont_LineSpace, N_conv(temp->getLineSpacing()));
    }
    //------------------------------------------------------------------------
    void FontXmlAnalyze::writePixel(XmlSerializer * out, const Font * src) const
    {
        Font::TextureGlyphList::const_iterator j, jend = src->getGlyphList().end();
        for(j = src->getGlyphList().begin(); j != jend; ++j)
        {
            Font::CodePointList::const_iterator i, iend = j->second->end();
            for (i = j->second->begin(); i != iend; ++i)
            {
                out->begin(_T("FontDefine"))
                    .attribute(N_PropertyFont_CharTextureIndx, N_conv(j->first->getOriginID()))
                    .attribute(N_PropertyFont_CharCodeValue, N_conv(i->first))
                    .attribute(N_PropertyFont_CharPos, N_conv(i->second.getTextureCoord().left()) + _T(" ") +
                        N_conv(i->second.getTextureCoord().top()))
                    .attribute(N_PropertyFont_CharSize, N_conv(i->second.getTextureCoord().width()) + _T(" ") +
                        N_conv(i->second.getTextureCoord().height()))
                    .attribute(N_PropertyFont_CharPosOft, N_conv(i->second.getOffset().x) + _T(" ") +
                        N_conv(i->second.getOffset().y))
                    .attribute(N_PropertyFont_CharXOft, N_conv(i->second.getHoriAdvance() / src->getViewXScale()));
            }
            out->end();
        }
    }
    //------------------------------------------------------------------------
}