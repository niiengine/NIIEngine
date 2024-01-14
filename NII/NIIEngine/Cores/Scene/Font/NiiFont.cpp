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
#include "NiiFont.h"
#include "NiiFontManager.h"
#include "NiiUIManager.h"
#include "NiiPixelBuffer.h"
#include "NiiGeometryPixel.h"

namespace NII
{
    //------------------------------------------------------------------------
    // amount of bits in a Nui32
    #define BITS_PER_UINT (sizeof(Nui32) * 8)
    // must be a power of two
    #define GLYPHS_PER_PAGE 256
    //------------------------------------------------------------------------
    class _EngineInner CmdFontIDProperty : public PropertyCmd
    {
    public:
        CmdFontIDProperty() : PropertyCmd(N_PropertyFont_ID, _T("name"),
            _T("This is font name.  Value is a string."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Font * obj = static_cast<const Font *>(own);
            StrConv::conv(obj->getID(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Font * obj = static_cast<Font *>(own);
            Nui32 fid;
            StrConv::conv(val, fid);
            obj->setID(fid);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {

        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {

        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFontIDProperty();
        }
        /// @copydetails PropertyCmd::isWritable
        bool isWritable() const
        {
            return false;
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdFontSrc : public PropertyCmd
    {
    public:
        CmdFontSrc() : PropertyCmd(N_PropertyFont_Src, _T("image_name_prefix"),
            _T("This is the name prefix used by the images that contain the glyph "
                "imagery for this font."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Font *>(own)->getSource();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<Font *>(own)->setSource(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {

        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {

        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFontSrc();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdNativeRes : public PropertyCmd
    {
    public:
        CmdNativeRes() : PropertyCmd(N_PropertyFont_NativeRes, _T("native_res"),
            _T("Native screen resolution for this font.Value uses the 'w:# h:#' format."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Font * obj = static_cast<const Font *>(own);
            const PlaneSizef & val = obj->getNativeRes();
            Ntchar buff[128];
            Nsntprintf(buff, sizeof(buff), _T("%g %g"), val.mWidth, val.mHeight);
            out.assign(buff);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Font * obj = static_cast<Font *>(own);
            PlaneSizef temp(0, 0);
            Nsscanf(val.c_str(), _T("%g %g"), &temp.mWidth, &temp.mHeight);
            obj->setNativeRes(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {

        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {

        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdNativeRes();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            Ntchar buff[128];
            Nsntprintf(buff, sizeof(buff), _T("%g %g"), 0.0f, 0.0f);
            out = String(buff);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdAutoScaled : public PropertyCmd
    {
    public:
        CmdAutoScaled() : PropertyCmd(N_PropertyFont_AutoRes, _T("auto_scaled"),
            _T("This indicating whether and how to autoscale font depending on "
            "resolution.  Value can be 'false', 'vertical', 'horizontal' or 'true'."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            const Font * obj = static_cast<const Font *>(own);

            StrConv::conv(obj->getScaleMode(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Font * obj = static_cast<Font *>(own);
            ScaleMode temp;
            StrConv::conv(val, temp);

            obj->setScaleMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {

        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {

        }
        
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoScaled();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(SM_Unknow, out);
        }
    };
    //------------------------------------------------------------------------
    const ARGB Font::DefaultColour = 0xFFFFFFFF;
    const NIIf Font::DefaultNativeHorzRes = 640.0f;
    const NIIf Font::DefaultNativeVertRes = 480.0f;
    //------------------------------------------------------------------------
    Font::Font(ScriptTypeID stid, FontID fid, FontType type, const String & src, ResourceID rid,
        GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
            ScaleMode mode, const PlaneSizef & nativeRes, LangID lid) :
        Resource(stid, rid, gid, ls, rs, lid),
        mFontID(fid),
        mType(type),
        mSource(src),
        mAscLine(0),
        mDesLine(0),
        mHeight(0),
        mScaleMode(mode),
        mNativeRes(nativeRes),
        mMinCodePoint(0),
        mMaxCodePoint(0)
    {
        Vector2f temp;
        PixelBuffer::calcAspectRatio(mScaleMode, N_Only(UI).getDisplaySize(N_PRIMARY_Viewport), mNativeRes, temp);
        mXScale = temp.x;
        mYScale = temp.y;
        //setupCmd(N_CmdObj_Font);
    }
    //------------------------------------------------------------------------
    Font::~Font()
    {
    }
    //------------------------------------------------------------------------
    void Font::setID(FontID fid)
    {
        mFontID = fid;
    }
    //------------------------------------------------------------------------
    FontID Font::getID() const
    {
        return mFontID;
    }
    //------------------------------------------------------------------------
    FontType Font::getFontType() const
    {
        return mType;
    }
    //------------------------------------------------------------------------
    void Font::setSource(const String & src)
    {
        mSource = src;
    }
    //------------------------------------------------------------------------
    const String & Font::getSource() const
    {
        return mSource;
    }
    //------------------------------------------------------------------------
    void Font::addDefineRange(const CodePointRange & range)
    {
        mDefineRangeList.push_back(range);
    }
    //------------------------------------------------------------------------
    void Font::clearAllDefineRange()
    {
        mDefineRangeList.clear();
    }
    //------------------------------------------------------------------------
    const Font::CodePointRangeList & Font::getDefineRangeList() const
    {
        return mDefineRangeList;
    }
    //------------------------------------------------------------------------
    bool Font::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdFontIDProperty());
        dest->add(N_new CmdFontSrc());
        dest->add(N_new CmdNativeRes());
        dest->add(N_new CmdAutoScaled());
        return true;
    }
    //------------------------------------------------------------------------
    const FontGlyph * Font::getCodePoint(CodePoint value, Nidx & pageidx) const
    {
        if(value > mMaxCodePoint || value < mMinCodePoint)
            return 0;

        const FontGlyph * glyph = getCodePointImpl(value, pageidx);

        return glyph;
    }
    //------------------------------------------------------------------------
    Material * Font::getMaterial(Nidx pageidx) const
    {
        if(pageidx > mMaterialList.size())
            return 0;
        return mMaterialList[pageidx];
    }
    //------------------------------------------------------------------------
    Texture * Font::getTexture(Nidx pageidx) const
    {
        if(pageidx > mTextureGlyphList.size())
            return 0;
        return mTextureGlyphList[pageidx].first;
    }
    //------------------------------------------------------------------------
    bool Font::isCodePointExist(CodePoint value) const
    {
        NCount index = 0;
        CodePointRangeList::const_iterator i, iend = mLoadRangeList.end();
        for(i = mLoadRangeList.begin() + 1; i != iend; ++i)
        {
            if(i->second < value)
            {
                --i;
                if(i->first < value && i->second > value)
                {
                    return (mTextureGlyphList[index].second->find(value) != mTextureGlyphList[index].second->end());
                }
                else
                {
                    return false;
                }
            }
            ++index;
        }
        return false;
    }
    //------------------------------------------------------------------------
    const FontGlyph * Font::getCodePointImpl(CodePoint value, Nidx & pageidx) const
    {
        NCount index = 0;
        CodePointRangeList::const_iterator i, iend = mLoadRangeList.end();
        for(i = mLoadRangeList.begin() + 1; i != iend; ++i)
        {
            if(i->second < value)
            {
                --i;
                if(i->first < value && i->second > value)
                {
                    CodePointList::const_iterator pos = mTextureGlyphList[index].second->find(value);
                    if(pos != mTextureGlyphList[index].second->end())
                    {
                        pageidx = index;
                        return &pos->second;
                    }
                    else
                    {
                        pageidx = 0;
                        return 0;
                    }
                }
                else
                {
                    pageidx = 0;
                    return 0;
                }
            }
            ++index;
        }
        pageidx = 0;
        return 0;
    }
    //------------------------------------------------------------------------
    NIIf Font::getTextExtent(const String & text) const
    {
        const FontGlyph * glyph;
        Nidx page;
        NIIf cur_extent = 0, adv_extent = 0, width;

        for(NCount t = 0; t < text.length(); ++t)
        {
            glyph = getCodePoint(text[t], page);
            if(glyph)
            {
                width = getCodePointExtentImpl(glyph);

                if(adv_extent + width > cur_extent)
                    cur_extent = adv_extent + width;

                adv_extent += glyph->getHoriAdvance();
            }
        }
        return N_MAX(NIIf, adv_extent, cur_extent);
    }
    //------------------------------------------------------------------------
    NIIf Font::getTextAdvance(const String & text) const
    {
        const FontGlyph * glyph;
        Nidx page;
        NIIf advance = 0.0f;
        for(NCount t = 0; t < text.length(); ++t)
        {
            glyph = getCodePoint(text[t], page);
            if (glyph)
            {
                advance += glyph->getHoriAdvance();
            }

        }
        return advance;
    }
    //------------------------------------------------------------------------
    NCount Font::getCharAtPixel(const String & text, NIIf pixel, NIIf xScale) const
    {
        return getCharAtPixel(text, 0, pixel, xScale);
    }
    //------------------------------------------------------------------------
    NCount Font::getCharAtPixel(const String & text, NCount start, NIIf pixel, NIIf xScale) const
    {
        const FontGlyph * glyph;
        Nidx page;
        NIIf cur_extent = 0;
        NCount char_count = text.length();

        // handle simple cases
        if((pixel <= 0) || (char_count <= start))
            return start;

        for(NCount t = start; t < char_count; ++t)
        {
            glyph = getCodePoint(text[t], page);
            if(glyph)
            {
                cur_extent += glyph->getHoriAdvance() * xScale;
                if(pixel < cur_extent)
                    return t;
            }
        }

        return char_count;
    }
    //------------------------------------------------------------------------
    NIIf Font::drawText(GeometryPixel * buffer, const String & text, const Vector2f & pos,
        const Rectf * clipper, const ColourArea & c, NIIf spaceExtent, NIIf x_scale,
            NIIf y_scale) const
    {
        NIIf base_y = pos.y + getBaseline() * y_scale;
        Vector2f glyph_pos(pos);

        NCount t, tend = text.length();
        for(t = 0; t < tend; ++t)
        {
            const FontGlyph * glyph;
            Nidx page;
            if(glyph = getCodePoint(text[t], page)) // NB: assignment
            {
                glyph_pos.y = base_y - (glyph->getOffset().y - glyph->getOffset().y * y_scale);
                buffer->add(getTexture(page), glyph->getTextureCoord(),
                    Rectf(glyph_pos, PlaneSizef(getCodePointWidth(glyph) * x_scale,
                    getCodePointHeight(glyph) * y_scale)), clipper, c);
                glyph_pos.x += glyph->getHoriAdvance() * x_scale;
                // apply extra spacing to space chars
                if(text[t] == ' ')
                    glyph_pos.x += spaceExtent;
            }
        }

        return glyph_pos.x;
    }
    //------------------------------------------------------------------------
    const Font::CodePointRangeList & Font::getCodePointList() const
    {
        return mLoadRangeList;
    }
    //------------------------------------------------------------------------
    void Font::setNativeRes(const PlaneSizef & size)
    {
        mNativeRes = size;
        setViewRes(N_Only(UI).getDisplaySize(N_PRIMARY_Viewport));
    }
    //------------------------------------------------------------------------
    void Font::setViewRes(const PlaneSizei & size)
    {
        Vector2f temp;
        PixelBuffer::calcAspectRatio(mScaleMode, size, mNativeRes, temp);
        mXScale = temp.x;
        mYScale = temp.y;
        if(mScaleMode != SM_Unknow && mState == LS_LOADED)
        {
            reflushCodePointImpl();
            N_Only(Font).notifySize(this);
        }
    }
    //------------------------------------------------------------------------
    const PlaneSizef & Font::getNativeRes() const
    {
        return mNativeRes;
    }
    //------------------------------------------------------------------------
    NIIf Font::getFontHeight() const
    {
        return mAscLine - mDesLine;
    }
    //------------------------------------------------------------------------
    NIIf Font::getLineSpacing() const
    {
        return mHeight;
    }
    //------------------------------------------------------------------------
    NIIf Font::getCharAspect(CodePoint c) const
    {
        if(c > mMaxCodePoint || c < mMinCodePoint)
            return 0;

        Nidx pageidx;
        const FontGlyph * glyph = getCodePointImpl(c, pageidx);

        return getCodePointWidth(glyph) / getCodePointHeight(glyph);
    }
    //------------------------------------------------------------------------
    NIIf Font::getCharAspect(const FontGlyph * glyph) const
    {
        if(glyph)
            return getCodePointWidth(glyph) / getCodePointHeight(glyph);
        return 1;
    }
    //------------------------------------------------------------------------
    NIIf Font::getBaseline() const
    {
        return mAscLine;
    }
    //------------------------------------------------------------------------
    void Font::setScaleMode(ScaleMode sm)
    {
        if(sm != mScaleMode)
        {
            mScaleMode = sm;

            if(mState == LS_LOADED)
                reflushCodePointImpl();
            N_Only(Font).notifySize(this);
        }
    }
    //------------------------------------------------------------------------
    ScaleMode Font::getScaleMode() const
    {
        return mScaleMode;
    }
    //------------------------------------------------------------------------
    NIIf Font::getViewXScale() const
    {
        return mXScale;
    }
    //------------------------------------------------------------------------
    NIIf Font::getViewYScale() const
    {
        return mYScale;
    }
    //------------------------------------------------------------------------
    const Font::TextureGlyphList & Font::getGlyphList() const
    {
        return mTextureGlyphList;
    }
    //------------------------------------------------------------------------
}