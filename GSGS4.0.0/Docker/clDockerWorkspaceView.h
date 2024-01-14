#ifndef CLDOCKERWORKSPACEVIEW_H
#define CLDOCKERWORKSPACEVIEW_H

#include "gsgsTreeCtrlPanel.h"
#include "gsgsEvent.h"
#include "gsgsConfig.h"
#include <wx/menu.h>

class clDockerWorkspaceView : public gsgs::TreeCtrlPanel
{
    gsgs::Config m_config;

public:
    clDockerWorkspaceView(wxWindow* parent);
    ~clDockerWorkspaceView();

protected:
    void DoDockerfileContextMenu(wxMenu* menu, const wxString& dockerfile);
    void DoDockerComposeContextMenu(wxMenu* menu, const wxString& docker_compose);

protected:
    void OnWorkspaceClosed(wxCommandEvent& event);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnFileContextMenu(ContextMenuEvent& event);
    void OnFindInFilesDismissed(gsgs::FindInFilesEvent& event);
    void OnFindInFilesShowing(gsgs::FindInFilesEvent& event); // override parent's method
};

#endif // CLDOCKERWORKSPACEVIEW_H
