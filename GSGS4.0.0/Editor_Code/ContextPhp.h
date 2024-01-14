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

#ifndef CONTEXTPHP_H
#define CONTEXTPHP_H

#include "gsgsContextManager.h"

namespace gsgs
{
    class ContextPhp : public ContextGeneric
    {
    public:
        ContextPhp();
        ContextPhp(Editor *Editor);
        virtual ~ContextPhp();
        
    public:
        bool IsStringTriggerCodeComplete(const wxString& str) const;
        virtual int GetActiveKeywordSet() const;
        virtual int  DoGetCalltipParamterIndex();
        virtual wxMenu* GetMenu();
        virtual void AddMenuDynamicContent(wxMenu* menu);
        virtual void ApplySettings();
        virtual void AutoIndent(const wxChar&);
        virtual wxString CallTipContent();
        virtual wxString GetCurrentScopeName();
        virtual gsgs::TagEntryPtr GetTagAtCaret(bool scoped, bool impl);
        virtual void GotoPreviousDefintion();
        virtual bool IsComment(long pos);
        virtual bool IsCommentOrString(long pos);
        virtual bool IsDefaultContext() const;
        virtual ContextBase* NewInstance(Editor* container);
        virtual void OnCallTipClick(wxStyledTextEvent& event);
        virtual void OnCalltipCancel();
        virtual void OnDbgDwellEnd(wxStyledTextEvent& event);
        virtual void OnDbgDwellStart(wxStyledTextEvent& event);
        virtual void OnDwellEnd(wxStyledTextEvent& event);
        virtual void OnDwellStart(wxStyledTextEvent& event);
        virtual void OnEnterHit();
        virtual void OnFileSaved();
        virtual void OnKeyDown(wxKeyEvent& event);
        virtual void OnSciUpdateUI(wxStyledTextEvent& event);
        virtual void RemoveMenuDynamicContent(wxMenu* menu);
        virtual void RetagFile();
        virtual void SemicolonShift();
        virtual void SetActive();
        virtual bool IsAtBlockComment() const;
        virtual bool IsAtLineComment() const;
        void ProcessIdleActions();
        
    };
}
#endif