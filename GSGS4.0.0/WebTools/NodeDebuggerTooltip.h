#ifndef NODEDEBUGGERTOOLTIP_H
#define NODEDEBUGGERTOOLTIP_H

#include "gsgsEvent.h"
#include "nSerializableObject.h"
#include "gsgsStringUtil.h"
#include "NewProjectDialog.h"

class NodeDebuggerTooltip : public gsgs::ResizableTooltip
{
    std::unordered_map<wxString, wxTreeItemId> m_pendingItems;

protected:
    void OnItemExpanding(wxTreeEvent& event);
    wxString GetObjectId(const wxTreeItemId& item) const;
    void OnObjectProperties(gsgs::DebugEvent& event);

public:
    NodeDebuggerTooltip(wxWindow* parent);
    virtual ~NodeDebuggerTooltip();
    void Show(nSerializableObject::Ptr_t RemoteObject);
};

#endif // NODEDEBUGGERTOOLTIP_H
