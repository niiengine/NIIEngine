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
#include "CommonDialogs/ColourPicker/ColourPicker.h"
#include "CommonDialogs/ColourPicker/Controls.h"

    //------------------------------------------------------------------------
    extern "C"
    void initialiseCEGUICommonDialogs()
    {
        N_WidgetFactory(ColourPicker, N_WIDGET_ColourPicker).plugin();
        N_WidgetFactory(ColourPickerControls, N_WIDGET_ColourPickerControls).plugin();
    }
    //------------------------------------------------------------------------
    extern "C"
    void uninitialiseCEGUICommonDialogs()
    {
        N_WidgetFactory(ColourPicker, N_WIDGET_ColourPicker).unplugin();
        N_WidgetFactory(ColourPickerControls, N_WIDGET_ColourPickerControls).unplugin();
    }
    //------------------------------------------------------------------------