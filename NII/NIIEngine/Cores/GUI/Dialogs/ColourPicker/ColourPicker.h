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

#ifndef _CEGUIColourPicker_h_
#define _CEGUIColourPicker_h_

#include "CEGUI/CommonDialogs/Module.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace NII_MEDIA
{
namespace UI
{
    ///Base class for the ColourPicker widget
    class CEGUI_COMMONDIALOGS_API ColourPicker : public Container
    {
    public:
        /// Constructor for ColourPicker class.
        ColourPicker(const String & type, const String & name);
        /// Destructor for ColourPicker class.
        ~ColourPicker(void);

        // generated internally by Widget
        /** 
        */
        static const String EventOpenedPicker;
        // generated internally by Widget
        /** 
        */
        static const String EventClosedPicker;

        // generated internally by Widget
        /** 
        */
        static const String EventAcceptedColour;

        /**
        \brief Set the current colour of the colour picker manually and refresh the
            ColourPicker elements accordingly.
        @param setting newColour the selected Colour for the ColourPicker
        */
        void setColour(const Colour & newColour);

        /**
        \brief Return the current colour of the colour picker.
        */
        Colour getColour();
    protected:
        /// @copydetails Container::initChild
        void initChild();
    protected:
        /// Widget name for the open button (colour rect) component.
        static const String ColourRectName;

        /*!\brief
            Initialises the properties for the creation of the ColourPickerControls
            window and decides if a new window of this type is necessary.
        @note This will be called once during the initialisation of the components.
        */
        void initialiseColourPickerControlsWindow();


        /*!\brief Creates the ColourPickerControls window.
        @param colourPickerControlsStyle
            The window type of the window that will be created.
        @note
            This will be called once during the initialisation of the components.
        */
        void createColourPickerControlsWindow(const String& colourPickerControlsStyle);

        /**
        \brief Return a Widget pointer to the ColourArea component widget for this
            ColourPicker.
        @return Pointer to a Widget object.
        @exception UnknownObjectException
            Thrown if the colour rectangle component does not exist.
        */
        Widget * getColourRect(void);

        bool colourRect_ColourRectClickedHandler(const EventArgs & e);
        virtual void onColourRectClicked(const WidgetEventArgs * arg);

        static std::map<Widget *, int> s_colourPickerWindows;

        bool d_shareColourPickerControlsWindow;

        ColourPickerControls * d_colourPickerControlsWindow;

        /// selected colour of the ColourPickerControls
        Colour d_selectedColour;
    };
}
}
}
#endif