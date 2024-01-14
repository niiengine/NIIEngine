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
#ifndef _NII_UI_COLOUR_PICKER_TYPES_H_
#define _NII_UI_COLOUR_PICKER_TYPES_H_

#include "CEGUI/CommonDialogs/Module.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    enum ColourPickerSliderMode
    {
        ColourPickerSliderMode_L,
        ColourPickerSliderMode_A,
        ColourPickerSliderMode_B
    };

    class CEGUI_COMMONDIALOGS_API Lab_Colour;
    class CEGUI_COMMONDIALOGS_API RGB_Colour;
    class CEGUI_COMMONDIALOGS_API HSV_Colour;

    class CEGUI_COMMONDIALOGS_API RGB_Colour : public UIAlloc
    {
    public:
        RGB_Colour(unsigned char red, unsigned char green, unsigned char blue) :
            r(red), g(green), b(blue){}

        RGB_Colour() : r(0.0f), g(0.0f), b(0.0f){}

        RGB_Colour(const Lab_Colour & colour);
        RGB_Colour(const HSV_Colour& colour);
        RGB_Colour(const Colour & colour);

        unsigned char r;
        unsigned char g;
        unsigned char b;

        RGB_Colour operator *(const NIIf& number) const;
        RGB_Colour operator +(const RGB_Colour& colour) const;
    };

    class CEGUI_COMMONDIALOGS_API Lab_Colour : public UIAlloc
    {
    public:
        Lab_Colour(NIIf LValue, NIIf aValue, NIIf bValue) :
            L(LValue), a(aValue), b(bValue){}

        Lab_Colour() : L(0.0f), a(0.0f), b(0.0f){}

        Lab_Colour(const RGB_Colour & colour);
        Lab_Colour(const HSV_Colour & colour);
        Lab_Colour(const Colour & colour);

        NIIf L;
        NIIf a;
        NIIf b;
    };

    class CEGUI_COMMONDIALOGS_API HSV_Colour : public UIAlloc
    {
    public:
        HSV_Colour(NIIf HValue, NIIf SValue, NIIf VValue) :
            H(HValue), S(SValue), V(VValue){}

        HSV_Colour() : H(0.0f), S(0.0f), V(0.0f){}

        HSV_Colour(const RGB_Colour & colour);
        HSV_Colour(const Lab_Colour & colour);
        HSV_Colour(const Colour & colour);

        NIIf H;
        NIIf S;
        NIIf V;
    };
}
}
}
#endif