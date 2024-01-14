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

#ifndef SFTPSTATUSPAGE_H
#define SFTPSTATUSPAGE_H

#include "UI.h"
#include "gsgsEvent.h"
#include "gsgsCommonHandler.h"
#include "sftp.h"
#include <wx/menu.h>
#include <wx/stc/stc.h>

class SFTPThreadMessage;
class SFTP;

class SFTPGrepStyler : public gsgs::FindResultsStyler
{
public:
    SFTPGrepStyler(wxStyledTextCtrl* stc);
    SFTPGrepStyler();
    virtual ~SFTPGrepStyler();

    void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasScope);
    void Reset();
};

class SFTPStatusPage : public SFTPStatusPageBase
{
public:
    SFTPStatusPage(wxWindow* parent, SFTPPlugin * plugin);
    virtual ~SFTPStatusPage();

    void AddLine(SFTPThreadMessage* message);
    void ShowContextMenu();
    void SetStatusBarMessage(const wxString& message);
    void ClearSearchOutput();
    void ShowSearchTab();
    void ShowLogTab();
    void AddSearchText(const wxString& text);
    
protected:
    virtual void OnContentMenu(wxContextMenuEvent& event);
    virtual void OnClearLog(wxCommandEvent& event);
    virtual void OnCopy(wxCommandEvent& event);
    virtual void OnSelectAll(wxCommandEvent& event);
    void OnTheme(wxCommandEvent& event);

    void OnFindOutput(gsgs::Event& event);
    void OnFindFinished(gsgs::Event& event);
    void OnFindError(gsgs::Event& event);
    void OnHotspotClicked(wxStyledTextEvent& event);
private:
    SFTPImages m_bitmaps;
    SFTPPlugin* m_plugin;
    SFTPGrepStyler::Ptr_t m_styler;
};
#endif // SFTPSTATUSPAGE_H
