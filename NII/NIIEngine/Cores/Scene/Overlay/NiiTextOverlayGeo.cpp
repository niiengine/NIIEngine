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
#include "NiiTextOverlayGeo.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"
#include "NiiOverlayManager.h"
#include "NiiGBufferManager.h"
#include "NiiFontManager.h"
#include "NiiMaterial.h"
#include "NiiGpuBuffer.h"
#include "NiiViewport.h"
#include "NiiOverlayTypeList.h"
#include "NiiStrConv.h"

#define DEFAULT_INITIAL_CHARS 32

namespace NII
{
    //--------------------------------------------------------------------
    class _EngineInner CmdCharHeight : public PropertyCmd
    {
    public:
        CmdCharHeight() : PropertyCmd(N_PropertyTextCharGeo_GlyphHeight, _T("char_height"),
            _T("Sets the height of the characters in relation to the screen."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextOverlayGeo *>(own)->getGlyphHeight(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<TextOverlayGeo *>(own)->setGlyphHeight(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const TextOverlayGeo *>(own)->getGlyphHeight();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<TextOverlayGeo *>(own)->setGlyphHeight(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCharHeight();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.02f, out);
        }
    };
    //--------------------------------------------------------------------
    class _EngineInner CmdSpaceWidth : public PropertyCmd
    {
    public:
        CmdSpaceWidth() : PropertyCmd(N_PropertyTextCharGeo_SpaceWidth, _T("space_width"),
            _T("Sets the width of a space in relation to the screen."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextOverlayGeo *>(own)->getSpaceWidth(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<TextOverlayGeo *>(own)->setSpaceWidth(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const TextOverlayGeo *>(own)->getSpaceWidth();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<TextOverlayGeo *>(own)->setSpaceWidth(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSpaceWidth();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(0.0f, out);
        }
    };
    //--------------------------------------------------------------------
    class _EngineInner CmdFontName : public PropertyCmd
    {
    public:
        CmdFontName() : PropertyCmd(N_PropertyTextCharGeo_FontID, _T("font_name"),
            _T("Sets the name of the font to use."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextOverlayGeo *>(own)->getFontID(), out);
        }
        
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 fid;
            StrConv::conv(val, fid);
            static_cast<TextOverlayGeo *>(own)->setFontID(fid);
        }
        
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const TextOverlayGeo *>(own)->getFontID();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<TextOverlayGeo *>(own)->setFontID(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdFontName();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //--------------------------------------------------------------------
    class _EngineInner CmdColour : public PropertyCmd
    {
    public:
        CmdColour() : PropertyCmd(N_PropertyTextCharGeo_Colour, _T("colour"),
            _T("Sets the colour of the font (a solid colour)."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TextOverlayGeo *>(own)->getColour(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Colour temp;
            StrConv::conv(val, temp);
            static_cast<TextOverlayGeo *>(own)->setColour(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Colour * temp = (Colour *)out;
            *temp = static_cast<const TextOverlayGeo *>(own)->getColour();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            Colour * temp = (Colour *)in;
            static_cast<TextOverlayGeo *>(own)->setColour(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdColour();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(Colour::White, out);
        }
    };
    //--------------------------------------------------------------------
    class _EngineInner CmdAlignment : public PropertyCmd
    {
    public:
        CmdAlignment() : PropertyCmd(N_PropertyTextCharGeo_AlignMode, _T("alignment"),
            _T("Sets the alignment of the text: 'left', 'center' or 'right'."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            TextOverlayGeo::AlignMode align = static_cast<const TextOverlayGeo *>(own)->getAlignMode();
            switch(align)
            {
            case TextOverlayGeo::AM_Left:
                out = _T("left");
                break;
            case TextOverlayGeo::AM_Center:
                out = _T("center");
                break;
            case TextOverlayGeo::AM_Right:
                out = _T("right");
                break;
            default:
                out = _T("left");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            TextOverlayGeo * obj = static_cast<TextOverlayGeo *>(own);
            if(val == _T("center"))
            {
                obj->setAlignMode(TextOverlayGeo::AM_Center);
            }
            else if(val == _T("right"))
            {
                obj->setAlignMode(TextOverlayGeo::AM_Right);
            }
            else
            {
                obj->setAlignMode(TextOverlayGeo::AM_Left);
            }
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            TextOverlayGeo::AlignMode * temp = (TextOverlayGeo::AlignMode *)out;
            *temp = static_cast<const TextOverlayGeo *>(own)->getAlignMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const void * in)
        {
            TextOverlayGeo::AlignMode * temp = (TextOverlayGeo::AlignMode *)in;
            static_cast<TextOverlayGeo *>(own)->setAlignMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAlignment();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("left");
        }
    };
    //--------------------------------------------------------------------
    class _EngineInner TextCharGeo : public GeometryObj, public OverlayAlloc
    {
    public:
        TextCharGeo(TextOverlayGeo * layer, Material * mat, const GeometryRaw & geo, NCount oft, NCount cnt);

        virtual ~TextCharGeo();

        /// @copydetails GeometryObj::getMaterial
        const Material * getMaterial() const;

        /// @copydetails GeometryObj::getMatrix
        void getMatrix(Matrix4f * out, NCount & count) const;

        /// @copydetails GeometryObj::getMatrixCount
        NCount getMatrixCount() const;

        /// @copydetails GeometryObj::distanceSquared
        NIIf distanceSquared(const Camera * cam) const;

        /// @copydetails GeometryObj::getLight
        void getLight(LightList & ll) const;

        /// @copydetails GeometryObj::getGeometry
        void getGeometry(GeometryRaw & op) const;
    protected:
        TextOverlayGeo * mOverlay;
        Material * mMaterial;
        GeometryRaw * mGeo;
    };
    //--------------------------------------------------------------------
    TextCharGeo::TextCharGeo(TextOverlayGeo * layer, Material * mat, const GeometryRaw & geo,
        NCount oft, NCount cnt) :
            mOverlay(layer)
    {
        mGeo = N_new GeometryRaw(geo, oft, cnt);
        mMaterial = mat;
        if(mMaterial)
            mMaterial->ref();
    }
    //--------------------------------------------------------------------
    TextCharGeo::~TextCharGeo()
    {
        N_delete mGeo;
        if(mMaterial)
            mMaterial->unref();
    }
    //--------------------------------------------------------------------
    const Material * TextCharGeo::getMaterial() const
    {
        return mMaterial;
    }
    //--------------------------------------------------------------------
    void TextCharGeo::getMatrix(Matrix4f * out, NCount & count) const
    {
        mOverlay->getMatrix(out, count);
    }
    //--------------------------------------------------------------------
    NCount TextCharGeo::getMatrixCount() const
    {
        return 1;
    }
    //--------------------------------------------------------------------
    void TextCharGeo::getGeometry(GeometryRaw & op) const
    {
        op = *mGeo;
    }
    //--------------------------------------------------------------------
    NIIf TextCharGeo::distanceSquared(const Camera * cam) const
    {
        (void)cam;
        return 10000.0f - (NIIf)mOverlay->getZOrder();
    }
    //--------------------------------------------------------------------
    void TextCharGeo::getLight(LightList & ll) const
    {
        ll.clear();
    }
    //------------------------------------------------------------------------
    #define POS_TEX_BINDING 0
    #define COLOUR_BINDING 1
    #define UNICODE_NEL 0x0085
    #define UNICODE_CR 0x000D
    #define UNICODE_LF 0x000A
    #define UNICODE_SPACE 0x0020
    #define UNICODE_ZERO 0x0030
    //------------------------------------------------------------------------
    TextOverlayGeo::TextOverlayGeo(OverlayGeoID id, LangID lid) :
        OverlayGeo(id, N_CmdObj_TextOverlayGeo, lid),
        mSpaceWidth(0),
        mCharCount(0),
        mAlignment(TextOverlayGeo::AM_Left),
        mColour(Colour::White)
    {
        mCharHeight = 0.02f;
        mPixelCharHeight = 12;
        mPixelSpaceWidth = 0;

        setupCmd(N_CmdObj_TextOverlayGeo);
    }
    //------------------------------------------------------------------------
    TextOverlayGeo::~TextOverlayGeo()
    {
        if(mInit)
        {
            N_Only(GBuffer).destroy(mGeo.mVertex);
        }
        if(mFont)
        {
            mFont->unref();
            mFont = 0;
        }
        freeChar();
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::init()
    {
        if(!mInit)
        {
            NCount offset = 0;
            mGeo.mType = GeometryRaw::OT_TRIANGLE_LIST;
            mGeo.remove(GRT_Index | GRT_Instancing);

            // Set up the render op
            // Combine positions and texture coords since they tend to change together
            // since character sizes are different
            N_Only(GBuffer).create(mGeo.mVertex);
            VertexData * vd = mGeo.mVertex;
            vd->mOffset = 0;

            vd->add(DT_Float3x, VT_Pos, POS_TEX_BINDING, offset);

            offset += DataTypeUtil::getSize(DT_Float3x);
            vd->add(DT_Float2x, VT_Tex_Coord, POS_TEX_BINDING, offset, 0);

            offset += DataTypeUtil::getSize(DT_Float2x);
            vd->add(DT_Colour, VT_Diffuse, COLOUR_BINDING, 0);

            // Vertex buffer will be created in updateText
            updateText(DEFAULT_INITIAL_CHARS);
            mInit = true;
        }
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::updateText(NCount count)
    {
        if(mCharCount < count)
        {
            VertexBuffer * vbuf;
            // Create and bind new buffers
            // Note that old buffers will be deleted automatically through reference counting

            // 6 verts per NIIb since we're doing tri lists without indexes
            // Allocate space for positions & texture coords
            VertexData * vd = mGeo.mVertex;
            vd->mCount = count * 6;

            N_Only(GBuffer).create(vbuf, vd->getUnitSize(POS_TEX_BINDING), vd->mCount, Buffer::M_DEVWRITE | Buffer::M_DEVREAD);
            vd->attach(POS_TEX_BINDING, vbuf);

            N_Only(GBuffer).create(vbuf, vd->getUnitSize(COLOUR_BINDING), vd->mCount, Buffer::M_DEVWRITE | Buffer::M_DEVREAD);
            vd->attach(COLOUR_BINDING, vbuf);

            mCharCount = count;
            updateColours();
        }
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::freeChar()
    {
        CharGeoList::iterator i, iend = mCharGeoList.end();
        for(i = mCharGeoList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mCharGeoList.clear();
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::updateGeoPos()
    {
        if(mFont == 0)
        {
            return;
        }
        freeChar();

        updateText(mCaption.size());

        NIIf largestWidth = 0;
        NIIf left = getEnvLeft() * 2.0f - 1.0f;
        NIIf top = -((getEnvTop() * 2.0f) - 1.0f);
        NIIf aspectCoef = (NIIf)(mViewport->getViewHeight())/(NIIf)(mViewport->getViewWidth());

        // Derive space with from a number 0
        if(mSpaceWidth == 0)
        {
            mSpaceWidth = mFont->getCharAspect(UNICODE_ZERO) * mCharHeight * 2.0f * aspectCoef;
        }

        Nidx pageidx = 0;
        NCount curchar = 0;
        // Use iterator
        bool newLine = true;

        VertexData * vb = mGeo.mVertex;
        VertexBuffer * vbuf = vb->getAttach(POS_TEX_BINDING);
        NIIf * data = static_cast<NIIf *>(vbuf->lock(Buffer::MM_WHOLE));
        ViewString::iterator i, iend = mCaption.end();
        for(i = mCaption.begin(); i != iend; ++i, ++curchar)
        {
            if(newLine)
            {
                NIIf len = 0.0f;
                for(ViewString::iterator j = i; j != iend; j++)
                {
                    CodePoint character = N_Font_Char(j);

                    if(character == UNICODE_CR || character == UNICODE_NEL || character == UNICODE_LF)
                    {
                        break;
                    }
                    else if(character == UNICODE_SPACE) // space
                    {
                        len += mSpaceWidth;
                    }
                    else
                    {
                        len += mFont->getCharAspect(character) * mCharHeight * 2.0f * aspectCoef;
                    }
                }

                if(mAlignment == TextOverlayGeo::AM_Right)
                    left -= len;
                else if(mAlignment == TextOverlayGeo::AM_Center)
                    left -= len * 0.5f;

                newLine = false;
            }

            CodePoint character = N_Font_Char(i);
            if(character == UNICODE_CR || character == UNICODE_NEL || character == UNICODE_LF)
            {
                left = getEnvLeft() * 2.0f - 1.0f;
                top -= mCharHeight * 2.0f;
                newLine = true;
                // Also reduce tri count
                vb->mCount -= 6;

                // consume CR/LF in one
                if(character == UNICODE_CR)
                {
                    ViewString::iterator peeki = i;
                    peeki++;
                    if (peeki != iend && N_Font_Char(peeki) == UNICODE_LF)
                    {
                        i = peeki; // skip both as one newline
                        // Also reduce tri count
                        vb->mCount -= 6;
                    }

                }
                continue;
            }
            else if(character == UNICODE_SPACE) // space
            {
                // Just leave a gap, no tris
                left += mSpaceWidth;
                // Also reduce tri count
                vb->mCount -= 6;
                continue;
            }

            const FontGlyph * glyph = mFont->getCodePoint(character, pageidx);

            NIIf horiz_height = mFont->getCharAspect(glyph) * aspectCoef ;
            const Rectf & coord = glyph->getTextureCoord();
            Material * mat = mFont->getMaterial(pageidx);
            Texture * tex = mFont->getTexture(pageidx);

            NIIf zValue = N_Engine().getRender()->getMaxDepth();
            // each vert is (x, y, z, u, v)
            //-----------------------------------------------------
            // First tri
            //
            // Upper left
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mLeft / tex->getWidth();
            *data++ = coord.mTop / tex->getHeight();

            top -= mCharHeight * 2.0f;

            // Bottom left
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mLeft / tex->getWidth();
            *data++ = coord.mBottom / tex->getHeight();

            top += mCharHeight * 2.0f;
            left += horiz_height * mCharHeight * 2.0f;

            // Top right
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mRight / tex->getWidth();
            *data++ = coord.mTop / tex->getHeight();
            //---------------------------------------------------

            //---------------------------------------------------
            // Second tri
            //
            // Top right (again)
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mRight / tex->getWidth();
            *data++ = coord.mTop / tex->getHeight();

            top -= mCharHeight * 2.0f;
            left -= horiz_height  * mCharHeight * 2.0f;

            // Bottom left (again)
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mLeft / tex->getWidth();
            *data++ = coord.mBottom / tex->getHeight();

            left += horiz_height  * mCharHeight * 2.0f;

            // Bottom right
            *data++ = left;
            *data++ = top;
            *data++ = zValue;
            *data++ = coord.mRight / tex->getWidth();
            *data++ = coord.mBottom / tex->getHeight();
            //--------------------------------------------------

            // Go back up with top
            top += mCharHeight * 2.0f;

            NIIf currentWidth = (left + 1) / 2 - getEnvLeft();
            if(currentWidth > largestWidth)
            {
                largestWidth = currentWidth;
            }

            mCharGeoList.push_back(N_new TextCharGeo(this, mat, mGeo, curchar * 6, 6));
        }
        vbuf->unlock();

        if(mMetricsMode == SM_Abs)
        {
            // Derive parametric version of dimensions
            NIIf vpWidth = (NIIf)(mViewport->getViewWidth());
            largestWidth *= vpWidth;
        }

        if(getWidth() < largestWidth)
            setWidth(largestWidth);
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::updateGeoTex()
    {
        // Nothing to do, we combine positions and textures
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setCaption(const ViewString & caption)
    {
        mCaption = caption;
        mGeoPosDirty = true;
        mGeoTexDirty = true;
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setFontID(FontID id)
    {
        if(mFont && mFont->getID() != id)
        {
            mFont->unref();
        }
        else
        {
            return;
        }

        mFont = static_cast<Font *>(N_Only(Font).get(id));
        if(mFont == 0)
            N_EXCEPT(NotExist, _I18n("Could not find font ") + id);
        mFont->ref(true);

        if(mMaterial == 0)
        {
            mMaterial = mFont->getMaterial(0);
            if(mMaterial)
            {
                mMaterial->ref(false);
                mMaterialID = mMaterial->getOriginID();
            }
            else
            {
                mMaterialID = 0;
            }
        }

        mGeoPosDirty = true;
        mGeoTexDirty = true;
    }
    //------------------------------------------------------------------------
    FontID TextOverlayGeo::getFontID() const
    {
        return mFont->getID();
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setGlyphHeight(NIIf height)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelCharHeight = static_cast<Nui16>(height);
        }
        else
        {
            mCharHeight = height;
        }
        mGeoPosDirty = true;
    }
    //------------------------------------------------------------------------
    NIIf TextOverlayGeo::getGlyphHeight() const
    {
        if(mMetricsMode == SM_Abs)
        {
            return mPixelCharHeight;
        }
        else
        {
            return mCharHeight;
        }
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setSpaceWidth(NIIf width)
    {
        if(mMetricsMode != SM_Unit)
        {
            mPixelSpaceWidth = static_cast<Nui16>(width);
        }
        else
        {
            mSpaceWidth = width;
        }

        mGeoPosDirty = true;
    }
    //------------------------------------------------------------------------
    NIIf TextOverlayGeo::getSpaceWidth() const
    {
        if(mMetricsMode == SM_Abs)
        {
            return mPixelSpaceWidth;
        }
        else
        {
            return mSpaceWidth;
        }
    }
    //------------------------------------------------------------------------
    OverlayGeoType TextOverlayGeo::getTypeID() const
    {
        return N_OverlayType_Text;
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::getGeometry(GeometryRaw & op) const
    {
        op = mGeo;
    }
    //------------------------------------------------------------------------
    bool TextOverlayGeo::initCmd(PropertyCmdSet * dest)
    {
        OverlayGeo::initCmd(dest);
        dest->add(N_new CmdCharHeight());
        dest->add(N_new CmdSpaceWidth());
        dest->add(N_new CmdFontName());
        dest->add(N_new CmdColour());
        dest->add(N_new CmdAlignment());
        return true;
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setColour(const Colour & col)
    {
        mColour = col;
        if(mInit)
            updateColours();
    }
    //------------------------------------------------------------------------
    const Colour & TextOverlayGeo::getColour() const
    {
        return mColour;
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setAlignMode(AlignMode a)
    {
        mAlignment = a;
        mGeoPosDirty = true;
    }
    //------------------------------------------------------------------------
    TextOverlayGeo::AlignMode TextOverlayGeo::getAlignMode() const
    {
        return mAlignment;
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::updateColours()
    {
        RGBA colour;
        N_Engine().getRender()->convertType(mColour, &colour);

        /**
            1--------2
            |        |
            |        |
            |        |
            |        |
            4--------3
        */
        VertexBuffer * vbuf = mGeo.mVertex->getAttach(COLOUR_BINDING);
        RGBA * data = static_cast<RGBA *>(vbuf->lock(Buffer::MM_WHOLE));
        for(NCount i = 0; i < mCharCount; ++i)
        {
            // First tri (1, 3, 2)
            *data++ = colour;
            *data++ = colour;
            *data++ = colour;
            // Second tri (1, 4, 3)
            *data++ = colour;
            *data++ = colour;
            *data++ = colour;
        }
        vbuf->unlock();
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::setSizeMode(SizeMode sm)
    {
        NIIf vpWidth = (NIIf)(mViewport->getViewWidth());
        NIIf vpHeight = (NIIf)(mViewport->getViewHeight());

        OverlayGeo::setSizeMode(sm);

        switch(mMetricsMode)
        {
        case SM_Abs:
        {
            // set pixel variables based on viewport multipliers
            mPixelCharHeight = static_cast<Nui16>(mCharHeight * vpHeight);
            mPixelSpaceWidth = static_cast<Nui16>(mSpaceWidth * vpHeight);
        }
        break;
        case SM_Rel:
        {
            // set pixel variables multiplied by the height constant
            mPixelCharHeight = static_cast<Nui16>(mCharHeight * 10000.0);
            mPixelSpaceWidth = static_cast<Nui16>(mSpaceWidth * 10000.0);
        }
        break;
        default:
            break;
        }
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::_update()
    {
        NIIf vpWidth = (NIIf)(mViewport->getViewWidth());
        NIIf vpHeight = (NIIf)(mViewport->getViewHeight());

        // Check size if pixel-based / relative-aspect-adjusted
        switch(mMetricsMode)
        {
        case SM_Abs:
        {
            if(!mViewport->isSyncAspect() || mGeoPosDirty)
            {
                // recalculate character size
                mCharHeight = (NIIf) mPixelCharHeight / vpHeight;
                mSpaceWidth = (NIIf) mPixelSpaceWidth / vpHeight;
                mGeoPosDirty = true;
            }
        }
        break;
        case SM_Rel:
        {
            if(!mViewport->isSyncAspect() || mGeoPosDirty)
            {
                // recalculate character size
                mCharHeight = (NIIf) mPixelCharHeight / 10000.0f;
                mSpaceWidth = (NIIf) mPixelSpaceWidth / 10000.0f;
                mGeoPosDirty = true;
            }
        }
        break;
        default:
            break;
        }

        OverlayGeo::_update();
    }
    //------------------------------------------------------------------------
    void TextOverlayGeo::queue(RenderQueue * queue)
    {
        if(mVisible)
        {
            //queue->add(this, RQG_Overlay, mZOrder);
            CharGeoList::iterator i, iend = mCharGeoList.end();
            for(i = mCharGeoList.begin(); i != iend; ++i)
            {
                queue->add(*i, RQG_Overlay, mZOrder);
            }
        }
    }
    //------------------------------------------------------------------------
}