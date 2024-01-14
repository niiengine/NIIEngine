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

#include "gsgsCommonPanel.h"
#include "gsgsTreeCtrl.h"
#include "gsgsCommonDialog.h"
#include "globals.h"
#include <wx/dataview.h>
#include "gsgsStyleManager.h"
#include "gsgsWindowManager.h"
#include "globals.h"
#include "imanager.h"
#include "plugin.h"
#include <wx/app.h>

namespace gsgs
{
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // TableWithPagination
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    TableWithPagination::TableWithPagination(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                                 long style, const wxString& name)
        : wxPanel(parent, wxID_ANY, pos, size, style, name)
        , m_linesPerPage(100)
        , m_currentPage(0)
        , m_ctrl(NULL)
    {
        SetSizer(new wxBoxSizer(wxVERTICAL));
        wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
        GetSizer()->Add(hSizer, 1, wxEXPAND | wxALL, 5);
        m_staticText = gsgs_Style().createStaticText(this, wxID_ANY, "");
        GetSizer()->Add(m_staticText, 0, wxEXPAND | wxALIGN_CENTER, 5);

        m_ctrl = new gsgs::ThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
        hSizer->Add(m_ctrl, 1, wxEXPAND);
        wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
        hSizer->Add(vSizer, 0, wxEXPAND);

        m_btnNextPage = new Button(this, wxID_FORWARD);
        vSizer->Add(m_btnNextPage, 0, wxALIGN_RIGHT | wxALL | wxEXPAND, 5);

        m_btnPrevPage = new Button(this, wxID_BACKWARD);
        vSizer->Add(m_btnPrevPage, 0, wxALIGN_RIGHT | wxALL | wxEXPAND, 5);
        // Bind the command events
        m_btnNextPage->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
            wxUnusedVar(event);
            m_currentPage++;
            ShowPage(m_currentPage);
        });
        m_btnPrevPage->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
            wxUnusedVar(event);
            m_currentPage--;
            ShowPage(m_currentPage);
        });

        // And the 'update-ui' events
        m_btnNextPage->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(CanNext()); });
        m_btnPrevPage->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(CanPrev()); });
        m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TableWithPagination::OnLineActivated, this);
        GetSizer()->Fit(this);
    }
    //-------------------------------------------------------------------------------
    TableWithPagination::~TableWithPagination() {}
    //-------------------------------------------------------------------------------
    void TableWithPagination::SetLinesPerPage(int numLines) { m_linesPerPage = numLines; }
    //-------------------------------------------------------------------------------
    void TableWithPagination::SetColumns(const wxArrayString& columns)
    {
        m_columns = columns;
        ClearAll();
        for(size_t i = 0; i < m_columns.size(); ++i) {
            m_ctrl->AppendTextColumn(m_columns.Item(i), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
        }
    }
    //-------------------------------------------------------------------------------
    void TableWithPagination::SetData(std::vector<wxArrayString>& data)
    {
        m_data.clear();
        m_data.swap(data);
        ShowPage(0);
    }
    //-------------------------------------------------------------------------------
    void TableWithPagination::ClearAll()
    {
        m_data.clear();
        m_ctrl->DeleteAllItems();
        m_ctrl->ClearColumns();
    }
    //-------------------------------------------------------------------------------
    void TableWithPagination::ShowPage(int nPage)
    {
        m_ctrl->DeleteAllItems();
        if(m_data.empty()) return;
        int startIndex = (nPage * m_linesPerPage);
        int lastIndex = startIndex + m_linesPerPage - 1; // last index, including
        if(lastIndex >= (int)m_data.size()) { lastIndex = (m_data.size() - 1); }
        m_currentPage = nPage;
        for(int i = startIndex; i <= lastIndex; ++i) {
            wxVector<wxVariant> cols;
            const wxArrayString& items = m_data[i];
            for(size_t j = 0; j < items.size(); ++j) {
                const wxString& cellContent = items.Item(j);
                cols.push_back(wxVariant(MakeDisplayString(cellContent)));
            }
            m_ctrl->AppendItem(cols, (wxUIntPtr)&items);
        }

        m_staticText->SetLabel(wxString() << _("Showing entries from: ") << startIndex << _(":") << lastIndex
                                          << " Total of: " << m_data.size() << _(" entries"));
    }
    //-------------------------------------------------------------------------------
    bool TableWithPagination::CanNext() const
    {
        int startIndex = ((m_currentPage + 1) * m_linesPerPage);
        return startIndex < (int)m_data.size();
    }
    //-------------------------------------------------------------------------------
    bool TableWithPagination::CanPrev() const { return (((m_currentPage - 1) >= 0) && !m_data.empty()); }
    //-------------------------------------------------------------------------------
    void TableWithPagination::ClearAllItems()
    {
        // truncate the string to a reasonable string
        m_ctrl->DeleteAllItems();
    }
    //-------------------------------------------------------------------------------
    wxString TableWithPagination::MakeDisplayString(const wxString& str) const
    {
        wxString truncatedString = str;
        if(truncatedString.Length() > 100) {
            truncatedString = truncatedString.Mid(0, 100);
            truncatedString.Append(wxT("..."));
        }

        // Convert all whitespace chars into visible ones
        truncatedString.Replace(wxT("\n"), wxT("\\n"));
        truncatedString.Replace(wxT("\r"), wxT("\\r"));
        truncatedString.Replace(wxT("\t"), wxT("\\t"));
        return truncatedString;
    }
    //-------------------------------------------------------------------------------
    void TableWithPagination::OnLineActivated(wxDataViewEvent& event)
    {
        wxDataViewItem item = event.GetItem();
        CHECK_ITEM_RET(item);

        wxArrayString* data = reinterpret_cast<wxArrayString*>(m_ctrl->GetItemData(item));
        CHECK_PTR_RET(data);

        gsgs::TableLineEditorDlg* dlg = new gsgs::TableLineEditorDlg(::wxGetTopLevelParent(this), m_columns, *data);
        dlg->Show();
    }
    //-------------------------------------------------------------------------------
    ConsoleFrame::ConsoleFrame(wxWindow* parent)
        : wxFrame(parent, wxID_ANY, _("Console"))
    {
        CreateViewImpl();
    }
    //-------------------------------------------------------------------------------
    #if USE_SFTP
    ConsoleFrame::ConsoleFrame(wxWindow* parent, gsgs::SSH::Ptr_t ssh)
        : wxFrame(parent, wxID_ANY, _("Console"))
        , m_ssh(ssh)
    {
        CreateViewImpl();
        m_terminal->Bind(wxEVT_TERMINAL_EXECUTE_COMMAND, &ConsoleFrame::OnExecuteRemoteCommand, this);
        m_channel.reset(new gsgs::SSHChannel(m_ssh));
        Bind(wxEVT_SSH_CHANNEL_CLOSED, &ConsoleFrame::OnChannelClosed, this);
        Bind(wxEVT_SSH_CHANNEL_READ_ERROR, &ConsoleFrame::OnChannelReadError, this);
        Bind(wxEVT_SSH_CHANNEL_READ_OUTPUT, &ConsoleFrame::OnChannelRead, this);
    }
    #endif
    //-------------------------------------------------------------------------------
    ConsoleFrame::~ConsoleFrame()
    {
        m_terminal->Unbind(wxEVT_TERMINAL_EXIT_WHEN_DONE, &ConsoleFrame::OnExitWhenDone, this);
        m_terminal->Unbind(wxEVT_TERMINAL_CTRL_C, &ConsoleFrame::OnTerminalCtrlC, this);
    }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::CreateViewImpl()
    {
        wxBoxSizer* bSizer1;
        bSizer1 = new wxBoxSizer(wxVERTICAL);
        m_terminal = new wxTerminal(this);
        m_terminal->SetInteractive(true);
        bSizer1->Add(m_terminal, 1, wxEXPAND);

        this->SetSizer(bSizer1);
        this->Layout();
        m_terminal->Focus();
        SetSize(wxDLG_UNIT(this, wxSize(500, 300)));
        SetName("ConsoleFrame");
        CenterOnScreen();
        gsgs::WindowManager::Load(this);
        m_terminal->Bind(wxEVT_TERMINAL_EXIT_WHEN_DONE, &ConsoleFrame::OnExitWhenDone, this);
        m_terminal->Bind(wxEVT_TERMINAL_CTRL_C, &ConsoleFrame::OnTerminalCtrlC, this);
    }
    //-------------------------------------------------------------------------------
    #if USE_SFTP
    void ConsoleFrame::OnExecuteRemoteCommand(gsgs::Event& event)
    {
        try {
            if(m_channel->IsOpen()) {
                return;
            }
            if(!m_channel->IsOpen()) {
                m_channel->Open();
            }
            m_channel->Execute(event.GetString(), this);

        } catch(gsgs::Exception& e) {
            m_terminal->AddTextWithEOL(e.What());
        }
    }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::OnChannelReadError(gsgs::Event& event)
    {
        wxUnusedVar(event);
        m_terminal->AddTextRaw("\n");
        m_terminal->CaretToEnd();
        m_channel->Close();
    }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::OnChannelRead(gsgs::Event& event)
    {
        m_terminal->AddTextRaw(event.GetString());
        m_terminal->CaretToEnd();
    }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::OnChannelClosed(gsgs::Event& event)
    {
        wxUnusedVar(event);
        m_terminal->AddTextRaw("\n");
        m_terminal->CaretToEnd();
        m_channel->Close();
    }
    #endif
    //-------------------------------------------------------------------------------
    void ConsoleFrame::Execute(const wxString& command, const wxString& wd) { m_terminal->Execute(command, true, wd); }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::OnExitWhenDone(gsgs::Event& event)
    {
        wxUnusedVar(event);
        Close();
    }
    //-------------------------------------------------------------------------------
    void ConsoleFrame::OnTerminalCtrlC(gsgs::Event& event)
    {
        event.Skip();
    #if USE_SFTP
        if(m_channel->IsOpen()) {
            m_channel->Close();
            m_terminal->AddTextWithEOL(_("\nInterrupted"));
            event.Skip(false); // No need for the default action
        }
    #endif
    }
    //-------------------------------------------------------------------------------
}