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
 *  Name        : OgreSamplesBrowserApp.cpp
 *  Part of     : OgreSamplesBrowser
 *
 * ==============================================================================
 */

// INCLUDE FILES
#include    <eikstart.h>
#include    "OgreSamplesBrowserApp.h"
#include    "OgreSamplesBrowserDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// COgreSamplesBrowserApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid COgreSamplesBrowserApp::AppDllUid() const
    {
    return KUidOgreSamplesBrowser;
    }


// ---------------------------------------------------------
// COgreSamplesBrowserApp::CreateDocumentL()
// Creates COgreSamplesBrowserDocument object
// ---------------------------------------------------------
//
CApaDocument* COgreSamplesBrowserApp::CreateDocumentL()
    {
    return COgreSamplesBrowserDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// NewApplication()
// Creates and returns the COgreSamplesBrowserApp object
// ---------------------------------------------------------
//
CApaApplication* NewApplication()
    {
    return new COgreSamplesBrowserApp;
    }

// ---------------------------------------------------------
// E32Main()
// Main entry point for exe application. Creates and starts
// the rest of the application.
// ---------------------------------------------------------
//
TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File
