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
#include "NiiColour.h"
#include "NiiMath.h"

namespace NII
{
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // Colour
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    const Colour Colour::ZERO =    Colour(0.0, 0.0, 0.0, 0.0);
    const Colour Colour::Black =   Colour(0.0, 0.0, 0.0);
    const Colour Colour::White =   Colour(1.0, 1.0, 1.0);
    const Colour Colour::Red =     Colour(1.0, 0.0, 0.0);
    const Colour Colour::Green =   Colour(0.0, 1.0, 0.0);
    const Colour Colour::Blue =    Colour(0.0, 0.0, 1.0);
    const Colour Colour::Yellow =  Colour(1.0, 1.0, 0.0);
    const Colour Colour::CyanBlue =Colour(0.0, 1.0, 1.0);
    const Colour Colour::Magenta = Colour(1.0, 0.0, 1.0);
    //---------------------------------------------------------------------
    Colour::Colour(NIIf val[4])
    {
        r = val[0];
        g = val[1];
        b = val[2];
        a = val[3];
    }
    //---------------------------------------------------------------------
    Colour::Colour(NIIf val[3], NIIf _a):
        a(_a)
    {
        r = val[0];
        g = val[1];
        b = val[2];
    }
    //---------------------------------------------------------------------
    Colour::Colour(NIIf _r, NIIf _g, NIIf _b, NIIf _a) :
        r(_r),
        g(_g),
        b(_b),
        a(_a)
    {
    }
    //---------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    ABGR Colour::getABGR() const
#else
    RGBA Colour::getRGBA() const
#endif
    {
        Nui8 val8;
        Nui32 val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Red
        val8 = static_cast<Nui8>(r * 255);
        val32 = val8 << 24;

        // Green
        val8 = static_cast<Nui8>(g * 255);
        val32 += val8 << 16;

        // Blue
        val8 = static_cast<Nui8>(b * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = static_cast<Nui8>(a * 255);
        val32 += val8;

        return val32;
    }
    //---------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    BGRA Colour::getBGRA() const
#else
    ARGB Colour::getARGB() const
#endif
    {
        Nui8 val8;
        Nui32 val32 = 0;

        // Convert to 32bit pattern
        // (ARGB = 8888)

        // Alpha
        val8 = static_cast<Nui8>(a * 255);
        val32 = val8 << 24;

        // Red
        val8 = static_cast<Nui8>(r * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<Nui8>(g * 255);
        val32 += val8 << 8;

        // Blue
        val8 = static_cast<Nui8>(b * 255);
        val32 += val8;

        return val32;
    }
    //---------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    ARGB Colour::getARGB() const
#else
    BGRA Colour::getBGRA() const
#endif
    {
        Nui8 val8;
        Nui32 val32 = 0;

        // Convert to 32bit pattern
        // (ARGB = 8888)

        // Blue
        val8 = static_cast<Nui8>(b * 255);
        val32 = val8 << 24;

        // Green
        val8 = static_cast<Nui8>(g * 255);
        val32 += val8 << 16;

        // Red
        val8 = static_cast<Nui8>(r * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = static_cast<Nui8>(a * 255);
        val32 += val8;

        return val32;
    }
    //---------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    RGBA Colour::getRGBA() const
#else
    ABGR Colour::getABGR() const
#endif
    {
        Nui8 val8;
        Nui32 val32 = 0;

        // Convert to 32bit pattern
        // (ABRG = 8888)

        // Alpha
        val8 = static_cast<Nui8>(a * 255);
        val32 = val8 << 24;

        // Blue
        val8 = static_cast<Nui8>(b * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<Nui8>(g * 255);
        val32 += val8 << 8;

        // Red
        val8 = static_cast<Nui8>(r * 255);
        val32 += val8;

        return val32;
    }
    //---------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    void Colour::setABGR(ABGR val)
#else
    void Colour::setRGBA(RGBA val)
#endif
    {
        Nui32 val32 = val;

        // Convert from 32bit pattern
        // (RGBA = 8888)

        // Red
        r = ((val32 >> 24) & 0xFF) / 255.0f;

        // Green
        g = ((val32 >> 16) & 0xFF) / 255.0f;

        // Blue
        b = ((val32 >> 8) & 0xFF) / 255.0f;

        // Alpha
        a = (val32 & 0xFF) / 255.0f;
    }
    //---------------------------------------------------------------------
    NIIf Colour::getHue() const
    {
        NIIf pred = r;
        NIIf pgreen = g;
        NIIf pblue = b;

        NIIf pmax = std::max(std::max(r, g), b);
        NIIf pmin = std::min(std::min(r, g), b);

        NIIf phue;

        if(pmax == pmin)
        {
            phue = 0;
        }
        else
        {
            if(pmax == pred)
            {
                phue = (pgreen - pblue) / (pmax - pmin);
            }
            else if(pmax == pgreen)
            {
                phue = 2 + (pblue - pred) / (pmax - pmin);
            }
            else
            {
                phue = 4 + (pred - pgreen) / (pmax - pmin);
            }
        }

        NIIf hue = phue / 6;
        if(hue < 0)
            hue += 1;

        return hue;
    }
    //------------------------------------------------------------------------
    NIIf Colour::getSatura() const
    {
        NIIf pmax = std::max(std::max(r, g), b);
        NIIf pmin = std::min(std::min(r, g), b);

        NIIf plum = (pmax + pmin) / 2;
        NIIf psat;

        if(pmax == pmin)
        {
            psat = 0;
        }
        else
        {
            if(plum < 0.5)
                psat = (pmax - pmin) / (pmax + pmin);
            else
                psat = (pmax - pmin) / (2 - pmax - pmin);
        }

        return psat;
    }
    //------------------------------------------------------------------------
    NIIf Colour::getLumen() const
    {
        NIIf pmax = std::max(std::max(r, g), b);
        NIIf pmin = std::min(std::min(r, g), b);

        NIIf plum = (pmax + pmin) / 2;
        return plum;
    }
    //------------------------------------------------------------------------
    void Colour::invert()
    {
        r = 1.0f - r;
        g = 1.0f - g;
        b = 1.0f - b;
    }
    //------------------------------------------------------------------------
    void Colour::clamp()
    {
        if (r < 0.0f)
            r = 0.0f;
        else if (r > 1.0f)
            r = 1.0f;

        if (g < 0.0f)
            g = 0.0f;
        else if (g > 1.0f)
            g = 1.0f;

        if (b < 0.0f)
            b = 0.0f;
        else if (b > 1.0f)
            b = 1.0f;

        if (a < 0.0f)
            a = 0.0f;
        else if (a > 1.0f)
            a = 1.0f;
    }
    //------------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    void Colour::setBGRA(BGRA val)
#else
    void Colour::setARGB(ARGB val)
#endif
    {
        Nui32 val32 = val;

        // Convert from 32bit pattern
        // (ARGB = 8888)

        // Alpha
        a = ((val32 >> 24) & 0xFF) / 255.0f;

        // Red
        r = ((val32 >> 16) & 0xFF) / 255.0f;

        // Green
        g = ((val32 >> 8) & 0xFF) / 255.0f;

        // Blue
        b = (val32 & 0xFF) / 255.0f;
    }
    //------------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    void Colour::setARGB(ARGB val)
#else
    void Colour::setBGRA(BGRA val)
#endif
    {
        Nui32 val32 = val;

        // Convert from 32bit pattern
        // (ARGB = 8888)

        // Blue
        b = ((val32 >> 24) & 0xFF) / 255.0f;

        // Green
        g = ((val32 >> 16) & 0xFF) / 255.0f;

        // Red
        r = ((val32 >> 8) & 0xFF) / 255.0f;

        // Alpha
        a = (val32 & 0xFF) / 255.0f;
    }
    //------------------------------------------------------------------------
#if N_ENDIAN == N_ENDIAN_B
    void Colour::setRGBA(RGBA val)
#else
    void Colour::setABGR(ABGR val)
#endif
    {
        Nui32 val32 = val;

        // Convert from 32bit pattern
        // (ABGR = 8888)

        // Alpha
        a = ((val32 >> 24) & 0xFF) / 255.0f;

        // Blue
        b = ((val32 >> 16) & 0xFF) / 255.0f;

        // Green
        g = ((val32 >> 8) & 0xFF) / 255.0f;

        // Red
        r = (val32 & 0xFF) / 255.0f;
    }
    //------------------------------------------------------------------------
    Colour Colour::operator / (NIIf f) const
    {
        N_assert(f != 0.0, "error");
        NIIf fInv = 1.0f / f;
        return Colour(r * fInv, g * fInv, b * fInv, a * fInv);
    }
    //------------------------------------------------------------------------
    Colour operator * (NIIf f, const Colour & o)
    {
        Colour re;
        re.r = f * o.r;
        re.g = f * o.g;
        re.b = f * o.b;
        re.a = f * o.a;

        return re;
    }
    //------------------------------------------------------------------------
    Colour operator / (NIIf f, const Colour & o)
    {
        N_assert(f != 0.0, "error");
        Colour re;
        NIIf fInv = 1.0f / f;
        re.r = o.r * fInv;
        re.g = o.g * fInv;
        re.b = o.b * fInv;
        re.a = o.a * fInv;

        return re;
    }
    //------------------------------------------------------------------------
    Colour & Colour::operator /= (const Colour & o)
    {
        r = o.r == 0 ? r : r / o.r;
        g = o.g == 0 ? g : g / o.g;
        b = o.b == 0 ? b : b / o.b;
        a = o.a == 0 ? a : a / o.a;

        return *this;
    }
    //------------------------------------------------------------------------
    Colour & Colour::operator /= (const NIIf f)
    {
        N_assert(f != 0.0, "error");
        NIIf fInv = 1.0f / f;
        r *= fInv;
        g *= fInv;
        b *= fInv;
        a *= fInv;

        return *this;
    }
    //------------------------------------------------------------------------
    void Colour::setHSL(NIIf hue, NIIf satura, NIIf lumen)
    {
        // wrap hue
        if(hue > 1.0f)
        {
            hue -= (NIIi)hue;
        }
        else if (hue < 0.0f)
        {
            hue += (NIIi)hue + 1;
        }
        // clamp satura / lumen
        satura = std::min(satura, 1.0f);
        satura = std::max(satura, 0.0f);
        lumen = std::min(lumen, 1.0f);
        lumen = std::max(lumen, 0.0f);

        if (lumen == 0.0f)
        {
            // early exit, this has to be black
            r = g = b = 0.0f;
            return;
        }

        if (satura == 0.0f)
        {
            // early exit, this has to be grey

            r = g = b = lumen;
            return;
        }

        NIIf hueDomain  = hue * 6.0f;
        if (hueDomain >= 6.0f)
        {
            // wrap around, and allow mathematical errors
            hueDomain = 0.0f;
        }
        Nui16 domain = (Nui16)hueDomain;
        NIIf f1 = lumen * (1 - satura);
        NIIf f2 = lumen * (1 - satura * (hueDomain - domain));
        NIIf f3 = lumen * (1 - satura * (1 - (hueDomain - domain)));

        switch (domain)
        {
        case 0:
            // red domain; green ascends
            r = lumen;
            g = f3;
            b = f1;
            break;
        case 1:
            // yellow domain; red descends
            r = f2;
            g = lumen;
            b = f1;
            break;
        case 2:
            // green domain; blue ascends
            r = f1;
            g = lumen;
            b = f3;
            break;
        case 3:
            // cyan domain; green descends
            r = f1;
            g = f2;
            b = lumen;
            break;
        case 4:
            // blue domain; red ascends
            r = f3;
            g = f1;
            b = lumen;
            break;
        case 5:
            // magenta domain; blue descends
            r = lumen;
            g = f1;
            b = f2;
            break;
        }
    }
    //------------------------------------------------------------------------
    void Colour::getHSL(NIIf * hue, NIIf * satura, NIIf * lumen) const
    {
        NIIf vMin = std::min(r, std::min(g, b));
        NIIf vMax = std::max(r, std::max(g, b));
        NIIf delta = vMax - vMin;

        *lumen = vMax;

        if (Math::isEqual(delta, 0.0f, 1e-6f))
        {
            // grey
            *hue = 0;
            *satura = 0;
        }
        else
        {
            // a colour
            *satura = delta / vMax;

            NIIf deltaR = (((vMax - r) / 6.0f) + (delta / 2.0f)) / delta;
            NIIf deltaG = (((vMax - g) / 6.0f) + (delta / 2.0f)) / delta;
            NIIf deltaB = (((vMax - b) / 6.0f) + (delta / 2.0f)) / delta;

            if (Math::isEqual(r, vMax))
                *hue = deltaB - deltaG;
            else if (Math::isEqual(g, vMax))
                *hue = 0.3333333f + deltaR - deltaB;
            else if (Math::isEqual(b, vMax))
                *hue = 0.6666667f + deltaG - deltaR;

            if (*hue < 0.0f)
                *hue += 1.0f;
            if (*hue > 1.0f)
                *hue -= 1.0f;
        }
    }
    //------------------------------------------------------------------------
    String Colour::toStr() const
    {
        Nostringstream temp;
        temp << this;
        return temp.str();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ColourArea
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ColourArea::ColourArea() :
        mID(0),
        mTL(Colour::ZERO),
        mTR(Colour::ZERO),
        mBL(Colour::ZERO),
        mBR(Colour::ZERO)
    {
    }
    //------------------------------------------------------------------------
    ColourArea::ColourArea(const Colour & top_left, const Colour & top_right,
        const Colour & bottom_left, const Colour & bottom_right) :
        mID(0),
        mTL(top_left),
        mTR(top_right),
        mBL(bottom_left),
        mBR(bottom_right)
    {
    }
    //------------------------------------------------------------------------
    ColourArea::ColourArea(const Colour & col) :
        mID(0),
        mTL(col),
        mTR(col),
        mBL(col),
        mBR(col){}
    //------------------------------------------------------------------------
    ColourArea & ColourArea::operator *=(const ColourArea & o)
    {
        mTL *= o.mTL;
        mTR *= o.mTR;
        mBL *= o.mBL;
        mBR *= o.mBR;

        return *this;
    }
    //------------------------------------------------------------------------
    ColourArea & ColourArea::operator=(const ColourArea & o)
    {
        mTL = o.mTL;
        mTR = o.mTR;
        mBL = o.mBL;
        mBR = o.mBR;

        return *this;
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator+(NIIf factor) const
    {
        return ColourArea(mTL + factor, mTR + factor, mBL + factor, mBR + factor);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator-(NIIf factor) const
    {
        return ColourArea(mTL - factor, mTR - factor, mBL - factor, mBR - factor);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator*(NIIf factor) const
    {
        return ColourArea(mTL * factor, mTR * factor, mBL * factor, mBR * factor);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator/(NIIf factor) const
    {
        return ColourArea(mTL / factor, mTR / factor, mBL / factor, mBR / factor);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator+(const ColourArea & o) const
    {
        return ColourArea(mTL + o.mTL, mTR + o.mTR, mBL + o.mBL, mBR + o.mBR);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator-(const ColourArea & o) const
    {
        return ColourArea(mTL - o.mTL, mTR - o.mTR, mBL - o.mBL, mBR - o.mBR);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator*(const ColourArea & o) const
    {
        return ColourArea(mTL * o.mTL, mTR * o.mTR, mBL * o.mBL, mBR * o.mBR);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::operator/(const ColourArea & o) const
    {
        return ColourArea(mTL, mTR, mBL, mBR);
    }
    //------------------------------------------------------------------------
    Colour ColourArea::get(NIIf x, NIIf y) const
    {
        Colour h1((mTR - mTL) * x + mTL);
        Colour h2((mBR - mBL) * x + mBL);
        return Colour((h2 - h1) * y + h1);
    }
    //------------------------------------------------------------------------
    ColourArea ColourArea::get(const Rectf & rect) const
    {
        return ColourArea(get(rect.mLeft, rect.mTop), get(rect.mRight, rect.mTop),
            get(rect.mLeft, rect.mBottom), get(rect.mRight, rect.mBottom));
    }
    //------------------------------------------------------------------------
    void ColourArea::setAlphaFactor(NIIf alpha)
    {
        mTL.setAlpha(mTL.getAlpha() * alpha);
        mTR.setAlpha(mTR.getAlpha() * alpha);
        mBL.setAlpha(mBL.getAlpha() * alpha);
        mBR.setAlpha(mBR.getAlpha() * alpha);
    }
    //------------------------------------------------------------------------
}