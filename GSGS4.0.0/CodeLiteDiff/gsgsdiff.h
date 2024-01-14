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

#ifndef __CodeLiteDiff__
#define __CodeLiteDiff__

#include "gsgsPreInclude.h"
#include "plugin.h"

class gsgsDiffPlugin : public Plugin
{
    wxFileName m_leftFile;

protected:
    void OnNewDiff(wxCommandEvent& e);
    void OnNewDiffFolder(wxCommandEvent& e);
    void OnTabContextMenu(ContextMenuEvent& event);
    void OnDiff(wxCommandEvent& event);
    void DoClear();
    wxFileName SaveEditorToTmpfile(gsgs::IEditor* editor) const;

public:
    gsgsDiffPlugin(IManager* manager);
    ~gsgsDiffPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);
    virtual void UnPlug();
};

#endif // gsgsDiffPlugin
