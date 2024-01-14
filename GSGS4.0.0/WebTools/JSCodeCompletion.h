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

#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "gsgsSharedPtr.h"
#include "clTernServer.h"
#include "wxCodeCompletionBoxEntry.h"
#include "gsgsTipInfo.h"
#include "gsgsCommonHandler.h"
#include "gsgsEvent.h"

class WebTools;
class JSCodeCompletion : public ServiceProvider
{
    clTernServer m_ternServer;
    int m_ccPos;
    wxString m_workingDirectory;
    WebTools* m_plugin = nullptr;

public:
    typedef SharedPtr<JSCodeCompletion> Ptr_t;
    void OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename);
    void OnFunctionTipReady(gsgsTipInfoPtr calltip, const wxString& filename);
    void OnDefinitionFound(const clTernDefinition& loc);
    void OnGotoDefinition(wxCommandEvent& event);
    void OnInfoBarClicked(gsgs::Event& event);

    void OnFindSymbol(CodeCompleteEvent& event);
    void OnCodeComplete(CodeCompleteEvent& event);
    void OnCodeCompleteFunctionCalltip(CodeCompleteEvent& event);
   
    /**
     * @brief start code completion based on the word completion plugin
     */
    void TriggerWordCompletion();

protected:
    bool SanityCheck();
    void DoPromptForInstallNodeJS();
    void DoPromptForInstallTern();

public:
    JSCodeCompletion(const wxString& workingDirectory, WebTools* plugin);
    virtual ~JSCodeCompletion();

    /**
     * @brief Is Java Script code completion enabled?
     */
    bool IsEnabled() const;

    /**
     * @brief code complete the current expression
     */
    void CodeComplete(IEditor* editor);

    /**
     * @brief find definition of selected expression or the word under the caret
     */
    void FindDefinition(IEditor* editor);

    /**
     * @brief notify tern to clear the cache
     * @param editor
     */
    void ResetTern(bool force);

    /**
     * @brief reparse the file
     */
    void ReparseFile(IEditor* editor);

    /**
     * @brief add java script context menu entries
     */
    void AddContextMenu(wxMenu* menu, IEditor* editor);

    /**
     * @brief restart the tern server
     */
    void Reload();

    void ClearFatalError() { m_ternServer.ClearFatalErrorFlag(); }
};

#endif // JSCODECOMPLETION_H
