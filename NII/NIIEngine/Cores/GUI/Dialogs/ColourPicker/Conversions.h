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
#ifndef _NII_ColourPickerConversions_H_
#define _NII_ColourPickerConversions_H_

#include "NiiUIPreInclude.h"
#include "CEGUI/CommonDialogs/Module.h"
#include "CEGUI/CommonDialogs/ColourPicker/Types.h"
#include "CEGUI/Colour.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    class CEGUI_COMMONDIALOGS_API ColourPickerConversions
    {
    public:
        /// Function for converting an RGB_Colour to a Lab_Colour
        static Lab_Colour toLab(RGB_Colour colour);

        /// Function for converting an RGB_Colour to an HSV_Colour
        static HSV_Colour toHSV(RGB_Colour colour);

        /// Function for converting a Lab_Colour to an RGB_Colour
        static RGB_Colour toRGB(const Lab_Colour & colour);

        /// Function for converting a Lab_Colour to an RGB_Colour
        static RGB_Colour toRGB(NIIf L, NIIf a, NIIf b);

        /// Function for converting a CEGUI Colour to an RGB_Colour
        static RGB_Colour toRGB(const Colour & colour);

        /// Function for converting a HSV to an RGB_Colour
        static RGB_Colour toRGB(const HSV_Colour & colour);

        /// Conversion from RGB_Colour to Colour
        static Colour toCeguiColour(const RGB_Colour & colourRGB);

        /// Function for calculating X based on the RGB components
        static NIIf toX(unsigned char R, unsigned char G, unsigned char B);

        /// Function for calculating Y based on the RGB components
        static NIIf toY(unsigned char R, unsigned char G, unsigned char B);

        /// Function for calculating Z based on the RGB components
        static NIIf toZ(unsigned char R, unsigned char G, unsigned char B);

        /// Function for calculating X based on the RGB components
        static NIIf toX(NIIf R, NIIf G, NIIf B);

        /// Function for calculating Y based on the RGB components
        static NIIf toY(NIIf R, NIIf G, NIIf B);

        /// Function for calculating Z based on the RGB components
        static NIIf toZ(NIIf R, NIIf G, NIIf B);

        /// Function for calculating L (of CIELab) based on the Y components
        static NIIf toL(NIIf Y);

        /// Function for calculating a (of CIELab) based on the X and Y components
        static NIIf toA(NIIf X, NIIf Y);

        /// Function for calculating b (of CIELab) based on the Y and Z components
        static NIIf toB(NIIf Y, NIIf Z);

        /// Linear interpolation helper function for RGB_Colour
        static RGB_Colour linearInterpolationRGB(NIIf interPolBalance,
            const RGB_Colour& start,
            const RGB_Colour& end);

        /// Linear interpolation helper function for Alpha
        static unsigned char linearInterpolationAlpha(NIIf interPolBalance,
            unsigned char startAlpha,
            unsigned char endAlpha);

        /// Linear interpolation helper function for Lab_Colour
        static Lab_Colour linearInterpolationLab(NIIf interPolBalance,
            const Lab_Colour & start, const Lab_Colour & end);

        /// Linear interpolation helper function for HSV_Colour
        static HSV_Colour linearInterpolationHSV(NIIf interPolBalance,
            const HSV_Colour & start, const HSV_Colour & end);
    private:
        /// Clamp function for clamping interpolation values between 0.0 and 1.0
        static void clampInterpolationValue(NIIf & interPolBalance);

        /// Helper function for the conversion from XYZ to CIELab
        static NIIf normCalc(NIIf div);

        /// Helper function for the conversion from XYZ to CIELab
        static NIIf XNormCalc(NIIf X);

        /// Helper function for the conversion from XYZ to CIELab
        static NIIf YNormCalc(NIIf Y);

        /// Helper function for the conversion from XYZ to CIELab
        static NIIf ZNormCalc(NIIf Z);

        /// Helper function for clamping values
        static void clamp(NIIf & value, NIIf min_val, NIIf max_val);

        /// Normed reference values for Lab calculations in accordance with CIELab (D65)
        static const NIIf Xn;
        static const NIIf Yn;
        static const NIIf Zn;

        /// Reference value needed in the Lab calculations to check for low
        // values of P/Pn (P/Pn < LAB_COMPARE_VALUE_CONST)
        static const NIIf LAB_COMPARE_VALUE_CONST;
    };
}
}
}
#endif