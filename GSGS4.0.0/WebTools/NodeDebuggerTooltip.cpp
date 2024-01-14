#include "NodeDebuggerTooltip.h"
#include "NodeJSEvents.h"
#include "NoteJSWorkspace.h"
#include "PropertyDescriptor.h"
#include "RemoteObject.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include "gsgsJSON.h"

class NodeTreeItemData : public wxTreeItemData
{
    wxString m_data;

public:
    NodeTreeItemData(const wxString& data)
        : m_data(data)
    {
    }
    ~NodeTreeItemData() {}
    void SetData(const wxString& data) { this->m_data = data; }
    const wxString& GetData() const { return m_data; }
};

NodeDebuggerTooltip::NodeDebuggerTooltip(wxWindow* parent)
    : gsgs::ResizableTooltip(parent)
{
    m_treeCtrl->AddHeader("Name");
    m_treeCtrl->AddHeader("Value");
    gsgs_Event().Bind(wxEVT_NODEJS_DEBUGGER_OBJECT_PROPERTIES, &NodeDebuggerTooltip::OnObjectProperties, this);
}

NodeDebuggerTooltip::~NodeDebuggerTooltip()
{
    gsgs_Event().Unbind(wxEVT_NODEJS_DEBUGGER_OBJECT_PROPERTIES, &NodeDebuggerTooltip::OnObjectProperties,
                                 this);
}

void NodeDebuggerTooltip::Show(nSerializableObject::Ptr_t remoteObject)
{
    m_pendingItems.clear();
    m_treeCtrl->DeleteAllItems();
    RemoteObject* ro = remoteObject->To<RemoteObject>();
    m_treeCtrl->AddRoot(ro->GetExpression(), wxNOT_FOUND, wxNOT_FOUND, new NodeTreeItemData(ro->GetObjectId()));
    wxString preview = ro->GetTextPreview();

    // Set the value in a different column
    m_treeCtrl->SetItemText(m_treeCtrl->GetRootItem(), preview, 1);
    if(ro->HasChildren()) { m_treeCtrl->AppendItem(m_treeCtrl->GetRootItem(), "<dummy>"); }
    ResizableTooltip::ShowTip();
}

void NodeDebuggerTooltip::OnItemExpanding(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemIdValue cookie;
    wxTreeItemId child = m_treeCtrl->GetFirstChild(event.GetItem(), cookie);
    if(child.IsOk() && (m_treeCtrl->GetItemText(child) == "<dummy>")) {
        // Change the text value first
        m_treeCtrl->SetItemText(child, "Loading...");
        // Request for data more info from the debugger
        wxString objectId = GetObjectId(event.GetItem());
        if(objectId.IsEmpty()) {
            m_treeCtrl->DeleteChildren(event.GetItem());
            return;
        }
        m_pendingItems.insert({ objectId, event.GetItem() });
        NodeJSWorkspace::Get()->GetDebugger()->GetObjectProperties(objectId, wxEVT_NODEJS_DEBUGGER_OBJECT_PROPERTIES);
    }
}

wxString NodeDebuggerTooltip::GetObjectId(const wxTreeItemId& item) const
{
    NodeTreeItemData* d = dynamic_cast<NodeTreeItemData*>(m_treeCtrl->GetItemData(item));
    if(!d) { return ""; }
    return d->GetData();
}

void NodeDebuggerTooltip::OnObjectProperties(gsgs::DebugEvent& event)
{
    wxString objectId = event.GetStartupCommands();
    if(m_pendingItems.count(objectId) == 0) { return; }
    wxTreeItemId item = m_pendingItems[objectId];
    m_pendingItems.erase(objectId);

    // Get the output result (an array of PropertyDescriptor)
    wxString s = event.GetString();
    JSON root(s);
    JSONObject prop_arr = root.getObject();
    int size = prop_arr.getArraySize();
    std::vector<PropertyDescriptor> propVec;
    for(int i = 0; i < size; ++i) {
        JSONObject prop = prop_arr.getArray(i);
        PropertyDescriptor propDesc;
        propDesc.FromJSON(prop);
        if(!propDesc.IsEmpty()) { propVec.push_back(propDesc); }
    }

    m_treeCtrl->DeleteChildren(item);
    for(size_t i = 0; i < propVec.size(); ++i) {
        const PropertyDescriptor& prop = propVec[i];
        wxTreeItemId child = m_treeCtrl->AppendItem(item, prop.GetName());
        m_treeCtrl->SetItemText(child, prop.GetTextPreview(), 1);
        m_treeCtrl->SetItemData(child, new NodeTreeItemData(prop.GetValue().GetObjectId()));
        if(prop.HasChildren()) { m_treeCtrl->AppendItem(child, "<dummy>"); }
    }
}
