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

#include "NiiUIPushButton.h"
#include "NiiUIEditbox.h"
#include "NiiUICursor.h"
#include "NiiWidgetManager.h"
#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Conversions.h"

namespace NII
{
    //------------------------------------------------------------------------
    N_EVENT(CommandObj, GameObj, EventOpenedPicker,     0);
    N_EVENT(CommandObj, GameObj, EventClosedPicker,     1);
    N_EVENT(CommandObj, GameObj, EventAcceptedColour,   2);
    N_EVENT(CommandObj, GameObj, EventCount,            3);
    //------------------------------------------------------------------------
    // Child Widget names
    const String ColourPicker::ColourRectName("__auto_colourrect__");
    //------------------------------------------------------------------------
    std::map<Widget *, int> ColourPicker::s_colourPickerWindows;
    //------------------------------------------------------------------------
    ColourPicker::ColourPicker(const String & type, const String & name) :
        Widget(type, name),
        d_shareColourPickerControlsWindow(false),
        d_colourPickerControlsWindow(0){}
    //------------------------------------------------------------------------
    ColourPicker::~ColourPicker(void)
    {
        if (d_colourPickerControlsWindow != 0)
        {
            if (d_shareColourPickerControlsWindow)
            {
                std::map<Widget *, int>::iterator iter =
                    s_colourPickerWindows.find(d_colourPickerControlsWindow);

                if (iter != s_colourPickerWindows.end())
                {
                    iter->second;

                    if (iter->second <= 0)
                        N_Only(Widget).destroy(iter->first);
                }
            }
            else
                N_Only(Widget).destroy(d_colourPickerControlsWindow);
        }
    }
    //------------------------------------------------------------------------
    void ColourPicker::initChild()
    {
        // get component windows
        Widget * colourRect = getColourRect();

        // bind handler to close button 'Click' event
        colourRect->add(Widget::CursorTouchEvent,
            EventFunctor(this, &ColourPicker::colourRect_ColourRectClickedHandler));

        layout();
        ColourArea temp;
        get("Colour", &temp);
        d_selectedColour = temp.mTL;

        initialiseColourPickerControlsWindow();
    }
    //------------------------------------------------------------------------
    void ColourPicker::setColour(const Colour & newColour)
    {
        d_selectedColour = newColour;

        WidgetEventArgs arg(this);
        signal(EventAcceptedColour, arg);

        Colour selectedColourNoAlpha = d_selectedColour;
        selectedColourNoAlpha.setAlpha(1.0f);
        String colourRectString(StrConv::conv(ColourArea(selectedColourNoAlpha)));

        this->set("Colour", colourRectString);
    }
    //------------------------------------------------------------------------
    Colour ColourPicker::getColour()
    {
        return d_selectedColour;
    }
    //------------------------------------------------------------------------
    Widget * ColourPicker::getColourRect(void)
    {
        return get(ColourRectName);
    }
    //------------------------------------------------------------------------
    void ColourPicker::initialiseColourPickerControlsWindow()
    {
        FactoryID colourPickerControlsStyle;
        this->get("ColourPickerControlsStyle", &colourPickerControlsStyle);
        this->get("ShareColourPickerControlsWidget", &d_shareColourPickerControlsWindow);

        if(d_shareColourPickerControlsWindow)
        {
            std::map<Widget *, int>::iterator iter = s_colourPickerWindows.begin();

            while(iter != s_colourPickerWindows.end() &&
                iter->first->getType() == colourPickerControlsStyle)
            {
                ++iter;
            }

            if (iter != s_colourPickerWindows.end())
            {
                ++(iter->second);
            }
            else
            {
                createColourPickerControlsWindow(colourPickerControlsStyle);
                s_colourPickerWindows[d_colourPickerControlsWindow] = 1;
            }
        }
        else
            createColourPickerControlsWindow(colourPickerControlsStyle);
    }
    //------------------------------------------------------------------------
    void ColourPicker::createColourPickerControlsWindow(
        FactoryID colourPickerControlsStyle)
    {
        d_colourPickerControlsWindow = dynamic_cast<ColourPickerControls *>(
            N_Only(Widget).create(colourPickerControlsStyle, 0, 0));
    }
    //------------------------------------------------------------------------
    bool ColourPicker::colourRect_ColourRectClickedHandler(const EventArgs &)
    {
        WidgetEventArgs args(this);
        onColourRectClicked(args);

        return true;
    }
    //------------------------------------------------------------------------
    void ColourPicker::onColourRectClicked(const WidgetEventArgs * arg)
    {
        if(d_colourPickerControlsWindow)
        {
            if(d_colourPickerControlsWindow->mParent == 0)
            {
                mSheet->getActive()->add(d_colourPickerControlsWindow);

                d_colourPickerControlsWindow->setCallingColourPicker(this);
                d_colourPickerControlsWindow->setColour(d_selectedColour);
                d_colourPickerControlsWindow->setPreviousColour(d_selectedColour);
                d_colourPickerControlsWindow->refreshAllElements();
                signal(EventOpenedPicker, arg);
            }
            else
            {
                if (d_colourPickerControlsWindow->mParent != 0)
                {
                    d_colourPickerControlsWindow->mParent->
                        remove(d_colourPickerControlsWindow);

                    d_colourPickerControlsWindow->setCallingColourPicker(0);
                    signal(EventClosedPicker, arg);
                }
            }
        }
    }
    //------------------------------------------------------------------------
}