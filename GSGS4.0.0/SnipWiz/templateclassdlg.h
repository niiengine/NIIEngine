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

#ifndef __templateclassdlg__
#define __templateclassdlg__

#include "templateclassbasedlg.h"

class SnipWiz;
class swStringDb;


class TemplateClassDlg : public TemplateClassBaseDlg
{
    SnipWiz* m_plugin;
    bool m_modified;
    wxString m_pluginPath;
    int m_curEol;
    wxString m_virtualFolder; // name of current project
    wxString m_projectPath;   // path to current project
    IManager* m_pManager;

protected:
    virtual void OnStcHeaderFileContentChnaged(wxStyledTextEvent& event);
    virtual void OnStcImplFileContentChnaged(wxStyledTextEvent& event);
    void OnClassNameEntered(wxCommandEvent& event);
    void OnBrowseVD(wxCommandEvent& event);
    void OnBrowseFilePath(wxCommandEvent& event);
    void OnGenerate(wxCommandEvent& event);
    void OnGenerateUI(wxUpdateUIEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnTemplateClassSelected(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonAddUI(wxUpdateUIEvent& event);
    void OnButtonChange(wxCommandEvent& event);
    void OnButtonChangeUI(wxUpdateUIEvent& event);
    void OnButtonRemove(wxCommandEvent& event);
    void OnButtonRemoveUI(wxUpdateUIEvent& event);
    void OnInsertClassKeyword(wxCommandEvent& event);
    void OnInsertClassKeywordUI(wxUpdateUIEvent& event);
    void OnButtonClear(wxCommandEvent& e);
    void OnButtonClearUI(wxUpdateUIEvent& e);

    swStringDb* GetStringDb();
    void RefreshTemplateList();
    void Initialize();

public:
    /** Constructor */
    TemplateClassDlg(wxWindow* parent, SnipWiz* plugin, IManager* manager);

    bool SaveBufferToFile(const wxString filename, const wxString buffer, int eolType = 0);

    void SetModified(const bool& modified) { this->m_modified = modified; }
    void SetPlugin(SnipWiz* plugin) { this->m_plugin = plugin; }
    const bool& GetModified() const { return m_modified; }
    SnipWiz* GetPlugin() { return m_plugin; }

    void SetCurEol(const int& curEol) { this->m_curEol = curEol; }
    void SetPluginPath(const wxString& pluginPath) { this->m_pluginPath = pluginPath; }
    void SetProjectPath(const wxString& projectPath) { this->m_projectPath = projectPath; }
    void SetVirtualFolder(const wxString& virtualFolder) { this->m_virtualFolder = virtualFolder; }
    const int& GetCurEol() const { return m_curEol; }
    const wxString& GetPluginPath() const { return m_pluginPath; }
    const wxString& GetProjectPath() const { return m_projectPath; }
    const wxString& GetVirtualFolder() const { return m_virtualFolder; }
};

#endif // __templateclassdlg__
