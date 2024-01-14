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
#include "NiiConfigDialog.h"
#include "NiiEngine.h"
#include "resource.h"
#include <tchar.h>
#include <algorithm>

namespace
{
    NII::ConfigDialog *dlg;  // This is a pointer to instance, since this is a static member
}

namespace NII
{
    //-----------------------------------------------------------------
    ConfigDialog::ConfigDialog()
    {
#ifdef NIIEngine_Static
        mHInstance = GetModuleHandle(NULL);
#else
    #if N_DEBUG == 1
        mHInstance = GetModuleHandle(_T("NIIEngine.dll"));
    #else
        mHInstance = GetModuleHandle(_T("NIIEngine.dll"));
    #endif
#endif
        mCurrentSys = 0;
    }
    //----------------------------------------------------------------
    ConfigDialog::~ConfigDialog()
    {
    }
    //----------------------------------------------------------------
    PROCOUT ConfigDialog::proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
        HWND hwndDlgItem;
        static const RenderSysList * rsyslist;
        static SysOptionList opts;
        RenderSysList::const_iterator rsysiterator;

        NIIi i, sel, savedSel;

        switch (iMsg)
        {
        case WM_INITDIALOG:
            // Load saved settings
            dlg->mCurrentSys = N_Engine().getRender();
            // Get all render systems
            rsyslist = &N_Engine().getRenderList();
            rsysiterator = rsyslist->begin();
            i = 0;
            while (rsysiterator != rsyslist->end())
            {
                hwndDlgItem = GetDlgItem(hDlg, IDC_CBO_RENDERSYSTEM);

                SendMessage(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)(NIIb*)(*rsysiterator)->getName().c_str());

                if (*rsysiterator == dlg->mCurrentSys)
                {
                    // Select
                    SendMessage(hwndDlgItem, CB_SETCURSEL, (WPARAM)i, 0);
                    // Refresh Options
                    // Get options from render system
                    opts = (*rsysiterator)->getConfig();
                    // Reset list box
                    hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                    //SendMessage(hwndDlgItem, LB_RESETCONTENT, 0, 0);
                    // Iterate through options
                    SysOptionList::iterator pOpt = opts.begin();
                    String strLine;
                    while( pOpt!= opts.end() )
                    {
                        strLine = pOpt->second.mName + _T(": ") + pOpt->second.mValue;
                        SendMessage(hwndDlgItem, LB_ADDSTRING, 0, (LPARAM)strLine.c_str());
                        ++pOpt;
                    }
                }

                ++rsysiterator;
                ++i;
            }

            // Center myself
            NIIi x, y, screenWidth, screenHeight;
            RECT rcDlg;
            GetWindowRect(hDlg, &rcDlg);
            screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
            screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

            x = (screenWidth / 2) - ((rcDlg.right - rcDlg.left) / 2);
            y = (screenHeight / 2) - ((rcDlg.bottom - rcDlg.top) / 2);

            MoveWindow(hDlg, x, y, (rcDlg.right - rcDlg.left), (rcDlg.bottom - rcDlg.top), TRUE);

            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDC_CBO_RENDERSYSTEM:
                hwndDlgItem = GetDlgItem(hDlg, IDC_CBO_RENDERSYSTEM);
                sel = SendMessage(hwndDlgItem, CB_GETCOUNT, 0, 0);

                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    // RenderSys selected
                    // Get selected index
                    hwndDlgItem = GetDlgItem(hDlg, IDC_CBO_RENDERSYSTEM);
                    sel = SendMessage(hwndDlgItem, CB_GETCURSEL, 0, 0);
                    if (sel != -1)
                    {
                        // Get RenderSys selected
                        rsysiterator = rsyslist->begin();
                        dlg->mCurrentSys = rsysiterator[sel];
                        // refresh options
                        // Get options from render system
                        opts = rsysiterator[sel]->getConfig();
                        // Reset list box
                        hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                        SendMessage(hwndDlgItem, LB_RESETCONTENT, 0, 0);
                        // Iterate through options
                        SysOptionList::iterator pOpt = opts.begin();
                        String strLine;
                        while (pOpt != opts.end())
                        {
                            strLine = pOpt->second.mName + _T(": ") + pOpt->second.mValue;
                            SendMessage(hwndDlgItem, LB_ADDSTRING, 0, (LPARAM)strLine.c_str());
                            ++pOpt;
                        }
                    }
                }

                return TRUE;

            case IDC_LST_OPTIONS:
                if (HIWORD(wParam) == LBN_SELCHANGE)
                {
                    // Selection in list box of options changed
                    // Update combo and label in edit section
                    hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                    sel = SendMessage(hwndDlgItem, LB_GETCURSEL, 0, 0);
                    if (sel != -1)
                    {
                        SysOptionList::iterator pOpt = opts.begin();
                        for (i = 0; i < sel; i++)
                            ++pOpt;
                        // Set label text
                        hwndDlgItem = GetDlgItem(hDlg, IDC_LBL_OPTION);
                        //#ifdef UNICODE
                        //                        TCHAR temp[256];
                        //                        MultiByteToWideChar(CP_ACP, 0, pOpt->second.mName.c_str(), pOpt->second.mName.length(), temp, 255);
                        //                        SetWindowText(hwndDlgItem, temp);
                        //#else
                        SetWindowText(hwndDlgItem, pOpt->second.mName.c_str());
                        //#endif
                                                // Set combo options
                        hwndDlgItem = GetDlgItem(hDlg, IDC_CBO_OPTION);
                        SendMessage(hwndDlgItem, CB_RESETCONTENT, 0, 0);
                        StringList::iterator pPoss = pOpt->second.mReserve.begin();
                        i = 0;
                        while (pPoss != pOpt->second.mReserve.end())
                        {
                            SendMessage(hwndDlgItem, CB_ADDSTRING, 0, (LPARAM)pPoss[0].c_str());
                            if (pPoss[0] == pOpt->second.mValue)
                                // Select current value
                                SendMessage(hwndDlgItem, CB_SETCURSEL, (WPARAM)i, 0);
                            ++pPoss;
                            ++i;
                        }
                        // Enable/disable combo depending on mReserveEnable
                        EnableWindow(hwndDlgItem, (pOpt->second.mReserveEnable));
                    }
                }

                return TRUE;

            case IDC_CBO_OPTION:
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    // Updated an option
                    // Get option
                    hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                    sel = SendMessage(hwndDlgItem, LB_GETCURSEL, 0, 0);
                    savedSel = sel;
                    SysOptionList::iterator pOpt = opts.begin();
                    for (i = 0; i < sel; i++)
                        ++pOpt;
                    // Get selected value
                    hwndDlgItem = GetDlgItem(hDlg, IDC_CBO_OPTION);
                    sel = SendMessage(hwndDlgItem, CB_GETCURSEL, 0, 0);

                    if (sel != -1)
                    {
                        StringList::iterator pPoss = pOpt->second.mReserve.begin();

                        // Set option
                        dlg->mCurrentSys->setConfig(pOpt->second.mName, pPoss[sel]);
                        // Re-retrieve options
                        opts = dlg->mCurrentSys->getConfig();

                        // Reset options list box
                        hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                        SendMessage(hwndDlgItem, LB_RESETCONTENT, 0, 0);
                        // Iterate through options
                        pOpt = opts.begin();
                        String strLine;
                        while (pOpt != opts.end())
                        {
                            strLine = pOpt->second.mName + _T(": ") + pOpt->second.mValue;
                            SendMessage(hwndDlgItem, LB_ADDSTRING, 0, (LPARAM)strLine.c_str());
                            ++pOpt;
                        }
                        // Select previously selected item
                        SendMessage(hwndDlgItem, LB_SETCURSEL, savedSel, 0);
                    }

                }
                return TRUE;

            case IDOK:
            {
                // Set render system
                if (!dlg->mCurrentSys)
                {
                    MessageBox(NULL, _T("Please choose a rendering system."), _T("NII"), MB_OK | MB_ICONEXCLAMATION);
                    return TRUE;
                }
                String err = dlg->mCurrentSys->checkConfig();
                if (err.length() > 0)
                {
                    // refresh options incase updated by validation
                    // Get options from render system
                    opts = dlg->mCurrentSys->getConfig();
                    // Reset list box
                    hwndDlgItem = GetDlgItem(hDlg, IDC_LST_OPTIONS);
                    SendMessage(hwndDlgItem, LB_RESETCONTENT, 0, 0);
                    // Iterate through options
                    SysOptionList::iterator pOpt = opts.begin();
                    String strLine;
                    while (pOpt != opts.end())
                    {
                        strLine = pOpt->second.mName + _T(": ") + pOpt->second.mValue;
                        SendMessage(hwndDlgItem, LB_ADDSTRING, 0, (LPARAM)strLine.c_str());
                        ++pOpt;
                    }
                    //#ifdef UNICODE
                    //                    TCHAR temp[256];
                    //                    MultiByteToWideChar(CP_ACP, 0, err.c_str(), err.length(), temp, 255);
                    //                    MessageBox(NULL, temp, _T("NII"), MB_OK | MB_ICONEXCLAMATION);
                    //#else 
                    MessageBox(NULL, err.c_str(), _I18n("NII"), MB_OK | MB_ICONEXCLAMATION);
                    //#endif
                    return TRUE;
                }

                N_Engine().setRender(dlg->mCurrentSys);

                EndDialog(hDlg, TRUE);
                return TRUE;
            }
            case IDCANCEL:
                EndDialog(hDlg, FALSE);
                return TRUE;
            }
        }

        return FALSE;
    }
    //-----------------------------------------------------------------------
    bool ConfigDialog::display(void)
    {
        // Display dialog
        // Don't return to caller until dialog dismissed
        NIIi i;
        dlg = this;

        i = DialogBox(mHInstance, MAKEINTRESOURCE(IDD_DLG_CONFIG), NULL, proc);

        if (i == -1)
        {
            NIIi winError = GetLastError();
            Ntchar * errDesc;
            NIIi i;

            errDesc = new Ntchar[255];
            // Try windows errors first
            i = FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                winError,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR) errDesc,
                255,
                NULL
            );

            throw Exception(N_conv(winError) + errDesc, _T("DialogException"), N_FLF);
        }
        if (i)
            return true;
        else
            return false;
    }
    //-------------------------------------------------------------------------
}