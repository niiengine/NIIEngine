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
 *  Name        : OgreSamplesBrowserAppUi.cpp
 *  Part of     : OgreSamplesBrowser
 *
 * ==============================================================================
 */

// INCLUDE FILES
#include "OgreSamplesBrowserAppUi.h"
#include "OgreSamplesBrowserContainer.h"
#include <OgreSamplesBrowser.rsg>
#include "ogresamplesbrowser.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// COgreSamplesBrowserAppUi::ConstructL()
// ----------------------------------------------------------
//
void COgreSamplesBrowserAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) COgreSamplesBrowserContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// COgreSamplesBrowserAppUi::~COgreSamplesBrowserAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
COgreSamplesBrowserAppUi::~COgreSamplesBrowserAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// COgreSamplesBrowserAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void COgreSamplesBrowserAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// COgreSamplesBrowserAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse COgreSamplesBrowserAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// COgreSamplesBrowserAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void COgreSamplesBrowserAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        default:
            break;
        }
    }

// End of File
