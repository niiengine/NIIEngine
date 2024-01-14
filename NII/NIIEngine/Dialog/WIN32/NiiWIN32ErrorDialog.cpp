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
#include "NiiErrorDialog.h"
#include "resource.h"
#include <tchar.h>
#include <algorithm>

namespace 
{
    NII::ErrorDialog * errdlg;  // This is a pointer to instance, since this is a static member
}

namespace NII
{
    //-------------------------------------------------------------------------
    ErrorDialog::ErrorDialog()
    {
#ifdef NIIEngine_Static
        mHInstance = GetModuleHandle(NULL);
#else
#  if N_DEBUG == 1
        mHInstance = GetModuleHandle(_T("NIIEngine.dll"));
#  else
        mHInstance = GetModuleHandle(_T("NIIEngine.dll"));
#  endif
#endif
    }
    //-------------------------------------------------------------------------
    PROCOUT CALLBACK ErrorDialog::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
        HWND hwndDlgItem;

        switch (iMsg)
        {

        case WM_INITDIALOG:
            // Center myself
            NIIi x, y, screenWidth, screenHeight;
            RECT rcDlg;
            GetWindowRect(hDlg, &rcDlg);
            screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
            screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

            x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
            y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);

            MoveWindow(hDlg, x, y, (rcDlg.right - rcDlg.left),
                (rcDlg.bottom - rcDlg.top), TRUE);

            // Fill in details of error
            hwndDlgItem = GetDlgItem(hDlg, IDC_ERRMSG);
//#ifdef UNICODE
//            TCHAR temp[256];
//            MultiByteToWideChar(CP_ACP, 0, errdlg->mErrorMsg.c_str(), errdlg->mErrorMsg.length(), temp, 255);
//            SetWindowText(hwndDlgItem, temp);
//#else
            SetWindowText(hwndDlgItem, errdlg->mErrorMsg.c_str());
//#endif
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDOK:

                EndDialog(hDlg, TRUE);
                return TRUE;
            }
        }
        return FALSE;
    }
    //-----------------------------------------------------------------------
    void ErrorDialog::display(const String & errorMessage, const String & logName)
    {
        // Display dialog
        // Don't return to caller until dialog dismissed
        errdlg = this;
        mErrorMsg = errorMessage;
        DialogBox(mHInstance, MAKEINTRESOURCE(IDD_DLG_ERROR), NULL, DlgProc);
    }
    //-----------------------------------------------------------------------
}
