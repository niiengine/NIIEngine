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

#ifndef WXCODECOMPLETIONBOXMANAGER_H
#define WXCODECOMPLETIONBOXMANAGER_H

#include "gsgsPreInclude.h"
#include "NewProjectDialog.h"

class wxStyledTextCtrl;
namespace gsgs
{
    class _gsgsAPI CodeBoxManager : public wxEvtHandler
    {
        friend class CodeBox;
    public:
        static CodeBoxManager& Get();

        /**
         * @brief uninitialize all event handlers and destroy the CC box manager
         */
        static void Free();

        /**
         * @brief show the completion box
         * @param ctrl the wxSTC control requesting the completion box
         * @param entries list of entries to display
         * @param flags code completion box options. See CodeBox::eOptions
         * for possible values
         * @param eventObject all events fired by the cc box will have eventObject
         * as the event object (wxEvent::GetEventObject())
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl,
            const wxCodeCompletionBoxEntry::Vec_t& entries,
            size_t flags,
            int startPos,
            wxEvtHandler* eventObject = NULL);
        /**
         * @brief show the completion box
         * @param ctrl the wxSTC control requesting the completion box
         * @param entries list of entries to display
         * @param bitmaps alternative bitmap list to use
         * @param flags code completion box options. See CodeBox::eOptions
         * for possible values
         * @param eventObject all events fired by the cc box will have eventObject
         * as the event object (wxEvent::GetEventObject())
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl,
            const wxCodeCompletionBoxEntry::Vec_t& entries,
            const CodeBox::BmpVec_t& bitmaps,
            size_t flags,
            int startPos,
            wxEvtHandler* eventObject = NULL);

        /**
         * @brief show the completion box
         * @param ctrl the wxSTC control requesting the completion box
         * @param tags list of entries to display
         * @param flags code completion box options. See CodeBox::eOptions
         * for possible values
         * @param eventObject all events fired by the cc box will have eventObject
         * as the event object (wxEvent::GetEventObject())
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl,
            const TagEntryPtrList& tags,
            size_t flags,
            int startPos,
            wxEvtHandler* eventObject = NULL);

        /**
         * @brief show the completion box. Language Server Protocol version
         */
        void ShowCompletionBox(wxStyledTextCtrl* ctrl,
            const LSP::CompletionItem::Vec_t& completions,
            size_t flags = 0,
            int startPos = wxNOT_FOUND,
            wxEvtHandler* eventObject = NULL);

        /**
         * @brief do we have a completion box shown?
         */
        bool IsShown() const { return m_box != NULL; }

        /**
         * @brief return the code completion box
         */
        CodeBox* GetCCWindow() { return m_box; }
    protected:
        void DestroyCCBox();
        void DestroyCurrent();
        void InsertSelection(wxCodeCompletionBoxEntry::Ptr_t match);
        void InsertSelectionTemplateFunction(const wxString& selection);

        // Handle the current editor events
        void OnStcModified(wxStyledTextEvent& event);
        void OnStcCharAdded(wxStyledTextEvent& event);
        void OnStcKeyDown(wxKeyEvent& event);
        void OnStcLeftDown(wxMouseEvent& event);
        void OnDismissBox(wxCommandEvent &event);
        void OnAppActivate(wxActivateEvent& event);

        void DoShowCCBoxTags(const TagEntryPtrList& tags);
        void DoShowCCBoxLSPItems(const LSP::CompletionItem::Vec_t& items);
        void DoShowCCBoxEntries(const wxCodeCompletionBoxEntry::Vec_t& entries);
        void DoConnectStcEventHandlers(wxStyledTextCtrl* ctrl);
    private:
        CodeBoxManager();
        ~CodeBoxManager();
    protected:
        CodeBox* m_box;
        wxStyledTextCtrl* m_stc;
    };
}
#endif
