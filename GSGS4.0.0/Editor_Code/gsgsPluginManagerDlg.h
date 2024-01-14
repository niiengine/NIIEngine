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

#ifndef gsgs_PluginManagerDlg_H
#define gsgs_PluginManagerDlg_H

#include "gsgsPreInclude.h"
#include "gsgsSplitterWindow.h"
#include "gsgsTreeCtrl.h"
#include <wx/richtext/richtextctrl.h>

namespace gsgs
{
    class PluginMgrDlg : public wxDialog
    {
    public:
        PluginMgrDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Manage Plugins"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsDlgSize,
            long style = /*wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/ wxSIMPLE_BORDER);

        virtual ~PluginMgrDlg();

        DataViewListCtrl* GetDvListCtrl()         { return m_dvListCtrl; }
        wxPanel* GetSplitterPage40()            { return m_splitterPage40; }
        wxRichTextCtrl* GetRichTextCtrl()       { return m_richTextCtrl; }
        wxPanel* GetSplitterPage44()            { return m_splitterPage44; }
        SplitterWindow* GetSplitter36()         { return m_splitter36; }
        Button* GetButton20()                   { return m_button20; }
        Button* GetButton22()                   { return m_button22; }
        Button* GetButton23()                   { return m_button23; }
        Button* GetButton24()                   { return m_button24; }
        Button* GetButton25()                   { return m_button25; }
        Button* GetButton26()                   { return m_button26; }
        Button* GetButton27()                   { return m_button27; }
    protected:
        void WritePropertyLine(const wxString& text);
        void WritePropertyLine(const wxString& label, const wxString& text);
        void WritePropertyNewLine(const wxString& text);
        void WritePropertyNewLine(const wxString& label, const wxString& text);
        void Initialize();
        void CreateInfoPage(const wxDataViewItem& item);
    protected:
        virtual void OnCheckAll(wxCommandEvent& event);
        virtual void OnCheckAllUI(wxUpdateUIEvent& event);
        virtual void OnUncheckAll(wxCommandEvent& event);
        virtual void OnUncheckAllUI(wxUpdateUIEvent& event);
        void OnItemSelected(wxDataViewEvent& event);
        void OnButtonOK(wxCommandEvent& event);
    protected:
        SplitterWindow* m_splitter36;
        DataViewListCtrl* m_dvListCtrl;
        wxPanel* m_splitterPage40;
        wxPanel* m_splitterPage44;
        wxRichTextCtrl* m_richTextCtrl;
        DialogButtonSizer* m_stdBtnSizer26;
        Button* m_buttonOK;
        Button* m_button30;
        Button* m_button20;
        Button* m_button22;
        Button* m_button23;
        Button* m_button24;
        Button* m_button25;
        Button* m_button26;
        Button* m_button27;
        wxArrayString m_initialDisabledPlugins;
    };
}
#endif
