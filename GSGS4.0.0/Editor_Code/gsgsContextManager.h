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
#ifndef CONTEXT_MANAGER_H
#define CONTEXT_MANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsSingleton.h"
#include "gsgsLexerStyle.h"
#include "gsgsTag.h"
#include <wx/stc/stc.h>

namespace gsgs
{
    class Editor;
    /**
     * \ingroup LiteEditor
     * \brief This class defines the language context of the editor
     *
     */
    class ContextBase : public wxEvtHandler
    {
    protected:
        Editor* m_container;
        wxString m_name;
        wxString m_selectedWord;
        std::vector<wxMenuItem*> m_dynItems;
        gsgsStringSet m_completionTriggerStrings;

    protected:
        void SetName(const wxString& name) { m_name = name; }
        void DoApplySettings(LexerStyle::Ptr_t lexPtr);
        void PrependMenuItem(wxMenu* menu, const wxString& text, wxObjectEventFunction func, int eventId = -1);
        void PrependMenuItem(wxMenu* menu, const wxString& text, int id);
        void PrependMenuItemSeparator(wxMenu* menu);

        /**
         * @brief search for the next 'what' in the editor
         */
        int FindNext(const wxString& what, int& pos, bool wholePage = false);
        /**
         * @brief search for the next 'what' in the editor
         */
        int FindPrev(const wxString& what, int& pos, bool wholePage = false);

    public:
        // ctor-dtor
        ContextBase(Editor* container);
        ContextBase(const wxString& name);

        virtual ~ContextBase();

        /**
         * @brief user typed '@' inside a block comment. Code complete possible keywords
         */
        virtual void BlockCommentComplete();

        /**
         * @brief return true if str is a string that should trigger a code completion
         */
        virtual bool IsStringTriggerCodeComplete(const wxString& str) const;

        /**
         * @brief when the context is requested to display a code completion for keywords
         * this functions determines which keyword set to display ( there are up to 5, indexed from: 0-4)
         */
        virtual int GetActiveKeywordSet() const { return 0; }

        /**
         * Return the context parent control
         */
        Editor& GetCtrl() { return *m_container; }

        Editor& GetCtrl() const { return *m_container; }
        /**
         * Return the context name
         */
        const wxString& GetName() const { return m_name; }

        // every Context derived class must implement the following methods
        virtual ContextBase* NewInstance(Editor* container) = 0;
        virtual void ApplySettings() = 0;

        // functions with default implementation:
        virtual bool IsDefaultContext() const { return true; }
        virtual void OnCallTipClick(wxStyledTextEvent& event) { event.Skip(); }
        virtual void OnCalltipCancel(){};
        virtual void OnDwellEnd(wxStyledTextEvent& event) { event.Skip(); }
        virtual void OnDbgDwellEnd(wxStyledTextEvent& event) { event.Skip(); }
        virtual void OnDbgDwellStart(wxStyledTextEvent& event) { event.Skip(); }
        virtual void OnKeyDown(wxKeyEvent& event) { event.Skip(); }
        virtual void AddMenuDynamicContent(wxMenu* WXUNUSED(menu)) {}
        virtual void RemoveMenuDynamicContent(wxMenu* WXUNUSED(menu)) {}
        virtual void OnSciUpdateUI(wxStyledTextEvent& WXUNUSED(event)) {}
        virtual void OnFileSaved() {}
        virtual void OnEnterHit() {}
        virtual void RetagFile() {}
        virtual void OnUserTypedXChars(const wxString& word);
        virtual wxString CallTipContent() { return wxEmptyString; }
        virtual void SetActive() {}
        virtual bool IsCommentOrString(long WXUNUSED(pos)) { return false; }
        /**
         * @brief return true if the caret is at a block comment
         */
        virtual bool IsAtBlockComment() const { return false; }
        /**
         * @brief return true if the caret is at a line comment
         */
        virtual bool IsAtLineComment() const { return false; }
        virtual void AutoIndent(const wxChar&);
        virtual void AutoAddComment();
        virtual bool CompleteWord() { return false; }
        virtual bool CodeComplete(long pos = wxNOT_FOUND)
        {
            wxUnusedVar(pos);
            return false;
        }
        virtual bool GetHoverTip(int pos) { return false; }
        virtual bool GotoDefinition() { return false; }
        virtual TagEntryPtr GetTagAtCaret(bool scoped, bool impl) { return NULL; }
        virtual wxString GetCurrentScopeName() { return wxEmptyString; }
        virtual void SemicolonShift() {}
        virtual int DoGetCalltipParamterIndex();

        // ctrl-click style navigation support
        virtual int GetHyperlinkRange(int pos, int& start, int& end);
        virtual void GoHyperlink(int start, int end, int type, bool alt);

        // override this method if you wish to provide context based right click menu
        virtual wxMenu* GetMenu();

        /**
         * @brief colour tokens in the current editor
         * @param workspaceTokens list of token that are associated with the workspace
         */
        virtual void ColourContextTokens(const wxString& workspaceTokensStr, const wxString& localsTokensStr)
        {
            wxUnusedVar(workspaceTokensStr);
            wxUnusedVar(localsTokensStr);
        }

        /**
         * @brief process any idle actions by the context
         */
        virtual void ProcessIdleActions() {}
    };

    typedef SharedPtr<ContextBase> ContextBasePtr;
    
    /**
     * \ingroup LiteEditor
     * \brief the basic editor from which complicated editors derives from (e.g. ContextCpp)
     */
    class ContextGeneric : public ContextBase
    {
    public:
        //---------------------------------------
        // ctors-dtor
        //---------------------------------------
        ContextGeneric(Editor* container, const wxString& name);
        ContextGeneric()
            : ContextBase(wxT("Text")){};
        ContextGeneric(const wxString& name)
            : ContextBase(name){};

        virtual ~ContextGeneric();
        virtual ContextBase* NewInstance(Editor* container);

        //---------------------------------------
        // Operations
        //---------------------------------------
        virtual void ApplySettings();
        
        void ProcessIdleActions();
    };

    class ContextManager : public Singleton<ContextManager>
    {
        friend class Singleton<ContextManager>;
        std::map<wxString, ContextBasePtr> m_contextPool;

    public:
        /**
         * Return new context by name
         * \param lexerName the lexer name
         * \param parent the context parent
         * \return
         */
        ContextBasePtr NewContext(Editor *parent, const wxString &lexerName);
        ContextBasePtr NewContextByFileName(Editor *parent, const wxFileName &fileName);
        void Initialize();
        
    private:
        ContextManager();
        virtual ~ContextManager();
    };
}
#endif
