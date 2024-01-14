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
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"
#include <math.h>
#include <algorithm>

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    //------------------------------------------------------------------------
    const NIIf ColourPickerConversions::Xn(0.95047f);
    const NIIf ColourPickerConversions::Yn(1.00000f);
    const NIIf ColourPickerConversions::Zn(1.08883f);

    const NIIf ColourPickerConversions::LAB_COMPARE_VALUE_CONST(0.00885645167903563081717167575546f);
    //------------------------------------------------------------------------
    void ColourPickerConversions::clamp(NIIf & value, NIIf min_val, NIIf max_val)
    {
        value = value < min_val ? min_val : (value > max_val ? max_val : value);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toX(unsigned char R, unsigned char G, unsigned char B)
    {
        return toX(R / 255.0f, G / 255.0f, B / 255.0f);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toY(unsigned char R, unsigned char G, unsigned char B)
    {
        return toY(R / 255.0f, G / 255.0f, B / 255.0f);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toZ(unsigned char R, unsigned char G, unsigned char B)
    {
        return toZ(R / 255.0f, G / 255.0f, B / 255.0f);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toX(NIIf R, NIIf G, NIIf B)
    {
        return (0.4124564f * R + 0.3575761f * G + 0.1804375f * B);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toY(NIIf R, NIIf G, NIIf B)
    {
        return (0.2126729f * R + 0.7151522f * G + 0.0721750f * B);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toZ(NIIf R, NIIf G, NIIf B)
    {
        return (0.0193339f * R + 0.1191920f * G + 0.9503041f * B);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toL(NIIf Y)
    {
        return (116.0f * YNormCalc(Y) - 16.0f);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toA(NIIf X, NIIf Y)
    {
        return (500.0f * (XNormCalc(X) - YNormCalc(Y)));
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::toB(NIIf Y, NIIf Z)
    {
        return (200.0f * (YNormCalc(Y) - ZNormCalc(Z)));
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::XNormCalc(NIIf X)
    {
        NIIf div = (X / Xn);
        return normCalc(div);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::YNormCalc(NIIf Y)
    {
        NIIf div = (Y / Yn);
        return normCalc(div);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::ZNormCalc(NIIf Z)
    {
        NIIf div = (Z / Zn);
        return normCalc(div);
    }
    //------------------------------------------------------------------------
    NIIf ColourPickerConversions::normCalc(NIIf div)
    {
        if (div < LAB_COMPARE_VALUE_CONST)
            return ((24389.0f / 27.0f * div + 16.0f) / 116.0f);
        else
            return pow(div, 1.0f / 3.0f);
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerConversions::toRGB(const Lab_Colour & colour)
    {
        return toRGB(colour.L, colour.a, colour.b);
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerConversions::toRGB(NIIf L, NIIf a, NIIf b)
    {
        NIIf vy = (L + 16.0f) / 116.0f;
        NIIf vx = a / 500.0f + vy;
        NIIf vz = vy - b / 200.0f;

        {
            NIIf vx3 = pow(vx, 3);
            NIIf vy3 = pow(vy, 3);
            NIIf vz3 = pow(vz, 3);

            if (vy3 > LAB_COMPARE_VALUE_CONST)
                vy = vy3;
            else
                vy = (vy - 16.0f / 116.0f) / 7.787f;

            if (vx3 > LAB_COMPARE_VALUE_CONST)
                vx = vx3;
            else
                vx = (vx - 16.0f / 116.0f) / 7.787f;

            if (vz3 > LAB_COMPARE_VALUE_CONST)
                vz = vz3;
            else
                vz = (vz - 16.0f / 116.0f) / 7.787f;
        }

        vx *= Xn;
        vz *= Zn;

        NIIf vr = vx *  3.2406f + vy * -1.5372f + vz * -0.4986f;
        NIIf vg = vx * -0.9689f + vy *  1.8758f + vz *  0.0415f;
        NIIf vb = vx *  0.0557f + vy * -0.2040f + vz *  1.0570f;

        clamp(vr, 0.0f, 1.0f);
        clamp(vg, 0.0f, 1.0f);
        clamp(vb, 0.0f, 1.0f);

        return RGB_Colour(static_cast<unsigned char>(255.0f * vr),
            static_cast<unsigned char>(255.0f * vg),
                static_cast<unsigned char>(255.0f * vb));
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerConversions::toRGB(const Colour & colour)
    {
        return RGB_Colour(static_cast<unsigned char>(colour.getRed() * 255.0f),
            static_cast<unsigned char>(colour.getGreen() * 255.0f),
                static_cast<unsigned char>(colour.getBlue() * 255.0f));
    }
    //------------------------------------------------------------------------
    Colour ColourPickerConversions::toCeguiColour(const RGB_Colour & colourRGB)
    {
        return Colour(colourRGB.r / 255.0f, colourRGB.g / 255.0f,
            colourRGB.b / 255.0f);
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerConversions::linearInterpolationRGB(NIIf interPolBalance,
        const RGB_Colour & start, const RGB_Colour & end)
    {
        RGB_Colour colour;

        clampInterpolationValue(interPolBalance);

        colour.r = static_cast<unsigned char>(
            (1 - interPolBalance) * start.r + (interPolBalance * end.r));
        colour.g = static_cast<unsigned char>(
            (1 - interPolBalance) * start.g + (interPolBalance * end.g));
        colour.b = static_cast<unsigned char>(
            (1 - interPolBalance) * start.b + (interPolBalance * end.b));

        return colour;
    }
    //------------------------------------------------------------------------
    Lab_Colour ColourPickerConversions::linearInterpolationLab(NIIf interPolBalance,
        const Lab_Colour & start, const Lab_Colour & end)
    {
        Lab_Colour colour;

        clampInterpolationValue(interPolBalance);

        colour.L = (1 - interPolBalance) * start.L + (interPolBalance * end.L);
        colour.a = (1 - interPolBalance) * start.a + (interPolBalance * end.a);
        colour.b = (1 - interPolBalance) * start.b + (interPolBalance * end.b);

        return colour;
    }
    //------------------------------------------------------------------------
    HSV_Colour ColourPickerConversions::linearInterpolationHSV(NIIf interPolBalance,
        const HSV_Colour & start, const HSV_Colour & end)
    {
        HSV_Colour colour;

        clampInterpolationValue(interPolBalance);

        colour.H = (1 - interPolBalance) * start.H + (interPolBalance * end.H);
        colour.S = (1 - interPolBalance) * start.S + (interPolBalance * end.S);
        colour.V = (1 - interPolBalance) * start.V + (interPolBalance * end.V);

        return colour;
    }
    //------------------------------------------------------------------------
    unsigned char ColourPickerConversions::linearInterpolationAlpha(NIIf interPolBalance,
        unsigned char startAlpha, unsigned char endAlpha)
    {
        clampInterpolationValue(interPolBalance);

        return static_cast<unsigned char>(
            (1 - interPolBalance) * startAlpha + (interPolBalance * endAlpha));
    }
    //------------------------------------------------------------------------
    void ColourPickerConversions::clampInterpolationValue(NIIf & interPolBalance)
    {
        interPolBalance = std::max(0.0f, interPolBalance);
        interPolBalance = std::min(1.0f, interPolBalance);
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerConversions::toRGB(const HSV_Colour & colour)
    {
        NIIf r, g, b;

        int i = (int)(colour.H * 6.0f);
        NIIf f = colour.H * 6 - i;
        NIIf p = colour.V * (1 - colour.S);
        NIIf q = colour.V * (1 - f * colour.S);
        NIIf t = colour.V * (1 - (1 - f) * colour.S);

        switch (i % 6)
        {
        case 0:
            r = colour.V;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = colour.V;
            b = p;
            break;
        case 2:
            r = p;
            g = colour.V;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = colour.V;
            break;
        case 4:
            r = t;
            g = p;
            b = colour.V;
            break;
        case 5:
            r = colour.V;
            g = p;
            b = q;
            break;
        }

        return RGB_Colour(static_cast<unsigned char>(r * 255),
            static_cast<unsigned char>(g * 255),
                static_cast<unsigned char>(b * 255));
    }
    //------------------------------------------------------------------------
    Lab_Colour ColourPickerConversions::toLab(RGB_Colour colour)
    {
        NIIf X = toX(colour.r, colour.g, colour.b);
        NIIf Y = toY(colour.r, colour.g, colour.b);
        NIIf Z = toZ(colour.r, colour.g, colour.b);

        NIIf L = toL(Y);
        NIIf a = toA(X, Y);
        NIIf b = toB(Y, Z);

        return Lab_Colour(L, a, b);
    }
    //------------------------------------------------------------------------
    HSV_Colour ColourPickerConversions::toHSV(RGB_Colour colour)
    {
        NIIf r = colour.r / 255.0f;
        NIIf g = colour.g / 255.0f;
        NIIf b = colour.b / 255.0f;

        NIIf max_comp = N_MAX(NIIf, N_MAX(NIIf, r, g), b);
        NIIf min_comp = N_MIN(NIIf, N_MIN(NIIf, r, g), b);
        NIIf h;
        NIIf s;
        NIIf v = max_comp;

        NIIf diff = max_comp - min_comp;
        s = (max_comp == 0.0f ? 0.0f : diff / max_comp);

        if (max_comp == min_comp)
        {
            h = 0; // achromatic
        }
        else
        {
            if (max_comp == r)
                h = (g - b) / diff + (g < b ? 6.0f : 0.0f);

            if (max_comp == g)
                h = (b - r) / diff + 2.0f;

            if (max_comp == b)
                h = (r - g) / diff + 4.0f;

            h /= 6.0f;
        }

        return HSV_Colour(h, s, v);
    }
    //------------------------------------------------------------------------
}
}
}