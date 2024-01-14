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

#ifndef PHPCODECOMPLETION_H
#define PHPCODECOMPLETION_H

#include "PHPEntityBase.h"
#include "PHPExpression.h"
#include "PHPLookupTable.h"
#include "ieditor.h"
#include "php_event.h"
#include "gsgsPreProcess.h"
#include "gsgsBoxTip.h"
#include "gsgsSharedPtr.h"

struct PHPLocation {
    wxString what;     // Token name
    wxString filename; // file name (absolute path)
    int linenumber;    // line number within filename
    PHPEntityBase::Ptr_t entity;
    typedef SharedPtr<PHPLocation> Ptr_t;
};

class ResourceItem;
class PHPCodeCompletion : public ServiceProvider
{
public:
    enum {
        kCC_FUNCTIONS_ONLY = 0x00000001,
        kCC_SELF_FUNCTIONS = 0x00000002, // self::
        kCC_SELF_MEMBERS = 0x00000004,   // self::
        kCC_STATIC = 0x00000008,         // static::
        kCC_SELF = kCC_SELF_FUNCTIONS | kCC_SELF_MEMBERS,
    };

    static PHPCodeCompletion* Instance();
    static void Release();

protected:
    static PHPCodeCompletion* m_instance;
    gsgs::IManager* m_manager;
    gsgs::BoxTipWindow* m_typeInfoTooltip;
    PHPLookupTable m_lookupTable;
    std::unordered_map<wxString, PHPEntityBase::Ptr_t> m_currentNavBarFunctions;

    static bool CanCodeComplete(CodeCompleteEvent& e);
    void DoShowCompletionBox(const PHPEntityBase::List_t& entries, PHPExpression::Ptr_t expr);

protected:
    /**
     * @brief convert PHP's entity to CodeLite's TagEntry class
     * This is needed mainly for display purposes (CodeLite's API requires
     * TagEntry for display)
     * This function never fails.
     */
    TagEntryPtr DoPHPEntityToTagEntry(PHPEntityBase::Ptr_t entry);
    PHPEntityBase::Ptr_t DoGetPHPEntryUnderTheAtPos(gsgs::IEditor* editor, int pos, bool forFunctionCalltip);
    PHPEntityBase::List_t PhpKeywords(const wxString& prefix) const;

private:
    PHPCodeCompletion();
    virtual ~PHPCodeCompletion();

    // Event handlers
    void OnCodeComplete(CodeCompleteEvent& e);
    void OnCodeCompleteLangKeywords(CodeCompleteEvent& e);
    void OnFunctionCallTip(CodeCompleteEvent& e);
    void OnTypeinfoTip(CodeCompleteEvent& e);
    void OnCodeCompletionBoxDismissed(CodeCompleteEvent& e);
    void OnFindSymbol(CodeCompleteEvent& e);
    void OnQuickJump(CodeCompleteEvent& e);
    void OnInsertDoxyBlock(CodeCompleteEvent& e);
    void OnRetagWorkspace(wxCommandEvent& event);
    void OnParseEnded(gsgs::ParseEvent& event);
    void OnUpdateNavigationBar(CodeCompleteEvent& e);
    void OnNavigationBarMenuSelectionMade(gsgs::Event& e);
    void OnNavigationBarMenuShowing(ContextMenuEvent& e);

    // Workspace events
    void OnFileSaved(gsgs::Event& event);
    void DoSelectInEditor(gsgs::IEditor* editor, const wxString& what, int from);

    void DoOpenEditorForEntry(PHPEntityBase::Ptr_t entry);

public:
    /**
     * @brief go to the definition of the word starting at pos
     * @param editor
     * @param pos
     */
    void GotoDefinition(gsgs::IEditor* editor, int pos);

    void SetManager(gsgs::IManager* manager) { this->m_manager = manager; }
    /**
     * @brief return a PHPLocation::Ptr_t for the declaration of the
     * expression in the given editor / position
     * @return a valid PHPLocation or NULL
     */
    PHPLocation::Ptr_t FindDefinition(gsgs::IEditor* editor, int pos);

    /**
     * @brief return the PHPEntity under the caret
     */
    PHPEntityBase::Ptr_t GetPHPEntityAtPos(gsgs::IEditor* editor, int pos);

    /**
     * @brief open the symbols database for the given workspace file.
     * Close any opened database
     */
    void Open(const wxFileName& workspaceFile);

    /**
     * @brief close the lookup database
     */
    void Close();

    /**
     * @brief called by the PHP symbols cache job.
     * This is to optimize the searching the database (loading the symbols into the
     * memory forces a kernel caching)
     */
    void OnSymbolsCached();

    /**
     * @brief same as the above function, but the caching went bad...
     */
    void OnSymbolsCacheError();
    /**
     * @brief expand 'require_once' line (or any require) by replacing __file__ etc with the proper
     * values and appending everything
     */
    wxString ExpandRequire(const wxFileName& curfile, const wxString& require);

    /**
     * @brief return the best location for inserting generated code inside a class name
     * @param filecontent
     * @return wxNOT_FOUND when could not determine the location
     */
    int GetLocationForSettersGetters(const wxString& filecontent, const wxString& classname);

    /**
     * @brief list members of a class defined in an editor at the current position
     * This only returns member variables (i.e. no constants, nor static members)
     */
    void GetMembers(gsgs::IEditor* editor, PHPEntityBase::List_t& members, wxString& scope);
};

#endif // PHPCODECOMPLETION_H
