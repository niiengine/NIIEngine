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
#ifndef _CLICEGuiRendererSelector_h_
#define _CLICEGuiRendererSelector_h_

#include "CEGuiRendererSelector.h"


class CLICEGuiRendererSelector : public CEGuiRendererSelector
{
public:
    /*!
    \brief
        Constructor.
    */
    CLICEGuiRendererSelector();

    /*!
    \brief
        Destructor.
    */
    ~CLICEGuiRendererSelector();

    bool invokeDialog();
};

#endif  // end of guard _CLICEGuiRendererSelector_h_
