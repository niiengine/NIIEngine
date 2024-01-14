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
#ifndef __outputtabwindow__
#define __outputtabwindow__

#include <wx/stc/stc.h>
#include "gsgsStyleManager.h"
#include "gsgsCommonHandler.h"

class wxBoxSizer;

namespace gsgs
{
    class OutputTabWindow : public wxPanel
    {
    public:
        OutputTabWindow(wxWindow* parent, wxWindowID id, const wxString& name);
        virtual ~OutputTabWindow();

        virtual void Clear();
        const wxString& GetCaption() const { return m_name; }
        wxStyledTextCtrl* GetSci() { return m_sci; }
        virtual void AppendText(const wxString& text);
    protected:
        static void DefineMarker(wxStyledTextCtrl* sci, int marker, int markerType, wxColor fore, wxColor back);
        static void InitStyle(wxStyledTextCtrl* sci, int lexer, bool folding);

        void CreateViewImpl();
        virtual bool IsFocused();

        // Event handlers
        virtual void OnOutputScrolls(wxCommandEvent& e);
        virtual void OnClearAll(wxCommandEvent& e);
        virtual void OnWordWrap(wxCommandEvent& e);
        virtual void OnCollapseAll(wxCommandEvent& e);
        virtual void OnRepeatOutput(wxCommandEvent& e);
        virtual void OnEdit(wxCommandEvent& e);
        virtual void OnToggleHoldOpen(wxCommandEvent& e);

        virtual void OnOutputScrollsUI(wxUpdateUIEvent& e);
        virtual void OnClearAllUI(wxUpdateUIEvent& e);
        virtual void OnWordWrapUI(wxUpdateUIEvent& e);
        virtual void OnCollapseAllUI(wxUpdateUIEvent& e);
        virtual void OnRepeatOutputUI(wxUpdateUIEvent& e);

        virtual void OnMouseDClick(wxStyledTextEvent& e);
        virtual void OnHotspotClicked(wxStyledTextEvent& e);
        virtual void OnMarginClick(wxStyledTextEvent& e);
        virtual void OnEditUI(wxUpdateUIEvent& e);
        virtual void OnTheme(wxCommandEvent& e);

        DECLARE_EVENT_TABLE()
    protected:
        wxString m_name;
        ToolBar* m_tb;
        wxStyledTextCtrl* m_sci;
        bool m_outputScrolls;
        bool m_autoAppear;
        bool m_autoAppearErrors;
        bool m_errorsFirstLine;
        wxBoxSizer* m_vSizer;
        ThemeHandlerHelper* m_themeHelper;
        FindResultsStyler::Ptr_t m_styler;
    };
}
#endif
