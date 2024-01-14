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

#include "CEGUI/CommonDialogs/ColourPicker/Types.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    //------------------------------------------------------------------------
    RGB_Colour::RGB_Colour(const Lab_Colour & colour)
    {
        *this = ColourPickerConversions::toRGB(colour);
    }
    //------------------------------------------------------------------------
    RGB_Colour::RGB_Colour(const HSV_Colour & colour)
    {
        *this = ColourPickerConversions::toRGB(colour);
    }
    //------------------------------------------------------------------------
    RGB_Colour::RGB_Colour(const Colour & colour)
    {
        *this = ColourPickerConversions::toRGB(colour);
    }
    //------------------------------------------------------------------------
    Lab_Colour::Lab_Colour(const RGB_Colour & colour)
    {
        *this = ColourPickerConversions::toLab(colour);
    }
    //------------------------------------------------------------------------
    Lab_Colour::Lab_Colour(const HSV_Colour & colour)
    {
        *this = ColourPickerConversions::toLab(ColourPickerConversions::toRGB(colour));
    }
    //------------------------------------------------------------------------
    Lab_Colour::Lab_Colour(const Colour & colour)
    {
        *this = ColourPickerConversions::toLab(ColourPickerConversions::toRGB(colour));
    }
    //------------------------------------------------------------------------
    HSV_Colour::HSV_Colour(const RGB_Colour & colour)
    {
        *this = ColourPickerConversions::toHSV(colour);
    }
    //------------------------------------------------------------------------
    HSV_Colour::HSV_Colour(const Lab_Colour & colour)
    {
        *this = ColourPickerConversions::toHSV(ColourPickerConversions::toRGB(colour));
    }
    //------------------------------------------------------------------------
    HSV_Colour::HSV_Colour(const Colour & colour)
    {
        *this = ColourPickerConversions::toHSV(ColourPickerConversions::toRGB(colour));
    }
    //------------------------------------------------------------------------
    RGB_Colour RGB_Colour::operator*(const NIIf & number) const
    {
        RGB_Colour col;

        col.r = static_cast<unsigned char>(r * number);
        col.g = static_cast<unsigned char>(g * number);
        col.b = static_cast<unsigned char>(b * number);

        return col;
    }
    //------------------------------------------------------------------------
    RGB_Colour RGB_Colour::operator+(const RGB_Colour & colour) const
    {
        RGB_Colour col;

        col.r = static_cast<int>(r) + static_cast<int>(colour.r);
        col.g = static_cast<int>(g) + static_cast<int>(colour.g);
        col.b = static_cast<int>(b) + static_cast<int>(colour.b);

        return col;
    }
    //------------------------------------------------------------------------
}
}
}