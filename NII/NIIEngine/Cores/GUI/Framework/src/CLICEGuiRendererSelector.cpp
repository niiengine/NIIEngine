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
#include "CLICEGuiRendererSelector.h"
#include <iostream>

CLICEGuiRendererSelector::CLICEGuiRendererSelector()
{}

CLICEGuiRendererSelector::~CLICEGuiRendererSelector()
{}

bool CLICEGuiRendererSelector::invokeDialog()
{
    unsigned int selection;
    unsigned int rendererNumber = 0;

    std::cout << "-- CEGUI Sample Application Framework --" << std::endl;
    std::cout << "Please select a renderer:" << std::endl;

    CEGuiRendererType last_available;

    // print options for enabled renderers
    if (d_rendererAvailability[OgreGuiRendererType])
    {
        ++rendererNumber;
        last_available = OgreGuiRendererType;
        std::cout << rendererNumber << ". NiiEngine" << std::endl;
    }

    // abort if no renderers are available.
    if (rendererNumber == 0)
    {
        std::cout << "Oops!  There are no renderer modules available, check your config!" << std::endl;
        return false;
    }

    // if we only have one available renderer, then pick that one automatically
    if (rendererNumber == 1)
    {
        d_lastSelected = last_available;
        std::cout << "Picked the only renderer. Starting..." << std::endl;
        return true;
    }
    // get user to pick a valid option.
    std::cin >> selection;
    while (selection > rendererNumber)
    {
        std::cout << "Oops!  That was not a valid selection, please try again..." << std::endl;
        selection = 0;
        std::cin >> selection;
    }

    // discover which renderer was actually selected.
    if ((d_rendererAvailability[OgreGuiRendererType]) && (--selection == 0))
    {
        d_lastSelected = OgreGuiRendererType;
    }
    else if ((d_rendererAvailability[OpenGLGuiRendererType]) && (--selection == 0))
    {
        d_lastSelected = OpenGLGuiRendererType;
    }
    else if ((d_rendererAvailability[OpenGL3GuiRendererType]) && (--selection == 0))
    {
        d_lastSelected = OpenGL3GuiRendererType;
    }
    else if ((d_rendererAvailability[IrrlichtGuiRendererType]) && (--selection == 0))
    {
        d_lastSelected = IrrlichtGuiRendererType;
    }
    else if ((d_rendererAvailability[DirectFBGuiRendererType]) && (--selection == 0))
    {
        d_lastSelected = DirectFBGuiRendererType;
    }
    else
    {
        std::cout << "Something went horribly wrong :(" << std::endl;
        return false;
    }

    return true;
}
