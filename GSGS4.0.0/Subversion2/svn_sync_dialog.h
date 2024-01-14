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

#ifndef __svn_sync_dialog__
#define __svn_sync_dialog__

#include "subversion2_ui.h"
#include "wxcrafter.h"

class Subversion2;
class SvnSyncDialog : public SvnSyncDialogBaseClass
{
    Subversion2 *m_plugin;
private:
    void UpdateUrl(const wxString& rootDir);

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnButtonOK( wxCommandEvent& event );

public:
    /** Constructor */
    SvnSyncDialog( wxWindow* parent, Subversion2 *plugin, const wxString& rootDir, bool excludeBin, const wxString& excludeExtensions );
    virtual ~SvnSyncDialog();

    const wxString& GetRootDir() const {
        return m_rootDir;
    }
    const wxString& GetExcludeExtensions() const {
        return m_excludeExtensions;
    }
    bool GetExcludeBin() const {
        return m_excludeBin;
    }

private:
    wxString m_rootDir;
    wxString m_excludeExtensions;
    bool m_excludeBin;
};

#endif // __svn_sync_dialog__
