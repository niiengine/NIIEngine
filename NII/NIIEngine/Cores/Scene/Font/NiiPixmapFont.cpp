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
#include "NiiPixmapFont.h"
#include "NiiTextureManager.h"
#include "NiiMaterialManager.h"
#include "NiiException.h"

namespace NII
{
    //------------------------------------------------------------------------
    class _EngineInner CmdDefineCodePoint : public PropertyCmd
    {
    public:
        CmdDefineCodePoint() : PropertyCmd(N_PropertyFont_PixelMap, _T("mapping"),
            _T("This is the glyph-to-image mapping font property. It cannot be read. "
                "Format is: codepoint,advance,imagename"))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            (void)own;
            (void)out;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 texidx, codepoint;
            NIIf coord[4];
            NIIf oft[2];
            NIIf adv;
            if(Nsscanf(val.c_str(), _T("%u %u %g %g %g %g %g %g %g"),
                &texidx, &codepoint, &coord[0], &coord[1], &coord[2], &coord[3],
                    &oft[0], &oft[1], &adv) != 9)
                N_EXCEPT(InvalidRequest, _T("Bad glyph Mapping specified: ") + val);

            static_cast<PixmapFont *>(own)->defineCodePoint(texidx, codepoint,
                Rectf(coord[0], coord[1], coord[2], coord[3]), Vector2f(oft[0], oft[1]), adv);
        }

        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            (void)own;
            (void)out;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * tempi0 = (Nui32 *)in;
            Nui32 * tempi1 = (Nui32 *)in;
            ++tempi1;
            ++tempi1;
            NIIf * tempf = (NIIf *)tempi1;

            static_cast<PixmapFont *>(own)->defineCodePoint(*tempi0, *++tempi0,
                Rectf(*tempf, *++tempf, *++tempf, *++tempf), Vector2f(*++tempf, *++tempf), *++tempf);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdDefineCodePoint();
        }
        /// @copydetails PropertyCmd::isReadable
        bool isReadable() const
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
    PixmapFont::PixmapFont(FontID fid, ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, LangID lid) :
            Font(N_CmdObj_PixmapFont, fid, FT_Pixel, _T(""), rid, gid, ls, rs, SM_Unknow, PlaneSizef(640.0f, 480.0f), lid),
            mLastXScale(1.0f),
            mLastYScale(1.0f)
    {
        setupCmd(N_CmdObj_PixmapFont);
    }
    //------------------------------------------------------------------------
    PixmapFont::PixmapFont(FontID fid, const String & src, ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, ScaleMode mode, const PlaneSizef & nativeRes, 
        LangID lid):
            Font(N_CmdObj_PixmapFont, fid, FT_Pixel, src, rid, gid, ls, rs, mode, nativeRes, lid),
            mLastXScale(1.0f),
            mLastYScale(1.0f)
    {
        setupCmd(N_CmdObj_PixmapFont);
    }
    //------------------------------------------------------------------------
    PixmapFont::~PixmapFont()
    {
    }
    //------------------------------------------------------------------------
    bool PixmapFont::initCmd(PropertyCmdSet * dest)
    {
        Font::initCmd(dest);
        dest->add(N_new CmdDefineCodePoint());
        return true;
    }
    //------------------------------------------------------------------------
    void PixmapFont::reflushCodePointImpl()
    {
        NIIf xfactor = (mScaleMode != SM_Unknow ? mXScale : 1.0f) / mLastXScale;
        NIIf yfactor = (mScaleMode != SM_Unknow ? mYScale : 1.0f) / mLastYScale;

        CodePointList::iterator j, jend;
        TextureGlyphList::iterator i, iend = mTextureGlyphList.end();
        for(i = mTextureGlyphList.begin(); i != iend; ++i)
        {
            for(j = i->second->begin(), jend = i->second->end(); j != jend; ++j)
            {
                j->second.setHoriAdvance(j->second.getHoriAdvance() * xfactor);
            }
        }

        mAscLine *= yfactor;
        mDesLine *= yfactor;
        mHeight *= yfactor;

        mLastXScale = mScaleMode != SM_Unknow ? mXScale : 1.0f;
        mLastYScale = mScaleMode != SM_Unknow ? mYScale : 1.0f;
    }
    //------------------------------------------------------------------------
    void PixmapFont::rasterCodePointImpl(CodePoint, CodePoint)
    {
        // 不支持
    }
    //------------------------------------------------------------------------
    NIIf PixmapFont::getCodePointWidth(const FontGlyph * value) const
    {
        return value->getTextureCoord().width() * mXScale;
    }
    //------------------------------------------------------------------------
    NIIf PixmapFont::getCodePointHeight(const FontGlyph * value) const
    {
        return value->getTextureCoord().height() * mYScale;
    }
    //------------------------------------------------------------------------
    NIIf PixmapFont::getCodePointExtentImpl(const FontGlyph * value) const
    {
        return (value->getTextureCoord().width() + value->getOffset().x) * mXScale;
    }
    //------------------------------------------------------------------------
    void PixmapFont::defineCodePoint(Nidx texidx, CodePoint value, const Rectf & coord,
        const Vector2f & oft, NIIf extent)
    {
        N_assert(texidx < mTextureGlyphList.size(), "error");
        NIIf adv = (extent == -1.0f) ? (NIIf)(coord.width() + oft.x) : extent;
        adv *= mLastXScale;

        if(value > mMaxCodePoint)
            mMaxCodePoint = value;
        else if(value < mMinCodePoint)
            mMinCodePoint = value;

        if(value > mLoadRangeList[texidx].second)
            mLoadRangeList[texidx].second = value;
        if(value < mLoadRangeList[texidx].first)
            mLoadRangeList[texidx].first = value;

        // create a new FontGlyph with given character code
        const FontGlyph glyph(coord, oft, adv);

        if(oft.y < -mAscLine)
            mAscLine = -oft.y;
        if(coord.height() + oft.y > -mDesLine)
            mDesLine = -(coord.height() + oft.y);

        mAscLine *= mLastYScale;
        mDesLine *= mLastYScale;
        mHeight = (mAscLine - mDesLine) * mLastYScale;
        mTextureGlyphList[texidx].second->insert(Npair(value, glyph));
    }
    //------------------------------------------------------------------------
    void PixmapFont::setTexture(ResourceID * rid, NCount count)
    {
        TextureGlyphList::iterator i, iend = mTextureGlyphList.end();
        for(i = mTextureGlyphList.begin(); i != iend; ++i)
        {
            i->first->unref();
        }

        for(NCount c = 0; c < count; ++c)
        {
            Texture * tex = static_cast<Texture *>(N_Only(Texture).get(*rid + c, mGroup));
            tex->ref();

            Material * mat = mMaterialList[c];
            mat->setLight(false);

            mat->get(0)->get(0)->setColour(ShaderCh::SOT_Set)->setColourTracking(CTM_T_DIFFUSE);
            mat->get(0)->get(0)->setBlend(ShaderCh::SOT_Set)->setBlend(PixelUtil::isAlpha(tex->getFormat()) ? FBM_INV_ALPHA : FBM_ADD);
            mat->get(0)->get(0)->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);
            mat->get(0)->get(0)->setTexture(ShaderCh::SOT_Set)->removeAll();
            ShaderChTextureUnit * texunit = mat->get(0)->get(0)->setTexture(ShaderCh::SOT_Set)->create(tex);
            // 避免模糊边缘
            texunit->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
            // 充许min/mag 过滤, 但没有 mip
            texunit->getSample()->setFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_NONE);

            mat->load();
            mTextureGlyphList[c].first = tex;
        }
    }
    //------------------------------------------------------------------------
    void PixmapFont::getTexture(TextureList & out) const
    {
        out.clear();
        TextureGlyphList::const_iterator t, tend = mTextureGlyphList.end();
        for(t = mTextureGlyphList.begin(); t != tend; ++t)
        {
            out.push_back(t->first);
        }
    }
    //------------------------------------------------------------------------
    void PixmapFont::loadImpl()
    {
        mLastXScale = mScaleMode != SM_Unknow ? mXScale : 1.0f;
        mLastYScale = mScaleMode != SM_Unknow ? mYScale : 1.0f;

        StringList vec;
        StrUtil::split(mSource, vec);
        StringList::iterator c, cend = vec.end();
        for(c = vec.begin(); c != cend; ++c)
        {
            Texture * tex = N_Only(Texture).createTexture(0, mGroup, *c);
            tex->ref();

            Material * mat = static_cast<Material *>(N_Only(Material).create(0, mGroup));
            mat->setLight(false);

            mat->get(0)->get(0)->setColour(ShaderCh::SOT_Set)->setColourTracking(CTM_T_DIFFUSE);
            mat->get(0)->get(0)->setBlend(ShaderCh::SOT_Set)->setBlend(PixelUtil::isAlpha(tex->getFormat()) ? FBM_INV_ALPHA : FBM_ADD);
            mat->get(0)->get(0)->setStencil(ShaderCh::SOT_Set)->setDepthEnable(false);
            ShaderChTextureUnit * texunit = mat->get(0)->get(0)->setTexture(ShaderCh::SOT_Set)->create(tex);
            // 避免模糊边缘
            texunit->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
            // 充许min/mag 过滤, 但没有 mip
            texunit->getSample()->setFiltering(TFOP_LINEAR, TFOP_LINEAR, TFOP_NONE);

            mat->ref();

            mMaterialList.push_back(mat);
            mTextureGlyphList.push_back(Npair(tex, N_new_t(CodePointList)));
            mLoadRangeList.push_back(CodePointRange());
        }
    }
    //------------------------------------------------------------------------
    void PixmapFont::unloadImpl()
    {
        TextureGlyphList::iterator i, iend = mTextureGlyphList.end();
        for(i = mTextureGlyphList.begin(); i != iend; ++i)
        {
            i->first->unref();
            N_delete_t(i->second, CodePointList);
        }
        mTextureGlyphList.clear();

        MaterialList::iterator m, mend = mMaterialList.end();
        for(m = mMaterialList.begin(); m != mend; ++m)
        {
            (*m)->unref();
        }
        mMaterialList.clear();

        mLoadRangeList.clear();
    }
    //------------------------------------------------------------------------
    NCount PixmapFont::calcSize() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
}