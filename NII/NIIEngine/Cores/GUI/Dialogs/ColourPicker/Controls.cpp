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
#include "NiiUIPushButton.h"
#include "NiiUIEditbox.h"
#include "NiiUIRadioButton.h"
#include "NiiUITitlebar.h"
#include "NiiUISlider.h"
#include "NiiUIPosButton.h"

#include "NiiUICursor.h"
#include "NiiWidgetManager.h"
#include "CEGUI/Exceptions.h"
#include "NiiPixelManager.h"

#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

#include "CEGUI/RegexMatcher.h"

#include <sstream>
#include <algorithm>
#include <math.h>

namespace CEGUI
{
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, GameObj, EventColourAccepted,       0);
    N_EVENT(CommandObj, GameObj, EventClosed,               1);
    N_EVENT(CommandObj, GameObj, EventCount,                2);
    const String ColourPickerControls::ColourRectPropertyName("Colour");
    //------------------------------------------------------------------------
    const String ColourPickerControls::ColourPickerControlsPickingTextureImageName("ColourPickingTexture");
    const String ColourPickerControls::ColourPickerControlsColourSliderTextureImageName("ColourSliderTexture");
    const String ColourPickerControls::ColourPickerControlsAlphaSliderTextureImageName("AlphaSliderTexture");
    //------------------------------------------------------------------------
    const NIIf ColourPickerControls::LAB_L_MIN(0.0f);
    const NIIf ColourPickerControls::LAB_L_MAX(100.0f);
    const NIIf ColourPickerControls::LAB_L_DIFF(LAB_L_MAX - LAB_L_MIN);
    const NIIf ColourPickerControls::LAB_A_MIN(-130.0f);
    const NIIf ColourPickerControls::LAB_A_MAX(130.0f);
    const NIIf ColourPickerControls::LAB_A_DIFF(LAB_A_MAX - LAB_A_MIN);
    const NIIf ColourPickerControls::LAB_B_MIN(-130.0f);
    const NIIf ColourPickerControls::LAB_B_MAX(130.0f);
    const NIIf ColourPickerControls::LAB_B_DIFF(LAB_B_MAX - LAB_B_MIN);
    //------------------------------------------------------------------------
    // Child Widget name constants
    const String ColourPickerControls::CancelButtonName("__auto_cancelbutton__");
    const String ColourPickerControls::AcceptButtonName("__auto_acceptbutton__");
    const String ColourPickerControls::HexaDecimalDescriptionName("__auto_hexadecimalstatictext__");
    const String ColourPickerControls::HexaDecimalEditBoxName("__auto_hexadecimaleditbox__");
    const String ColourPickerControls::TitleBarName("__auto_titlebar__");

    const String ColourPickerControls::ColourPickerStaticImageName("__auto_colourpickerstaticimage__");
    const String ColourPickerControls::ColourPickerImageSliderName("__auto_colourpickerimageslider__");
    const String ColourPickerControls::ColourPickerAlphaSliderName("__auto_colourpickeralphaslider__");

    const String ColourPickerControls::NewColourDescriptionName("__auto_newcolourdescription__");
    const String ColourPickerControls::OldColourDescriptionName("__auto_oldcolourdescription__");

    const String ColourPickerControls::NewColourRectName("__auto_newcolourrect__");
    const String ColourPickerControls::OldColourRectName("__auto_oldcolourrect__");

    const String ColourPickerControls::ColourEditBoxRDescriptionName("__auto_coloureditboxRdescription__");
    const String ColourPickerControls::ColourEditBoxRName("__auto_coloureditboxR__");
    const String ColourPickerControls::ColourEditBoxGDescriptionName("__auto_coloureditboxGdescription__");
    const String ColourPickerControls::ColourEditBoxGName("__auto_coloureditboxG__");
    const String ColourPickerControls::ColourEditBoxBDescriptionName("__auto_coloureditboxBdescription__");
    const String ColourPickerControls::ColourEditBoxBName("__auto_coloureditboxB__");

    const String ColourPickerControls::HSVRadioButtonHName("__auto_HSVradiobuttonH__");
    const String ColourPickerControls::HSVEditBoxHName("__auto_HSVeditboxH__");
    const String ColourPickerControls::HSVRadioButtonSName("__auto_HSVradiobuttonS__");
    const String ColourPickerControls::HSVEditBoxSName("__auto_HSVeditboxS__");
    const String ColourPickerControls::HSVRadioButtonVName("__auto_HSVradiobuttonV__");
    const String ColourPickerControls::HSVEditBoxVName("__auto_HSVeditboxV__");

    const String ColourPickerControls::LabRadioButtonLName("__auto_LabradiobuttonL__");
    const String ColourPickerControls::LabEditBoxLName("__auto_LabeditboxL__");
    const String ColourPickerControls::LabRadioButtonAName("__auto_Labradiobuttona__");
    const String ColourPickerControls::LabEditBoxAName("__auto_Labeditboxa__");
    const String ColourPickerControls::LabRadioButtonBName("__auto_Labradiobuttonb__");
    const String ColourPickerControls::LabEditBoxBName("__auto_Labeditboxb__");

    const String ColourPickerControls::AlphaEditBoxName("__auto_alphaeditbox__");

    const String ColourPickerControls::ColourPickerCursorName("__colourpickercursor__");
    //------------------------------------------------------------------------
    ColourPickerControls::ColourPickerControls(const String & type, const String & name) :
        Widget(type, name),
        d_callingColourPicker(0),
        d_colourPickerCursor(0),
        d_sliderMode(SliderMode_Lab_L),
        d_selectedColour(0.75f, 0.75f, 0.75f),
        d_colourPickerControlsTextureTarget(0),
        d_colourPickerImageOffset(2),
        d_colourPickerPickingImageHeight(260),
        d_colourPickerPickingImageWidth(260),
        d_colourPickerColourSliderImageWidth(1),
        d_colourPickerColourSliderImageHeight(260),
        d_colourPickerAlphaSliderImageWidth(260),
        d_colourPickerAlphaSliderImageHeight(60),
        d_colourPickerControlsTextureSize(512),
        d_draggingColourPickerCursor(false),
        d_colourPickingTexture(new RGB_Colour[d_colourPickerControlsTextureSize *
            d_colourPickerControlsTextureSize]),
        d_ignoreEvents(false)
    {
        N_Only(Regex).create(*d_regexMatcher);
    }
    //------------------------------------------------------------------------
    ColourPickerControls::~ColourPickerControls()
    {
        deinitColourPickerControlsTexture();

        if (d_colourPickerCursor)
            N_Only(Widget).destroy(d_colourPickerCursor);

        delete[] d_colourPickingTexture;

        N_Only(Regex).destroy(&d_regexMatcher);
    }
    //------------------------------------------------------------------------
    Lab_Colour ColourPickerControls::getColourPickingPositionColourLAB(NIIf xAbs,
        NIIf yAbs)
    {
        NIIf L = 0.0f;
        NIIf a = 0.0f;
        NIIf b = 0.0f;

        NIIf xRel = xAbs / (NIIf)(d_colourPickerPickingImageWidth - 1);
        NIIf yRel = yAbs / (NIIf)(d_colourPickerPickingImageHeight - 1);

        if (d_sliderMode != SliderMode_Lab_L)
        {
            L = LAB_L_MAX - LAB_L_DIFF * yRel ;

            if (d_sliderMode != SliderMode_Lab_A)
            {
                b = d_selectedColourLAB.b;
                a = LAB_A_MAX - LAB_A_DIFF * xRel;
            }
            else
            {
                a = d_selectedColourLAB.a;
                b = LAB_B_MAX - LAB_B_DIFF * xRel;
            }
        }
        else
        {
            L = d_selectedColourLAB.L;
            a = LAB_A_MAX - LAB_A_DIFF * xRel;
            b = LAB_B_MAX - LAB_B_DIFF * yRel;
        }

        return Lab_Colour(L, a, b);
    }
    //------------------------------------------------------------------------
    HSV_Colour ColourPickerControls::getColourPickingPositionColourHSV(NIIf xAbs,
        NIIf yAbs)
    {
        HSV_Colour colour;

        NIIf xRel = xAbs / (NIIf)(d_colourPickerPickingImageWidth - 1);
        NIIf yRel = yAbs / (NIIf)(d_colourPickerPickingImageHeight - 1);

        if(d_sliderMode != SliderMode_HSV_H)
        {
            NIIf xCoord = (xRel - 0.5f) * 2.0f;
            NIIf yCoord = (yRel - 0.5f) * 2.0f;

            NIIf angle = std::atan2(yCoord, xCoord);

            if (angle < 0.0f)
                angle += 2.0f * 3.1415926535897932384626433832795f;

            angle /= 2.0f * 3.1415926535897932384626433832795f;
            colour.H = angle;

            NIIf length = std::sqrt(xCoord * xCoord + yCoord * yCoord);
            NIIf value = N_MIN(NIIf, length, 1.0f);

            if (d_sliderMode != SliderMode_HSV_S)
            {
                colour.V = d_selectedColourHSV.V;
                colour.S = value;
            }
            else
            {
                colour.S = d_selectedColourHSV.S;
                colour.V = value;
            }
        }
        else
        {
            colour.H = d_selectedColourHSV.H;
            colour.S = xRel;
            colour.V = 1.0f - yRel;
        }

        return colour;
    }
    //------------------------------------------------------------------------
    Vector2f ColourPickerControls::getColourPickingColourPosition()
    {
        NIIf x;
        NIIf y;

        switch (d_sliderMode)
        {
        case SliderMode_Lab_L:
            x = 1.0f - (d_selectedColourLAB.a - LAB_A_MIN) / LAB_A_DIFF;
            y = 1.0f - (d_selectedColourLAB.b - LAB_B_MIN) / LAB_B_DIFF;
            break;

        case SliderMode_Lab_A:
            x = 1.0f - (d_selectedColourLAB.b - LAB_B_MIN) / LAB_B_DIFF;
            y = 1.0f - (d_selectedColourLAB.L - LAB_L_MIN) / LAB_L_DIFF;
            break;

        case SliderMode_Lab_B:
            x = 1.0f - (d_selectedColourLAB.a - LAB_A_MIN) / LAB_A_DIFF;
            y = 1.0f - (d_selectedColourLAB.L - LAB_L_MIN) / LAB_L_DIFF;
            break;

        case SliderMode_HSV_H:
            x = d_selectedColourHSV.S;
            y = 1.0f - d_selectedColourHSV.V;
            break;

        case SliderMode_HSV_S:
            getColourPickingColourPositionHSV(x, y);
            break;

        case SliderMode_HSV_V:
            getColourPickingColourPositionHSV(x, y);
            break;

        default:
            break;
        }

        return Vector2f(x, y);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::getColourPickingColourPositionHSV(NIIf & x, NIIf & y)
    {
        NIIf radius;

        switch (d_sliderMode)
        {
        case SliderMode_HSV_S:
            radius = d_selectedColourHSV.V;
            break;
        case SliderMode_HSV_V:
            radius = d_selectedColourHSV.S;
            break;
        default:
            radius = 0.0f; //This shouldn't happen.  (PDT: Why is it here, then?)
            break;
        }

        NIIf angle = d_selectedColourHSV.H;
        angle *= 2.0f * 3.1415926535897932384626433832795f;

        x = radius * cos(angle) * 0.5f + 0.5f;
        y = radius * sin(angle) * 0.5f + 0.5f;
    }
    //------------------------------------------------------------------------
    Lab_Colour ColourPickerControls::getColourSliderPositionColourLAB(NIIf value)
    {
        Lab_Colour colour = d_selectedColourLAB;

        switch (d_sliderMode)
        {
        case SliderMode_Lab_L:
            colour.L = LAB_L_MAX - LAB_L_DIFF * value;
            break;
        case SliderMode_Lab_A:
            colour.a = LAB_A_MAX - LAB_A_DIFF * value;
            break;
        case SliderMode_Lab_B:
            colour.b = LAB_B_MAX - LAB_B_DIFF * value;
            break;
        default:
            break;
        }

        return colour;
    }
    //------------------------------------------------------------------------
    HSV_Colour ColourPickerControls::getColourSliderPositionColourHSV(NIIf value)
    {
        HSV_Colour colour = d_selectedColourHSV;

        switch (d_sliderMode)
        {
        case SliderMode_HSV_H:
            colour.H = 1.0f - value;
            break;
        case SliderMode_HSV_S:
            colour.S = 1.0f - value;
            break;
        case SliderMode_HSV_V:
            colour.V = 1.0f - value;
            break;
        default:
            break;
        }
        return colour;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::initColourPickerControlsImageSet()
    {
        d_colourPickerControlsTextureTarget = N_Only(TextureManager).createTexture(
            N_Only(Texture).genValidID(), GroupDefault, Texture::T_2D,
                128, 128, 1, 0, PF_A8R8G8B8, Texture::MM_FRAME_RGB, 0, 0,
                    false, 0, StrUtil::BLANK);

        const String baseName(d_colourPickerControlsTextureTarget->getSrc());

        PixelBuffer * pb = static_cast<PixelBuffer *>(N_Only(Pixel).
            create(baseName, ColourPickerControlsPickingTextureImageName));

        pb->setTexture(d_colourPickerControlsTextureTarget);
        pb->setArea(Rectf(Vector2f(0.0f, 0.0f),
            PlaneSizef(static_cast<NIIf>(d_colourPickerPickingImageWidth),
                static_cast<NIIf>(d_colourPickerPickingImageHeight))));

        pb = static_cast<PixelBuffer *>(N_Only(Pixel).
            create(baseName, ColourPickerControlsColourSliderTextureImageName));

        pb->setTexture(d_colourPickerControlsTextureTarget);
        pb->setArea(
            Rectf(Vector2f(static_cast<NIIf>(d_colourPickerPickingImageWidth + d_colourPickerImageOffset), 0.0f),
                PlaneSizef(static_cast<NIIf>(d_colourPickerColourSliderImageWidth),
                    static_cast<NIIf>(d_colourPickerColourSliderImageHeight))));

        pb = static_cast<PixelBuffer *>(N_Only(Pixel).
            create(baseName, ColourPickerControlsAlphaSliderTextureImageName));

        pb->setTexture(d_colourPickerControlsTextureTarget);
        pb->setArea(
            Rectf(Vector2f(0.0f, static_cast<NIIf>(d_colourPickerPickingImageHeight + d_colourPickerImageOffset)),
                  PlaneSizef(static_cast<NIIf>(d_colourPickerAlphaSliderImageWidth),
                        static_cast<NIIf>(d_colourPickerAlphaSliderImageHeight))));

        getColourPickerStaticImage()->set(
            "Image", baseName + '/' + ColourPickerControlsPickingTextureImageName);
        getColourPickerImageSlider()->set(
            "ScrollImage", baseName + '/' + ColourPickerControlsColourSliderTextureImageName);
        getColourPickerAlphaSlider()->set(
            "ScrollImage", baseName + '/' + ColourPickerControlsAlphaSliderTextureImageName);

        refreshColourPickerControlsTextures();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::deinitColourPickerControlsTexture()
    {
        if(d_colourPickerControlsTextureTarget)
        {
            N_Only(Pixel).destroy(d_colourPickerControlsTextureTarget->getSrc(), false);

            N_Only(Texture).destroy(d_colourPickerControlsTextureTarget);
        }
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourPickerControlsTextures()
    {
        N_Only(Log).log(_I18n("[ColourPicker] Refreshing ColourPickerTexture");

        refreshColourPickingImage();
        refreshColourSliderImage();
        refreshAlphaSliderImage();

        reloadColourPickerControlsTexture();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::reloadColourPickerControlsTexture()
    {
        d_colourPickerControlsTextureTarget->load(
            d_colourPickingTexture,
            d_colourPickerControlsTextureSize,
            d_colourPickerControlsTextureSize,
            PF_B8G8R8UN);

        getColourPickerImageSlider()->refresh();
        getColourPickerAlphaSlider()->refresh();
        getColourPickerStaticImage()->refresh();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::initChild()
    {
        getCancelButton()->add(PushButton::PushEvent,
            EventFunctor(this, &ColourPickerControls::handleCancelButtonClicked));

        getAcceptButton()->add(PushButton::PushEvent,
            EventFunctor(this, &ColourPickerControls::handleAcceptButtonClicked));

        getHexadecimalEditbox()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getColourEditBoxR()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getColourEditBoxG()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getColourEditBoxB()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getHSVEditBoxH()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getHSVEditBoxS()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getHSVEditBoxV()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getLabEditBoxL()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getLabEditBoxA()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getLabEditBoxB()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getAlphaEditBox()->add(Editbox::DeactivateEvent,
            EventFunctor(this, &ColourPickerControls::handleEditboxDeactivated));

        getHSVRadioButtonH()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getHSVRadioButtonS()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getHSVRadioButtonV()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getLabRadioButtonL()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getLabRadioButtonA()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getLabRadioButtonB()->add(RadioButton::SelectChangeEvent,
            EventFunctor(this, &ColourPickerControls::handleRadioButtonModeSelection));

        getHexadecimalEditbox()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleHexadecimalEditboxTextChanged));

        getColourEditBoxR()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleRGBEditboxTextChanged));

        getColourEditBoxG()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleRGBEditboxTextChanged));

        getColourEditBoxB()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleRGBEditboxTextChanged));

        getLabEditBoxL()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleLABEditboxTextChanged));

        getLabEditBoxA()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleLABEditboxTextChanged));

        getLabEditBoxB()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleLABEditboxTextChanged));

        getHSVEditBoxH()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleHSVEditboxTextChanged));

        getHSVEditBoxS()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleHSVEditboxTextChanged));

        getHSVEditBoxV()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleHSVEditboxTextChanged));

        getAlphaEditBox()->add(Editbox::TextEvent,
            EventFunctor(this, &ColourPickerControls::handleAlphaEditboxTextChanged));

        getColourPickerImageSlider()->add(Slider::ValueEvent,
            EventFunctor(this, &ColourPickerControls::handleColourPickerSliderValueChanged));

        getColourPickerAlphaSlider()->add(Slider::ValueEvent,
            EventFunctor(this, &ColourPickerControls::handleAlphaSliderValueChanged));

        getColourPickerStaticImage()->add(Widget::CursorLeaveEvent,
            EventFunctor(this, &ColourPickerControls::handleColourPickerStaticImageMouseLeaves));

        getColourPickerStaticImage()->add(Widget::ButtonUpEvent,
            EventFunctor(this, &ColourPickerControls::handleColourPickerStaticImageMouseButtonUp));

        getColourPickerStaticImage()->add(Widget::ButtonDownEvent,
            EventFunctor(this, &ColourPickerControls::handleColourPickerStaticImageMouseButtonDown));

        getColourPickerStaticImage()->add(Widget::CursorMoveEvent,
            EventFunctor(this, &ColourPickerControls::handleColourPickerStaticImageMouseMove));

        initColourPicker();

        layout();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setPreviousColour(const Colour & previousColour)
    {
        d_previouslySelectedColour = previousColour;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setColour(const Colour & newColour)
    {
        d_selectedColourRGB = RGB_Colour(newColour);
        d_selectedColourLAB = Lab_Colour(d_selectedColourRGB);
        d_selectedColourHSV = HSV_Colour(d_selectedColourRGB);
        d_selectedColour = newColour;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setColour(const Lab_Colour & newColourLAB)
    {
        d_selectedColourLAB = newColourLAB;
        d_selectedColourRGB = RGB_Colour(newColourLAB);
        d_selectedColourHSV = HSV_Colour(d_selectedColourRGB);

        NIIf alpha = d_selectedColour.getAlpha();
        d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
        d_selectedColour.setAlpha(alpha);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setColour(const RGB_Colour & newColourRGB)
    {
        d_selectedColourLAB = Lab_Colour(newColourRGB);
        d_selectedColourRGB = newColourRGB;
        d_selectedColourHSV = HSV_Colour(newColourRGB);

        NIIf alpha = d_selectedColour.getAlpha();
        d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
        d_selectedColour.setAlpha(alpha);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setColour(const HSV_Colour & newColourHSV)
    {
        d_selectedColourRGB = RGB_Colour(newColourHSV);
        d_selectedColourLAB = Lab_Colour(d_selectedColourRGB);
        d_selectedColourHSV = newColourHSV;

        NIIf alpha = d_selectedColour.getAlpha();
        d_selectedColour = ColourPickerConversions::toCeguiColour(d_selectedColourRGB);
        d_selectedColour.setAlpha(alpha);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setCallingColourPicker(ColourPicker * colourPicker)
    {
        d_callingColourPicker = colourPicker;
    }
    //------------------------------------------------------------------------
    PushButton * ColourPickerControls::getCancelButton()
    {
        return static_cast<PushButton *>(get(CancelButtonName));
    }
    //------------------------------------------------------------------------
    PushButton * ColourPickerControls::getAcceptButton()
    {
        return static_cast<PushButton *>(get(AcceptButtonName));
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getHexadecimalDescription()
    {
        return get(HexaDecimalDescriptionName);
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getHexadecimalEditbox()
    {
        return static_cast<Editbox *>(get(HexaDecimalEditBoxName));
    }
    //------------------------------------------------------------------------
    Titlebar * ColourPickerControls::getTitleBar()
    {
        return static_cast<Titlebar *>(get(TitleBarName));
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getColourPickerStaticImage()
    {
        return get(ColourPickerStaticImageName);
    }
    //------------------------------------------------------------------------
    Slider * ColourPickerControls::getColourPickerImageSlider()
    {
        return static_cast<Slider *>(get(ColourPickerImageSliderName));
    }
    //------------------------------------------------------------------------
    Slider * ColourPickerControls::getColourPickerAlphaSlider()
    {
        return static_cast<Slider *>(get(ColourPickerAlphaSliderName));
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getNewColourDescription()
    {
        return get(NewColourDescriptionName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getOldColourDescription()
    {
        return get(OldColourDescriptionName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getNewColourRect()
    {
        return get(NewColourRectName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getOldColourRect()
    {
        return get(OldColourRectName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getColourEditBoxRDescription()
    {
        return get(ColourEditBoxRDescriptionName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getColourEditBoxGDescription()
    {
        return get(ColourEditBoxGDescriptionName);
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getColourEditBoxBDescription()
    {
        return get(ColourEditBoxBDescriptionName);
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getColourEditBoxR()
    {
        return static_cast<Editbox *>(get(ColourEditBoxRName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getColourEditBoxG()
    {
        return static_cast<Editbox *>(get(ColourEditBoxGName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getColourEditBoxB()
    {
        return static_cast<Editbox *>(get(ColourEditBoxBName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getHSVRadioButtonH()
    {
        return static_cast<RadioButton *>(get(HSVRadioButtonHName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getHSVRadioButtonS()
    {
        return static_cast<RadioButton *>(get(HSVRadioButtonSName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getHSVRadioButtonV()
    {
        return static_cast<RadioButton *>(get(HSVRadioButtonVName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getHSVEditBoxH()
    {
        return static_cast<Editbox *>(get(HSVEditBoxHName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getHSVEditBoxS()
    {
        return static_cast<Editbox *>(get(HSVEditBoxSName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getHSVEditBoxV()
    {
        return static_cast<Editbox *>(get(HSVEditBoxVName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getLabRadioButtonL()
    {
        return static_cast<RadioButton *>(get(LabRadioButtonLName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getLabRadioButtonA()
    {
        return static_cast<RadioButton *>(get(LabRadioButtonAName));
    }
    //------------------------------------------------------------------------
    RadioButton * ColourPickerControls::getLabRadioButtonB()
    {
        return static_cast<RadioButton *>(get(LabRadioButtonBName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getLabEditBoxL()
    {
        return static_cast<Editbox *>(get(LabEditBoxLName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getLabEditBoxA()
    {
        return static_cast<Editbox *>(get(LabEditBoxAName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getLabEditBoxB()
    {
        return static_cast<Editbox *>(get(LabEditBoxBName));
    }
    //------------------------------------------------------------------------
    Editbox * ColourPickerControls::getAlphaEditBox()
    {
        return static_cast<Editbox *>(get(AlphaEditBoxName));
    }
    //------------------------------------------------------------------------
    Widget * ColourPickerControls::getColourPickerCursorStaticImage()
    {
        return d_colourPickerCursor;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleCancelButtonClicked(const EventArgs &)
    {
        WidgetEventArgs args(this);
        onCancelButtonClicked(args);

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleEditboxDeactivated(const EventArgs & args)
    {
        Editbox * editbox = static_cast<Editbox *>(
            static_cast<const WidgetEventArgs &>(args).mWidget);
        editbox->setSelect(0, 0);

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleHexadecimalEditboxTextChanged(const EventArgs &)
    {
        if (d_ignoreEvents == true)
            return true;

        String hexaText = getHexadecimalEditbox()->getText();

        if (hexaText.length() == 8)
        {
            Colour hexaColour;
            StrConv::conv(hexaText, hexaColour);
            setColour(hexaColour);
            refreshAllElements();
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleRGBEditboxTextChanged(const EventArgs &)
    {
        if (d_ignoreEvents == true)
            return true;

        int colourR, colourG, colourB;
        StrConv::conv(getColourEditBoxR()->getText(), colourR);
        StrConv::conv(getColourEditBoxG()->getText(), colourG);
        StrConv::conv(getColourEditBoxB()->getText(), colourB);
        RGB_Colour newColour(colourR, colourG, colourB);

        setColour(newColour);

        refreshAllElements();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleLABEditboxTextChanged(const EventArgs &)
    {
        if (d_ignoreEvents == true)
            return true;

        static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
        d_regexMatcher.setRegex(labRegEx);

        String LabLString = getLabEditBoxL()->getText();
        String LabAString = getLabEditBoxA()->getText();
        String LabBString = getLabEditBoxB()->getText();

        bool matchingRegEx = true;
        matchingRegEx &= d_regexMatcher.match(LabLString) == RegexMatcher::MS_VALID;
        matchingRegEx &= d_regexMatcher.match(LabAString) == RegexMatcher::MS_VALID;
        matchingRegEx &= d_regexMatcher.match(LabBString) == RegexMatcher::MS_VALID;

        if (!matchingRegEx)
            return true;

        NIIf LabColourL; StrConv::conv(LabLString, LabColourL);
        NIIf LabColourA; StrConv::conv(LabAString, LabColourA);
        NIIf LabColourB; StrConv::conv(LabBString, LabColourB);

        LabColourL = N_MIN(NIIf, N_MAX(NIIf, LabColourL, LAB_L_MIN), LAB_L_MAX);
        LabColourA = N_MIN(NIIf, N_MAX(NIIf, LabColourA, LAB_A_MIN), LAB_A_MAX);
        LabColourB = N_MIN(NIIf, N_MAX(NIIf, LabColourB, LAB_B_MIN), LAB_B_MAX);

        Lab_Colour newColour(LabColourL, LabColourA, LabColourB);

        setColour(newColour);

        refreshAllElements();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleHSVEditboxTextChanged(const EventArgs &)
    {
        if (d_ignoreEvents == true)
            return true;

        static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
        d_regexMatcher.setRegex(labRegEx);

        String HString = getHSVEditBoxH()->getText();
        String SString = getHSVEditBoxS()->getText();
        String VString = getHSVEditBoxV()->getText();

        bool matchingRegEx = true;
        matchingRegEx &= d_regexMatcher.match(HString) == RegexMatcher::MS_VALID;
        matchingRegEx &= d_regexMatcher.match(SString) == RegexMatcher::MS_VALID;
        matchingRegEx &= d_regexMatcher.match(VString) == RegexMatcher::MS_VALID;

        if (!matchingRegEx)
            return true;

        NIIf Hue; StrConv::conv(HString, Hue);
        NIIf Saturation; PStrConv::conv(SString, Saturation);
        NIIf Value; StrConv::conv(VString, Value);

        Hue = N_MIN(NIIf, N_MAX(NIIf, Hue, 0.0f), 1.0f);
        Saturation = N_MIN(NIIf, N_MAX(NIIf, Saturation, 0.0f), 1.0f);
        Value = N_MIN(NIIf, N_MAX(NIIf, Value, 0.0f), 1.0f);

        HSV_Colour newColour(Hue, Saturation, Value);

        setColour(newColour);

        refreshAllElements();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleAlphaEditboxTextChanged(const EventArgs &)
    {
        if (d_ignoreEvents == true)
            return true;

        static const String labRegEx = "[-+]?[0-9]*\\.?[0-9]+";
        d_regexMatcher.setRegex(labRegEx);

        String ValueString = getAlphaEditBox()->getText();
        bool matchingRegEx = d_regexMatcher.match(ValueString) == RegexMatcher::MS_VALID;

        if (!matchingRegEx)
            return true;

        NIIf value; StrConv::conv(ValueString, value);

        value = N_MAX(NIIf, N_MIN(NIIf, value, 1.0f), 0.0f);

        setColourAlpha(value);

        refreshAllElements();

        return true;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::setColourAlpha(NIIf value)
    {
        d_selectedColour.setAlpha(value);
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleAcceptButtonClicked(const EventArgs &)
    {
        WidgetEventArgs args(this);
        onAcceptButtonClicked(args);

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleRadioButtonModeSelection(const EventArgs & args)
    {
        RadioButton * radioButton = static_cast<RadioButton *>(
            static_cast<const WidgetEventArgs &>(args).mWidget);

        if (getLabRadioButtonL() == radioButton)
            d_sliderMode = SliderMode_Lab_L;
        else if (getLabRadioButtonA() == radioButton)
            d_sliderMode = SliderMode_Lab_A;
        else if (getLabRadioButtonB() == radioButton)
            d_sliderMode = SliderMode_Lab_B;
        else if (getHSVRadioButtonH() == radioButton)
            d_sliderMode = SliderMode_HSV_H;
        else if (getHSVRadioButtonS() == radioButton)
            d_sliderMode = SliderMode_HSV_S;
        else if (getHSVRadioButtonV() == radioButton)
            d_sliderMode = SliderMode_HSV_V;

        refreshColourPickerCursorPosition();

        refreshColourSliderPosition();

        refreshColourPickerControlsTextures();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleColourPickerSliderValueChanged(
        const EventArgs & args)
    {
        if (d_ignoreEvents == true)
            return true;

        Slider * imageSlider = static_cast<Slider *>(
            static_cast<const WidgetEventArgs &>(args).mWidget);

        NIIf sliderValue = imageSlider->getCurrent();

        Lab_Colour colourLAB = d_selectedColourLAB;
        HSV_Colour colourHSV = d_selectedColourHSV;

        switch (d_sliderMode)
        {
        case SliderMode_Lab_L:
            colourLAB.L = LAB_L_MIN + LAB_L_DIFF * sliderValue;
            setColour(colourLAB);
            break;
        case SliderMode_Lab_A:
            colourLAB.a = LAB_A_MIN + LAB_A_DIFF * sliderValue;
            setColour(colourLAB);
            break;
        case SliderMode_Lab_B:
            colourLAB.b = LAB_B_MIN + LAB_B_DIFF * sliderValue;
            setColour(colourLAB);
            break;
        case SliderMode_HSV_H:
            colourHSV.H = sliderValue;
            setColour(colourHSV);
            break;
        case SliderMode_HSV_S:
            colourHSV.S = sliderValue;
            setColour(colourHSV);
            break;
        case SliderMode_HSV_V:
            colourHSV.V = sliderValue;
            setColour(colourHSV);
            break;
        default:
            break;
        }

        onColourSliderChanged();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleAlphaSliderValueChanged(const EventArgs & args)
    {
        if (d_ignoreEvents == true)
            return true;

        Slider * imageSlider = static_cast<Slider *>(
            static_cast<const WidgetEventArgs &>(args).mWidget);
        NIIf sliderValue = 1.0f - imageSlider->getCurrent();

        setColourAlpha(sliderValue);

        refreshAlpha();

        refreshColourRects();

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleColourPickerStaticImageMouseButtonUp(
        const EventArgs & args)
    {
        const CursorEventArgs & mouseArgs = static_cast<const CursorEventArgs &>(args);

        if(mouseArgs.mButton == MB_Left)
            d_draggingColourPickerCursor = false;

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleColourPickerStaticImageMouseButtonDown(
        const EventArgs & args)
    {
        const CursorEventArgs & mouseArgs = static_cast<const CursorEventArgs &>(args);

        if(mouseArgs.mButton == MB_Left)
        {
            d_draggingColourPickerCursor = true;

            refreshColourPickerCursorPosition(mouseArgs);
        }

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleColourPickerStaticImageMouseMove(
        const EventArgs & args)
    {
        if(d_colourPickerCursor && d_draggingColourPickerCursor)
            refreshColourPickerCursorPosition(
                static_cast<const CursorEventArgs &>(args));

        return true;
    }
    //------------------------------------------------------------------------
    bool ColourPickerControls::handleColourPickerStaticImageMouseLeaves(
        const EventArgs &)
    {
        if(d_colourPickerCursor)
            d_draggingColourPickerCursor = false;

        return true;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::onCancelButtonClicked(const WidgetEventArgs * arg)
    {
        if(this->mParent != 0)
            this->mParent->remove(this);

        signal(EventClosed, arg);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::onAcceptButtonClicked(const WidgetEventArgs * arg)
    {
        d_callingColourPicker->setColour(d_selectedColour);

        if (this->mParent != 0)
            this->mParent->remove(this);

        signal(EventColourAccepted, arg);
        signal(EventClosed, arg);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourPickingImage()
    {
        if (d_sliderMode & (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
        {
            for (int y = 0; y < d_colourPickerPickingImageHeight; ++y)
            {
                for (int x = 0; x < d_colourPickerPickingImageWidth; ++x)
                {
                    int i = d_colourPickerControlsTextureSize * y + x;

                    Lab_Colour colour =
                        getColourPickingPositionColourLAB(static_cast<NIIf>(x),
                            static_cast<NIIf>(y));

                    d_colourPickingTexture[i] = RGB_Colour(colour);
                }
            }
        }
        else if (d_sliderMode &
                 (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
        {
            for (NCount y = 0; y < d_colourPickerPickingImageHeight; ++y)
            {
                for (NCount x = 0; x < d_colourPickerPickingImageWidth; ++x)
                {
                    int i = d_colourPickerControlsTextureSize * y + x;

                    HSV_Colour colour =
                        getColourPickingPositionColourHSV(static_cast<NIIf>(x),
                            static_cast<NIIf>(y));

                    d_colourPickingTexture[i] = RGB_Colour(colour);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourSliderImage()
    {
        if (d_sliderMode & (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
        {
            for (NCount y = 0; y < d_colourPickerPickingImageHeight; ++y)
            {
                for (NCount x = 0; x < d_colourPickerColourSliderImageWidth; ++x)
                {
                    int i = d_colourPickerControlsTextureSize * y +
                            (x + d_colourPickerPickingImageWidth + 2);

                    Lab_Colour colour = getColourSliderPositionColourLAB(
                        y / static_cast<NIIf>(d_colourPickerPickingImageHeight - 1));

                    d_colourPickingTexture[i] = RGB_Colour(colour);
                }
            }
        }
        else if (d_sliderMode & (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
        {
            for(NCount y = 0; y < d_colourPickerPickingImageHeight; ++y)
            {
                for(NCount x = 0; x < d_colourPickerColourSliderImageWidth; ++x)
                {
                    int i = d_colourPickerControlsTextureSize * y +
                            (x + d_colourPickerPickingImageWidth + 2);

                    HSV_Colour colour = getColourSliderPositionColourHSV(
                        y / static_cast<NIIf>(d_colourPickerPickingImageHeight - 1));

                    d_colourPickingTexture[i] = RGB_Colour(colour);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshAlphaSliderImage()
    {
        for(NCount y = 0; y < d_colourPickerAlphaSliderImageHeight; ++y)
        {
            for(NCount x = 0; x < d_colourPickerAlphaSliderImageWidth; ++x)
            {
                int i = x + d_colourPickerControlsTextureSize *
                    (y + d_colourPickerImageOffset + d_colourPickerPickingImageHeight);

                RGB_Colour curColour = getAlphaSliderPositionColour(x, y);
                d_colourPickingTexture[i] = curColour;
            }
        }
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerControls::getAlphaSliderPositionColour(int x, int y)
    {
        static const RGB_Colour background1(255, 255, 255);
        static const RGB_Colour background2(122, 122, 122);

        NIIf xRel = x / static_cast<NIIf>(d_colourPickerAlphaSliderImageWidth - 1);
        RGB_Colour final;

        bool isBackground1;

        if ((x % 30) >= 15)
            isBackground1 = false;
        else
            isBackground1 = true;

        if ((y % 30) >= 15)
            isBackground1 = !isBackground1;

        if (isBackground1)
            final = d_selectedColourRGB * (1.0f - xRel) + background1 * xRel;
        else
            final = d_selectedColourRGB * (1.0f - xRel) + background2 * xRel;

        return final;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::initColourPicker()
    {
        initColourPickerControlsImageSet();

        d_sliderMode = SliderMode_Lab_L;
        getLabRadioButtonL()->select(true);

        String temp;
        get("ColourPickerCursorStyle", temp);
        d_colourPickerCursor = N_Only(Widget).create(temp, getID() + ColourPickerCursorName, 0);

        d_colourPickerCursor->set("BackgroundEnabled", StrConv::conv(false));

        d_colourPickerCursor->set("FrameEnabled", StrConv::conv(false));

        get("ColourPickerCursorImage", temp);
        d_colourPickerCursor->set("Image", temp);

        d_colourPickerCursor->set("ImageColours", StrConv::conv(ColourArea(Colour(0.0f, 0.0f, 0.0f))));

        d_colourPickerCursor->setWidth(RelPos(0.05f, 0));
        d_colourPickerCursor->setHeight(RelPos(0.05f, 0));
        d_colourPickerCursor->setOvergo(true);
        d_colourPickerCursor->setAutoClip(false);

        getColourPickerImageSlider()->getThumb()->setSynEvent(false);

        getColourPickerStaticImage()->add(d_colourPickerCursor);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::positionColourPickerCursorRelative(NIIf x, NIIf y)
    {
        if (d_colourPickerCursor)
        {
            d_colourPickerCursor->setPos(RelVector2(
                RelPos(d_colourPickerCursor->getWidth().mScale * -0.5f + x, 0.0f),
                    d_colourPickerCursor->getPos().y));

            d_colourPickerCursor->setPos(RelVector2(
                d_colourPickerCursor->getPos().x,
                    RelPos(d_colourPickerCursor->getHeight().mScale * -0.5f + y, 0.0f)));
        }
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::positionColourPickerCursorAbsolute(NIIf x, NIIf y)
    {
        if(d_colourPickerCursor)
        {
            d_colourPickerCursor->setPos(RelVector2(
                RelPos(d_colourPickerCursor->getWidth().mScale * -0.5f, x),
                    d_colourPickerCursor->getPos().y));

            d_colourPickerCursor->setPos(RelVector2(
                d_colourPickerCursor->getPos().x,
                    RelPos(d_colourPickerCursor->getHeight().mScale * -0.5f, y)));
        }
    }
    //------------------------------------------------------------------------
    RGB_Colour ColourPickerControls::getSelectedColourRGB()
    {
        return d_selectedColourRGB;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourPickerCursorPosition()
    {
        Vector2f pos = getColourPickingColourPosition();

        positionColourPickerCursorRelative(pos.x, pos.y);
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourPickerCursorPosition(
        const CursorEventArgs & mouseEventArgs)
    {
        Vector2f position = Vector2f(mouseEventArgs->mLocalX, mouseEventArgs->mLocalY);

        NIIf x = position.x - mouseEventArgs.mWidget->getActualPosX();
        NIIf y = position.y - mouseEventArgs.mWidget->getActualPosY();

        positionColourPickerCursorAbsolute(x, y);

        if(d_sliderMode & (SliderMode_Lab_L | SliderMode_Lab_A | SliderMode_Lab_B))
        {
            Lab_Colour col = getColourPickingPositionColourLAB(x, y);
            setColour(col);
        }
        else if(d_sliderMode & (SliderMode_HSV_H | SliderMode_HSV_S | SliderMode_HSV_V))
        {
            HSV_Colour col = getColourPickingPositionColourHSV(x, y);
            setColour(col);
        }

        onColourCursorPositionChanged();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourSliderPosition()
    {
        String editboxText;

        switch (d_sliderMode)
        {
        case SliderMode_Lab_L:
            editboxText = getLabEditBoxL()->getText();
            break;
        case SliderMode_Lab_A:
            editboxText = getLabEditBoxA()->getText();
            break;
        case SliderMode_Lab_B:
            editboxText = getLabEditBoxB()->getText();
            break;
        case SliderMode_HSV_H:
            editboxText = getHSVEditBoxH()->getText();
            break;
        case SliderMode_HSV_S:
            editboxText = getHSVEditBoxS()->getText();
            break;
        case SliderMode_HSV_V:
            editboxText = getHSVEditBoxV()->getText();
            break;
        default:
            N_Only(Log).log(_I18n("[ColourPicker] Unknown slider mode - action not processed");
            break;
        }

        NIIf value;
        StrConv::conv(editboxText, value);
        NIIf sliderValue;

        switch (d_sliderMode)
        {
        case SliderMode_Lab_L:
            sliderValue = 1.0f - (LAB_L_MAX - value) / LAB_L_DIFF;
            break;
        case SliderMode_Lab_A:
            sliderValue = 1.0f - (LAB_A_MAX - value) / LAB_A_DIFF;
            break;
        case SliderMode_Lab_B:
            sliderValue = 1.0f - (LAB_B_MAX - value) / LAB_B_DIFF;
            break;
        case SliderMode_HSV_H:
            sliderValue = value;
            break;
        case SliderMode_HSV_S:
            sliderValue = value;
            break;
        case SliderMode_HSV_V:
            sliderValue = value;
            break;
        default:
            N_Only(Log).log(
                _I18n("[ColourPicker] Unknown slider mode - action not processed"));
            break;
        }

        d_ignoreEvents = true;
        getColourPickerImageSlider()->setCurrent(sliderValue);
        d_ignoreEvents = false;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshAlpha()
    {
        d_ignoreEvents = true;

        getColourPickerAlphaSlider()->setCurrent(1.0f - d_selectedColour.getAlpha());

        std::stringstream floatStringStream;
        floatStringStream.precision(4);
        floatStringStream.setf(std::ios::fixed, std::ios::floatfield);
        floatStringStream << d_selectedColour.getAlpha();
        getAlphaEditBox()->setText(floatStringStream.str().c_str());

        d_ignoreEvents = false;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::onColourSliderChanged()
    {
        refreshEditboxesAndColourRects();

        refreshColourPickerCursorPosition();

        refreshColourPickerControlsTextures();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshEditboxesAndColourRects()
    {
        refreshColourRects();

        d_ignoreEvents = true;

        getColourEditBoxR()->setText(StrConv::conv(d_selectedColourRGB.r));
        getColourEditBoxG()->setText(StrConv::conv(d_selectedColourRGB.g));
        getColourEditBoxB()->setText(StrConv::conv(d_selectedColourRGB.b));

        std::stringstream floatStringStream;
        floatStringStream.precision(3);
        floatStringStream.setf(std::ios::fixed, std::ios::floatfield);

        floatStringStream << d_selectedColourLAB.L;
        getLabEditBoxL()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");
        floatStringStream << d_selectedColourLAB.a;
        getLabEditBoxA()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");
        floatStringStream << d_selectedColourLAB.b;
        getLabEditBoxB()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");

        floatStringStream.precision(5);

        floatStringStream << d_selectedColourHSV.H;
        getHSVEditBoxH()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");
        floatStringStream << d_selectedColourHSV.S;
        getHSVEditBoxS()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");
        floatStringStream << d_selectedColourHSV.V;
        getHSVEditBoxV()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");

        floatStringStream.precision(4);
        floatStringStream << d_selectedColour.getAlpha();
        getAlphaEditBox()->setText(floatStringStream.str().c_str());
        floatStringStream.str("");

        d_ignoreEvents = false;
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshAllElements()
    {
        refreshEditboxesAndColourRects();

        refreshColourSliderPosition();

        refreshColourPickerCursorPosition();

        refreshAlpha();

        refreshColourPickerControlsTextures();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::onColourCursorPositionChanged()
    {
        refreshEditboxesAndColourRects();

        refreshAlphaSliderImage();

        refreshOnlyColourSliderImage();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshOnlyColourSliderImage()
    {
        refreshColourSliderImage();
        reloadColourPickerControlsTexture();
    }
    //------------------------------------------------------------------------
    void ColourPickerControls::refreshColourRects()
    {
        d_ignoreEvents = true;

        Colour newColourNoAlpha(d_selectedColour.getRed(),
            d_selectedColour.getGreen(),
                d_selectedColour.getBlue());

        String newColourRectString =
            StrConv::conv(ColourArea(newColourNoAlpha));

        Colour previousColourNoAlpha(d_previouslySelectedColour.getRed(),
            d_previouslySelectedColour.getGreen(),
                d_previouslySelectedColour.getBlue());

        String previousColourRectString =
            StrConv::conv(ColourArea(previousColourNoAlpha));

        getNewColourRect()->set(ColourRectPropertyName, newColourRectString);

        getOldColourRect()->set(ColourRectPropertyName, previousColourRectString);

        String colourString = StrConv::conv(d_selectedColour);
        getHexadecimalEditbox()->setText(colourString);

        d_ignoreEvents = false;
    }
    //------------------------------------------------------------------------
}