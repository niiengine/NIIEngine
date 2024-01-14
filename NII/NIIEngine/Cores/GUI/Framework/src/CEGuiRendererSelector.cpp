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
#include "CEGuiRendererSelector.h"


/*************************************************************************
    Constructor.
*************************************************************************/
CEGuiRendererSelector::CEGuiRendererSelector()
{
    d_lastSelected = InvalidGuiRendererType;

    // make all renderer types un-available.  sub-classes should enable appropriate types.
    for (int i = 0; i < RendererTypeCount; ++i)
    {
        d_rendererAvailability[i] = false;
    }
}


/*************************************************************************
    Destructor.
*************************************************************************/
CEGuiRendererSelector::~CEGuiRendererSelector()
{}


/*************************************************************************
    Return the CEGuiRendererType value of the renderer most recently
    selected by the user.
*************************************************************************/
CEGuiRendererType CEGuiRendererSelector::getSelectedRendererType()
{
    return d_lastSelected;
}


/*************************************************************************
    Set whether or not a specific renderer type will be available for
    selection from the dialog the next time it is displayed.
*************************************************************************/
void CEGuiRendererSelector::setRendererAvailability(CEGuiRendererType rendererType, bool available)
{
    if (rendererType < RendererTypeCount)
    {
        d_rendererAvailability[rendererType] = available;
    }
}
