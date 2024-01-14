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

#include "CommonController1.h"
#include "CommonController.h"
#include "gsgsDebugPane.h"
#include "gsgsWindowManager.h"
#include "gsgsEditorConfig.h"
#include "NewProjectDialog.h"
#include "plugin_general_wxcp.h"
#include "gsgsConfig.h"
#include "gsgsFrame.h"
#include "gsgsStyleManager.h"
#include "gsgsBreakpointManager.h"
#include "gsgsLexerStyle.h"
#include "plugin.h"
#include "globals.h"
#include "gsgsManager.h"
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include <wx/generic/textdlgg.h>
#include "wx/dcbuffer.h"
#include "wx/xrc/xmlres.h"
#include <wx/aui/framemanager.h>

extern const char* stop_xpm[]; // Breakpoint

namespace gsgs
{
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerTreeListCtrlBase
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    DebuggerTreeListCtrlBase::DebuggerTreeListCtrlBase(wxWindow* parent, wxWindowID id, bool withButtonsPane,
        const wxPoint& pos, const wxSize& size, long style): 
        wxPanel(parent, id, pos, size, style), 
        m_withButtons(withButtonsPane)
    {
        wxBoxSizer* boxSizer29 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer29);

        m_toolbar = new ToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTB_FLAT);
        m_toolbar->SetToolBitmapSize(wxSize(16, 16));

        boxSizer29->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(0));

        m_listTable = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxTR_SINGLE | wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH);

        boxSizer29->Add(m_listTable, 1, wxALL | wxEXPAND, WXC_FROM_DIP(0));

        // Connect events
        m_listTable->Connect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListEditLabelBegin),
            NULL, this);
        m_listTable->Connect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListEditLabelEnd),
            NULL, this);
        m_listTable->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnItemRightClick), NULL,
            this);
        m_listTable->Connect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListKeyDown), NULL, this);
        m_listTable->Connect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnItemExpanding), NULL,
            this);
            
        m_listTable->SetShowHeader(true);
        m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), gsgs_Image().LoadBitmap("file_reload"));
        m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnRefresh, this, wxID_REFRESH);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnRefreshUI, this, wxID_REFRESH);

        if (m_withButtons) {
            m_toolbar->AddTool(wxID_DELETE, _("Delete"), gsgs_Image().LoadBitmap("clean"));
            m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnDeleteWatch, this, wxID_DELETE);
            m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnDeleteWatchUI, this, wxID_DELETE);

            m_toolbar->AddTool(wxID_NEW, _("New"), gsgs_Image().LoadBitmap("file_new"));
            m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnNewWatch, this, wxID_NEW);
            m_toolbar->Bind(wxEVT_UPDATE_UI, &DebuggerTreeListCtrlBase::OnNewWatchUI, this, wxID_NEW);
        }
        else {
            m_toolbar->AddTool(wxID_SORT_ASCENDING, _("Sort"), gsgs_Image().LoadBitmap("sort"));
            m_toolbar->Bind(wxEVT_TOOL, &DebuggerTreeListCtrlBase::OnSortItems, this, wxID_SORT_ASCENDING);
        }
        m_toolbar->Realize();
    }
    //-----------------------------------------------------------------------------
    DebuggerTreeListCtrlBase::~DebuggerTreeListCtrlBase()
    {
        m_listTable->Disconnect(wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,
            wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListEditLabelBegin), NULL, this);
        m_listTable->Disconnect(wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListEditLabelEnd),
            NULL, this);
        m_listTable->Disconnect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnItemRightClick), NULL,
            this);
        m_listTable->Disconnect(wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnListKeyDown), NULL,
            this);
        m_listTable->Disconnect(wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler(DebuggerTreeListCtrlBase::OnItemExpanding),
            NULL, this);
    }
    //-----------------------------------------------------------------------------
    IDebugger* DebuggerTreeListCtrlBase::DoGetDebugger()
    {
        if (!gsgs_Manager()->DbgCanInteract()) return NULL;

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        return dbgr;
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::DoResetItemColour(const wxTreeItemId& item, size_t itemKind)
    {
        wxColour bgColour = wxNullColour;
        wxColour fgColour = wxNullColour;

        wxTreeItemIdValue cookieOne;
        wxTreeItemId child = m_listTable->GetFirstChild(item, cookieOne);
        while (child.IsOk()) {
            DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(child);

            bool resetColor = ((itemKind == 0) || (data && (data->_kind & itemKind)));
            if (resetColor) { m_listTable->SetItemTextColour(child, fgColour, 1); }

            m_listTable->SetItemBackgroundColour(child, bgColour, 1);

            if (m_listTable->HasChildren(child)) { DoResetItemColour(child, itemKind); }
            child = m_listTable->GetNextChild(item, cookieOne);
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnEvaluateVariableObj(const DebuggerEventData& event)
    {
        wxString gdbId = event.m_expression;
        wxString value = event.m_evaluated;

        std::map<wxString, wxTreeItemId>::iterator iter = m_gdbIdToTreeId.find(gdbId);
        if (iter != m_gdbIdToTreeId.end()) {
            wxString newValue = value;
            wxString curValue = m_listTable->GetItemText(iter->second, 1);
            if (!(newValue == curValue || curValue.IsEmpty())) { m_listTable->SetItemTextColour(iter->second, *wxRED, 1); }
            m_listTable->SetItemText(iter->second, value, 1);

            // keep the red items IDs in the array
            m_gdbIdToTreeId.erase(iter);
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::DoRefreshItemRecursively(IDebugger* dbgr, const wxTreeItemId& item,
        wxArrayString& itemsToRefresh)
    {
        if (itemsToRefresh.IsEmpty()) return;

        wxTreeItemIdValue cookieOne;
        wxTreeItemId exprItem = m_listTable->GetFirstChild(item, cookieOne);
        while (exprItem.IsOk()) {

            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(exprItem));
            if (data) {
                int where = itemsToRefresh.Index(data->_gdbId);
                if (where != wxNOT_FOUND) {
                    dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
                    m_gdbIdToTreeId[data->_gdbId] = exprItem;
                    itemsToRefresh.RemoveAt((size_t)where);
                }
            }

            if (m_listTable->HasChildren(exprItem)) { DoRefreshItemRecursively(dbgr, exprItem, itemsToRefresh); }
            exprItem = m_listTable->GetNextChild(item, cookieOne);
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::Clear()
    {
        wxTreeItemId root = m_listTable->GetRootItem();
        if (root.IsOk()) {
            if (m_listTable->HasChildren(root)) {
                wxTreeItemIdValue cookie;
                wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);

                while (item.IsOk()) {
                    DoDeleteWatch(item);
                    item = m_listTable->GetNextChild(root, cookie);
                }

                m_listTable->DeleteChildren(root);
            }
        }

        m_listChildItemId.clear();
        m_createVarItemId.clear();
        m_gdbIdToTreeId.clear();
        m_curStackInfo.Clear();
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item, bool forceCreate)
    {
        if (!dbgr || !item.IsOk()) return;

        DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
        if (data && data->_gdbId.IsEmpty() == false) {
            dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
            m_gdbIdToTreeId[data->_gdbId] = item;

        }
        else if (data && forceCreate) {

            // try to re-create this variable object
            if (m_withButtons) {
                // HACK: m_withButton is set to true when we are in the context of
                // the 'Watches' table
                dbgr->CreateVariableObject(m_listTable->GetItemText(item), true, m_DBG_USERR);
            }
            else {
                dbgr->CreateVariableObject(m_listTable->GetItemText(item), false, m_DBG_USERR);
            }

            m_createVarItemId[m_listTable->GetItemText(item)] = item;
        }
    }
    //-----------------------------------------------------------------------------
    wxString DebuggerTreeListCtrlBase::DoGetGdbId(const wxTreeItemId& item)
    {
        wxString gdbId;
        if (!item.IsOk()) return gdbId;

        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(item);
        if (data) { return data->_gdbId; }
        return gdbId;
    }
    //-----------------------------------------------------------------------------
    wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByGdbId(const wxString& gdbId)
    {
        wxTreeItemId root = m_listTable->GetRootItem();
        wxTreeItemIdValue cookieOne;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {

            wxString id = DoGetGdbId(item);
            if (id.IsEmpty() == false && id == gdbId) return item;

            item = m_listTable->GetNextChild(root, cookieOne);
        }
        return wxTreeItemId();
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::DoDeleteWatch(const wxTreeItemId& item)
    {
        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr || !item.IsOk()) { return; }

        wxString gdbId = DoGetGdbId(item);
        if (gdbId.IsEmpty() == false) { dbgr->DeleteVariableObject(gdbId); }

    #ifdef __WXMAC__

        // Mac's GDB does not delete all the children of the variable object
        // instead we will do it manually

        if (m_listTable->HasChildren(item)) {
            // Delete this item children
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_listTable->GetFirstChild(item, cookie);
            while (child.IsOk()) {
                gdbId = DoGetGdbId(child);
                if (gdbId.IsEmpty() == false) { dbgr->DeleteVariableObject(gdbId); }

                if (m_listTable->HasChildren(child)) { DoDeleteWatch(child); }

                child = m_listTable->GetNextChild(item, cookie);
            }
        }
    #endif
    }
    //-----------------------------------------------------------------------------
    wxTreeItemId DebuggerTreeListCtrlBase::DoFindItemByExpression(const wxString& expr)
    {
        wxTreeItemId root = m_listTable->GetRootItem();
        wxTreeItemIdValue cookieOne;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {

            if (m_listTable->GetItemText(item) == expr) return item;
            item = m_listTable->GetNextChild(root, cookieOne);
        }
        return wxTreeItemId();
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::ResetTableColors() { DoResetItemColour(m_listTable->GetRootItem(), 0); }
    //-----------------------------------------------------------------------------
    wxString DebuggerTreeListCtrlBase::GetItemPath(const wxTreeItemId& item)
    {
        wxArrayString pathArr;
        if (item.IsOk() == false) return wxT("");

        DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(item);
        if (data && data->_gdbId.IsEmpty()) {
            // not a variable object item
            return m_listTable->GetItemText(item);
        }

        wxTreeItemId parent = item;
        while (parent.IsOk() && m_listTable->GetRootItem() != parent) {
            DbgTreeItemData* itemData = (DbgTreeItemData*)m_listTable->GetItemData(parent);
            if (itemData && !itemData->_isFake) { pathArr.Add(m_listTable->GetItemText(parent)); }
            parent = m_listTable->GetItemParent(parent);
        }

        if (pathArr.IsEmpty()) return wxT("");

        wxString itemPath;
        for (int i = (int)pathArr.GetCount() - 1; i >= 0; i--) {
            itemPath << pathArr.Item(i) << wxT(".");
        }
        itemPath.RemoveLast();
        return itemPath;
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnCreateVariableObjError(const DebuggerEventData& event)
    {
        // failed to create a variable object!
        // remove this expression from the table
        wxTreeItemId root = m_listTable->GetRootItem();
        wxTreeItemIdValue cookieOne;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {

            if (event.m_expression == m_listTable->GetItemText(item)) {
                m_listTable->Delete(item);
                break;
            }
            item = m_listTable->GetNextChild(root, cookieOne);
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnDeleteWatch(wxCommandEvent& event) 
    { 
        event.Skip(); 
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnDeleteWatchUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(!m_withButtons); 
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnItemExpanding(wxTreeEvent& event) 
    { 
        event.Skip(); 
    }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnItemRightClick(wxTreeEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnListEditLabelBegin(wxTreeEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnListEditLabelEnd(wxTreeEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnListKeyDown(wxTreeEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnNewWatch(wxCommandEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnNewWatchUI(wxUpdateUIEvent& event) { event.Enable(m_withButtons); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnRefresh(wxCommandEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnRefreshUI(wxUpdateUIEvent& event) { event.Skip(); }
    //-----------------------------------------------------------------------------
    void DebuggerTreeListCtrlBase::OnSortItems(wxCommandEvent& event) {}
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // LocalsTable
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(LocalsTable, DebuggerTreeListCtrlBase)
    EVT_MENU(XRCID("Change_Value"), LocalsTable::OnEditValue)
    EVT_UPDATE_UI(XRCID("Change_Value"), LocalsTable::OnEditValueUI)
    END_EVENT_TABLE()
    //-----------------------------------------------------------------------------
    LocalsTable::LocalsTable(wxWindow* parent)
        : DebuggerTreeListCtrlBase(parent, wxID_ANY, false)
        , m_arrayAsCharPtr(false)
        , m_sortAsc(true)
    {
        m_toolbar->Hide();
        //m_listTable->AddRoot(_("Locals"));
        m_listTable->AddRoot(_("LocalValue"));
        m_listTable->AddHeader(_("Name"));
        m_listTable->AddHeader(_("Type"));
        m_listTable->AddHeader(_("Value"));
        
        // Only sort top level items, don't sort their children
        m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        
        SetSortingFunction();
        m_DBG_USERR = DBG_USERR_LOCALS;
        m_QUERY_NUM_CHILDS = QUERY_LOCALS_CHILDS;
        m_LIST_CHILDS = LIST_LOCALS_CHILDS;

        gsgs_Event().Connect(wxEVT_DEBUGGER_FRAME_SELECTED, gsgsEventHandler(LocalsTable::OnStackSelected),
                                      NULL, this);
    }
    //-----------------------------------------------------------------------------
    LocalsTable::~LocalsTable() 
    {
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::UpdateLocals(const LocalVariables& locals) 
    { 
        DoUpdateLocals(locals, DbgTreeItemData::Locals); 
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::UpdateFuncArgs(const LocalVariables& args) 
    { 
        DoUpdateLocals(args, DbgTreeItemData::FuncArgs); 
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::Initialize()
    {
        // Read the debugger defined commands
        DebuggerSettingsPreDefMap data;
        gsgs_Config().Read(wxT("DebuggerCommands"), &data);

        DebuggerInformation info;
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr) {
            DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
            m_resolveLocals = info.resolveLocals;
            m_arrayAsCharPtr = info.charArrAsPtr;
            m_defaultHexDisplay = info.defaultHexDisplay;
        }

        m_preDefTypes = data.GetActiveSet();
        m_curStackInfo.Clear();
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnCreateVariableObj(const DebuggerEventData& event)
    {
        wxString expr = event.m_expression;
        std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
        if(iter != m_createVarItemId.end()) 
        {
            // set the variable object
            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(iter->second));
            if(data) {

                data->_gdbId = event.m_variableObject.gdbId;
                data->_kind = DbgTreeItemData::VariableObject;

                // variable object's type name is extracted from the event.m_variableObject.typeName
                m_listTable->SetItemText(iter->second, event.m_variableObject.typeName, 2);

                // refresh this item only
                IDebugger* dbgr = DoGetDebugger();
                if (dbgr)
                {
                    DoRefreshItem(dbgr, iter->second, false);

                    dbgr->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
                    dbgr->ListChildren(data->_gdbId, m_LIST_CHILDS);
                }
                m_listChildItemId[data->_gdbId] = iter->second;
            }
            m_createVarItemId.erase(iter);
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnListChildren(const DebuggerEventData& event)
    {
        wxString gdbId = event.m_expression;
        std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
        if(iter == m_listChildItemId.end()) return;

        wxTreeItemId item = iter->second;
        m_listChildItemId.erase(iter);

        if(event.m_userReason == m_LIST_CHILDS) {
            if(event.m_varObjChildren.empty() == false) {
                for(size_t i = 0; i < event.m_varObjChildren.size(); i++) {

                    IDebugger* dbgr = DoGetDebugger();
                    if(!dbgr) return;

                    VariableObjChild ch = event.m_varObjChildren.at(i);
                    if(ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
                        // not really a node...
                        // ask for information about this node children
                        dbgr->ListChildren(ch.gdbId, m_LIST_CHILDS);
                        m_listChildItemId[ch.gdbId] = item;

                    } else {

                        DbgTreeItemData* data = new DbgTreeItemData();
                        data->_gdbId = ch.gdbId;
                        data->_isFake = ch.isAFake;

                        wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);
                        m_listTable->SetItemText(child, ch.type, 2);

                        // Add a dummy node
                        if(child.IsOk() && ch.numChilds > 0) { m_listTable->AppendItem(child, wxT("<dummy>")); }

                        // refresh this item only
                        dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
                        // ask the value for this node
                        m_gdbIdToTreeId[data->_gdbId] = child;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnVariableObjUpdate(const DebuggerEventData& event)
    {
        VariableObjectUpdateInfo updateInfo = event.m_varObjUpdateInfo;

        // remove all obsolete items
        for(size_t i = 0; i < updateInfo.removeIds.GetCount(); i++) 
        {
            wxString gdbId = updateInfo.removeIds.Item(i);
            wxTreeItemId item = DoFindItemByGdbId(gdbId);
            if(item.IsOk()) 
            {
                DoDeleteWatch(item);
                m_listTable->Delete(item);
            }
        }

        // refresh the values of the items that requires that
        IDebugger* dbgr = DoGetDebugger();
        if(dbgr) 
        {
            wxArrayString itemsToRefresh = event.m_varObjUpdateInfo.refreshIds;
            DoRefreshItemRecursively(dbgr, m_listTable->GetRootItem(), itemsToRefresh);
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnItemExpanding(wxTreeEvent& event)
    {
        wxBusyCursor bc;
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

        IDebugger* dbgr = DoGetDebugger();
        if(!dbgr || !event.GetItem()) {
            // dont allow the expansion of this item
            event.Veto();
            return;
        }

        size_t childCount = m_listTable->GetChildrenCount(event.GetItem());
        if(childCount > 1) {
            // make sure there is no <dummy> node and continue
            wxTreeItemIdValue cookieOne;
            wxTreeItemId dummyItem = m_listTable->GetFirstChild(event.GetItem(), cookieOne);
            while(dummyItem.IsOk()) {
                if(m_listTable->GetItemText(dummyItem) == wxT("<dummy>")) {
                    m_listTable->Delete(dummyItem);
                    event.Skip();
                    return;
                }
                dummyItem = m_listTable->GetNextChild(event.GetItem(), cookieOne);
            }

        } else if(child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {

            // a dummy node, replace it with the real node content
            m_listTable->Delete(child);

            wxString gdbId = DoGetGdbId(event.GetItem());
            if(gdbId.IsEmpty() == false) {
                dbgr->UpdateVariableObject(gdbId, m_DBG_USERR);
                dbgr->ListChildren(gdbId, m_LIST_CHILDS);
                m_listChildItemId[gdbId] = event.GetItem();

            } else {
                // first time
                // create a variable object
                wxString expres = m_listTable->GetItemText(event.GetItem());
                DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(event.GetItem());
                if(data && data->_kind == DbgTreeItemData::FuncRetValue) { expres = data->_retValueGdbValue; }

                dbgr->CreateVariableObject(expres,                                       // the expression
                                           data->_kind == DbgTreeItemData::FuncRetValue, // bound to creation frame?
                                           m_DBG_USERR);                                 // user reason
                m_createVarItemId[expres] = event.GetItem();
            }
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags,
                                                      std::map<wxString, wxString>& oldValues)
    {
        wxTreeItemIdValue cookie;
        std::vector<wxTreeItemId> itemsToRemove;

        wxTreeItemId item = m_listTable->GetFirstChild(m_listTable->GetRootItem(), cookie);
        while(item.IsOk()) {
            wxString gdbId = DoGetGdbId(item);
            if(gdbId.IsEmpty()) {
                DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
                if(data && (data->_kind & flags)) {
                    // not a variable object entry, remove it
                    itemsToRemove.push_back(item);
                }

            } else {
                itemsNotRemoved.Add(m_listTable->GetItemText(item));
            }
            item = m_listTable->GetNextChild(m_listTable->GetRootItem(), cookie);
        }

        for(size_t i = 0; i < itemsToRemove.size(); i++) {
            oldValues[m_listTable->GetItemText(itemsToRemove[i])] = m_listTable->GetItemText(itemsToRemove[i], 1);
            m_listTable->Delete(itemsToRemove[i]);
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::DoUpdateLocals(const LocalVariables& localsUnSorted, size_t kind)
    {
        LocalVariables locals = localsUnSorted;
        wxTreeItemId root = m_listTable->GetRootItem();
        if(!root.IsOk()) return;

        IDebugger* dbgr = DoGetDebugger();
        wxArrayString itemsNotRemoved;
        // remove the non-variable objects and return a list
        // of all the variable objects (at the top level)

        std::map<wxString, wxString> oldValues;
        DoClearNonVariableObjectEntries(itemsNotRemoved, kind, oldValues);
        for(size_t i = 0; i < locals.size(); i++) {

            // try to replace the
            wxString newVarName;
            if(m_resolveLocals) { newVarName = m_preDefTypes.GetPreDefinedTypeForTypename(locals[i].type, locals[i].name); }

            // Evaluate arrays as char*?
            static wxRegEx reConstArr(wxT("(const )?[ ]*(w)?char(_t)? *[\\[0-9\\]]*"));
            wxString type = locals[i].type;
            if(m_arrayAsCharPtr && reConstArr.Matches(type)) {
                // array
                newVarName.Clear();
                newVarName << wxT("(char*)") << locals[i].name;
            }

            if(newVarName.IsEmpty() == false && !newVarName.Contains(wxT("@"))) {
                // if(newVarName.Contains(wxT("@"))) {
                //
                //	// using GDB special array print,
                //	// we need to delete this variable object and re-create it
                //	// otherwise its content wont be updated
                //	int where = itemsNotRemoved.Index(newVarName);
                //	if(where != wxNOT_FOUND)
                //		itemsNotRemoved.RemoveAt(where);
                //}

                wxTreeItemId treeItem = DoFindItemByExpression(newVarName);
                if(treeItem.IsOk()) {
                    // an item with this expression already exists, skip it
                    continue;
                }

                // replace the local with a variable object
                // but make sure we dont enter a duplicate item
                if(itemsNotRemoved.Index(newVarName) == wxNOT_FOUND) {
                    // this type has a pre-defined type, use it instead

                    // Mark this item as VariableObject so incase another call
                    // is made to this function, it wont get deleted by mistake
                    DbgTreeItemData* data = new DbgTreeItemData();
                    data->_kind = DbgTreeItemData::VariableObject;
                    wxTreeItemId item = m_listTable->AppendItem(root, newVarName, -1, -1, data);

                    m_listTable->AppendItem(item, wxT("<dummy>"));
                    m_listTable->Collapse(item);

                    if(dbgr) {
                        dbgr->CreateVariableObject(newVarName, false, m_DBG_USERR);
                        m_createVarItemId[newVarName] = item;
                    }
                }

            } else {

                if(itemsNotRemoved.Index(locals[i].name) == wxNOT_FOUND) {
                    // New entry
                    wxTreeItemId item = m_listTable->AppendItem(root, locals[i].name, -1, -1, new DbgTreeItemData());
                    // m_listTable->SetItemTextColour(item, gsgs_Style().GetMenuTextColour());

                    m_listTable->SetItemText(item, locals[i].value, 1);
                    m_listTable->SetItemText(item, locals[i].type, 2);

                    std::map<wxString, wxString>::iterator iter = oldValues.find(locals[i].name);
                    if(iter != oldValues.end() && iter->second != locals[i].value) {
                        m_listTable->SetItemTextColour(item, *wxRED, 1);
                    }

                    m_listTable->AppendItem(item, wxT("<dummy>"));
                    m_listTable->Collapse(item);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::UpdateFrameInfo()
    {
        if(gsgs_Manager()->DbgGetCurrentFrameInfo().IsValid() &&
           gsgs_Manager()->DbgGetCurrentFrameInfo() != m_curStackInfo) {
            Clear();
            m_curStackInfo = gsgs_Manager()->DbgGetCurrentFrameInfo();
            gsgs_Frame()->GetDebuggerPane()->GetDebugWindow()->AppendLine(wxString::Format(
                _("Current scope is now set to: \"%s\", depth: %d\n"), m_curStackInfo.func.c_str(), m_curStackInfo.depth));
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnRefresh(wxCommandEvent& event)
    {
        IDebugger* dbgr = DoGetDebugger();
        if(dbgr) {
            Clear();
            dbgr->QueryLocals();
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnRefreshUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(DoGetDebugger() != NULL); 
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnItemRightClick(wxTreeEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("Change_Value"), _("Change value..."), wxT(""), wxITEM_NORMAL);
        PopupMenu(&menu);
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnEditValue(wxCommandEvent& event)
    {
        wxTreeItemId selectedItem = m_listTable->GetSelection();
        if(selectedItem.IsOk() == false) return;

        wxString itemPath = GetItemPath(selectedItem);
        wxString newValue =
            wxGetTextFromUser(wxString::Format(_("Insert new value for '%s':"), itemPath.c_str()), _("Edit expression"));
        if(newValue.IsEmpty()) return;

        IDebugger* debugger = DoGetDebugger();
        if(debugger) {
            debugger->AssignValue(itemPath, newValue);

            // refresh the item
            DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(selectedItem);
            if(data && data->_gdbId.IsEmpty()) {
                m_listTable->Delete(selectedItem);
                debugger->QueryLocals();

            } else {
                debugger->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnEditValueUI(wxUpdateUIEvent& event)
    {
        IDebugger* debugger = DoGetDebugger();
        wxTreeItemId selectedItem = m_listTable->GetSelection();
        if(selectedItem.IsOk() == false) {
            event.Enable(false);

        } else {
            DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(selectedItem);
            event.Enable(debugger && data && (data->_gdbId.IsEmpty() || !data->_isFake));
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::UpdateFuncReturnValue(const wxString& retValueGdbId)
    {
        wxTreeItemId root = m_listTable->GetRootItem();
        if(!root.IsOk()) return;

        wxArrayString itemsNotRemoved;
        // remove the non-variable objects and return a list
        // of all the variable objects (at the top level)
        wxTreeItemId item = DoFindItemByExpression(wxT("Function Returned"));
        if(item.IsOk()) { DoDeleteWatch(item); }
        m_listTable->Delete(item);

        DbgTreeItemData* data = new DbgTreeItemData();
        data->_kind = DbgTreeItemData::FuncRetValue;
        data->_retValueGdbValue = retValueGdbId;

        item = m_listTable->AppendItem(root, wxT("Function Returned"), -1, -1, data);

        m_listTable->SetItemText(item, retValueGdbId, 1);
        m_listTable->SetItemText(item, wxT(""), 2);
        m_listTable->SetItemTextColour(item, *wxRED, 1);

        m_listTable->AppendItem(item, wxT("<dummy>"));
        m_listTable->Collapse(item);

        // fake an expanding
        wxTreeEvent evt;
        evt.SetItem(item);
        OnItemExpanding(evt);
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnStackSelected(Event& event)
    {
        event.Skip();
        Clear();
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && gsgs_Manager()->IsDebuggerViewVisible(DebuggerPane::LOCALS)) {
            dbgr->QueryLocals();
        }
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::OnSortItems(wxCommandEvent& event)
    {
        // Change the sorting type and refresh the view
        m_sortAsc = !m_sortAsc;
        // Update the sorting function
        SetSortingFunction();
        OnRefresh(event);
    }
    //-----------------------------------------------------------------------------
    void LocalsTable::SetSortingFunction()
    {
        // Should a be placed before b?
        TreeSortFunc func = [=](RowEntry* a, RowEntry* b) 
        {
            if(m_sortAsc) {
                return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
            } else {
                return (a->GetLabel().CmpNoCase(b->GetLabel()) > 0);
            }
        };
        m_listTable->SetSortFunction(func);
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // WatchesTable
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    WatchesTable::WatchesTable(wxWindow* parent)
        : DebuggerTreeListCtrlBase(parent)
    {
        InitTable();
        // Only sort top level items, don't sort their children
        m_listTable->AddTreeStyle(wxTR_SORT_TOP_LEVEL);

        m_DBG_USERR = DBG_USERR_WATCHTABLE;
        m_QUERY_NUM_CHILDS = QUERY_NUM_CHILDS;
        m_LIST_CHILDS = LIST_WATCH_CHILDS;
        // Should a be placed before b?
        TreeSortFunc func = [&](RowEntry* a, RowEntry* b) {
            return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
        };

        // Load the right click menu
        m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("dbg_watch_rmenu"));
        Connect(
            XRCID("del_expr"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(WatchesTable::OnDeleteWatch), NULL, this);
        Connect(XRCID("del_expr_all"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnDeleteAll),
            NULL,
            this);
        Connect(XRCID("edit_expr"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuEditExpr),
            NULL,
            this);
        Connect(XRCID("copy_value"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuCopyValue),
            NULL,
            this);
        Connect(XRCID("add_watch"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnNewWatch_Internal),
            NULL,
            this);
        Connect(XRCID("copy_both"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuCopyBoth),
            NULL,
            this);

        Connect(XRCID("watches_df_natural"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
        Connect(XRCID("watches_df_hex"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
        Connect(XRCID("watches_df_bin"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
        Connect(XRCID("watches_df_octal"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);
        Connect(XRCID("watches_df_decimal"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(WatchesTable::OnMenuDisplayFormat),
            NULL,
            this);

        // UI events
        Connect(XRCID("edit_expr"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WatchesTable::OnMenuEditExprUI), NULL, this);
        Connect(XRCID("del_expr"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WatchesTable::OnDeleteWatchUI), NULL, this);

        gsgs_Event().Connect(
            wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, gsgsEventHandler(WatchesTable::OnTypeResolveError), NULL, this);
        SetDropTarget(new WatchDropTarget(this));
    }
    //-----------------------------------------------------------------------------
    WatchesTable::~WatchesTable()
    {
        gsgs_Event().Connect(
            wxEVT_DEBUGGER_TYPE_RESOLVE_ERROR, gsgsEventHandler(WatchesTable::OnTypeResolveError), NULL, this);

        if (m_rclickMenu) {
            delete m_rclickMenu;
            m_rclickMenu = NULL;
        }

        Clear();
        m_listTable->DeleteAllItems();
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::Clear()
    {
        wxTreeItemId root = m_listTable->GetRootItem();
        if (root.IsOk()) {
            if (m_listTable->HasChildren(root)) {
                m_listTable->DeleteChildren(root);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::InitTable()
    {
        m_listTable->AddRoot("Watches");
        m_listTable->AddHeader(_("Expression"));
        m_listTable->AddHeader(_("Value"));
        m_listTable->AddHeader(_("Type"));
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnListKeyDown(wxTreeEvent& event)
    {
        if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {

            wxTreeItemId item = m_listTable->GetSelection();
            if (item.IsOk() == false) return;

            DoDeleteWatch(item);
            m_listTable->Delete(item);

        }
        else {
            event.Skip();
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnItemRightClick(wxTreeEvent& event)
    {
        wxUnusedVar(event);
        if (m_rclickMenu) {
            PopupMenu(m_rclickMenu);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnNewWatch(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxString expr = wxGetTextFromUser(_("Expression to watch:"), _("New watch"));
        if (expr.IsEmpty() == false) {
            AddExpression(expr);
            RefreshValues(false);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnNewWatchUI(wxUpdateUIEvent& event)
    {
        event.Enable(true);
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnDeleteAll(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        Clear();
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnDeleteAllUI(wxUpdateUIEvent& event)
    {
        wxTreeItemId root = m_listTable->GetRootItem();

        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_listTable->GetFirstChild(root, cookie);

        event.Enable(root.IsOk() && child.IsOk());
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnDeleteWatch(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = m_listTable->GetSelection();
        if (item.IsOk() == false) return;

        DoDeleteWatch(item);
        m_listTable->Delete(item);
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnDeleteWatchUI(wxUpdateUIEvent& event)
    {
        wxTreeItemId item = m_listTable->GetSelection();
        event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::AddExpression(const wxString& expr)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (dbgr && gsgs_Manager()->DbgCanInteract()) {
            dbgr->ResolveType(expr, DBG_USERR_WATCHTABLE);

        }
        else {
            DebuggerEventData dummy;
            dummy.m_expression = expr;
            dummy.m_evaluated = expr;
            OnTypeResolved(dummy);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::UpdateVariableObjects()
    {
        IDebugger *debugger = DebuggerMgr::Get().GetActiveDebugger();
        CHECK_PTR_RET(debugger);

        wxTreeItemId root = m_listTable->GetRootItem();
        wxTreeItemIdValue cookieOne;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {
            wxString gdbID = DoGetGdbId(item);
            if (!gdbID.IsEmpty()) {
                debugger->UpdateWatch(gdbID);
            }
            item = m_listTable->GetNextChild(root, cookieOne);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::RefreshValues(bool repositionEditor)
    {
        // indicate in the global manager if we want to actually reposition the editor's position after the
        // dbgr->QueryFileLine() refresh
        gsgs_Manager()->SetRepositionEditor(repositionEditor);

        // loop over the childrens, if we got a valid variable object, re-evaluate it
        wxTreeItemId root = m_listTable->GetRootItem();
        if (root.IsOk() == false) return;

        // Obtain the debugger and make sure that we can interact with it
        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr) return;

        // rese tree items colour to black
        DoResetItemColour(root, 0);

        // Send command to update all variable objects of this tree
        UpdateVariableObjects();

        // Loop over the top level entries and search for items that has no gdbId
        // for those items, create a variable object
        wxTreeItemIdValue cookieOne;
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {

            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
            if (data && data->_gdbId.IsEmpty()) {
                dbgr->CreateVariableObject(m_listTable->GetItemText(item), false, m_DBG_USERR);
                m_createVarItemId[m_listTable->GetItemText(item)] = item;
            }

            item = m_listTable->GetNextChild(root, cookieOne);
        }
    }
    //-----------------------------------------------------------------------------
    wxArrayString WatchesTable::GetExpressions()
    {
        wxArrayString expressions;

        wxTreeItemId root = m_listTable->GetRootItem();
        if (root.IsOk() && m_listTable->HasChildren(root)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);
            while (item.IsOk()) {
                expressions.Add(m_listTable->GetItemText(item));
                item = m_listTable->GetNextChild(root, cookie);
            }
        }

        return expressions;
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnMenuEditExpr(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        wxTreeItemId item = m_listTable->GetSelection();
        if (item.IsOk() == false) return;

        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr) {
            return;
        }

        wxString newExpr = wxGetTextFromUser(_("Update expression:"), _("Update Watch"), m_listTable->GetItemText(item));
        if (newExpr.IsEmpty()) return;

        DoUpdateExpression(item, newExpr);
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::DoUpdateExpression(const wxTreeItemId& item, const wxString& newExpr)
    {
        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr)
        {
            return;
        }

        DoDeleteWatch(item);
        m_listTable->Delete(item);
        AddExpression(newExpr);
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnMenuCopyBoth(wxCommandEvent& event)
    {
        wxTreeItemId item = m_listTable->GetSelection();
        if (item.IsOk()) {
            CopyToClipboard(m_listTable->GetItemText(item) + wxT(" ") + m_listTable->GetItemText(item, 1));
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnMenuCopyValue(wxCommandEvent& event)
    {
        wxTreeItemId item = m_listTable->GetSelection();
        if (item.IsOk()) {
            CopyToClipboard(m_listTable->GetItemText(item, 1));
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnNewWatch_Internal(wxCommandEvent& event)
    {
        wxString expr = event.GetString();
        if (expr.empty() == false) {
            AddExpression(expr);
            RefreshValues(false);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnCreateVariableObject(const DebuggerEventData& event)
    {
        wxString expr = event.m_expression;
        IDebugger* dbgr = DoGetDebugger();
        std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
        if (iter != m_createVarItemId.end()) {
            wxTreeItemId item = iter->second;
            m_createVarItemId.erase(iter);

            if (event.m_variableObject.gdbId.IsEmpty() == false) {
                // set the variable object
                DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
                if (data) {
                    data->_gdbId = event.m_variableObject.gdbId;

                    // set the type
                    m_listTable->SetItemText(item, event.m_variableObject.typeName, 2);
                    if (dbgr->GetDebuggerInformation().defaultHexDisplay == true)
                        dbgr->SetVariableObbjectDisplayFormat(DoGetGdbId(item), DBG_DF_HEXADECIMAL);
                    // refresh this item only
                    if (dbgr) DoRefreshItem(dbgr, item, true);

                    // Query the debugger to see if this node has a children
                    // In case it does, we add a dummy node so we will get the [+] sign
                    dbgr->ListChildren(data->_gdbId, m_QUERY_NUM_CHILDS);
                    m_listChildItemId[data->_gdbId] = item;
                }

            }
            else {

                if (dbgr) {
                    // the refresh will create the item again if it has no gdbId
                    DoRefreshItem(dbgr, item, true);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnListChildren(const DebuggerEventData& event)
    {
        wxString gdbId = event.m_expression;
        std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
        if (iter == m_listChildItemId.end()) return;

        wxTreeItemId item = iter->second;
        m_listChildItemId.erase(iter);

        if (event.m_userReason == m_QUERY_NUM_CHILDS) {
            if (event.m_varObjChildren.empty() == false) m_listTable->AppendItem(item, wxT("<dummy>"));

        }
        else if (event.m_userReason == m_LIST_CHILDS) {
            if (event.m_varObjChildren.empty() == false) {
                for (size_t i = 0; i < event.m_varObjChildren.size(); i++) {
                    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
                    if (!dbgr || !gsgs_Manager()->DbgCanInteract()) return;

                    VariableObjChild ch = event.m_varObjChildren.at(i);
                    if (ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
                        // not really a node...
                        // ask for information about this node children
                        dbgr->ListChildren(ch.gdbId, m_LIST_CHILDS);
                        m_listChildItemId[ch.gdbId] = item;

                    }
                    else {

                        DbgTreeItemData* data = new DbgTreeItemData();
                        data->_gdbId = ch.gdbId;
                        wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);
                        m_listTable->SetItemText(child, ch.type, 2);

                        // Add a dummy node
                        if (child.IsOk() && ch.numChilds > 0) {
                            m_listTable->AppendItem(child, wxT("<dummy>"));
                        }

                        // refresh this item only
                        dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
                        // ask the value for this node
                        m_gdbIdToTreeId[data->_gdbId] = child;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnItemExpanding(wxTreeEvent& event)
    {
        wxBusyCursor bc;
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr) {
            // dont allow the expansion of this item
            event.Veto();
            return;
        }

        if (child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {
            // a dummy node, replace it with the real node content
            m_listTable->Delete(child);

            DbgTreeItemData* data = (DbgTreeItemData*)m_listTable->GetItemData(event.GetItem());
            if (data) {
                if (data->_gdbId.IsEmpty() == false) {
                    dbgr->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
                    dbgr->ListChildren(data->_gdbId, m_LIST_CHILDS);
                    m_listChildItemId[data->_gdbId] = event.GetItem();
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnMenuEditExprUI(wxUpdateUIEvent& event)
    {
        // we enable it for root elements only
        wxTreeItemId item = m_listTable->GetSelection();
        event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnListEditLabelBegin(wxTreeEvent& event)
    {
        if (event.GetItem().IsOk()) {
            if (m_listTable->GetItemParent(event.GetItem()) != m_listTable->GetRootItem()) {
                event.Veto();
                return;
            }
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnListEditLabelEnd(wxTreeEvent& event)
    {
        if (event.GetItem().IsOk()) {
            DoUpdateExpression(event.GetItem(), event.GetLabel());
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnMenuDisplayFormat(wxCommandEvent& event)
    {
        DisplayFormat df = DBG_DF_NATURAL;
        if (event.GetId() == XRCID("watches_df_natural"))
        {
            df = DBG_DF_NATURAL;
        }
        else if (event.GetId() == XRCID("watches_df_hex"))
        {
            df = DBG_DF_HEXADECIMAL;
        }
        else if (event.GetId() == XRCID("watches_df_bin"))
        {
            df = DBG_DF_BINARY;
        }
        else if (event.GetId() == XRCID("watches_df_octal"))
        {
            df = DBG_DF_OCTAL;
        }
        else if (event.GetId() == XRCID("watches_df_decimal"))
        {
            df = DBG_DF_DECIMAL;
        }
        wxArrayTreeItemIds arr;
        m_listTable->GetSelections(arr);
        IDebugger* dbgr = DoGetDebugger();
        if (!dbgr) return;
        for (size_t i = 0; i < arr.GetCount(); i++) {
            wxTreeItemId item = arr.Item(i);
            if (!item.IsOk()) return;
            wxString gdbId = DoGetGdbId(item);
            if (gdbId.IsEmpty() == false) {
                dbgr->SetVariableObbjectDisplayFormat(gdbId, df);
                dbgr->UpdateVariableObject(gdbId, m_DBG_USERR);
                DoRefreshItem(dbgr, item, true);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnUpdateVariableObject(const DebuggerEventData& event)
    {
        if (event.m_varObjUpdateInfo.refreshIds.IsEmpty()) return;

        wxArrayString itemsToRefresh = event.m_varObjUpdateInfo.refreshIds;
        IDebugger* dbgr = DoGetDebugger();
        if (dbgr) {
            DoRefreshItemRecursively(dbgr, m_listTable->GetRootItem(), itemsToRefresh);
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnTypeResolved(const DebuggerEventData& event)
    {
        wxString expr = ::DbgPrependCharPtrCastIfNeeded(event.m_expression, event.m_evaluated);

        // make sure that the expression does not exist
        wxTreeItemId root = m_listTable->GetRootItem();
        if (!root.IsOk()) {
            return;
        }

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (!dbgr) {
            return;
        }

        // Obtain the debugger and make sure that we can interact with it
        if (!gsgs_Manager()->DbgCanInteract()) return;

        // Append the new item and call the debugger to create a new variable object for this
        // expression
        wxTreeItemId item = m_listTable->AppendItem(root, expr, -1, -1, new DbgTreeItemData());
        dbgr->CreateVariableObject(expr, false, m_DBG_USERR);
        m_createVarItemId[expr] = item;
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnRefresh(wxCommandEvent& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if (!dbgr || !gsgs_Manager()->DbgCanInteract()) {
            return;
        }

        // First delete all variable objects
        wxArrayString watches;

        // Loop over the top level entries and search for items that has no gdbId
        // for those items, create a variable object
        wxTreeItemIdValue cookieOne;
        wxTreeItemId root = m_listTable->GetRootItem();
        wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
        while (item.IsOk()) {

            watches.Add(m_listTable->GetItemText(item));
            DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(item));
            if (data && data->_gdbId.IsEmpty()) {
                dbgr->DeleteVariableObject(data->_gdbId);
            }
            item = m_listTable->GetNextChild(root, cookieOne);
        }
        m_createVarItemId.clear();
        Clear();
        for (size_t i = 0; i < watches.GetCount(); ++i) {
            AddExpression(watches.Item(i));
        }
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnRefreshUI(wxUpdateUIEvent& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        event.Enable(dbgr && gsgs_Manager()->DbgCanInteract());
    }
    //-----------------------------------------------------------------------------
    void WatchesTable::OnTypeResolveError(Event& event)
    {
        DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(event.GetClientObject());
        if (ded && ded->m_userReason == m_DBG_USERR) {
            // this event was meant for us
            // could not resolve type
            wxTreeItemId id = DoFindItemByExpression(ded->m_expression);
            if (id.IsOk()) {
                // update
                m_listTable->SetItemText(id, ded->m_text, 1);
            }
            else {
                // add
                wxTreeItemId child = m_listTable->AppendItem(m_listTable->GetRootItem(), ded->m_expression);
                m_listTable->SetItemText(child, ded->m_text, 1);
            }
        }
        else {
            event.Skip();
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerAsciiViewer
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    static void sDefineMarker(wxStyledTextCtrl *s, int marker, int markerType, wxColor fore, wxColor back)
    {
        s->MarkerDefine(marker, markerType);
        s->MarkerSetForeground(marker, fore);
        s->MarkerSetBackground(marker, back);
    }
    //-----------------------------------------------------------------------------
    DebuggerAsciiViewer::DebuggerAsciiViewer( wxWindow* parent , wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : 
        wxPanel( parent, id, pos, size, style )
    {
        wxBoxSizer* mainSizer;
        mainSizer = new wxBoxSizer( wxVERTICAL );
        
        wxBoxSizer* bSizer2;
        bSizer2 = new wxBoxSizer( wxHORIZONTAL );
        
        m_staticText1 = gsgs_Style().createStaticText( this, wxID_ANY, _("Displaying:"), wxDefaultPosition, wxDefaultSize, 0 );
        m_staticText1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
        
        bSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
        
        m_textCtrlExpression = gsgs_Style().createTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
        
        bSizer2->Add( m_textCtrlExpression, 1, wxALL|wxEXPAND, 5 );
        
        m_buttonClear = new Button( this, wxID_ANY, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
        m_buttonClear->SetDefault(); 
        bSizer2->Add( m_buttonClear, 0, wxALL, 5 );
        
        mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
        
        m_textView = new wxStyledTextCtrl(this);
        mainSizer->Add( m_textView, 1, wxALL|wxEXPAND, 5 );
        
        this->SetSizer( mainSizer );
        //this->Layout();
        
        // Connect Events
        m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerAsciiViewer::OnClearView ), NULL, this );
        
        gsgs_Event().Connect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(DebuggerAsciiViewer::OnThemeColourChanged), NULL, this);
        LexerStyle::Ptr_t cpp_lexer = gsgs_EditorConfig().GetLexer("C++");
        if ( cpp_lexer ) {
            cpp_lexer->Apply( m_textView );
            m_textView->SetLexer(wxSTC_LEX_CPP);
            
        } else {
            // This code should not be called at all...
            // but still - just in case
            wxFont font(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

            // hide all margins
            m_textView->SetMarginWidth(0, 0);
            m_textView->SetMarginWidth(1, 0);
            m_textView->SetMarginWidth(2, 0);
            m_textView->SetMarginWidth(3, 0);
            m_textView->SetMarginWidth(4, 0);

            m_textView->SetMarginSensitive(4, true);
            m_textView->SetProperty(wxT("fold"), wxT("1"));

            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
            sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

            // set wrapped line indicator
            m_textView->SetWrapVisualFlags(1);
            m_textView->SetWrapStartIndent(4);
            m_textView->SetScrollWidthTracking(true);
            m_textView->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxT("GREY"));

            // Set wrap mode on
            m_textView->SetWrapMode(wxSTC_WRAP_WORD);
            // Use NULL lexer
            m_textView->SetLexer(wxSTC_LEX_CPP);
            m_textView->SetMarginMask(4, wxSTC_MASK_FOLDERS);

            // Set TELETYPE font (monospace)
            m_textView->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
            m_textView->StyleSetSize(wxSTC_STYLE_DEFAULT, 12  );
        }
        
        m_textView->SetReadOnly(true);

        wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
        wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
        wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
        wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
    }
    //-----------------------------------------------------------------------------
    bool DebuggerAsciiViewer::IsFocused()
    {
        wxWindow *win = wxWindow::FindFocus();
        return (win && win == m_textView);
    }
    //-----------------------------------------------------------------------------
    void DebuggerAsciiViewer::UpdateView(const wxString &expr, const wxString &value)
    {
        m_textCtrlExpression->SetValue(expr);

        wxString evaluated (value);
        evaluated.Replace(wxT("\r\n"), wxT("\n"));
        evaluated.Replace(wxT("\n,"), wxT(",\n"));
        evaluated.Replace(wxT("\n\n"), wxT("\n"));

        m_textView->SetReadOnly(false);
        m_textView->ClearAll();
        m_textView->SetText(evaluated);
        m_textView->SetReadOnly(true);

    }
    //-----------------------------------------------------------------------------
    void DebuggerAsciiViewer::OnClearView(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        UpdateView(wxT(""), wxT(""));
    }
    //-----------------------------------------------------------------------------
    void DebuggerAsciiViewer::OnEditUI(wxUpdateUIEvent& e)
    {
        if ( !IsFocused() ) 
        {
            e.Skip();
            return;
        }
        switch ( e.GetId() ) 
        {
        case wxID_SELECTALL:
            e.Enable(true);
            break;
        case wxID_COPY:
            e.Enable( m_textView->GetSelectedText().IsEmpty() == false );
            break;
        default:
            e.Enable(false);
            break;
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerAsciiViewer::OnEdit(wxCommandEvent& e)
    {
        if ( !IsFocused() ) 
        {
            e.Skip();
            return;
        }

        switch ( e.GetId() ) 
        {
        case wxID_SELECTALL:
            m_textView->SelectAll();
            break;
        case wxID_COPY:
            m_textView->Copy();
            break;
        default:
            break;
        }
    }
    //-----------------------------------------------------------------------------
    DebuggerAsciiViewer::~DebuggerAsciiViewer()
    {
        m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerAsciiViewer::OnClearView ), NULL, this );
        wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
        wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
        wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
        wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
        gsgs_Event().Disconnect(wxEVT_GSGS_THEME_CHANGED, wxCommandEventHandler(DebuggerAsciiViewer::OnThemeColourChanged), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void DebuggerAsciiViewer::OnThemeColourChanged(wxCommandEvent & e)
    {
        e.Skip();
        
        // Re-apply C++ lexer
        LexerStyle::Ptr_t cpp_lexer = gsgs_EditorConfig().GetLexer("C++");
        if ( cpp_lexer ) 
        {
            cpp_lexer->Apply( m_textView );
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerDisassemblyTab
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    #define CURLINE_MARKER 7
    #define BREAKPOINT_MARKER 8
    #define BREAKPOINT_MARKER_MASK 1 << BREAKPOINT_MARKER
    //-----------------------------------------------------------------------------
    DebuggerDisassemblyTab::DebuggerDisassemblyTab(wxWindow* parent, const wxString& label, wxWindowID id, const wxPoint& pos,
        const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
        , m_title(label)
    {
        wxBoxSizer* boxSizer14 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer14);

        wxFlexGridSizer* flexGridSizer22 = new wxFlexGridSizer(0, 2, 0, 0);
        flexGridSizer22->SetFlexibleDirection(wxBOTH);
        flexGridSizer22->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
        flexGridSizer22->AddGrowableCol(1);

        boxSizer14->Add(flexGridSizer22, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_staticText24 = gsgs_Style().createStaticText(this, wxID_ANY, _("Current function:"), wxDefaultPosition,
            wxDLG_UNIT(this, wxSize(-1, -1)), 0);

        flexGridSizer22->Add(m_staticText24, 0, wxALL | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, WXC_FROM_DIP(5));

        m_textCtrlCurFunction = gsgs_Style().createTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)), wxTE_READONLY);

        flexGridSizer22->Add(m_textCtrlCurFunction, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

        m_splitter51 = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
            wxSP_LIVE_UPDATE | wxSP_NO_XP_THEME | wxSP_3DSASH);
        m_splitter51->SetSashGravity(0.5);
        m_splitter51->SetMinimumPaneSize(10);

        boxSizer14->Add(m_splitter51, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_splitterPage55 = new wxPanel(m_splitter51, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter51, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);

        wxBoxSizer* boxSizer61 = new wxBoxSizer(wxVERTICAL);
        m_splitterPage55->SetSizer(boxSizer61);

        m_stc = new wxStyledTextCtrl(m_splitterPage55, wxID_ANY, wxDefaultPosition,
            wxDLG_UNIT(m_splitterPage55, wxSize(-1, -1)), 0);
        // Configure the fold margin
        m_stc->SetMarginType(4, wxSTC_MARGIN_SYMBOL);
        m_stc->SetMarginMask(4, wxSTC_MASK_FOLDERS);
        m_stc->SetMarginSensitive(4, true);
        m_stc->SetMarginWidth(4, 0);

        // Configure the tracker margin
        m_stc->SetMarginWidth(1, 0);

        // Configure the symbol margin
        m_stc->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
        m_stc->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
        m_stc->SetMarginWidth(2, 16);
        m_stc->SetMarginSensitive(2, true);

        // Configure the line numbers margin
        m_stc->SetMarginType(0, wxSTC_MARGIN_NUMBER);
        m_stc->SetMarginWidth(0, 0);

        // Configure the line symbol margin
        m_stc->SetMarginType(3, wxSTC_MARGIN_FORE);
        m_stc->SetMarginMask(3, 0);
        m_stc->SetMarginWidth(3, 0);
        // Select the lexer
        m_stc->SetLexer(wxSTC_LEX_ASM);
        // Set default font / styles
        m_stc->StyleClearAll();
        m_stc->SetWrapMode(0);
        m_stc->SetIndentationGuides(0);
        m_stc->SetKeyWords(0, wxT(""));
        m_stc->SetKeyWords(1, wxT(""));
        m_stc->SetKeyWords(2, wxT(""));
        m_stc->SetKeyWords(3, wxT(""));
        m_stc->SetKeyWords(4, wxT(""));

        boxSizer61->Add(m_stc, 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));

        m_splitterPage59 = new wxPanel(m_splitter51, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter51, wxSize(-1, -1)),
            wxTAB_TRAVERSAL);
        m_splitter51->SplitVertically(m_splitterPage55, m_splitterPage59, 0);

        wxBoxSizer* boxSizer63 = new wxBoxSizer(wxVERTICAL);
        m_splitterPage59->SetSizer(boxSizer63);

        m_dvListCtrlRegisters =
            new ThemedListCtrl(m_splitterPage59, wxID_ANY, wxDefaultPosition,
                wxDLG_UNIT(m_splitterPage59, wxSize(-1, -1)), wxDV_ENABLE_SEARCH | wxDV_ROW_LINES);

        boxSizer63->Add(m_dvListCtrlRegisters, 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));

        m_dvListCtrlRegisters->AppendTextColumn(_("Register"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrlRegisters->AppendTextColumn(_("Value"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);

        SetName(wxT("DebuggerDisassemblyTab"));
        //SetSize(wxDLG_UNIT(this, wxSize(500, 300)));
        //if (GetSizer()) { GetSizer()->Fit(this); }
        // Connect events
        m_stc->Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(DebuggerDisassemblyTab::OnMarginClicked), NULL,
            this);
        m_stc->MarkerDefine(CURLINE_MARKER, wxSTC_MARK_BACKGROUND, wxNullColour,
                            gsgs_EditorConfig().GetOptions()->GetDebuggerMarkerLine());
        m_stc->MarkerSetAlpha(CURLINE_MARKER, 50);

        m_stc->SetReadOnly(true);
        m_stc->SetMarginSensitive(2, true); // 2 is the symbol margin

        m_stc->MarkerDefineBitmap(BREAKPOINT_MARKER, wxBitmap(wxImage(stop_xpm)));

        m_stc->SetYCaretPolicy(wxSTC_CARET_SLOP, 30);
        gsgs_Event().Connect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,
                                      gsgsEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
        gsgs_Event().Connect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE,
                                      gsgsEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
        gsgs_Event().Connect(wxEVT_DEBUGGER_QUERY_FILELINE,
                                      gsgsEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
        gsgs_Event().Connect(wxEVT_DEBUGGER_LIST_REGISTERS,
                                      gsgsEventHandler(DebuggerDisassemblyTab::OnShowRegisters), NULL, this);
        gsgs_Event().Bind(wxEVT_DEBUG_ENDED, &DebuggerDisassemblyTab::OnDebuggerStopped, this);
        gsgs_Event().Connect(wxEVT_GSGS_ALL_BREAKPOINTS_DELETED,
                                      wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL, this);
        gsgs_Event().Connect(wxEVT_DEBUGGER_UPDATE_VIEWS,
                                      gsgsEventHandler(DebuggerDisassemblyTab::OnRefreshView), NULL, this);
        LexerStyle::Ptr_t lex = gsgs_EditorConfig().GetLexer("Assembly");
        if(lex) { lex->Apply(m_stc, true); }
    }
    //-----------------------------------------------------------------------------
    DebuggerDisassemblyTab::~DebuggerDisassemblyTab()
    {
        m_stc->Disconnect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(DebuggerDisassemblyTab::OnMarginClicked),
            NULL, this);
        gsgs_Event().Disconnect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,
                                         gsgsEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
        gsgs_Event().Disconnect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE,
                                         gsgsEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
        gsgs_Event().Disconnect(wxEVT_DEBUGGER_QUERY_FILELINE,
                                         gsgsEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_ENDED, &DebuggerDisassemblyTab::OnDebuggerStopped, this);
        gsgs_Event().Disconnect(wxEVT_GSGS_ALL_BREAKPOINTS_DELETED,
                                         wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL,
                                         this);
        gsgs_Event().Disconnect(wxEVT_DEBUGGER_LIST_REGISTERS,
                                         gsgsEventHandler(DebuggerDisassemblyTab::OnShowRegisters), NULL, this);
        gsgs_Event().Disconnect(wxEVT_DEBUGGER_UPDATE_VIEWS,
                                         gsgsEventHandler(DebuggerDisassemblyTab::OnRefreshView), NULL, this);
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnOutput(Event& e)
    {
        e.Skip();
        DoClearDisassembleView();
        m_stc->SetReadOnly(false);

        BreakpointInfoVec_t memBps;
        gsgsStringSet addressSet;
        gsgs_Manager()->GetBreakpointsMgr()->GetAllMemoryBreakpoints(memBps);
        for(size_t i = 0; i < memBps.size(); ++i) {
            addressSet.insert(memBps.at(i).memory_address);
        }
        // Parse the output
        DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
        if(ded) {

            m_lines.insert(m_lines.end(), ded->m_disassembleLines.begin(), ded->m_disassembleLines.end());

            for(size_t i = 0; i < ded->m_disassembleLines.size(); ++i) {
                m_stc->AppendText(ded->m_disassembleLines.at(i).m_address + "  " + ded->m_disassembleLines.at(i).m_inst +
                                  "\n");
                // restore breakpoints
                if(addressSet.count(ded->m_disassembleLines.at(i).m_address)) { m_stc->MarkerAdd(i, BREAKPOINT_MARKER); }
            }
        }
        gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();
        m_stc->SetReadOnly(true);
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnCurLine(Event& e)
    {
        e.Skip();
        size_t curline = wxString::npos;
        DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
        if(ded && ded->m_disassembleLines.size()) {
            DisassembleEntry entry = ded->m_disassembleLines.at(0);
            for(size_t i = 0; i < m_lines.size(); ++i) {
                if(m_lines.at(i).m_address == entry.m_address) {
                    curline = i;
                    break;
                }
            }

            if(curline != wxString::npos) {
                m_stc->MarkerAdd(curline, CURLINE_MARKER);

                int pos = m_stc->PositionFromLine(curline);
                m_stc->SetCurrentPos(pos);
                m_stc->SetSelection(pos, pos);
                DoCentrLine(curline);
                // m_stc->EnsureCaretVisible();
                m_textCtrlCurFunction->ChangeValue(entry.m_function);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnDebuggerStopped(DebugEvent& e)
    {
        e.Skip();
        DoClearDisassembleView();
        DoClearRegistersView();
        m_oldValues.clear();
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::DoClearDisassembleView()
    {
        m_stc->SetReadOnly(false);
        m_lines.clear();
        m_stc->ClearAll();
        m_textCtrlCurFunction->ChangeValue("");
        m_stc->SetReadOnly(true);
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnQueryFileLineDone(Event& e) 
    { 
        e.Skip(); 
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::DoCentrLine(int line)
    {
        // we want to make 'first' centered
        int numLinesOnScreen = m_stc->LinesOnScreen();
        int linesAboveIt = numLinesOnScreen / 2;

        line = line - linesAboveIt;
        if(line < 0) line = 0;

        m_stc->SetFirstVisibleLine(line);
        m_stc->ClearSelections();
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnMarginClicked(wxStyledTextEvent& event)
    {
        /// get the address of the line
        int nLine = m_stc->LineFromPosition(event.GetPosition());
        wxString line = m_stc->GetLine(nLine);
        wxString address = line.BeforeFirst(' ').Trim(true).Trim(false);

        if(m_stc->MarkerGet(nLine) & BREAKPOINT_MARKER_MASK) {

            // we already got a marker there
            m_stc->MarkerDelete(nLine, BREAKPOINT_MARKER);
            gsgs_Manager()->GetBreakpointsMgr()->DelBreakpointByAddress(address);

        } else {
            m_stc->MarkerAdd(nLine, BREAKPOINT_MARKER);
            gsgs_Manager()->GetBreakpointsMgr()->AddBreakpointByAddress(address);
        }
        gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnAllBreakpointsDeleted(wxCommandEvent& e)
    {
        e.Skip();
        /// all breakpoints were deleted, update the UI
        m_stc->MarkerDeleteAll(BREAKPOINT_MARKER);
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnShowRegisters(Event& e)
    {
        e.Skip();
        DoClearRegistersView();
        DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
        gsgsStringMap curvalues;
        if(ded) {
            // Split the list into 2
            size_t i = 0;
            while(i < ded->m_registers.size()) {
                RegistersViewModelClientData* cd = new RegistersViewModelClientData(false, false);

                wxVector<wxVariant> cols;
                cols.push_back(ded->m_registers.at(i).reg_name);
                cols.push_back(ded->m_registers.at(i).reg_value);
                if(!m_oldValues.empty()) {
                    gsgsStringMap::iterator iter = m_oldValues.find(ded->m_registers.at(i).reg_name);
                    if(iter != m_oldValues.end() && iter->second != ded->m_registers.at(i).reg_value) {
                        cd->SetFirstColModified(true);
                    }
                }
                curvalues.insert(std::make_pair(ded->m_registers.at(i).reg_name, ded->m_registers.at(i).reg_value));
                m_dvListCtrlRegisters->AppendItem(cols, (wxUIntPtr)cd);
                ++i;
            }
        }
        m_oldValues.swap(curvalues);
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::OnRefreshView(Event& e)
    {
        e.Skip();
        IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
        if(debugger && debugger->IsRunning() && gsgs_Manager()->DbgCanInteract()) {
            // Only update disass view if the view is visible
            if(gsgs_Manager()->IsDebuggerViewVisible(DebuggerPane::DISASSEMBLY)) {
                debugger->ListRegisters();
                debugger->Disassemble("", -1);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerDisassemblyTab::DoClearRegistersView()
    {
        m_dvListCtrlRegisters->DeleteAllItems([&](wxUIntPtr pData) {
            RegistersViewModelClientData* cd = reinterpret_cast<RegistersViewModelClientData*>(pData);
            wxDELETE(cd);
        });
    }
    //-----------------------------------------------------------------------------
    wxArrayString DebuggerDisassemblyTab::GetRegisterNames() const
    {
        wxArrayString names;
        for(size_t i = 0; i < m_dvListCtrlRegisters->GetItemCount(); ++i) {
            names.Add(m_dvListCtrlRegisters->GetItemText(m_dvListCtrlRegisters->RowToItem(i)));
        }
        return names;
    }
    
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // DebuggerPaneConfig
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    const wxString DebuggerPane::LOCALS         = _("LocalValue");
    const wxString DebuggerPane::WATCHES        = _("Watches");
    const wxString DebuggerPane::FRAMES         = _("CallStack");
    const wxString DebuggerPane::BREAKPOINT     = _("BreakPoint");
    const wxString DebuggerPane::Object         = _("Object");
    const wxString DebuggerPane::AnimationObject = _("AnimationObject");
    const wxString DebuggerPane::THREAD        = _("Thread");
    const wxString DebuggerPane::MEMORY         = _("Memory");
    const wxString DebuggerPane::ASCII_VIEWER   = _("Ascii");
    const wxString DebuggerPane::DEBUGGER_OUTPUT = _("Output");
    const wxString DebuggerPane::DISASSEMBLY    = _("Disassemble");
    const wxString DebuggerPane::Animation     = _("Animation");
    const wxString DebuggerPane::TimeLine       = _("TimeLine");
    const wxString DebuggerPane::VBO = _("VBO");
    const wxString DebuggerPane::FBO = _("FBO");

    #define IS_DETACHED(name) (detachedPanes.Index(name) != wxNOT_FOUND) ? true : false

    //-----------------------------------------------------------------------------
    DebuggerPaneConfig::DebuggerPaneConfig()
        : ObjectData("debuggerWindows")
        , m_windows(All)
    {
    }
    //-----------------------------------------------------------------------------
    DebuggerPaneConfig::~DebuggerPaneConfig() 
    {
    }
    //-----------------------------------------------------------------------------
    void DebuggerPaneConfig::FromJSON(const JSONObject& json)
    {
        json.getObject("m_windows").getValue(m_windows, All);
    }
    //-----------------------------------------------------------------------------
    JSONObject DebuggerPaneConfig::ToJSON() const
    {
        JSONObject e = JSONObject::createObject(GetName());
        e.addProperty("m_windows", m_windows);
        return e;
    }
    //-----------------------------------------------------------------------------
    wxString DebuggerPaneConfig::WindowName(eDebuggerWindows flag) const
    {
        switch (flag) 
        {
        default:
        case All:
            return wxEmptyString;
        case Locals:
            return wxGetTranslation(DebuggerPane::LOCALS);
        case Watches:
            return wxGetTranslation(DebuggerPane::WATCHES);
        case Thread:
            return wxGetTranslation(DebuggerPane::THREAD);
        case Callstack:
            return wxGetTranslation(DebuggerPane::FRAMES);
        case Breakpoint:
            return wxGetTranslation(DebuggerPane::BREAKPOINT);
        case Memory:
            return wxGetTranslation(DebuggerPane::MEMORY);
        case AsciiViewer:
            return wxGetTranslation(DebuggerPane::ASCII_VIEWER);
        case Output:
            return wxGetTranslation(DebuggerPane::DEBUGGER_OUTPUT);
        case Disassemble:
            return wxGetTranslation(DebuggerPane::DISASSEMBLY);
        }
    }
    //-----------------------------------------------------------------------------
    DebuggerPane::DebuggerPane(wxWindow* parent, const wxString& caption, wxAuiManager* mgr)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 300))
        , m_caption(caption)
        , m_initDone(false)
        , m_mgr(mgr)
    {
        wxWindowUpdateLocker temp(this);
        CreateViewImpl();
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &DebuggerPane::OnSettingsChanged, this);
    }
    //-----------------------------------------------------------------------------
    DebuggerPane::~DebuggerPane()
    {
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &DebuggerPane::OnSettingsChanged, this);
    }
    //-----------------------------------------------------------------------------
    void DebuggerPane::OnPageChanged(wxBookCtrlEvent& event)
    {
        if(m_initDone && DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning()) 
        {
            if(event.GetEventObject() == m_book) 
            {
                gsgs_Manager()->CallAfter(&Manager::UpdateDebuggerPane);
            } 
            else 
            {
                event.Skip();
            }
        } 
        else 
        {
            event.Skip();
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerPane::CreateViewImpl()
    {
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(mainSizer);

        long style = (kNotebook_Default | kNotebook_AllowDnD | kNotebook_UnderlineActiveTab);
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;
        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) 
        {
            style &= ~kNotebook_LightTabs;
            style |= kNotebook_DarkTabs;
        }
        if(gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs()) 
        { 
            style |= kNotebook_MouseScrollSwitchTabs; 
        }
        GeneralImages img;
        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, style);
        m_book->SetTabDirection(gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection());
        mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 0);

        // Calculate the widthest tab (the one with the 'Call Stack' label)
        /*int xx, yy;
        wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        wxWindow::GetTextExtent(_("Breakpoint"), &xx, &yy, NULL, NULL, &fnt);
        */
        // load list of detached panes
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject(wxT("DetachedPanesList"), &dpi);
        wxArrayString detachedPanes = dpi.GetPanes();

        wxString name = wxGetTranslation(LOCALS);
        wxBitmap bmp = wxNullBitmap;
        // Add the 'Locals View'
        if(IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_localsTable = new LocalsTable(cp);
            cp->SetChildNoReparent(m_localsTable);
        } 
        else 
        {
            m_localsTable = new LocalsTable(m_book);
            m_book->AddPage(m_localsTable, name, false, bmp);
        }

        // Add the 'Call Stack'
        name = wxGetTranslation(FRAMES);
        bmp = wxNullBitmap;
        if(IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_frameList = new DebuggerCallstackView(cp);
            cp->SetChildNoReparent(m_frameList);
        } 
        else 
        {
            m_frameList = new DebuggerCallstackView(m_book);
            m_book->AddPage(m_frameList, name, false, bmp);
        }

        // Add the 'watches View'
        /*name = wxGetTranslation(WATCHES);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_watchesTable = new WatchesTable(cp);
            cp->SetChildNoReparent(m_watchesTable);
        }
        else 
        {
            m_watchesTable = new WatchesTable(m_book);
            m_book->AddPage(m_watchesTable, name, false, bmp);
        }
        */
        // Add the 'Breakpoint'
        name = wxGetTranslation(BREAKPOINT);
        bmp = wxNullBitmap;
        if(IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_breakpoints = new BreakpointDlg(cp);
            cp->SetChildNoReparent(m_breakpoints);
        } 
        else 
        {
            m_breakpoints = new BreakpointDlg(m_book);
            m_book->AddPage(m_breakpoints, name, false, bmp);
        }

        // Add the 'Object'
        name = wxGetTranslation(Object);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'AnimationObject'
        name = wxGetTranslation(AnimationObject);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the "Output" tab
        name = wxGetTranslation(DEBUGGER_OUTPUT);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_outputDebug = new DebugTab(cp, wxID_ANY, wxGetTranslation(DEBUGGER_OUTPUT));
            cp->SetChildNoReparent(m_outputDebug);
        }
        else 
        {
            m_outputDebug = new DebugTab(m_book, wxID_ANY, wxGetTranslation(DEBUGGER_OUTPUT));
            m_book->AddPage(m_outputDebug, name, false, bmp);
        }

        // Add the 'Thread'
        name = wxGetTranslation(THREAD);
        bmp = wxNullBitmap;
        if(IS_DETACHED(name)) 
        {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_threads = new ThreadListPanel(cp);
            cp->SetChildNoReparent(m_threads);
        } 
        else 
        {
            m_threads = new ThreadListPanel(m_book);
            m_book->AddPage(m_threads, name, false, bmp);
        }

        // Add the 'Memory View'
        name = wxGetTranslation(MEMORY);
        bmp = wxNullBitmap;
        if(IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        } else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'Animation'
        name = wxGetTranslation(Animation);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'TimeLine'
        name = wxGetTranslation(TimeLine);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'VBO'
        name = wxGetTranslation(VBO);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'FBO'
        name = wxGetTranslation(FBO);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_memory = new MemoryView(cp);
            cp->SetChildNoReparent(m_memory);

        }
        else {
            m_memory = new MemoryView(m_book);
            m_book->AddPage(m_memory, name, false, bmp);
        }

        // Add the 'ASCII Viewer'
        /*name = wxGetTranslation(ASCII_VIEWER);
        bmp = wxNullBitmap;
        if (IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_asciiViewer = new DebuggerAsciiViewer(cp);
            cp->SetChildNoReparent(m_asciiViewer);

        }
        else {
            m_asciiViewer = new DebuggerAsciiViewer(m_book);
            m_book->AddPage(m_asciiViewer, name, false, bmp);
        }
        */
        // Add the "disassembly" tab
        /*name = wxGetTranslation(DISASSEMBLY);
        bmp = wxNullBitmap;
        if(IS_DETACHED(name)) {
            DockablePane* cp = new DockablePane(GetParent(), m_book, name, false, bmp, wxSize(200, 200));
            m_disassemble = new DebuggerDisassemblyTab(cp, wxGetTranslation(DISASSEMBLY));
            cp->SetChildNoReparent(m_disassemble);

        } else {
            m_disassemble = new DebuggerDisassemblyTab(m_book, wxGetTranslation(DISASSEMBLY));
            m_book->AddPage(m_disassemble, name, false, bmp);
        }
        */
        m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &DebuggerPane::OnPageChanged, this);
        m_initDone = true;
    }
    //-----------------------------------------------------------------------------
    void DebuggerPane::SelectTab(const wxString& tabName)
    {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) 
        {
            if(m_book->GetPageText(i) == tabName) 
            {
                m_book->SetSelection(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------------
    void DebuggerPane::Clear()
    {
        GetLocalsTable()->Clear();
        GetWatchesTable()->Clear();
        GetFrameListView()->Clear();
        GetThreadsView()->Clear();
        GetMemoryView()->Clear();
    }
    //-----------------------------------------------------------------------------
    void DebuggerPane::OnSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        long style = 0;
        wxDirection tempdir = gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection();
        if (tempdir == wxBOTTOM)
            style |= kNotebook_BottomTabs;
        else if (tempdir == wxRIGHT)
            style |= kNotebook_RightTabs;
        else if (tempdir == wxLEFT)
            style |= kNotebook_LeftTabs;
        //m_book->EnableStyle(kNotebook_BottomTabs, gsgs_EditorConfig().GetOptions()->GetOutputTabsDirection() == wxBOTTOM);
        if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) 
        {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_LightTabs) | kNotebook_DarkTabs | style);
        } 
        else 
        {
            m_book->SetStyle((m_book->GetStyle() & ~kNotebook_DarkTabs) | kNotebook_LightTabs | style);
        }
    }
    //-----------------------------------------------------------------------------
}