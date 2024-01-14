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
#include "NiiFreeTypeFont.h"
#include "NiiTextureManager.h"
#include "NiiMaterialManager.h"
#include "NiiFontManager.h"
#include "NiiLogManager.h"
#include "NiiBitwise.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define __N_FreeType_PAGESIZE 4096576

namespace NII
{
    //------------------------------------------------------------------------
    // Pixels to put between glyphs
    #define INTER_GLYPH_PAD_SPACE 2
    // A multiplication coefficient to convert FT_Pos values into normal floats (1.0/64.0)
    #define FT_POS_COEF  0.015625f
    //------------------------------------------------------------------------
    class _EngineInner CmdPointSize : public PropertyCmd
    {
    public:
        CmdPointSize() : PropertyCmd(N_PropertyFont_Size, _T("point_size"),
            _T("This is the point size of the font."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FreeTypeFont *>(own)->getPointSize(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<FreeTypeFont *>(own)->setPointSize(temp);
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
            return N_new CmdPointSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f , out);
        }
    };

    class _EngineInner CmdLineSpace : public PropertyCmd
    {
    public:
        CmdLineSpace() : PropertyCmd(N_PropertyFont_LineSpace, _T("point_size"),
            _T("This is the point size of the font."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FreeTypeFont *>(own)->getLineSpace(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<FreeTypeFont *>(own)->setLineSpace(temp);
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
            return N_new CmdLineSpace();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f,out);
        }
    };

    class _EngineInner CmdAntialiased : public PropertyCmd
    {
    public:
        CmdAntialiased() : PropertyCmd(N_PropertyFont_AntiAlias, _T("antialiased"),
            _T("This is a flag indicating whenever to render antialiased font or not. "
                "Value is either true or false."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const FreeTypeFont *>(own)->isAntialias(), out);
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<FreeTypeFont *>(own)->setAntialias(temp);
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
            return N_new CmdAntialiased();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    // Font objects usage count
    static int ft_usage_count = 0;
    // A handle to the FreeType library
    static FT_Library ft_lib;
    //------------------------------------------------------------------------
    #undef __FTERRORS_H__
    #define FT_ERRORDEF( e, v, s ) s,
    #define FT_ERROR_START_LIST static const char * ft_errors[] = {
    #define FT_ERROR_END_LIST 0};
    #include FT_ERRORS_H
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FreeTypeFont::FreeTypeFont(FontID fid, NIIf size, NCount pdi, bool antialias,
        const String & src, ResourceID rid, GroupID gid, ResLoadScheme * ls,
        ResResultScheme * rs, ScaleMode mode, const PlaneSizef & nativeRes, NIIf lineSpace, 
        LangID lid) :
            Font(N_CmdObj_FreeTypeFont, fid, FT_TrueType, src, rid, gid, ls, rs, mode, nativeRes, lid),
            mLineSpace(lineSpace),
            mPointSize(size),
            mDIP(pdi),
            mAntialias(antialias),
            mAlphaAsColour(false),
            mFontFace(0)
    {
        if(!ft_usage_count++)
            FT_Init_FreeType(&ft_lib);

        setupCmd(N_CmdObj_FreeTypeFont);
    }
    //------------------------------------------------------------------------
    FreeTypeFont::~FreeTypeFont()
    {
        destroyCodePointGlyph();

        if(!--ft_usage_count)
            FT_Done_FreeType(ft_lib);
    }
    //------------------------------------------------------------------------
    bool FreeTypeFont::initCmd(PropertyCmdSet * dest)
    {
        Font::initCmd(dest);
        dest->add(N_new CmdLineSpace());
        dest->add(N_new CmdPointSize());
        dest->add(N_new CmdAntialiased());
        return true;
    }
    //------------------------------------------------------------------------
    NCount FreeTypeFont::getTextureSize(Vector2ui & out, CodePoint s, CodePoint e) const
    {
        NCount glyphcnt = 0;
        NCount w = 0, h = 0, raw;
        CodePoint current = 0;

        for(current = s; current <= e; ++current)
        {
            // load glyph metrics (don't render)
            if(FT_Load_Char(mFontFace, current, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT))
                continue;

            if(ceil(mFontFace->glyph->metrics.width * FT_POS_COEF) > w)
                w = ceil(mFontFace->glyph->metrics.width * FT_POS_COEF);
            if(ceil(mFontFace->glyph->metrics.height * FT_POS_COEF) > h)
                h = ceil(mFontFace->glyph->metrics.height * FT_POS_COEF);

            ++glyphcnt;
        }
        raw = (w + INTER_GLYPH_PAD_SPACE) * (h + INTER_GLYPH_PAD_SPACE) * glyphcnt;
        raw = N_MIN(NCount, raw, __N_FreeType_PAGESIZE);

        Nui32 tex_side = static_cast<Nui32>(Math::sqrt((NIIf)raw));
        // 防止大小可能削减为半个字形.添加另一个字形宽/高
        tex_side += std::max(w, h);
        // 现在四舍五入到最接近2次方
        Nui32 roundUpSize = Bitwise::firstPO2From(tex_side);
        if(roundUpSize * roundUpSize * 0.5 >= raw)
        {
            out.y = static_cast<NCount>(roundUpSize * 0.5);
        }
        else
        {
            out.y = roundUpSize;
        }
        out.x = roundUpSize;

        glyphcnt = std::min(out.x / (w + INTER_GLYPH_PAD_SPACE), out.y / (h + INTER_GLYPH_PAD_SPACE));
        return glyphcnt;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::rasterCodePointImpl(CodePoint start, CodePoint end)
    {
        Texture * texture;
        CodePointList * codecol;
        bool finished = false;
        while(1)
        {
            Vector2ui texsize;
            getTextureSize(texsize, start, end);
            if(texsize == Vector2ui::ZERO)
                break;

            texture = N_Only(Texture).createTexture(0, GroupUnknow, texsize.x, texsize.y);
            codecol = N_new_t(CodePointList);

            // Create a memory buffer where we will render our glyphs
            Nui16 * mem_buffer = N_alloc_t(Nui16, texsize.x * texsize.y);
            memset(mem_buffer, 0, texsize.x * texsize.y * sizeof(Nui16));

            // Go ahead, line by line, top-left to bottom-right
            Nui32 x = INTER_GLYPH_PAD_SPACE, y = INTER_GLYPH_PAD_SPACE;
            Nui32 yb = INTER_GLYPH_PAD_SPACE;

            while(start <= end)
            {
                // Render the glyph
                if(FT_Load_Char(mFontFace, start, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                    (mAntialias ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO)))
                {
                    StringStream err;
                    err << "Font::loadFreetypeGlyph - Failed to load glyph for codepoint: ";
                    err << static_cast<Nui32>(start);
                    err << ".  Will use an empty image for this glyph!";
                    N_Only(Log).log(err.str().c_str());

                    codecol->insert(Npair(start, FontGlyph(Rectf(0, 0, 0, 0), Vector2f(0, 0), 0)));
                }
                else
                {
                    Nui32 glyph_w = mFontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
                    Nui32 glyph_h = mFontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;

                    // Check if glyph right margin does not exceed texture size
                    Nui32 x_next = x + glyph_w;
                    if(x_next > texsize.x)
                    {
                        x = INTER_GLYPH_PAD_SPACE;
                        x_next = x + glyph_w;
                        y = yb;
                    }

                    // Check if glyph bottom margine does not exceed texture size
                    Nui32 y_bot = y + glyph_h;
                    if(y_bot > texsize.y)
                        break;

                    genCodePointGlyph(mem_buffer + (y * texsize.x) + x, texsize.x, texsize.y);

                    // Create a new pb in the imageset
                    const Rectf area((NIIf)x, (NIIf)y,
                        (NIIf)(x + glyph_w - INTER_GLYPH_PAD_SPACE),
                        (NIIf)(y + glyph_h - INTER_GLYPH_PAD_SPACE));

                    const Vector2f offset(mFontFace->glyph->metrics.horiBearingX * FT_POS_COEF,
                        -mFontFace->glyph->metrics.horiBearingY * FT_POS_COEF);

                    codecol->insert(Npair(start, FontGlyph(area, offset,
                        mFontFace->glyph->metrics.horiAdvance * FT_POS_COEF)));

                    // Advance to next position
                    x = x_next;
                    if(y_bot > yb)
                    {
                        yb = y_bot;
                    }
                }

                finished = (start >= end);
                ++start;
            }

            texture->load((Nui8 *)mem_buffer, texsize.x, texsize.y, PF_L8A8i);
            N_free(mem_buffer);
            texture->ref(false);

            mTextureGlyphList.push_back(Npair(texture, codecol));
            mLoadRangeList.push_back(CodePointRange(codecol->begin()->first, (--codecol->end())->first));

            if(finished)
                break;
        }
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::genCodePointGlyph(Nui16 * out, NCount w, NCount) const
    {
        FT_Bitmap * glyph_bitmap = &mFontFace->glyph->bitmap;

        for(Nui i = 0; i < glyph_bitmap->rows; ++i)
        {
            Nui8 * src = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);
            switch(glyph_bitmap->pixel_mode)
            {
            case FT_PIXEL_MODE_GRAY:
            {
                Nui8 * dst = reinterpret_cast<Nui8 *>(out);
                for(Nui j = 0; j < glyph_bitmap->width; ++j)
                {
                    if(mAlphaAsColour)
                    {
                        *dst++ = *src;
                    }
                    else
                    {
                        *dst++ = 0xFF;
                    }
                    *dst++ = *src++;
                }
            }
            break;
            case FT_PIXEL_MODE_MONO:
            {
                for(Nui j = 0; j < glyph_bitmap->width; ++j)
                {
                    out[j] = (src [j / 8] & (0x80 >> (j & 7))) ? 0xFFFF : 0x0000;
                }
            }
            break;
            case FT_PIXEL_MODE_GRAY2:
            {
                Nui8 * dst = reinterpret_cast<Nui8 *>(out);
                for(Nui j = 0; j < glyph_bitmap->width; ++j)
                {
                    //Nui8 temp = (src[j / 4] & (0x3 << (j & 3) * 2)) * 85;  //分大小端？
                    Nui8 temp = (src[j / 4] & (0xC0 >> (j & 3) * 2)) * 85;
                    if(mAlphaAsColour)
                    {
                        *dst++ = temp;
                    }
                    else
                    {
                        *dst++ = 0xFF;
                    }
                    *dst++ = temp;
                }
            }
            break;
            case FT_PIXEL_MODE_GRAY4:
            {
                Nui8 * dst = reinterpret_cast<Nui8 *>(out);
                for(Nui j = 0; j < glyph_bitmap->width; ++j)
                {
                    //Nui8 temp = (src[j / 2] & (0xF << (j & 1) * 4)) * 17;  //分大小端？
                    Nui8 temp = (src[j / 2] & (0xF0 >> (j & 1) * 4)) * 17;
                    if(mAlphaAsColour)
                    {
                        *dst++ = temp;
                    }
                    else
                    {
                        *dst++ = 0xFF;
                    }
                    *dst++ = temp;
                }
            }
            break;
            case FT_PIXEL_MODE_BGRA:
            {
                Nui8 * dst = reinterpret_cast<Nui8 *>(out);
                for(Nui j = 0; j < glyph_bitmap->width; ++j)
                {
                    // RGBA
                    *dst++ = (*src + *(src + 1) + *(src + 2)) / 3;
                    *dst++ = *(src + 3);
                }
            }
            break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("TTF 文字像素错误"));
                break;
            }
            out += w;
        }
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::destroyCodePointGlyph()
    {
        if(mFontFace)
        {
            FT_Done_Face(mFontFace);
            mFontFace = 0;
        }

        TextureGlyphList::iterator i, iend = mTextureGlyphList.end();
        for(i = mTextureGlyphList.begin(); i != iend; ++i)
        {
            i->first->unref();
        }
        mTextureGlyphList.clear();

        MaterialList::iterator m, mend = mMaterialList.end();
        for(m = mMaterialList.begin(); m != mend; ++m)
        {
            (*mend)->unref();
        }
        mMaterialList.clear();

        if(mFontStream)
        {
            N_delete mFontStream;
            mFontStream = 0;
        }
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::reflushCodePointImpl()
    {
        destroyCodePointGlyph();

        planImpl();

        loadImpl();
    }
    //------------------------------------------------------------------------
    NIIf FreeTypeFont::getCodePointWidth(const FontGlyph * value) const
    {
        return value->getTextureCoord().width();
    }
    //------------------------------------------------------------------------
    NIIf FreeTypeFont::getCodePointHeight(const FontGlyph * value) const
    {
        return value->getTextureCoord().height();
    }
    //------------------------------------------------------------------------
    NIIf FreeTypeFont::getCodePointExtentImpl(const FontGlyph * value) const
    {
        return value->getTextureCoord().width() + value->getOffset().x;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::planImpl()
    {
        mFontStream = N_Only(ResourceScheme).open(mSource, mGroup == 0 ? N_Only(Font).getGroup() : mGroup);

        FT_Error error;

        Nui8 * tempdata;
        NCount datasize = mFontStream->cloneData(tempdata);
        // create face using input font
        if ((error = FT_New_Memory_Face(ft_lib, tempdata, static_cast<FT_Long>(datasize), 0, &mFontFace)) != 0)
        {
            String tempstr;
            StrConv::conv(ft_errors[error], tempstr);
            N_EXCEPT(Internal, _I18n("Failed to create face from font file '") + mSource +
                _I18n("' error was: ") + ((error < FT_Err_Max) ? tempstr : _I18n("unknown error")));
        }
        // check that default Unicode character map is available
        if(!mFontFace->charmap)
        {
            FT_Done_Face(mFontFace);
            mFontFace = 0;
            N_EXCEPT(Internal, _I18n("The font '") + N_conv(mFontID) +
                _I18n("' does not have a Unicode charmap, and cannot be used."));
        }

        NIIf hps = mPointSize * 64;
        NIIf vps = mPointSize * 64;
        if(mScaleMode != SM_Unknow)
        {
            hps *= mXScale;
            vps *= mYScale;
        }

        if(FT_Set_Char_Size(mFontFace, FT_F26Dot6(hps), FT_F26Dot6(vps), mDIP, mDIP))
        {
            // For bitmap fonts we can render only at specific point sizes.
            // Try to find nearest point size and use it, if that is possible
            NIIf ptSize_72 = (mPointSize * 72.0f) / mDIP;
            NIIf best_delta = 99999;
            NIIf best_size = 0;
            for(int i = 0; i < mFontFace->num_fixed_sizes; ++i)
            {
                NIIf size = mFontFace->available_sizes [i].size * FT_POS_COEF;
                NIIf delta = fabs(size - ptSize_72);
                if(delta < best_delta)
                {
                    best_delta = delta;
                    best_size = size;
                }
            }

            if((best_size <= 0) || FT_Set_Char_Size(mFontFace, 0, FT_F26Dot6(best_size * 64), 0, 0))
            {
                N_EXCEPT(Internal, _I18n("The font '") + N_conv(mFontID) + _I18n("' cannot be ")
                    _I18n("rasterised at a size of ") + N_conv(mPointSize) + _I18n(" points, and cannot be used."));
            }
        }

        if(mFontFace->face_flags & FT_FACE_FLAG_SCALABLE)
        {
            //NIIf x_scale = mFontFace->size->metrics.x_scale * FT_POS_COEF * (1.0/65536.0);
            NIIf y_scale = mFontFace->size->metrics.y_scale * FT_POS_COEF * (1.0f / 65536.0f);
            mAscLine = mFontFace->ascender * y_scale;
            mDesLine = mFontFace->descender * y_scale;
            mHeight = mFontFace->height * y_scale;
        }
        else
        {
            mAscLine = mFontFace->size->metrics.ascender * FT_POS_COEF;
            mDesLine = mFontFace->size->metrics.descender * FT_POS_COEF;
            mHeight = mFontFace->size->metrics.height * FT_POS_COEF;
        }

        if(mLineSpace > 0.0f)
        {
            mHeight = mLineSpace;
        }
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::loadImpl()
    {
        if(mDefineRangeList.empty())
        {
            mDefineRangeList.push_back(CodePointRange(33, 166));
        }

        CodePointRangeList::const_iterator r, rend = mDefineRangeList.end();
        for(r = mDefineRangeList.begin(); r != rend; ++r)
        {
            if(mMinCodePoint > r->first)
                mMinCodePoint = r->first;
            else if(mMinCodePoint > r->second)
                mMinCodePoint = r->second;

            if(mMaxCodePoint < r->first)
                mMaxCodePoint = r->first;
            else if(mMaxCodePoint < r->second)
                mMaxCodePoint = r->second;

            rasterCodePointImpl(r->first, r->second);
        }

        TextureGlyphList::iterator i, iend = mTextureGlyphList.end();
        for(i = mTextureGlyphList.begin(); i != iend; ++i)
        {
            Material * mat = static_cast<Material *>(N_Only(Material).create(0, mGroup));

            // 确保材质明确每顶点的颜色
            mat->setLight(false);
            mat->get(0)->get(0)->setColour(ShaderCh::SOT_Set)->setColourTracking(CTM_T_DIFFUSE);
            mat->get(0)->get(0)->setBlend(ShaderCh::SOT_Set)->setBlend(FBM_INV_ALPHA);
            mat->get(0)->get(0)->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);

            ShaderChTextureUnit * texunit = mat->get(0)->get(0)->setTexture(ShaderCh::SOT_Set)->create(i->first);
            // 避免模糊边缘
            texunit->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
            // 充许min/mag 过滤, 但没有 mip
            texunit->getSample()->setFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_NONE);

            mat->ref();
            mMaterialList.push_back(mat);
        }
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::unloadImpl()
    {
        destroyCodePointGlyph();
    }
    //------------------------------------------------------------------------
    NCount FreeTypeFont::calcSize() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::setDPI(NCount c)
    {
        mDIP = c;
    }
    //------------------------------------------------------------------------
    NCount FreeTypeFont::getDPI() const
    {
        return mDIP;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::setPointSize(NIIf size)
    {
        if(size == mPointSize)
            return;
        mPointSize = size;
        if(mState == LS_LOADED)
            reflushCodePointImpl();
        N_Only(Font).notifySize(this);
    }
    //------------------------------------------------------------------------
    NIIf FreeTypeFont::getPointSize() const
    {
        return mPointSize;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::setAntialias(bool b)
    {
        mAntialias = b;

        if(mState == LS_LOADED)
            reflushCodePointImpl();

        N_Only(Font).notifySize(this);
    }
    //------------------------------------------------------------------------
    bool FreeTypeFont::isAntialias() const
    {
        return mAntialias;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::setLineSpace(NIIf space)
    {
        mLineSpace = space;

        if(mState == LS_LOADED)
            reflushCodePointImpl();

        N_Only(Font).notifySize(this);
    }
    //------------------------------------------------------------------------
    NIIf FreeTypeFont::getLineSpace() const
    {
        return mLineSpace;
    }
    //------------------------------------------------------------------------
    void FreeTypeFont::setAlphaAsColour(bool b)
    {
        mAlphaAsColour = b;
        if(mState == LS_LOADED)
            reflushCodePointImpl();
    }
    //------------------------------------------------------------------------
    bool FreeTypeFont::isAlphaAsColour() const
    {
        return mAlphaAsColour;
    }
    //------------------------------------------------------------------------
}