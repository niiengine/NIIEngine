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
#ifndef _MacCEGuiRendererSelector_h_
#define _MacCEGuiRendererSelector_h_

#include "CEGuiRendererSelector.h"

class MacCEGuiRendererSelector_pimpl;

//! RendererSelector for Apple Mac via Carbon
class MacCEGuiRendererSelector : public CEGuiRendererSelector
{
public:
    MacCEGuiRendererSelector();
    ~MacCEGuiRendererSelector();

    // Implement CEGuiRendererSelector interface.
    bool invokeDialog();

private:
    int populateRendererMenu();

    // implementation
    MacCEGuiRendererSelector_pimpl* d_pimpl;
};

#endif  // end of guard _MacCEGuiRendererSelector_h_
