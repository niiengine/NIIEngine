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
#ifndef _NII_ColourPickerControls_h_
#define _NII_ColourPickerControls_h_

#include "CEGUI/CommonDialogs/Module.h"
#include "CEGUI/CommonDialogs/ColourPicker/Types.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    class ColourPicker;

    /// Base class for the ColourPickerControls widget
    class CEGUI_COMMONDIALOGS_API ColourPickerControls : public Container
    {
    public:
        enum SliderMode
        {
            SliderMode_Lab_L = 1,
            SliderMode_Lab_A = 1 << 1,
            SliderMode_Lab_B = 1 << 2,
            SliderMode_HSV_H = 1 << 3,
            SliderMode_HSV_S = 1 << 4,
            SliderMode_HSV_V = 1 << 5
        };

        /// Constructor for ColourPickerControls class.
        ColourPickerControls(const String & type, const String & name);

        /// Destructor for ColourPickerControls class.
        ~ColourPickerControls();

        /** 
        */
        static const String EventColourAccepted;
        static const String EventClosed;

        RGB_Colour getSelectedColourRGB();

        /**
        \brief Set the current colour of the colour picker manually and refresh the
            ColourPickerControls elements accordingly.

        @param setting
            newColour the selected Colour for the ColourPickerControls
        */
        void setColour(const Colour & newColour);

        void setPreviousColour(const Colour & previousColour);

        void refreshAllElements();

        /**
        \brief Set the local d_callingColourPicker pointer to the colour picker window.
        @param setting colourPicker the ColourPicker window.
        */
        void setCallingColourPicker(ColourPicker * colourPicker);
    protected:
        /// @copydetails Container::initChild
        void initChild();
    protected:
        /// Widget name for the cancel button component.
        static const String CancelButtonName;
        /// Widget name for the accept button component.
        static const String AcceptButtonName;
        /// Widget name for the static text label of the hexadecimal colour value EditBox.
        static const String HexaDecimalDescriptionName;
        /// Widget name for the colour value EditBox.
        static const String HexaDecimalEditBoxName;
        static const String TitleBarName;
        static const String ColourPickerStaticImageName;
        static const String ColourPickerImageSliderName;
        static const String ColourPickerAlphaSliderName;
        static const String NewColourDescriptionName;
        static const String OldColourDescriptionName;
        static const String NewColourRectName;
        static const String OldColourRectName;
        static const String ColourEditBoxRDescriptionName;
        static const String ColourEditBoxGDescriptionName;
        static const String ColourEditBoxBDescriptionName;
        static const String ColourEditBoxRName;
        static const String ColourEditBoxGName;
        static const String ColourEditBoxBName;
        static const String HSVRadioButtonHName;
        static const String HSVRadioButtonSName;
        static const String HSVRadioButtonVName;
        static const String HSVEditBoxHName;
        static const String HSVEditBoxSName;
        static const String HSVEditBoxVName;
        static const String LabRadioButtonLName;
        static const String LabRadioButtonAName;
        static const String LabRadioButtonBName;
        static const String LabEditBoxLName;
        static const String LabEditBoxAName;
        static const String LabEditBoxBName;
        static const String AlphaEditBoxName;
        static const String ColourPickerCursorName;
        /// Name of the Picking PixelBuffer in the ImageSet
        static const String ColourPickerControlsPickingTextureImageName;
        /// Name of the Slider PixelBuffer in the ImageSet
        static const String ColourPickerControlsColourSliderTextureImageName;
        /// Name of the Slider PixelBuffer in the ImageSet
        static const String ColourPickerControlsAlphaSliderTextureImageName;
        /// Name of the colour defining ColourAreaDefine of ColourArea windows
        static const String ColourRectPropertyName;

        static const NIIf LAB_L_MIN;
        static const NIIf LAB_L_MAX;
        static const NIIf LAB_L_DIFF;
        static const NIIf LAB_A_MIN;
        static const NIIf LAB_A_MAX;
        static const NIIf LAB_A_DIFF;
        static const NIIf LAB_B_MIN;
        static const NIIf LAB_B_MAX;
        static const NIIf LAB_B_DIFF;

        void initColourPickerControlsImageSet();
        void deinitColourPickerControlsTexture();
        void refreshColourPickerControlsTextures();

        void refreshColourSliderImage();
        void refreshColourPickingImage();
        void refreshAlphaSliderImage();

        void reloadColourPickerControlsTexture();

        Lab_Colour getColourSliderPositionColourLAB(NIIf value);
        Lab_Colour getColourPickingPositionColourLAB(NIIf xAbs, NIIf yAbs);

        HSV_Colour getColourSliderPositionColourHSV(NIIf value);
        HSV_Colour getColourPickingPositionColourHSV(NIIf xAbs, NIIf yAbs);

        RGB_Colour getAlphaSliderPositionColour(int x, int y);

        Vector2f getColourPickingColourPosition();
        void getColourPickingColourPositionHSV(NIIf& x, NIIf& y);

        // Handlers to relay child widget events so they appear to come from us
        bool handleCancelButtonClicked(const EventArgs & e);
        bool handleAcceptButtonClicked(const EventArgs & e);
        bool handleEditboxDeactivated(const EventArgs & args);
        bool handleRadioButtonModeSelection(const EventArgs & args);

        bool handleHexadecimalEditboxTextChanged(const EventArgs & args);
        bool handleRGBEditboxTextChanged(const EventArgs & args);
        bool handleLABEditboxTextChanged(const EventArgs & args);
        bool handleHSVEditboxTextChanged(const EventArgs & args);

        bool handleAlphaEditboxTextChanged(const EventArgs & args);

        bool handleColourPickerStaticImageMouseLeaves(const EventArgs & args);
        bool handleColourPickerStaticImageMouseButtonUp(const EventArgs & args);
        bool handleColourPickerStaticImageMouseButtonDown(const EventArgs & args);
        bool handleColourPickerStaticImageMouseMove(const EventArgs & args);

        virtual void onCancelButtonClicked(const WidgetEventArgs * arg);
        virtual void onAcceptButtonClicked(const WidgetEventArgs * arg);

        void onColourCursorPositionChanged();
        void onColourSliderChanged();

        void refreshColourPickerCursorPosition(const CursorEventArgs & arg);
        void refreshAlpha();

        void refreshOnlyColourSliderImage();
        bool handleColourPickerSliderValueChanged(const EventArgs & args);
        bool handleAlphaSliderValueChanged(const EventArgs & args);

        void refreshEditboxesAndColourRects();

        void refreshColourRects();
        void positionColourPickerCursorAbsolute(NIIf x, NIIf y);
        void positionColourPickerCursorRelative(NIIf x, NIIf y);
        void setColour(const Lab_Colour & newColourLAB);
        void setColour(const RGB_Colour & newColourRGB);
        void setColour(const HSV_Colour & newColourHSV);

        void setColourAlpha(NIIf alphaValue);

        void refreshColourPickerCursorPosition();
        void refreshColourSliderPosition();

        void initColourPicker();
        PushButton * getCancelButton();
        PushButton * getAcceptButton();
        Widget * getHexadecimalDescription();
        Editbox * getHexadecimalEditbox();
        Titlebar * getTitleBar();
        Slider * getColourPickerImageSlider();
        Slider * getColourPickerAlphaSlider();
        Widget * getColourPickerStaticImage();
        Widget * getNewColourDescription();
        Widget * getOldColourDescription();
        Widget * getNewColourRect();
        Widget * getOldColourRect();
        Widget * getColourEditBoxRDescription();
        Widget * getColourEditBoxGDescription();
        Widget * getColourEditBoxBDescription();
        Editbox * getColourEditBoxR();
        Editbox * getColourEditBoxG();
        Editbox * getColourEditBoxB();
        RadioButton * getHSVRadioButtonH();
        RadioButton * getHSVRadioButtonS();
        RadioButton * getHSVRadioButtonV();
        Editbox * getHSVEditBoxH();
        Editbox * getHSVEditBoxS();
        Editbox * getHSVEditBoxV();
        RadioButton * getLabRadioButtonL();
        RadioButton * getLabRadioButtonA();
        RadioButton * getLabRadioButtonB();
        Editbox * getLabEditBoxL();
        Editbox * getLabEditBoxA();
        Editbox * getLabEditBoxB();
        Editbox * getAlphaEditBox();
        Widget * getColourPickerCursorStaticImage();

        ColourPicker * d_callingColourPicker;
        Widget * d_colourPickerCursor;

        SliderMode d_sliderMode;

        /// selected colour of the ColourPickerControls as CEGUI colour
        Colour d_selectedColour;
        /** selected colour of the ColourPickerControls as RGB_Colour
         * (Each colour component as unsigned char 0 - 255)
         */
        RGB_Colour d_selectedColourRGB;
        /** selected colour of the ColourPickerControls as Lab_Colour
         * (Each component as NIIf)
         */
        Lab_Colour d_selectedColourLAB;
        /** selected colour of the ColourPickerControls as HSV_Colour
         * (Each component as NIIf)
         */
        HSV_Colour d_selectedColourHSV;

        /// Previously selected colour of the ColourPickerControls
        Colour d_previouslySelectedColour;

        Texture * d_colourPickerControlsTextureTarget;

        int d_colourPickerImageOffset;
        int d_colourPickerPickingImageHeight;
        int d_colourPickerPickingImageWidth;
        int d_colourPickerColourSliderImageWidth;
        int d_colourPickerColourSliderImageHeight;
        int d_colourPickerAlphaSliderImageWidth;
        int d_colourPickerAlphaSliderImageHeight;

        int d_colourPickerControlsTextureSize;

        bool d_draggingColourPickerCursor;

        RGB_Colour * d_colourPickingTexture;

        bool d_ignoreEvents;
        RegexMatcher & d_regexMatcher;
    };
}
}
}
#endif