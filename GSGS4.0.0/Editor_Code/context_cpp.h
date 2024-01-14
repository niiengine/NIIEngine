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
#ifndef CONTEXT_CPP_H
#define CONTEXT_CPP_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include "gsgsContextManager.h"
#include "cpptoken.h"
#include "gsgsTagManager.h"

namespace gsgs
{
    class ContextCpp : public ContextBase
    {
        std::map<wxString, int> m_propertyInt;
        wxMenu* m_rclickMenu;

        static wxBitmap m_cppFileBmp;
        static wxBitmap m_hFileBmp;
        static wxBitmap m_otherFileBmp;

    protected:
        void OnShowCodeNavMenu(CodeCompleteEvent& e);
        void OnCodeCompleteFiles(CodeCompleteEvent& event);

    private:
        bool TryOpenFile(const wxFileName& fileName, bool lookInEntireWorkspace = true);
        bool IsJavaScript() const;

        void DisplayFilesCompletionBox(const wxString& word);
        bool DoGetFunctionBody(long curPos, long& blockStartPos, long& blockEndPos, wxString& content);
        void Initialize();
        bool DoCodeComplete(long pos);
        void DoCreateFile(const wxFileName& fn);
        void DoUpdateCalltipHighlight();

    public:
        virtual void ColourContextTokens(const wxString& workspaceTokensStr, const wxString& localsTokensStr);
        /**
         * @brief
         * @return
         */
        virtual bool IsAtBlockComment() const;
        /**
         * @brief
         * @return
         */
        virtual bool IsAtLineComment() const;
        ContextCpp(Editor* container);
        virtual bool IsDefaultContext() const;

        virtual ~ContextCpp();
        ContextCpp();
        virtual ContextBase* NewInstance(Editor* container);
        virtual bool CompleteWord();
        virtual bool CodeComplete(long pos = wxNOT_FOUND);
        virtual bool GotoDefinition();
        virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
        virtual wxString GetCurrentScopeName();
        virtual void AutoIndent(const wxChar&);
        virtual bool IsCommentOrString(long pos);
        virtual bool IsComment(long pos);
        virtual void AddMenuDynamicContent(wxMenu* menu);
        virtual void RemoveMenuDynamicContent(wxMenu* menu);
        virtual void ApplySettings();
        virtual void RetagFile();
        virtual wxString CallTipContent();
        virtual void SetActive();
        virtual void SemicolonShift();

        // ctrl-click style navigation support
        virtual int GetHyperlinkRange(int pos, int& start, int& end);
        virtual void GoHyperlink(int start, int end, int type, bool alt);

        // override swapfiles features
        virtual void SwapFiles(const wxFileName& fileName);

        // Event handlers
        virtual void OnDwellEnd(wxStyledTextEvent& event);
        virtual bool GetHoverTip(int pos);
        virtual void OnDbgDwellEnd(wxStyledTextEvent& event);
        virtual void OnDbgDwellStart(wxStyledTextEvent& event);
        virtual void OnSciUpdateUI(wxStyledTextEvent& event);
        virtual void OnFileSaved();
        virtual void AutoAddComment();

        // Capture menu events
        // return this context specific right click menu
        virtual wxMenu* GetMenu();
        virtual void OnSwapFiles(wxCommandEvent& event);
        virtual void OnInsertDoxyComment(wxCommandEvent& event);
        virtual void OnCommentSelection(wxCommandEvent& event);
        virtual void OnCommentLine(wxCommandEvent& event);
        virtual void OnGenerateSettersGetters(wxCommandEvent& event);
        virtual void OnFindImpl(wxCommandEvent& event);
        virtual void OnFindDecl(wxCommandEvent& event);
        virtual void OnGotoFunctionStart(wxCommandEvent& event);
        virtual void OnGotoNextFunction(wxCommandEvent& event);
        virtual void OnKeyDown(wxKeyEvent& event);
        virtual void OnUpdateUI(wxUpdateUIEvent& event);
        virtual void OnContextOpenDocument(wxCommandEvent& event);
        virtual void OnAddIncludeFile(wxCommandEvent& e);
        virtual void OnAddForwardDecl(wxCommandEvent& e);
        virtual void OnMoveImpl(wxCommandEvent& e);
        virtual void OnAddImpl(wxCommandEvent& e);
        virtual void OnAddMultiImpl(wxCommandEvent& e);
        virtual void OnOverrideParentVritualFunctions(wxCommandEvent& e);
        virtual void OnRenameGlobalSymbol(wxCommandEvent& e);
        virtual void OnFindReferences(wxCommandEvent& e);
        virtual void OnSyncSignatures(wxCommandEvent& e);
        virtual void OnRetagFile(wxCommandEvent& e);
        virtual void OnUserTypedXChars(const wxString& word);
        virtual void OnCallTipClick(wxStyledTextEvent& e);
        virtual void OnCalltipCancel();
        DECLARE_EVENT_TABLE()

    private:
        wxString GetWordUnderCaret();
        wxString GetFileImageString(const wxString& ext);
        wxString GetImageString(const gsgs::TagEntry& entry);
        wxString GetExpression(long pos, bool onlyWord, Editor* editor = NULL, bool forCC = true);
        bool DoGotoSymbol(TagEntryPtr tag);
        bool IsIncludeStatement(const wxString& line, wxString* fileName = NULL, wxString* fileNameUpToCaret = NULL);
        void RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
        int FindLineToAddInclude();
        void MakeCppKeywordsTags(const wxString& word, std::vector<TagEntryPtr>& tags);
        void DoOpenWorkspaceFile();
        void DoSetProjectPaths();
        bool DoGetSingatureRange(int line, int& start, int& end, Editor* ctrl);

    public:
        void DoMakeDoxyCommentString(DoxygenComment& dc, const wxString& blockPrefix, wxChar keywordPrefix);
        /**
         * \brief replace list of tokens representd by li with 'word'
         * \param li
         * \return
         */
        static void ReplaceInFiles(const wxString& word, const CppToken::Vec_t& li);

    private:
        /**
         * \brief try to find a swapped file for this rhs. The logic is based on the C++ coding conventions
         * a swapped file for a.cpp will be a.h or a.hpp
         * \param rhs input
         * \param lhs output
         * \return true if such sibling file exist, false otherwise
         */
        bool FindSwappedFile(const wxFileName& rhs, gsgsStringSet& others);
        bool FindSwappedFile(const wxFileName& rhs, wxString& lhs);


        /**
         * @brief format editor
         * @param editor
         */
        void DoFormatEditor(Editor* editor);
    };
}
#endif