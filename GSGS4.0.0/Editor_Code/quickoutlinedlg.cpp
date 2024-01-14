
#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif // WX_PRECOMP

#include "gsgsPreProcess.h"
#include "gsgsEditor.h"
#include "gsgsStyleManager.h"
#include "gsgsPluginManager.h"
#include "gsgsImageManager.h"
#include "gsgsFrame.h"
#include "gsgsManager.h"
#include "quickoutlinedlg.h"
#include "globals.h"
#include <wx/imaglist.h>
#include <wx/xrc/xmlres.h>

const wxEventType wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED = wxNewEventType();

namespace gsgs
{
    //-----------------------------------------------------------------
    IMPLEMENT_DYNAMIC_CLASS(CppSymbolTree, SymbolTree)
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // CppSymbolTree
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    CppSymbolTree::CppSymbolTree() {}
    //-----------------------------------------------------------------
    CppSymbolTree::CppSymbolTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : SymbolTree(parent, id, pos, size, style)
    {
        Bind(wxEVT_TREE_ITEM_ACTIVATED, &CppSymbolTree::OnItemActivated, this);
        MSWSetNativeTheme(this);
    }
    //-----------------------------------------------------------------
    bool CppSymbolTree::DoItemActivated(wxTreeItemId item, wxEvent& event, bool notify)
    {
        //-----------------------------------------------------
        // Each tree items, keeps a private user data that
        // holds the key for searching the its corresponding
        // node in the m_tree data structure
        //-----------------------------------------------------
        if(item.IsOk() == false) return false;

        MyTreeItemData* itemData = static_cast<MyTreeItemData*>(GetItemData(item));
        if(!itemData) {
            event.Skip();
            return false;
        }

        wxString filename = itemData->GetFileName();
        wxString project = gsgs_Manager()->GetProjectNameByFile(filename);
        wxString pattern = itemData->GetPattern();
        int lineno = itemData->GetLine();

        // Open the file and set the cursor to line number
        if(gsgs_Frame()->GetMainBook()->OpenFile(filename, project, lineno - 1)) {
            // get the editor, and search for the pattern in the file
            Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
            if(editor) { FindAndSelect(editor, pattern, GetItemText(item)); }
        }

        // post an event that an item was activated
        if(notify) {
            wxCommandEvent e(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED);
            e.SetEventObject(this);
            wxPostEvent(GetEventHandler(), e);
        }
        return true;
    }
    //-----------------------------------------------------------------
    void CppSymbolTree::OnItemActivated(wxTreeEvent& event) 
    { 
        DoItemActivated(event.GetItem(), event, true); 
    }
    //-----------------------------------------------------------------
    void CppSymbolTree::FindAndSelect(Editor* editor, wxString& pattern, const wxString& name)
    {
        editor->FindAndSelectV(pattern, name);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // QuickOutlineDlg
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    QuickOutlineDlg::QuickOutlineDlg(wxWindow* parent, const wxString& fileName, int id, const wxString & title, 
            const wxPoint & pos, const wxSize & size, int style)
        : wxDialog(parent, id, title, pos, size, style)
        , m_fileName(fileName)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        SetSizer(sizer);

        //wxPanel* mainPanel = new wxPanel(this);
        //sizer->Add(mainPanel, 1, wxEXPAND);

        //wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        //mainPanel->SetSizer(mainSizer);

        // build the outline view
        m_treeOutline = new CppSymbolTree(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_ROW_LINES | wxTR_HIDE_ROOT);
        //m_treeOutline->Bind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
        m_treeOutline->Bind(wxEVT_CHAR_HOOK, &QuickOutlineDlg::OnCharHook, this);
        m_treeOutline->SetBitmaps(&gsgs_Image().GetBitmaps());

        Connect(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED, wxCommandEventHandler(QuickOutlineDlg::OnItemSelected), NULL, this);
        sizer->Add(m_treeOutline, 1, wxEXPAND);
    
        Button * m_buttonClose = new Button(this, wxID_CANCEL, _("&Close"), wxDefaultPosition, wxDefaultSize, 0);
        sizer->Add(m_buttonClose, 0, wxEXPAND, 5);

        CallAfter(&QuickOutlineDlg::DoParseActiveBuffer);
        CenterOnParent();
        //::SetDialogBestSizeAndPosition(this);
    }
    //-----------------------------------------------------------------
    QuickOutlineDlg::~QuickOutlineDlg() 
    { 
        //m_treeOutline->Unbind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
        m_treeOutline->Unbind(wxEVT_CHAR_HOOK, &QuickOutlineDlg::OnCharHook, this);
    }
    //-----------------------------------------------------------------
    void QuickOutlineDlg::OnItemSelected(wxCommandEvent&)
    {
        Close();
    }
    //-----------------------------------------------------------------
    /*void QuickOutlineDlg::OnKeyDown(wxKeyEvent& e)
    {
        e.Skip();
        if(e.GetKeyCode() == WXK_ESCAPE) { Close(); }
    }*/
    //-----------------------------------------------------------------
    void QuickOutlineDlg::OnCharHook(wxKeyEvent& e)
    {
        if (e.GetKeyCode() == WXK_ESCAPE) 
        { 
            Close(); 
        }
        else
        {
            e.Skip();
        }
    }
    //-----------------------------------------------------------------
    void QuickOutlineDlg::DoParseActiveBuffer()
    {
        IEditor* editor = GetManager()->GetActiveEditor();
        if (editor)
        {
            //wxString filename = editor->GetFileName().GetFullPath();
            TagEntryPtrList tags;
            if (!TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags)) {
                // Parse and update the cache
                tags = TagsManagerST::Get()->ParseBuffer(editor->GetCtrl()->GetText(), editor->GetFileName().GetFullPath());
                TagsManagerST::Get()->GetFileCache()->Update(editor->GetFileName(), tags);
            }
            m_treeOutline->BuildTree(m_fileName, tags);
            m_treeOutline->ExpandAll();
            wxTreeItemId selectItem = m_treeOutline->GetNextItem(m_treeOutline->GetRootItem());
            m_treeOutline->SelectItem(selectItem);
            m_treeOutline->CallAfter(&CppSymbolTree::SetFocus);
        }
    }
    //-----------------------------------------------------------------
}
