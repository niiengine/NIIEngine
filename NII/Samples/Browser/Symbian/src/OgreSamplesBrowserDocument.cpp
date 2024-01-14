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
/*
 * ==============================================================================
 *  Name        : OgreSamplesBrowserDocument.cpp
 *  Part of     : OpenGLEx / OgreSamplesBrowser
 *
 * ==============================================================================
 */

// INCLUDE FILES
#include "OgreSamplesBrowserDocument.h"
#include "OgreSamplesBrowserAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// COgreSamplesBrowserDocument::COgreSamplesBrowserDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
COgreSamplesBrowserDocument::COgreSamplesBrowserDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// COgreSamplesBrowserDocument::~COgreSamplesBrowserDocument()
// Destructor.
// ----------------------------------------------------
//
COgreSamplesBrowserDocument::~COgreSamplesBrowserDocument()
    {
    }

// ----------------------------------------------------
// COgreSamplesBrowserDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void COgreSamplesBrowserDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// COgreSamplesBrowserDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
COgreSamplesBrowserDocument* COgreSamplesBrowserDocument::NewL( CEikApplication& aApp )
    {
    COgreSamplesBrowserDocument* self = new (ELeave) COgreSamplesBrowserDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// COgreSamplesBrowserDocument::CreateAppUiL()
// constructs COgreSamplesBrowserAppUi
// ----------------------------------------------------
//
CEikAppUi* COgreSamplesBrowserDocument::CreateAppUiL()
    {
    return new (ELeave) COgreSamplesBrowserAppUi;
    }

// End of File
