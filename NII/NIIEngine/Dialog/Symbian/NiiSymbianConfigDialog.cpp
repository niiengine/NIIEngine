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

#include "NiiConfigDialog.h"
#include "NiiException.h"
#include "NiiLogManager.h"

namespace NII
{
    //-------------------------------------------------------------
    ConfigDialog::ConfigDialog () {}
    //-------------------------------------------------------------
    bool ConfigDialog::display(void)
    {
        RenderSys * rsys;
        N_Engine().get(rsys);
        if(rsys != NULL)
        {
            return true;
        }

        // just select the first available render system for now.
        const RenderSysList & lstRend = N_Engine().getRenderList();
        RenderSysList::const_iterator pRend = lstRend.begin();

        while(pRend != lstRend.end())
        {
            N_Engine().set(*pRend);
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------
}
