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
#ifndef FIND_REPLACE_DLG_H
#define FIND_REPLACE_DLG_H

#include "gsgsConfig.h"
#include <wx/dialog.h>
#include <wx/stattext.h>

class wxCheckBox;
class wxStaticText;

#define wxFRD_MATCHCASE (1 << 0)
#define wxFRD_MATCHWHOLEWORD (1 << 1)
#define wxFRD_REGULAREXPRESSION (1 << 2)
#define wxFRD_SEARCHUP (1 << 3)
#define wxFRD_WRAPSEARCH (1 << 4)
#define wxFRD_SELECTIONONLY (1 << 5)
#define wxFRD_DISPLAYSCOPE (1 << 6)
#define wxFRD_SAVE_BEFORE_SEARCH (1 << 7)
#define wxFRD_SKIP_COMMENTS (1 << 8)
#define wxFRD_SKIP_STRINGS (1 << 9)
#define wxFRD_COLOUR_COMMENTS (1 << 10)
#define wxFRD_SEPARATETAB_DISPLAY (1 << 11)
#define wxFRD_ENABLE_PIPE_SUPPORT (1 << 12)

#define FIND_DLG 0
#define REPLACE_DLG 1

#ifndef WXDLLIMPEXP_LE
#ifdef WXMAKINGDLL
#define WXDLLIMPEXP_LE WXIMPORT
#elif defined(WXUSINGDLL)
#define WXDLLIMPEXP_LE WXEXPORT
#else
#define WXDLLIMPEXP_LE
#endif // WXDLLIMPEXP_LE
#endif

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_FIND_NEXT, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_CLOSE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACE, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACEALL, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_BOOKMARKALL, wxCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_LE, wxEVT_FRD_CLEARBOOKMARKS, wxCommandEvent);

class wxStaticText;
class wxComboBox;
class wxGridBagSizer;
class wxStaticBoxSizer;

namespace gsgs
{
    class FindReplaceData : public ObjectData
    {
    public:
        virtual void FromJSON(const JSONObject& json);
        virtual JSONObject ToJSON() const;

        FindReplaceData();
        virtual ~FindReplaceData() {}

        wxString GetFindString() const;
        wxString GetReplaceString() const;
        void SetFindString(const wxString& str);
        void SetReplaceString(const wxString& str);

        wxArrayString GetFindStringArr() const;

        wxArrayString GetReplaceStringArr() const;

        void SetEncoding(const wxString& encoding) { this->m_encoding = encoding; }
        void SetFileMask(const wxArrayString& fileMask) { this->m_fileMask = fileMask; }
        void SetFlags(size_t flags) { this->m_flags = flags; }
        void SetSearchPaths(const wxString& searchPaths);
        void SetSelectedMask(const wxString& selectedMask) { this->m_selectedMask = selectedMask; }
        const wxString& GetEncoding() const { return m_encoding; }
        const wxArrayString& GetFileMask() const { return m_fileMask; }
        size_t GetFlags() const { return m_flags; }
        const wxString& GetSearchPaths() const { return m_paths; }
        const wxString& GetSelectedMask() const { return m_selectedMask; }
    protected:
        void TruncateArray(wxArrayString& arr, size_t maxSize);
    private:
        wxArrayString m_replaceString;
        wxArrayString m_findString;
        size_t m_flags;
        wxString m_paths;
        wxString m_encoding;
        wxArrayString m_fileMask;
        wxString m_selectedMask;
    };

    enum frd_showzero { frd_dontshowzeros, frd_showzeros };

    class FindReplaceDialog : public wxDialog
    {
        wxEvtHandler* m_owner;

        FindReplaceData m_data;

        // Options
        wxComboBox* m_findString;
        wxComboBox* m_replaceString;
        wxCheckBox* m_matchCase;
        wxCheckBox* m_matchWholeWord;
        wxCheckBox* m_regualrExpression;
        wxCheckBox* m_searchUp;
        wxCheckBox* m_selectionOnly;

        // Buttons
        Button* m_find;
        Button* m_replace;
        Button* m_replaceAll;
        Button* m_markAll;
        Button* m_clearBookmarks;
        Button* m_cancel;
        wxStaticText* m_replacementsMsg;
        wxStaticText* m_replaceWithLabel;
        wxGridBagSizer* gbSizer;
        wxStaticBoxSizer* sz;
        int m_kind;
        unsigned int m_replacedCount;

    public:
        virtual ~FindReplaceDialog();
        FindReplaceDialog();
        FindReplaceDialog(wxWindow* parent, const FindReplaceData& data, wxWindowID id = wxID_ANY,
            const wxString& caption = _("Find / Replace"), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

        // Creation
        bool Create(wxWindow* parent, const FindReplaceData& data, wxWindowID id = wxID_ANY,
            const wxString& caption = _("Find / Replace"), const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

        // Return the data
        FindReplaceData& GetData() { return m_data; }

        virtual bool Show(int kind);

        void SetEventOwner(wxEvtHandler* owner) { m_owner = owner; }
        wxEvtHandler* GetEventOwner() const { return m_owner; }

        // Set the replacements message
        void SetReplacementsMessage(enum frd_showzero showzero = frd_showzeros);
        unsigned int GetReplacedCount() { return m_replacedCount; }
        void IncReplacedCount() { ++m_replacedCount; }
        void ResetReplacedCount() { m_replacedCount = 0; }
        void ResetSelectionOnlyFlag();
        void SetFindReplaceData(FindReplaceData& data, bool focus);

    protected:
        void ShowReplaceControls(bool show);
        void CreateViewImpl();
        void ConnectEvents();
        void OnClick(wxCommandEvent& event);
        void OnFindEvent(wxCommandEvent& event);
        void OnSelectionOnlyUI(wxUpdateUIEvent& event);
        void SendEvent(wxEventType type);

        void OnKeyDown(wxKeyEvent& event);
        DECLARE_EVENT_TABLE()
        void OnClose(wxCloseEvent& event);
    };
}
#endif // FIND_REPLACE_DLG_H
