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
#ifndef _Win32CEGuiRendererSelector_h_
#define _Win32CEGuiRendererSelector_h_

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define WIN32_LEAN_AND_MEAN
#   include "windows.h"
#endif

#include "CEGuiRendererSelector.h"


/*!
\brief Class representing a modal renderer selection dialog under Win32.
*/
class Win32CEGuiRendererSelector : public CEGuiRendererSelector
{
public:
    /*!
    \brief
        Constructor
    */
    Win32CEGuiRendererSelector();


    /*!
    \brief
        Destructor.
    */
    virtual ~Win32CEGuiRendererSelector();


    /*!
    \brief
        Displays a dialog allowing the user to select a renderer to be used.

    \return
        false if the user cancelled.
    */
    virtual bool    invokeDialog();


protected:
    /*!
    \brief
        Helper method to add entries for renderers to the Combobox and pre-select
        the first item entered.
    */
    bool addComboboxOption(HWND combo, const char* name, CEGuiRendererType rendererType);

    /*************************************************************************
        Static methods
    *************************************************************************/
    /*!
    \brief
        Create a Win32 dialog template for the requester.
    */
    static LPDLGTEMPLATE createDialogTemplate();


    /*!
    \brief
        Win32 dialog procedure function.
    */
    static INT_PTR CALLBACK dialogProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


    /*!
    \brief
        Take an input pointer, return closest pointer that is aligned on a DWORD (4 byte) boundary.

    \note
        This is taken from the DynDlg sample in the Win32 Platform SDK.
    */
    static LPBYTE alignPointer(LPBYTE buff);


    /*!
    \brief
        Converts the Ansi string in 'pAnsiIn' into wide characters and copies the result
        into the WORD array at 'pWCStr'.

    \note
        This is taken from the DynDlg sample in the Win32 Platform SDK.

    \param outBuff
        Pointer to the destination buffer.

    \param pAnsiIn
        Input ANSI string.

    \return
        The number of wide characters copied, including the trailing wide char NULL.
    */
    static int copyAnsiToWideChar(LPBYTE outBuff, PTSTR ansiString);


    /*************************************************************************
        Data fields
    *************************************************************************/
    LPDLGTEMPLATE   d_template;
};

#endif  // end of guard _Win32CEGuiRendererSelector_h_
