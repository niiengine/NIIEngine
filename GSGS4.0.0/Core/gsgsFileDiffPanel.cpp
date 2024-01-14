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

#include "gsgsFileDiffPanel.h"
#include "art_metro.h"
#include "gsgsBookmarkManager.h"
#include "gsgsStyleManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsFileUtil.h"
#include "gsgsToolBar.h"
#include "gsgsQuickFindBar.h"
#include "gsgsWindowManager.h"
#include "gsgsCommonDialog.h"
#include "gsgsImageManager.h"
#include "globals.h"
#include "plugin.h"
#include <wx/dcbuffer.h>

#define RED_MARKER 5
#define GREEN_MARKER 6
#define PLACE_HOLDER_MARKER 7

#define RED_MARKER_MASK (1 << RED_MARKER)
#define GREEN_MARKER_MASK (1 << GREEN_MARKER)
#define PLACE_HOLDER_MARKER_MASK (1 << PLACE_HOLDER_MARKER)

#define MARKER_SEQUENCE 8
#define MARKER_SEQUENCE_VERTICAL 9

#define NUMBER_MARGIN_ID 0

#include "dtl/dtl.hpp"
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/icon.h>
#include <wx/xrc/xmlres.h>

namespace gsgs
{
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // DiffConfig
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    DiffConfig::DiffConfig()
        : ObjectData("gsgsDiff")
        , m_flags(kShowLineNumbers)
        , m_viewFlags(kViewVerticalSplit)
    {
    }
    //----------------------------------------------------------------------------
    DiffConfig::~DiffConfig() 
    {
    }
    //----------------------------------------------------------------------------
    void DiffConfig::FromJSON(const JSONObject & json)
    {
        json.getObject("m_flags").getValue(m_flags, 0);
        json.getObject("m_viewFlags").getValue(m_viewFlags, kViewVerticalSplit);
        json.getObject("m_leftFile").getValue(m_leftFile);
        json.getObject("m_rightFile").getValue(m_rightFile);
    }
    //----------------------------------------------------------------------------
    JSONObject DiffConfig::ToJSON() const
    {
        JSONObject element = JSONObject::createObject(GetName());
        element.addProperty("m_flags", m_flags);
        element.addProperty("m_viewFlags", m_viewFlags);
        element.addProperty("m_leftFile", m_leftFile);
        element.addProperty("m_rightFile", m_rightFile);
        return element;
    }
    //----------------------------------------------------------------------------
    DiffConfig& DiffConfig::Load()
    {
        gsgs_Config().ReadItem(this);
        return *this;
    }
    //----------------------------------------------------------------------------
    void DiffConfig::Save() 
    { 
        gsgs_Config().WriteItem(this); 
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // FileDiff
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    FileDiff::FileDiff()
    {
    }
    //----------------------------------------------------------------------------
    FileDiff::~FileDiff()
    {
    }
    //----------------------------------------------------------------------------
    void FileDiff::Diff(const wxFileName& fnLeft, const wxFileName& fnRight, DiffMode mode)
    {
        wxString leftFile, rightFile;

        {
            wxFFile fp1(fnLeft.GetFullPath(), "rb");
            wxFFile fp2(fnRight.GetFullPath(), "rb");

            if (!fp1.IsOpened() || !fp2.IsOpened())
                return;

            // Read the file content
            fp1.ReadAll(&leftFile);
            fp2.ReadAll(&rightFile);
        }

        m_resultLeft.clear();
        m_resultRight.clear();
        m_sequences.clear();

        typedef wxString elem;
        typedef std::pair<elem, dtl::elemInfo> sesElem;

        wxArrayString leftLines = wxStringTokenize(leftFile, "\n", wxTOKEN_RET_DELIMS);
        wxArrayString rightLines = wxStringTokenize(rightFile, "\n", wxTOKEN_RET_DELIMS);

        std::vector<elem> leftLinesVec;
        std::vector<elem> rightLinesVec;
        leftLinesVec.insert(leftLinesVec.end(), leftLines.begin(), leftLines.end());
        rightLinesVec.insert(rightLinesVec.end(), rightLines.begin(), rightLines.end());

        dtl::Diff<elem, std::vector<elem> > diff(leftLinesVec, rightLinesVec);
        diff.onHuge();
        diff.compose();

        if (0 == diff.getEditDistance()) {
            // nothing to be done - files are identical
            return;
        }

        if (mode & FileDiff::kTwoPanes) {

            ///////////////////////////////////////////////////////////////////
            // Two panes diff
            // designed for displayed on a two panes view where on the left
            // pane all deletions while on the right pane all the new lines
            ///////////////////////////////////////////////////////////////////

            // Loop over the diff and check if it is a whitespace only diff
            std::vector<sesElem> seq = diff.getSes().getSequence();
            m_resultLeft.reserve(seq.size());
            m_resultRight.reserve(seq.size());

            const int STATE_NONE = 0;
            const int STATE_IN_SEQ = 1;

            int state = STATE_NONE;
            int seqStartLine = wxNOT_FOUND;
            size_t seqSize = 0;

            LineInfoVec_t tmpSeqLeft;
            LineInfoVec_t tmpSeqRight;

            for (size_t i = 0; i < seq.size(); ++i) {
                switch (seq.at(i).second.type) {
                case dtl::SES_COMMON: {
                    if (state == STATE_IN_SEQ) {

                        // set the sequence size
                        seqSize = ::wxMax(tmpSeqLeft.size(), tmpSeqRight.size());

                        m_sequences.push_back(std::make_pair(seqStartLine, seqStartLine + seqSize));
                        seqStartLine = wxNOT_FOUND;
                        state = STATE_NONE;

                        // increase the buffer size
                        tmpSeqLeft.resize(seqSize);
                        tmpSeqRight.resize(seqSize);

                        m_resultLeft.insert(m_resultLeft.end(), tmpSeqLeft.begin(), tmpSeqLeft.end());
                        m_resultRight.insert(m_resultRight.end(), tmpSeqRight.begin(), tmpSeqRight.end());

                        tmpSeqLeft.clear();
                        tmpSeqRight.clear();
                        seqSize = 0;
                    }
                    FileDiff::LineInfo line(seq.at(i).first, LINE_COMMON);
                    m_resultLeft.push_back(line);
                    m_resultRight.push_back(line);
                    break;

                }
                case dtl::SES_ADD: {
                    FileDiff::LineInfo lineRight(seq.at(i).first, LINE_ADDED);
                    tmpSeqRight.push_back(lineRight);

                    if (state == STATE_NONE) {
                        seqStartLine = m_resultLeft.size();
                        state = STATE_IN_SEQ;
                    }
                    break;

                }
                case dtl::SES_DELETE: {
                    FileDiff::LineInfo lineLeft(seq.at(i).first, LINE_REMOVED);
                    tmpSeqLeft.push_back(lineLeft);

                    if (state == STATE_NONE) {
                        seqStartLine = m_resultLeft.size();
                        state = STATE_IN_SEQ;
                    }
                    break;
                }
                }
            }

            if (state == STATE_IN_SEQ) {
                // set the sequence size
                seqSize = ::wxMax(tmpSeqLeft.size(), tmpSeqRight.size());
                if (seqSize) {
                    m_sequences.push_back(std::make_pair(seqStartLine, seqStartLine + seqSize));
                    seqStartLine = wxNOT_FOUND;
                    state = STATE_NONE;

                    // increase the buffer size
                    tmpSeqLeft.resize(seqSize);
                    tmpSeqRight.resize(seqSize);

                    m_resultLeft.insert(m_resultLeft.end(), tmpSeqLeft.begin(), tmpSeqLeft.end());
                    m_resultRight.insert(m_resultRight.end(), tmpSeqRight.begin(), tmpSeqRight.end());

                    tmpSeqLeft.clear();
                    tmpSeqRight.clear();
                    seqSize = 0;
                }
            }
        }
        else {
            ///////////////////////////////////////////////////////////////////
            // One pane diff view
            // designed for displayed on a single editor
            ///////////////////////////////////////////////////////////////////
            std::vector<sesElem> seq = diff.getSes().getSequence();
            m_resultLeft.reserve(seq.size());
            int seqStartLine = wxNOT_FOUND;
            for (size_t i = 0; i < seq.size(); ++i) {
                switch (seq.at(i).second.type) {
                case dtl::SES_COMMON: {
                    if (seqStartLine != wxNOT_FOUND) {
                        m_sequences.push_back(std::make_pair(seqStartLine, m_resultLeft.size()));
                        seqStartLine = wxNOT_FOUND;
                    }
                    FileDiff::LineInfo line(seq.at(i).first, LINE_COMMON);
                    m_resultLeft.push_back(line);
                    break;
                }
                case dtl::SES_ADD: {
                    if (seqStartLine == wxNOT_FOUND) {
                        seqStartLine = m_resultLeft.size();
                    }
                    FileDiff::LineInfo line(seq.at(i).first, LINE_ADDED);
                    m_resultLeft.push_back(line);
                    break;

                }
                case dtl::SES_DELETE: {
                    if (seqStartLine == wxNOT_FOUND) {
                        seqStartLine = m_resultLeft.size();
                    }
                    FileDiff::LineInfo line(seq.at(i).first, LINE_REMOVED);
                    m_resultLeft.push_back(line);
                    break;
                }
                }
            }

            if (seqStartLine != wxNOT_FOUND) {
                m_sequences.push_back(std::make_pair(seqStartLine, m_resultLeft.size()));
                seqStartLine = wxNOT_FOUND;
            }
        }
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // FileDiffPanel
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    FileDiffPanel::FileDiffPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                                                 const wxSize& size, long style)
        : wxPanel(parent, id, pos, size, style)
        , m_darkTheme(false)
        , m_flags(0)
        , m_storeFilepaths(true)
    {
        wxBoxSizer* boxSizer13 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer13);

        wxBoxSizer* boxSizer314 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer13->Add(boxSizer314, 1, wxEXPAND, WXC_FROM_DIP(0));

        m_splitter = new SplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                                                wxSP_LIVE_UPDATE | wxSP_PERMIT_UNSPLIT | wxSP_3DSASH);
        m_splitter->SetSashGravity(0.5);
        m_splitter->SetMinimumPaneSize(10);

        boxSizer314->Add(m_splitter, 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));

        m_splitterPageLeft =
            new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_splitterPageLeft->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        wxBoxSizer* boxSizer111 = new wxBoxSizer(wxVERTICAL);
        m_splitterPageLeft->SetSizer(boxSizer111);

        wxBoxSizer* boxSizer285 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer111->Add(boxSizer285, 0, wxEXPAND | wxRIGHT, WXC_FROM_DIP(10));

        m_textCtrlLeftFile = gsgs_Style().createTextCtrl(m_splitterPageLeft, wxID_ANY, wxT(""), wxDefaultPosition,
                                            wxDLG_UNIT(m_splitterPageLeft, wxSize(-1, -1)), 0);
        m_textCtrlLeftFile->AutoCompleteFileNames();

        boxSizer285->Add(m_textCtrlLeftFile, 1, wxEXPAND, 0);

        m_button290 = new Button(m_splitterPageLeft, wxID_ANY, _("..."), wxDefaultPosition,
                                   wxDLG_UNIT(m_splitterPageLeft, wxSize(-1, -1)), wxBU_EXACTFIT);
        m_button290->SetToolTip(_("Choose a file"));

        boxSizer285->Add(m_button290, 0, wxLEFT| wxRIGHT | wxALIGN_CENTRE_HORIZONTAL, 4);

        wxBoxSizer* boxSizer310 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer111->Add(boxSizer310, 1, wxEXPAND, WXC_FROM_DIP(0));

        m_stcLeft = GetManager()->CreateEditor(m_splitterPageLeft);

        boxSizer310->Add(m_stcLeft->GetParent(), 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));

        m_panelOverviewL = new wxPanel(m_splitterPageLeft, wxID_ANY, wxDefaultPosition,
                                       wxDLG_UNIT(m_splitterPageLeft, wxSize(4, -1)), wxTAB_TRAVERSAL | wxNO_BORDER);
        m_panelOverviewL->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_panelOverviewL->SetBackgroundStyle(wxBG_STYLE_PAINT);
        boxSizer310->Add(m_panelOverviewL, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));


        //m_staticTextLeft = gsgs_Style().createStaticText(m_splitterPageLeft, wxID_ANY, wxT(""), wxDefaultPosition,
                                            //wxDLG_UNIT(m_splitterPageLeft, wxSize(-1, -1)), wxALIGN_CENTRE | wxBORDER_NONE);

        //boxSizer111->Add(m_staticTextLeft, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(2));

        m_splitterPageRight =
            new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_splitter, wxSize(-1, -1)), wxTAB_TRAVERSAL);
        m_splitterPageRight->SetBackgroundColour(gsgs_Style().GetAppBgColour());

        wxBoxSizer* boxSizer113 = new wxBoxSizer(wxVERTICAL);
        m_splitterPageRight->SetSizer(boxSizer113);

        wxBoxSizer* boxSizer286 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer113->Add(boxSizer286, 0, wxEXPAND | wxUP | wxDOWN, 0);

        m_textCtrlRightFile = gsgs_Style().createTextCtrl(m_splitterPageRight, wxID_ANY, wxT(""), wxDefaultPosition,
                                             wxDLG_UNIT(m_splitterPageRight, wxSize(-1, -1)), 0);

        m_textCtrlRightFile->AutoCompleteFileNames();

        boxSizer286->Add(m_textCtrlRightFile, 1, wxEXPAND, 0);

        m_button294 = new Button(m_splitterPageRight, wxID_ANY, _("..."), wxDefaultPosition,
                                   wxDLG_UNIT(m_splitterPageRight, wxSize(-1, -1)), wxBU_EXACTFIT);

        boxSizer286->Add(m_button294, 0, wxLEFT | wxRIGHT| wxALIGN_CENTRE_HORIZONTAL, 4);

        wxBoxSizer* boxSizer307 = new wxBoxSizer(wxHORIZONTAL);

        boxSizer113->Add(boxSizer307, 1, wxEXPAND, WXC_FROM_DIP(0));

        m_stcRight = GetManager()->CreateEditor(m_splitterPageRight);

        boxSizer307->Add(m_stcRight->GetParent(), 1, wxALL | wxEXPAND, WXC_FROM_DIP(2));
        //m_staticTextRight =
        //    gsgs_Style().createStaticText(m_splitterPageRight, wxID_ANY, wxT(""), wxDefaultPosition,
        //                     wxDLG_UNIT(m_splitterPageRight, wxSize(-1, -1)), wxALIGN_CENTRE | wxBORDER_NONE);

        //boxSizer113->Add(m_staticTextRight, 0, wxALL | wxEXPAND | wxALIGN_LEFT, WXC_FROM_DIP(2));

        m_panelOverviewFull =
            new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(4, -1)), wxTAB_TRAVERSAL | wxNO_BORDER);
        m_panelOverviewFull->SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_panelOverviewFull->SetBackgroundStyle(wxBG_STYLE_PAINT);
        boxSizer314->Add(m_panelOverviewFull, 0, wxALL | wxEXPAND, WXC_FROM_DIP(2));

        // Connect events
        m_button290->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileDiffPanel::OnLeftPickerUI), NULL, this);
        m_button290->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FileDiffPanel::OnBrowseLeftFile), NULL, this);
        m_stcLeft->GetParent()->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(FileDiffPanel::OnLeftStcPainted), NULL, this);
        m_stcLeft->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(FileDiffPanel::OnMouseWheel), NULL, this);
        m_panelOverviewL->Connect(wxEVT_PAINT, wxPaintEventHandler(FileDiffPanel::OnPaint), NULL, this);
        m_panelOverviewL->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FileDiffPanel::OnPaneloverviewLeftDown),
                                  NULL, this);
        m_button294->Connect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileDiffPanel::OnRightPickerUI), NULL, this);
        m_button294->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                             wxCommandEventHandler(FileDiffPanel::OnBrowseRightFile), NULL, this);
        m_stcRight->GetParent()->Connect(wxEVT_STC_PAINTED, wxScrollEventHandler(FileDiffPanel::OnRightStcPainted), NULL,
                            this);
        m_stcRight->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(FileDiffPanel::OnMouseWheel), NULL, this);

        m_panelOverviewFull->Connect(wxEVT_PAINT, wxPaintEventHandler(FileDiffPanel::OnPaint), NULL, this);
        m_panelOverviewFull->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FileDiffPanel::OnPaneloverviewLeftDown),
                                     NULL, this);
                                 
        m_config.Load();
        m_config.SetViewMode(DiffConfig::kViewVerticalSplit);
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++");
        if (lexer) {
            m_darkTheme = lexer->IsDark();
            lexer->Apply(m_stcLeft);
            lexer->Apply(m_stcRight);
        }
        gsgs_Style().RegisterWindow(this);

        m_findBar = new QuickFindBar(this);
        m_findBar->SetEditor(m_stcLeft);
        m_findBar->Show(false);

        // Changing the focus, updates the find bar managed editor
        m_stcLeft->Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& event) {
            event.Skip();
            m_findBar->SetEditor(m_stcLeft);
        });

        m_stcRight->Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& event) {
            event.Skip();
            m_findBar->SetEditor(m_stcRight);
        });

        // Vertical is the default; DoLayout() unsplits; but nothing implements Horizontal. So:
        if (m_config.IsSplitHorizontal()) {
            m_splitter->Unsplit();
            m_splitter->SplitHorizontally(m_splitterPageLeft, m_splitterPageRight);
        }
        else
        {
            m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight, 0);
        }

        ImageManager* bmps = &gsgs_Image();
        m_toolbar = new ToolBar(this);
        m_toolbar->SetMiniToolBar(true);
        m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), bmps->LoadBitmap("debugger_restart"));
        m_toolbar->AddTool(wxID_SAVE, _("Save"), bmps->LoadBitmap("file_save"));
        m_toolbar->AddSeparator();
        //m_toolbar->AddTool(wxID_FIND, _("Find"), bmps->LoadBitmap("find"));
        //m_toolbar->AddSeparator();
        m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_PREV"), _("Previous Diff"), bmps->LoadBitmap("up"));
        m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_NEXT"), _("Next Diff"), bmps->LoadBitmap("down"));
        m_toolbar->AddSeparator();
        m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_COPY_RIGHT"), _("Copy Right"), bmps->LoadBitmap("forward"));
        m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_COPY_LEFT"), _("Copy Left"), bmps->LoadBitmap("back"));
        m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_COPY_ALL"), _("Copy All"), bmps->LoadBitmap("copy"), "", wxITEM_DROPDOWN);
        m_toolbar->AddSeparator();
        //m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_VIEW"), _("View Type"), bmps->LoadBitmap("monitor"), "", wxITEM_DROPDOWN);
        //m_toolbar->AddSeparator();
        //m_toolbar->AddTool(XRCID("ID_DIFF_TOOL_SETTINGS"), _("Preferences"), bmps->LoadBitmap("cog"), "", wxITEM_DROPDOWN);
        m_toolbar->AddTool(wxID_CLOSE, _("Close"), bmps->LoadBitmap("file_close"));
        m_toolbar->Realize();
        boxSizer13->Insert(0, m_toolbar, 0, wxEXPAND | wxALL, 5);
        boxSizer13->Add(m_findBar, 0, wxEXPAND);

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnRefreshDiff, this, wxID_REFRESH);
        //m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnRefreshDiffUI, this, wxID_REFRESH);

        //m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnFind, this, wxID_FIND);

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnSaveChanges, this, wxID_SAVE);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnSaveChangesUI, this, wxID_SAVE);

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnNextDiffSequence, this, XRCID("ID_DIFF_TOOL_NEXT"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnNextDiffUI, this, XRCID("ID_DIFF_TOOL_NEXT"));

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnPrevDiffSequence, this, XRCID("ID_DIFF_TOOL_PREV"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnPrevDiffUI, this, XRCID("ID_DIFF_TOOL_PREV"));

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnCopyLeftToRight, this, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyLeftToRightUI, this, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));

        m_toolbar->Bind(wxEVT_TOOL, &FileDiffPanel::OnCopyRightToLeft, this, XRCID("ID_DIFF_TOOL_COPY_LEFT"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyRightToLeftUI, this, XRCID("ID_DIFF_TOOL_COPY_LEFT"));

        m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnCopyAllMenu, this, XRCID("ID_DIFF_TOOL_COPY_ALL"));
        //m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnViewMenu, this, XRCID("ID_DIFF_TOOL_VIEW"));
        //m_toolbar->Bind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnPreferences, this, XRCID("ID_DIFF_TOOL_SETTINGS"));

        gsgs_Event().Connect(wxEVT_NOTIFY_PAGE_CLOSING, wxNotifyEventHandler(FileDiffPanel::OnPageClosing), NULL, this);

        Connect(ID_COPY_LEFT_TO_RIGHT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyLeft2Right));
        Connect(ID_COPY_LEFT_TO_RIGHT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyLeft2Right));
        Connect(ID_COPY_RIGHT_TO_LEFT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyRight2Left));
        Connect(ID_COPY_RIGHT_TO_LEFT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyRight2Left));
        CallAfter(&FileDiffPanel::DoLayout);
        //CentreOnParent();
    }
    //----------------------------------------------------------------------------
    FileDiffPanel::~FileDiffPanel()
    {
        m_button290->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileDiffPanel::OnLeftPickerUI), NULL,
                                this);
        m_button290->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                wxCommandEventHandler(FileDiffPanel::OnBrowseLeftFile), NULL, this);
        m_stcLeft->GetParent()->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(FileDiffPanel::OnLeftStcPainted), NULL,
                              this);
        m_stcLeft->Disconnect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(FileDiffPanel::OnMouseWheel), NULL, this);
        m_panelOverviewL->Disconnect(wxEVT_PAINT,
            wxPaintEventHandler(FileDiffPanel::OnPaint), NULL,
                                     this);
        m_panelOverviewL->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(FileDiffPanel::OnPaneloverviewLeftDown),
                                     NULL, this);
        //m_textCtrlRightFile->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileDiffPanel::OnRightPickerUI),
        //                                NULL, this);
        m_button294->Disconnect(wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FileDiffPanel::OnRightPickerUI), NULL,
                                this);
        m_button294->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
                                wxCommandEventHandler(FileDiffPanel::OnBrowseRightFile), NULL, this);
        m_stcRight->GetParent()->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(FileDiffPanel::OnRightStcPainted),
                               NULL, this);
        m_stcRight->Disconnect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(FileDiffPanel::OnMouseWheel), NULL, this);

        m_panelOverviewFull->Disconnect(wxEVT_PAINT, wxPaintEventHandler(FileDiffPanel::OnPaint), NULL, this);
        m_panelOverviewFull->Disconnect(wxEVT_LEFT_DOWN,
                                        wxMouseEventHandler(FileDiffPanel::OnPaneloverviewLeftDown), NULL, this);
                                    
        if ((m_flags & kDeleteLeftOnExit)) { gsgsRemoveFile(m_textCtrlLeftFile->GetValue()); }
        if ((m_flags & kDeleteRightOnExit)) { gsgsRemoveFile(m_textCtrlRightFile->GetValue()); }

        if ((m_flags & kSavePaths)) {
            m_config.SetLeftFile(m_textCtrlLeftFile->GetValue());
            m_config.SetRightFile(m_textCtrlRightFile->GetValue());
        }

        // save the configuration
        m_config.Save();

        // Clean up any temporary files
        wxString tpath(wxFileName::GetTempDir());
        tpath << gsgsPathSep << "gsgsdifftemp";
        wxFileName::Rmdir(tpath, wxPATH_RMDIR_RECURSIVE);

        gsgs_Event().Disconnect(wxEVT_NOTIFY_PAGE_CLOSING,
            wxNotifyEventHandler(FileDiffPanel::OnPageClosing), NULL, this);

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnRefreshDiff, this, wxID_REFRESH);
        //m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnRefreshDiffUI, this, wxID_REFRESH);

        //m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnFind, this, wxID_FIND);

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnSaveChanges, this, wxID_SAVE);
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnSaveChangesUI, this, wxID_SAVE);

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnNextDiffSequence, this, XRCID("ID_DIFF_TOOL_NEXT"));
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnNextDiffUI, this, XRCID("ID_DIFF_TOOL_NEXT"));

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnPrevDiffSequence, this, XRCID("ID_DIFF_TOOL_PREV"));
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnPrevDiffUI, this, XRCID("ID_DIFF_TOOL_PREV"));

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnCopyLeftToRight, this, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyLeftToRightUI, this, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));

        m_toolbar->Unbind(wxEVT_TOOL, &FileDiffPanel::OnCopyRightToLeft, this, XRCID("ID_DIFF_TOOL_COPY_LEFT"));
        m_toolbar->Unbind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyRightToLeftUI, this, XRCID("ID_DIFF_TOOL_COPY_LEFT"));

        m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnCopyAllMenu, this, XRCID("ID_DIFF_TOOL_COPY_ALL"));
        //m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnViewMenu, this, XRCID("ID_DIFF_TOOL_VIEW"));
        //m_toolbar->Unbind(wxEVT_TOOL_DROPDOWN, &FileDiffPanel::OnPreferences, this, XRCID("ID_DIFF_TOOL_SETTINGS"));

        Disconnect(ID_COPY_LEFT_TO_RIGHT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyLeft2Right));
        Disconnect(ID_COPY_LEFT_TO_RIGHT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyLeft2Right));
        Disconnect(ID_COPY_RIGHT_TO_LEFT, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyRight2Left));
        Disconnect(ID_COPY_RIGHT_TO_LEFT_AND_MOVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FileDiffPanel::OnMenuCopyRight2Left));
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::Diff()
    {
        bool retu = 0;
        wxFileName fnLeft(m_textCtrlLeftFile->GetValue());
        wxFileName fnRight(m_textCtrlRightFile->GetValue());

        if (m_textCtrlLeftFile->GetValue().IsEmpty()) {
            retu = 1;
        }
        else if (!fnLeft.Exists())
        {
            ::wxMessageBox(wxString() << _("Left Side File:\n") << fnLeft.GetFullPath() << _(" does not exist!"),
                "gsgs", wxICON_ERROR | wxCENTER | wxOK);
            retu = 1;
        }
        else
        {
            m_stcLeft->LoadFile(fnLeft.GetFullPath());
        }

        if (m_textCtrlRightFile->GetValue().IsEmpty()) {
            retu = 1;
        }
        else if (!fnRight.Exists())
        {
            ::wxMessageBox(wxString() << _("Right Side File:\n") << fnRight.GetFullPath() << _(" does not exist!"),
                "gsgs", wxICON_ERROR | wxCENTER | wxOK);
            retu = 1;
        }
        else
        {
            m_stcRight->LoadFile(fnRight.GetFullPath());
        }
        if (retu)
            return;
        // Cleanup
        DoClean();

        // Prepare the views
        PrepareViews();

        // Prepare the diff
        FileDiff d;
        d.Diff(m_textCtrlLeftFile->GetValue(), m_textCtrlRightFile->GetValue(),
            m_config.IsSingleViewMode() ? FileDiff::kOnePane : FileDiff::kTwoPanes);
        FileDiff::LineInfoVec_t& resultLeft = d.GetResultLeft();
        FileDiff::LineInfoVec_t& resultRight = d.GetResultRight();
        m_sequences = d.GetSequences();

        if (m_sequences.empty()) {
            // Files are the same !
            m_stcLeft->SetReadOnly(false);
            m_stcRight->SetReadOnly(false);

            // Get the user converter
            wxString leftFileText, rightFileText;
            clMBConv(conv);
            if (!FileUtil::ReadFileContent(fnLeft, leftFileText, *conv)) {
                ::wxMessageBox(_("Failed to load file content: ") + fnLeft.GetFullPath(), "gsgs",
                    wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
            if (!FileUtil::ReadFileContent(fnRight, rightFileText, *conv)) {
                ::wxMessageBox(_("Failed to load file content: ") + fnRight.GetFullPath(), "gsgs",
                    wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            //m_stcLeft->LoadFile(fnLeft.GetFullPath());
            //m_stcRight->LoadFile(fnRight.GetFullPath());

            m_stcLeft->SetSavePoint();
            m_stcRight->SetSavePoint();

            m_stcLeft->SetReadOnly(true);
            m_stcRight->SetReadOnly(true);
            return;
        }

        if (m_config.IsIgnoreWhitespace() && !m_config.IsSingleViewMode()) {
            // If the user wants to ignore whitespace diffs, go through first to remove them
            // Note that this doesn't work in singleview mode where each change is shown on
            // 2 lines, before & after. Having those unmarked would be very confusing
            for (size_t l = 0, r = 0; (l < resultLeft.size()) && (r < resultRight.size()); ++l, ++r) {
                if (resultLeft.at(l).m_type == FileDiff::LINE_REMOVED || resultLeft.at(l).m_type == FileDiff::LINE_ADDED) {
                    wxString left(resultLeft.at(l).m_line);
                    left.Replace(" ", "");
                    left.Replace("\t", "");
                    left.Replace("\r", "");
                    wxString right(resultRight.at(r).m_line);
                    right.Replace(" ", "");
                    right.Replace("\t", "");
                    right.Replace("\r", "");
                    if (left == right) {
                        resultLeft.at(l).m_type = FileDiff::LINE_COMMON;
                        resultRight.at(r).m_type = FileDiff::LINE_COMMON;
                    }
                }
            }
        }

        m_cur_sequence = 0; // the first line of the sequence

        // Create 2 strings "left" and "right"
        wxString leftContent, rightContent;

        m_overviewPanelMarkers.SetCount(wxMax(resultLeft.size(), resultRight.size()) + 1, 0);

        // The left pane is always the one with the deletions "-"
        for (size_t i = 0; i < resultLeft.size(); ++i) {
            if (resultLeft.at(i).m_type == FileDiff::LINE_ADDED) {
                leftContent << resultLeft.at(i).m_line;
                m_leftGreenMarkers.push_back(i);
                m_overviewPanelMarkers.Item(i) = 1;

            }
            else if (resultLeft.at(i).m_type == FileDiff::LINE_REMOVED) {
                leftContent << resultLeft.at(i).m_line;
                m_leftRedMarkers.push_back(i);
                m_overviewPanelMarkers.Item(i) = 1;

            }
            else if (resultLeft.at(i).m_type == FileDiff::LINE_PLACEHOLDER) {
                // PLACEHOLDER
                leftContent << resultLeft.at(i).m_line;
                m_leftPlaceholdersMarkers.push_back(i);

            }
            else {
                // COMMON
                leftContent << resultLeft.at(i).m_line;
            }
        }

        // The right pane is always with the new additions "+"
        for (size_t i = 0; i < resultRight.size(); ++i) {
            if (resultRight.at(i).m_type == FileDiff::LINE_REMOVED) {
                rightContent << resultRight.at(i).m_line;
                m_rightRedMarkers.push_back(i);
                m_overviewPanelMarkers.Item(i) = 1;

            }
            else if (resultRight.at(i).m_type == FileDiff::LINE_ADDED) {
                rightContent << resultRight.at(i).m_line;
                m_rightGreenMarkers.push_back(i);
                m_overviewPanelMarkers.Item(i) = 1;

            }
            else if (resultRight.at(i).m_type == FileDiff::LINE_PLACEHOLDER) {
                rightContent << resultRight.at(i).m_line;
                m_rightPlaceholdersMarkers.push_back(i);

            }
            else {
                // COMMON
                rightContent << resultRight.at(i).m_line;
            }
        }
        UpdateViews(leftContent, rightContent);
        m_stcLeft->SetSavePoint();
        m_stcRight->SetSavePoint();

        // Select the first diff
        wxCommandEvent dummy;
        m_cur_sequence = -1;
        OnNextDiffSequence(dummy);

        if (m_config.IsSplitHorizontal()) {
            m_panelOverviewFull->Refresh();
        }
        else {
            m_panelOverviewL->Refresh();
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::PrepareViews()
    {
        // Prepare the views by selecting the proper syntax highlight
        wxFileName fnLeft(m_textCtrlLeftFile->GetValue());
        wxFileName fnRight(m_textCtrlRightFile->GetValue());

        bool useRightSideLexer = false;
        if (fnLeft.GetExt() == "svn-base") {
            // doing svn diff, use the lexer for the right side file
            useRightSideLexer = true;
        }

        LexerStyle::Ptr_t leftLexer =
            gsgs_EditorConfig().GetLexerForFile(useRightSideLexer ? fnRight.GetFullName() : fnLeft.GetFullName());
        wxASSERT(leftLexer);

        LexerStyle::Ptr_t rightLexer = gsgs_EditorConfig().GetLexerForFile(fnRight.GetFullName());
        wxASSERT(rightLexer);

        leftLexer->Apply(m_stcLeft, true);
        rightLexer->Apply(m_stcRight, true);

        wxStyledTextCtrl* stc = m_stcLeft;
        for (int n = 0; n < 2; ++n, stc = m_stcRight) {
            // Create the markers we need
            DefineMarkers(stc);

            // Turn off PP highlighting
            stc->SetProperty("lexer.cpp.track.preprocessor", "0");
            stc->SetProperty("lexer.cpp.update.preprocessor", "0");

            // Show number margins if desired
            stc->SetMarginType(NUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);
            stc->SetMarginMask(NUMBER_MARGIN_ID,
                ~(mmt_folds | mmt_all_bookmarks | mmt_indicator | mmt_compiler | mmt_all_breakpoints));
            int pixelWidth = 4 + 5 * stc->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
            stc->SetMarginWidth(NUMBER_MARGIN_ID, m_config.ShowLineNumbers() ? pixelWidth : 0);
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DefineMarkers(wxStyledTextCtrl* ctrl)
    {
        wxColour red, green, grey;
        if (m_darkTheme) {
            red = "RED";
            green = "GREEN";
            grey = "dark grey";
            //sideMarker = "CYAN";

        }
        else {
            red = "RED";
            green = "GREEN";
            grey = "LIGHT GREY";
        }
        const wxColour & sideMarker = gsgs_Style().GetHoverMarkerColour();

        ctrl->MarkerDefine(GREEN_MARKER, wxSTC_MARK_BACKGROUND);
        ctrl->MarkerSetBackground(GREEN_MARKER, green);
        ctrl->MarkerSetAlpha(GREEN_MARKER, 50);

        ctrl->MarkerDefine(RED_MARKER, wxSTC_MARK_BACKGROUND);
        ctrl->MarkerSetBackground(RED_MARKER, red);
        ctrl->MarkerSetAlpha(RED_MARKER, 50);

        ctrl->MarkerDefine(PLACE_HOLDER_MARKER, wxSTC_MARK_BACKGROUND);
        ctrl->MarkerSetBackground(PLACE_HOLDER_MARKER, grey);
        ctrl->MarkerSetAlpha(PLACE_HOLDER_MARKER, 50);

        ctrl->MarkerDefine(MARKER_SEQUENCE, wxSTC_MARK_CIRCLE);
        ctrl->MarkerSetBackground(MARKER_SEQUENCE, sideMarker);
        ctrl->MarkerSetForeground(MARKER_SEQUENCE, sideMarker);

        //ctrl->MarkerDefine(MARKER_SEQUENCE_VERTICAL, wxSTC_MARK_FULLRECT);
        //ctrl->MarkerSetBackground(MARKER_SEQUENCE_VERTICAL, sideMarker);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::UpdateViews(const wxString& left, const wxString& right)
    {
        m_stcLeft->SetEditable(true);
        m_stcRight->SetEditable(true);

        m_stcLeft->SetText(left);
        m_stcLeft->MarkerDeleteAll(RED_MARKER);

        m_stcRight->SetText(right);
        m_stcRight->MarkerDeleteAll(GREEN_MARKER);

        // Show whitespaces
        m_stcRight->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
        m_stcLeft->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);

        // apply the markers
        for (size_t i = 0; i < m_leftRedMarkers.size(); ++i) {
            int line = m_leftRedMarkers.at(i);
            m_stcLeft->MarkerAdd(line, RED_MARKER);
        }
        for (size_t i = 0; i < m_leftGreenMarkers.size(); ++i) {
            int line = m_leftGreenMarkers.at(i);
            m_stcLeft->MarkerAdd(line, GREEN_MARKER);
        }
        for (size_t i = 0; i < m_leftPlaceholdersMarkers.size(); ++i) {
            int line = m_leftPlaceholdersMarkers.at(i);
            m_stcLeft->MarkerAdd(line, PLACE_HOLDER_MARKER);
        }
        for (size_t i = 0; i < m_rightGreenMarkers.size(); ++i) {
            int line = m_rightGreenMarkers.at(i);
            m_stcRight->MarkerAdd(line, GREEN_MARKER);
        }
        for (size_t i = 0; i < m_rightRedMarkers.size(); ++i) {
            int line = m_rightRedMarkers.at(i);
            m_stcRight->MarkerAdd(line, RED_MARKER);
        }
        for (size_t i = 0; i < m_rightPlaceholdersMarkers.size(); ++i) {
            int line = m_rightPlaceholdersMarkers.at(i);
            m_stcRight->MarkerAdd(line, PLACE_HOLDER_MARKER);
        }

        // Restore the 'read-only' state
        m_stcLeft->SetEditable(false);
        m_stcRight->SetEditable(false);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnLeftStcPainted(wxScrollEvent &)
    {
        int rightFirstLine = m_stcRight->GetFirstVisibleLine();
        int leftFirsLine = m_stcLeft->GetFirstVisibleLine();
        if (rightFirstLine != leftFirsLine) 
        { 
            m_stcRight->SetFirstVisibleLine(leftFirsLine); 
            if (m_config.IsSplitHorizontal()) {
                m_panelOverviewFull->Refresh();
            }
            else {
                m_panelOverviewL->Refresh();
            }
        }

        int rightScrollPos = m_stcRight->GetXOffset();
        int leftScrollPos = m_stcLeft->GetXOffset();
        if (leftScrollPos != rightScrollPos) 
        { 
            m_stcRight->SetXOffset(leftScrollPos); 
            if (m_config.IsSplitHorizontal()) {
                m_panelOverviewFull->Refresh();
            }
            else {
                m_panelOverviewL->Refresh();
            }
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnRightStcPainted(wxScrollEvent &)
    {
        int rightFirstLine = m_stcRight->GetFirstVisibleLine();
        int leftFirsLine = m_stcLeft->GetFirstVisibleLine();
        if (rightFirstLine != leftFirsLine) 
        { 
            m_stcLeft->SetFirstVisibleLine(rightFirstLine);
            if (m_config.IsSplitHorizontal()) {
                m_panelOverviewFull->Refresh();
            }
            else {
                m_panelOverviewL->Refresh();
            }
        }

        int rightScrollPos = m_stcRight->GetXOffset();
        int leftScrollPos = m_stcLeft->GetXOffset();
        if (leftScrollPos != rightScrollPos) 
        { 
            m_stcLeft->SetXOffset(rightScrollPos); 
            if (m_config.IsSplitHorizontal()) {
                m_panelOverviewFull->Refresh();
            }
            else {
                m_panelOverviewL->Refresh();
            }
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::SetFilesDetails(const FileDiffPanel::FileInfo& leftFile,
        const FileDiffPanel::FileInfo& rightFile)
    {
        // left file
        m_textCtrlLeftFile->ChangeValue(leftFile.filename.GetFullPath());
        //m_staticTextLeft->SetLabel(leftFile.title);

        m_textCtrlRightFile->ChangeValue(rightFile.filename.GetFullPath());
        //m_staticTextRight->SetLabel(rightFile.title);

        m_flags = 0x0;
        if (leftFile.readOnly) m_flags |= kLeftReadOnly;
        if (leftFile.deleteOnExit) m_flags |= kDeleteLeftOnExit;
        if (rightFile.readOnly) m_flags |= kRightReadOnly;
        if (rightFile.deleteOnExit) m_flags |= kDeleteRightOnExit;
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnNextDiffSequence(wxCommandEvent& event)
    {
        m_cur_sequence++; // advance the sequence
        int firstLine = m_sequences.at(m_cur_sequence).first;
        int lastLine = m_sequences.at(m_cur_sequence).second;
        DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
        DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPrevDiffSequence(wxCommandEvent& event)
    {
        m_cur_sequence--;
        int firstLine = m_sequences.at(m_cur_sequence).first;
        int lastLine = m_sequences.at(m_cur_sequence).second;
        DoDrawSequenceMarkers(firstLine, lastLine, m_stcLeft);
        DoDrawSequenceMarkers(firstLine, lastLine, m_stcRight);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnRefreshDiff(wxCommandEvent& event)
    {
        if (m_stcLeft->IsModified() || m_stcRight->IsModified()) {
            wxStandardID res = ::PromptMessageDialogWithoutCancel(
                _("Refreshing the view will lose all your changes\nDo you want to continue?"), "DiffRefreshViewDlg");
            if (res != wxID_YES) { return; }
        }
        Diff();
        Refresh();
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoClean()
    {
        // Cleanup
        m_leftRedMarkers.clear();
        m_leftPlaceholdersMarkers.clear();
        m_leftGreenMarkers.clear();

        m_rightGreenMarkers.clear();
        m_rightRedMarkers.clear();
        m_rightPlaceholdersMarkers.clear();

        m_overviewPanelMarkers.Clear();
        m_sequences.clear();

        m_stcLeft->SetReadOnly(false);
        m_stcRight->SetReadOnly(false);
        //m_stcLeft->SetText("");
        //m_stcRight->SetText("");
        //m_stcLeft->SetSavePoint();
        //m_stcRight->SetSavePoint();
        m_stcLeft->SetReadOnly(true);
        m_stcRight->SetReadOnly(true);
        m_cur_sequence = wxNOT_FOUND;
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoDrawSequenceMarkers(int firstLine, int lastLine, wxStyledTextCtrl* ctrl)
    {
        // delete old markers
        ctrl->MarkerDeleteAll(MARKER_SEQUENCE);
        //ctrl->MarkerDeleteAll(MARKER_SEQUENCE_VERTICAL);

        //int line1 = firstLine;
        //int line2 = lastLine;

        for (int i = firstLine; i < lastLine; ++i) {
            ctrl->MarkerAdd(i, MARKER_SEQUENCE);
        }

        // Make sure that the seq lines are visible
        int visibleLine = firstLine - 5;
        if (visibleLine < 0) { visibleLine = 0; }

        ctrl->ScrollToLine(visibleLine);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnNextDiffUI(wxUpdateUIEvent& event) { event.Enable(CanNextDiff()); }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPrevDiffUI(wxUpdateUIEvent& event) { event.Enable(CanPrevDiff()); }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyLeftToRightUI(wxUpdateUIEvent& event)
    {
        event.Enable(!IsRightReadOnly() && !m_config.IsSingleViewMode());
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyRightToLeftUI(wxUpdateUIEvent& event)
    {
        event.Enable(!IsLeftReadOnly() && !m_config.IsSingleViewMode());
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyLeftToRight(wxCommandEvent& event)
    {
        //bool moveToNext = /*m_config.GetFlags() & DiffConfig::kCopyLeftToRightAndMove*/ true;
        DoCopyCurrentSequence(m_stcLeft, m_stcRight);
        if (/*moveToNext &&*/ CanNextDiff()) {
            wxCommandEvent dummy;
            OnNextDiffSequence(dummy);
        }
        else
        {
            m_stcLeft->MarkerDeleteAll(MARKER_SEQUENCE);
            m_stcRight->MarkerDeleteAll(MARKER_SEQUENCE);
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyRightToLeft(wxCommandEvent& event)
    {
        //bool moveToNext = /*m_config.GetFlags() & DiffConfig::kCopyRightToLeftAndMove*/ true;
        DoCopyCurrentSequence(m_stcRight, m_stcLeft);
        if (/*moveToNext && */CanNextDiff()) {
            wxCommandEvent dummy;
            OnNextDiffSequence(dummy);
        }
        else
        {
            m_stcLeft->MarkerDeleteAll(MARKER_SEQUENCE);
            m_stcRight->MarkerDeleteAll(MARKER_SEQUENCE);
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoCopyCurrentSequence(wxStyledTextCtrl* from, wxStyledTextCtrl* to)
    {
        if (m_cur_sequence == wxNOT_FOUND) return;
        to->SetReadOnly(false);
        int fromStartPos = wxNOT_FOUND;
        int fromEndPos = wxNOT_FOUND;

        int toStartPos = wxNOT_FOUND;
        int toEndPos = wxNOT_FOUND;
        int placeHolderMarkerFirstLine = wxNOT_FOUND;
        int placeHolderMarkerLastLine = wxNOT_FOUND;
        int dummy;
        DoGetPositionsToCopy(from, fromStartPos, fromEndPos, placeHolderMarkerFirstLine, placeHolderMarkerLastLine);
        if (fromStartPos == wxNOT_FOUND || fromEndPos == wxNOT_FOUND) return;

        DoGetPositionsToCopy(to, toStartPos, toEndPos, dummy, dummy);
        if (toStartPos == wxNOT_FOUND || toEndPos == wxNOT_FOUND) return;

        // remove the old markers from the range of lines
        int toLine1;// = to->LineFromPosition(toStartPos);
        int toLine2;// = to->LineFromPosition(toEndPos);

        DoGetLinesToCopy(to, toLine1, toLine2);

        for (int i = toLine1; i < toLine2; ++i) {
            to->MarkerDelete(i, RED_MARKER);
            to->MarkerDelete(i, GREEN_MARKER);
            to->MarkerDelete(i, PLACE_HOLDER_MARKER);
            to->MarkerDelete(i, MARKER_SEQUENCE);

            from->MarkerDelete(i, RED_MARKER);
            from->MarkerDelete(i, GREEN_MARKER);
            from->MarkerDelete(i, PLACE_HOLDER_MARKER);

            m_overviewPanelMarkers.Item(i) = 0;
        }

        wxString textToCopy = from->GetTextRange(fromStartPos, fromEndPos);
        to->SetSelection(toStartPos, toEndPos);
        to->ReplaceSelection(textToCopy);
        for (int i = placeHolderMarkerFirstLine; i < placeHolderMarkerLastLine; ++i) {
            to->MarkerAdd(i, PLACE_HOLDER_MARKER);
            from->MarkerAdd(i, PLACE_HOLDER_MARKER);
        }

        // Restore the MARKER_SEQUENCE
        for (int i = toLine1; i < toLine2; ++i) {
            to->MarkerAdd(i, MARKER_SEQUENCE);
        }
        to->SetReadOnly(true);

        if (m_config.IsSplitHorizontal()) {
            m_panelOverviewFull->Refresh();
        }
        else {
            m_panelOverviewL->Refresh();
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoGetPositionsToCopy(wxStyledTextCtrl* stc, int& startPos, int& endPos,
        int& placeHolderMarkerFirstLine, int& placeHolderMarkerLastLine)
    {
        startPos = wxNOT_FOUND;
        endPos = wxNOT_FOUND;
        placeHolderMarkerFirstLine = wxNOT_FOUND;

        int from_line = m_sequences.at(m_cur_sequence).first;
        int to_line = m_sequences.at(m_cur_sequence).second;

        for (int i = from_line; i < to_line; ++i) {
            if ((stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK)) {
                placeHolderMarkerFirstLine = i;      // first line of the placholder marker
                placeHolderMarkerLastLine = to_line; // last line of the placholder marker
                break;
            }
        }

        startPos = stc->PositionFromLine(from_line);
        endPos = stc->PositionFromLine(to_line);// +stc->LineLength(to_line);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoGetLinesToCopy(wxStyledTextCtrl* stc, int& from_line, int& to_line)
    {
        from_line = m_sequences.at(m_cur_sequence).first;
        to_line = m_sequences.at(m_cur_sequence).second;
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoSave(wxStyledTextCtrl* stc, const wxFileName& fn)
    {
        if (stc->IsModified())
        {
            // remove all lines that have the 'placeholder' markers
            wxString newContent = DoGetContentNoPlaceholders(stc);

            clMBConv(conv);
            if (!FileUtil::WriteFileContent(fn, newContent, *conv)) {
                ::wxMessageBox(_("Failed to save content to file: ") + fn.GetFullPath(), "gsgs",
                    wxICON_ERROR | wxOK | wxCENTER);
                return;
            }

            stc->SetReadOnly(false);
            stc->SetText(newContent);
            stc->SetReadOnly(true);
            stc->SetSavePoint();
            stc->SetModified(false);

            // Emit a file-saved event
            gsgs_Event().PostFileSavedEvent(fn.GetFullPath());

            // Reload any file opened in gsgs
            gsgs_Event().PostReloadExternallyModifiedEvent(false);
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnSaveChanges(wxCommandEvent& event)
    {
        DoSave(m_stcLeft, m_textCtrlLeftFile->GetValue());
        DoSave(m_stcRight, m_textCtrlRightFile->GetValue());
        Diff();
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnSaveChangesUI(wxUpdateUIEvent& event)
    {
        event.Enable((m_stcLeft->IsModified() || m_stcRight->IsModified()) && !m_config.IsSingleViewMode());
    }
    //----------------------------------------------------------------------------
    bool FileDiffPanel::CanNextDiff()
    {
        bool canNext = ((m_cur_sequence + 1) < (int)m_sequences.size());
        return !m_sequences.empty() && canNext;
    }
    //----------------------------------------------------------------------------
    bool FileDiffPanel::CanPrevDiff()
    {
        bool canPrev = ((m_cur_sequence - 1) >= 0);
        return !m_sequences.empty() && canPrev;
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyFileFromRight(wxCommandEvent& event) 
    { 
        DoCopyFileContent(m_stcRight, m_stcLeft); 
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyFileLeftToRight(wxCommandEvent& event) 
    { 
        DoCopyFileContent(m_stcLeft, m_stcRight); 
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoCopyFileContent(wxStyledTextCtrl* from, wxStyledTextCtrl* to)
    {
        to->SetReadOnly(false);
        wxString newContent = DoGetContentNoPlaceholders(from);
        to->SetText(newContent);
        to->SetReadOnly(true);

        // Clear RED and GREEN markers
        to->MarkerDeleteAll(RED_MARKER);
        to->MarkerDeleteAll(GREEN_MARKER);

        from->MarkerDeleteAll(RED_MARKER);
        from->MarkerDeleteAll(GREEN_MARKER);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPageClosing(wxNotifyEvent& event)
    {
        if (m_stcLeft->IsModified() || m_stcRight->IsModified()) {
            wxStandardID res = ::PromptMessageDialogWithoutCancel(
                _("Closing the diff viewer, will lose all your changes.\nContinue?"), "PromptDiffViewClose");
            if (res != wxID_YES) {
                event.Veto();
            }
            else {
                event.Skip();
            }

        }
        else {
            event.Skip();
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnHorizontal(wxCommandEvent& event)
    {
        if (m_config.IsSplitHorizontal())
            return;
        //m_splitter->Unsplit();
        m_config.SetViewMode(DiffConfig::kViewHorizontalSplit);

        //Diff();
        CallAfter(&FileDiffPanel::DoLayout);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnVertical(wxCommandEvent& event)
    {
        if (m_config.IsSplitVertical())
            return;
        //m_splitter->Unsplit();
        m_config.SetViewMode(DiffConfig::kViewVerticalSplit);

        //Diff();
        CallAfter(&FileDiffPanel::DoLayout);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DiffNew()
    {
        //m_staticTextLeft->Hide();
        //m_staticTextRight->Hide();
        if (m_storeFilepaths) {
            m_flags = kSavePaths; // store the paths on exit
        }
        //m_config.SetViewMode(DiffConfig::kViewVerticalSplit);
        //m_splitter->Unsplit();
        //m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);

        // Restore last used paths
        //m_config.Load();
        //m_textCtrlLeftFile->ChangeValue(m_config.GetLeftFile());
        //m_textCtrlRightFile->ChangeValue(m_config.GetRightFile());
        //CallAfter(&FileDiffPanel::Diff); // trigger a diff
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DiffNew(const wxFileName& left, const wxFileName& right)
    {
        if (!left.Exists()) {
            ::wxMessageBox(wxString() << _("Left Side File:\n") << left.GetFullPath() << _(" does not exist!"), "gsgs",
                wxICON_ERROR | wxCENTER | wxOK);
            return;
        }

        if (!right.Exists()) {
            ::wxMessageBox(wxString() << _("Right Side File:\n") << right.GetFullPath() << _(" does not exist!"),
                "gsgs", wxICON_ERROR | wxCENTER | wxOK);
            return;
        }

        //m_staticTextLeft->Hide();
        //m_staticTextRight->Hide();
        if (m_storeFilepaths) {
            m_flags = kSavePaths; // store the paths on exit
        }
        m_config.SetViewMode(DiffConfig::kViewVerticalSplit);
        m_splitter->Unsplit();
        m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);

        // Restore last used paths
        m_config.Load();
        m_textCtrlLeftFile->ChangeValue(left.GetFullPath());
        m_textCtrlRightFile->ChangeValue(right.GetFullPath());

        CallAfter(&FileDiffPanel::Diff); // trigger a diff
    }
    //----------------------------------------------------------------------------
    //void FileDiffPanel::OnRefreshDiffUI(wxUpdateUIEvent& event) { wxUnusedVar(event); }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnLeftPickerUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(!IsOriginSourceControl()); 
        m_textCtrlLeftFile->Enable(!IsOriginSourceControl());
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnRightPickerUI(wxUpdateUIEvent& event) 
    { 
        event.Enable(!IsOriginSourceControl()); 
        m_textCtrlRightFile->Enable(!IsOriginSourceControl());
    }
    //----------------------------------------------------------------------------
    wxString FileDiffPanel::DoGetContentNoPlaceholders(wxStyledTextCtrl* stc) const
    {
        wxString newContent;
        for (int i = 0; i < stc->GetLineCount(); ++i) {
            if (!(stc->MarkerGet(i) & PLACE_HOLDER_MARKER_MASK)) { newContent << stc->GetLine(i); }
        }
        return newContent;
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnMenuCopyLeft2Right(wxCommandEvent& event)
    {
        size_t flags = m_config.GetFlags();

        flags &= ~DiffConfig::kCopyLeftToRightAndMove;

        if (event.IsChecked() && event.GetId() == ID_COPY_LEFT_TO_RIGHT_AND_MOVE) {
            // save the new settings
            flags |= DiffConfig::kCopyLeftToRightAndMove;
        }
        m_config.SetFlags(flags);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnMenuCopyRight2Left(wxCommandEvent& event)
    {
        size_t flags = m_config.GetFlags();

        flags &= ~DiffConfig::kCopyRightToLeftAndMove;

        if (event.IsChecked() && event.GetId() == ID_COPY_RIGHT_TO_LEFT_AND_MOVE) {
            // save the new settings
            flags |= DiffConfig::kCopyRightToLeftAndMove;
        }
        m_config.SetFlags(flags);
    }
    //----------------------------------------------------------------------------
    /*void FileDiffPanel::OnSingleUI(wxUpdateUIEvent& event) 
    { 
        event.Check(m_config.IsSingleViewMode()); 
    }*/
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnSingleView(wxCommandEvent& event)
    {
        if (m_config.IsSingleViewMode())
            return;
        m_config.SetViewMode(DiffConfig::kViewSingle);
        //m_splitter->Unsplit();

        //Diff();
        CallAfter(&FileDiffPanel::DoLayout);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::DoLayout()
    {
        wxWindowUpdateLocker llt(this);
        if (m_config.IsSingleViewMode()) {
            m_panelOverviewFull->Hide();
            m_panelOverviewL->Show(m_config.IsOverviewBarShown());
            m_splitter->Unsplit();
        }
        else if (m_config.IsSplitHorizontal()) {
            m_splitter->Unsplit();
            m_panelOverviewFull->Show(m_config.IsOverviewBarShown());
            m_panelOverviewL->Hide();
            m_splitter->SplitHorizontally(m_splitterPageLeft, m_splitterPageRight);
        }
        else if (m_config.IsSplitVertical()) {
            m_splitter->Unsplit();
            m_panelOverviewFull->Hide();
            m_panelOverviewL->Show(m_config.IsOverviewBarShown());
            m_splitter->SplitVertically(m_splitterPageLeft, m_splitterPageRight);
        }
        GetSizer()->Layout();
        Refresh();
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnMouseWheel(wxMouseEvent& event)
    {
        event.Skip();
        if (::wxGetKeyState(WXK_CONTROL) && !gsgs_EditorConfig().GetOptions()->IsMouseZoomEnabled()) {
            event.Skip(false);
            return;
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnBrowseLeftFile(wxCommandEvent& event)
    {
        wxFileName path(m_textCtrlLeftFile->GetValue());
        wxString file = ::wxFileSelector(_("Choose a file"), path.GetPath());
        if (!file.IsEmpty()) { m_textCtrlLeftFile->ChangeValue(file); }
        CallAfter(&FileDiffPanel::Diff);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnBrowseRightFile(wxCommandEvent& event)
    {
        wxFileName path(m_textCtrlRightFile->GetValue());
        wxString file = ::wxFileSelector(_("Choose a file"), path.GetPath());
        if (!file.IsEmpty()) { m_textCtrlRightFile->ChangeValue(file); }
        CallAfter(&FileDiffPanel::Diff);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnIgnoreWhitespaceClicked(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        m_config.SetIgnoreWhitespace(event.IsChecked());
        m_config.Save();
        Diff();
        Refresh();
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnIgnoreWhitespaceUI(wxUpdateUIEvent& event)
    {
        event.Check(m_config.IsIgnoreWhitespace());
        event.Enable(!m_config.IsSingleViewMode());
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnShowLinenosClicked(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        m_config.SetShowLineNumbers(event.IsChecked());
        m_config.Save();
        Diff();
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnShowLinenosUI(wxUpdateUIEvent& event) 
    { 
        event.Check(m_config.ShowLineNumbers()); 
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnShowOverviewBarClicked(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        m_config.SetOverviewBarShow(event.IsChecked());
        m_config.Save();
        CallAfter(&FileDiffPanel::DoLayout);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnShowOverviewBarUI(wxUpdateUIEvent& event) 
    { 
        event.Check(m_config.IsOverviewBarShown()); 
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPaint(wxPaintEvent& event)
    {
        if (!m_config.IsOverviewBarShown()) { return; }

        wxWindow* win;
        if (m_config.IsSplitHorizontal()) {
            win = m_splitter;
        }
        else if (m_config.IsSingleViewMode()) {
            win = m_stcLeft;
        }
        else {
            win = m_stcRight;
        }
        wxWindow* panel = dynamic_cast<wxWindow*>(event.GetEventObject());

        size_t lines = m_stcLeft->GetLineCount();
        if (!lines || !win || !panel->IsShown()) { return; }

        int yOffset = 0, x1 = panel->GetClientSize().GetWidth();// -1;
        int ht = win->GetClientSize().GetHeight();

        if (m_config.IsSplitHorizontal()) {
            // It's harder if we have to span 2 wxSTCs...
            yOffset = (ht / 2) - m_stcLeft->GetSize().GetHeight(); // The height of the text bits above the stc
            ht -= yOffset;
        }
        else {
            // Without this, for short files the markers will be below the corresponding diffs
            ht = wxMin(ht, lines * m_stcLeft->TextHeight(0));
        }

        //int pixelsPerLine = wxMax(ht / lines, 1);
        int pixelsPerLine = m_stcLeft->TextHeight(0);

        //wxDC& dc = *event.GetDC();
        wxAutoBufferedPaintDC dc1(panel);
        wxGCDC dc(dc1);

        // Set a distinctive background colour, as the standard panel bg is the same as its container
        // NB When deciding whether to use dark-theme colours, don't use the gsgs theme which
        // probably won't affect the bar; measure its actual colour
        wxColour bg = panel->GetBackgroundColour();
        bool isDarkTheme = (bg.Red() + bg.Blue() + bg.Green()) < 384;
        bg.ChangeLightness(isDarkTheme ? 105 : 95);
        dc.SetBrush(bg);
        dc.SetPen(bg);
        //dc.DrawRectangle(0, yOffset, x1, ht - yOffset);
        dc.DrawRectangle(panel->GetClientRect());
        if (!m_overviewPanelMarkers.GetCount()) { return; }

        //if (m_stcLeft->LinesOnScreen() < lines) {
            // Make it clearer which markers correspond to any currently-displayed diff-lines
            int topVisibleLine = m_stcLeft->GetFirstVisibleLine();
            size_t depth = wxMin(m_stcLeft->LinesOnScreen(), lines);

            /*dc.SetBrush(bg.ChangeLightness(isDarkTheme ? 110 : 90));
            dc.SetPen(bg.ChangeLightness(isDarkTheme ? 120 : 80));
            if (m_config.IsSplitHorizontal()) {
                dc.DrawRectangle(0, yOffset + topVisibleLine * (ht - yOffset) / lines, x1,
                    wxMax(depth * (ht - yOffset) / lines, 1));
            }
            else {
                dc.DrawRectangle(0, yOffset + topVisibleLine * ht / lines, x1, wxMax(depth * ht / lines, 1));
            }*/
        //}

        dc.SetPen(gsgs_Style().GetActiveMarkerColour());
        dc.SetBrush(gsgs_Style().GetActiveMarkerColour());
        //for (size_t n = 0; n < (size_t)lines; ++n) {
        for (size_t n = 0; n < depth; ++n) {
            if (m_overviewPanelMarkers.Item(n + topVisibleLine)) {
                if (pixelsPerLine > 1) {
                    if (m_config.IsSplitHorizontal()) {
                        dc.DrawRectangle(0, yOffset + (n * pixelsPerLine), x1, pixelsPerLine);
                    }
                    else {
                        //dc.DrawRectangle(0, yOffset + (n * ht) / lines, x1,
                        //    pixelsPerLine); // Don't use pixelsPerLine for y0; it's wrong for short files
                        dc.DrawRectangle(0, yOffset + n * pixelsPerLine, x1,
                            pixelsPerLine); // Don't use pixelsPerLine for y0; it's wrong for short files
                    }
                }
                else {
                    if (m_config.IsSplitHorizontal()) {
                        int y = (n * (ht - yOffset)) / lines;
                        dc.DrawLine(0, yOffset + y, x1, yOffset + y);
                    }
                    else {
                        int y = n * ht / lines;
                        dc.DrawLine(0, yOffset + y, x1, yOffset + y);
                    }
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPaneloverviewLeftDown(wxMouseEvent& event)
    {
        event.Skip();

        if (!m_config.IsOverviewBarShown()) { return; }

        wxWindow* panel = static_cast<wxWindow*>(event.GetEventObject());
        wxWindow* win;
        if (m_config.IsSplitHorizontal()) {
            win = m_splitter;
        }
        else if (m_config.IsSingleViewMode()) {
            win = m_stcLeft;
        }
        else {
            win = m_stcRight;
        }

        int yOffset = 0, ht = win->GetClientSize().GetHeight();
        int pos = event.GetPosition().y;
        int lines = m_stcLeft->GetLineCount();

        if (m_config.IsSplitHorizontal()) {
            // It's harder if we have to span 2 wxSTCs...
            yOffset = (ht / 2) - m_stcLeft->GetSize().GetHeight(); // The height of the text bits above the stc
            pos -= yOffset;
            ht -= (2 * yOffset);
        }

        const int extra = 10; // Allow clicks just above/below the 'bar' to succeed
        if (!lines || !win || !panel->IsShown() || pos > (ht + extra) || pos < -extra) { return; }

        // Make the wxSTCs scroll to the line matching the mouse-click
        m_stcLeft->ScrollToLine((lines * pos) / ht);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnCopyAllMenu(wxCommandEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("ID_DIFF_TOOL_COPY_ALL_RIGHT_TO_LEFT"), _("Copy all: left <- right"));
        menu.Append(XRCID("ID_DIFF_TOOL_COPY_ALL_LEFT_TO_RIGHT"), _("Copy all: left -> right"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnCopyFileLeftToRight, this,
            XRCID("ID_DIFF_TOOL_COPY_ALL_LEFT_TO_RIGHT"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnCopyFileFromRight, this,
            XRCID("ID_DIFF_TOOL_COPY_ALL_RIGHT_TO_LEFT"));

        m_toolbar->ShowMenuForButton(XRCID("ID_DIFF_TOOL_COPY_ALL"), &menu);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnViewMenu(wxCommandEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("ID_DIFF_TOOL_VIEW_SINGLE"), _("Single View"), "", wxITEM_RADIO);
        menu.Check(XRCID("ID_DIFF_TOOL_VIEW_SINGLE"), m_config.IsSingleViewMode());

        menu.Append(XRCID("ID_DIFF_TOOL_VIEW_HORIZONTAL"), _("Horizontal Split"), "", wxITEM_RADIO);
        menu.Check(XRCID("ID_DIFF_TOOL_VIEW_HORIZONTAL"), m_config.IsSplitHorizontal());

        menu.Append(XRCID("ID_DIFF_TOOL_VERTICAL_VIEW"), _("Vertical Split"), "", wxITEM_RADIO);
        menu.Check(XRCID("ID_DIFF_TOOL_VERTICAL_VIEW"), m_config.IsSplitVertical());

        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnSingleView, this, XRCID("ID_DIFF_TOOL_VIEW_SINGLE"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnHorizontal, this, XRCID("ID_DIFF_TOOL_VIEW_HORIZONTAL"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnVertical, this, XRCID("ID_DIFF_TOOL_VERTICAL_VIEW"));

        m_toolbar->ShowMenuForButton(XRCID("ID_DIFF_TOOL_VIEW"), &menu);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnPreferences(wxCommandEvent& event)
    {
        wxMenu menu;
        menu.Append(XRCID("ID_DIFF_TOOL_IGNORE_WHITESPACE"), _("Ignore Whitespace"), "", wxITEM_CHECK);
        menu.Check(XRCID("ID_DIFF_TOOL_IGNORE_WHITESPACE"), m_config.IsIgnoreWhitespace());

        menu.Append(XRCID("ID_DIFF_TOOL_SHOW_LINENUMBERS"), _("Show Line Numbers"), "", wxITEM_CHECK);
        menu.Check(XRCID("ID_DIFF_TOOL_SHOW_LINENUMBERS"), m_config.ShowLineNumbers());

        menu.Append(XRCID("ID_DIFF_TOOL_SHOW_OVERVIEW_BAR"), _("Show Overview Bar"), "", wxITEM_CHECK);
        menu.Check(XRCID("ID_DIFF_TOOL_SHOW_OVERVIEW_BAR"), m_config.IsOverviewBarShown());

        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnIgnoreWhitespaceClicked, this,
            XRCID("ID_DIFF_TOOL_IGNORE_WHITESPACE"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnShowLinenosClicked, this, XRCID("ID_DIFF_TOOL_SHOW_LINENUMBERS"));
        menu.Bind(wxEVT_MENU, &FileDiffPanel::OnShowOverviewBarClicked, this,
            XRCID("ID_DIFF_TOOL_SHOW_OVERVIEW_BAR"));
        m_toolbar->ShowMenuForButton(XRCID("ID_DIFF_TOOL_SETTINGS"), &menu);
    }
    //----------------------------------------------------------------------------
    void FileDiffPanel::OnFind(wxCommandEvent&)
    {
        wxWindow* win = wxWindow::FindFocus();
        wxStyledTextCtrl* editor = dynamic_cast<wxStyledTextCtrl*>(win);
        if (!editor) { editor = m_stcLeft; }
        m_findBar->SetEditor(editor);
        if (!m_findBar->IsShown()) {
            m_findBar->Show(editor->GetSelectedText(), false);
            GetSizer()->Layout();
        }
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // FileDiffFrame
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    FileDiffFrame::FileDiffFrame(wxWindow* parent, const FileDiffPanel::FileInfo& left,
                             const FileDiffPanel::FileInfo& right, bool originSourceControl)
        : wxFrame(parent, wxID_ANY, _("gsgs - Diff View"), wxDefaultPosition, wxSize(1600, 1000),
                  wxFRAME_FLOAT_ON_PARENT)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetSizer(new wxBoxSizer(wxVERTICAL));
        m_diffView = new FileDiffPanel(this);
        m_diffView->SetFilesDetails(left, right);
        if(originSourceControl) { m_diffView->SetOriginSourceControl(); }
        GetSizer()->Add(m_diffView, 1, wxEXPAND, 0);

        /*wxIconBundle b;
        {
            wxIcon icn;
            icn.CopyFromBitmap(gsgs_Image().LoadBitmap("diff"));
            b.AddIcon(icn);
        }
        SetIcons(b);*/
        CreateMenuBar();
        CenterOnScreen();

        m_diffView->Diff();
    }
    //----------------------------------------------------------------------------
    FileDiffFrame::FileDiffFrame(wxWindow* parent)
        : wxFrame(parent, wxID_ANY, _("gsgs - Diff View"), wxDefaultPosition, wxSize(1600, 1000),
                     wxFRAME_FLOAT_ON_PARENT)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetSizer(new wxBoxSizer(wxVERTICAL));
        m_diffView = new FileDiffPanel(this);
        GetSizer()->Add(m_diffView, 1, wxEXPAND, 0);
        /*wxIconBundle b;
        {
            wxIcon icn;
            icn.CopyFromBitmap(gsgs_Image().LoadBitmap("diff"));
            b.AddIcon(icn);
        }
        SetIcons(b);*/
        CreateMenuBar();
        CenterOnScreen();
        Layout();
        m_diffView->DiffNew();
    }
    //----------------------------------------------------------------------------
    FileDiffFrame::FileDiffFrame(wxWindow* parent, const wxFileName& left, const wxFileName& right, bool isTempFile)
        : wxFrame(parent, wxID_ANY, _("gsgs - Diff View"), wxDefaultPosition, wxSize(1600, 1000),
                  wxFRAME_FLOAT_ON_PARENT)
    {
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetSizer(new wxBoxSizer(wxVERTICAL));
        m_diffView = new FileDiffPanel(this);
        GetSizer()->Add(m_diffView, 1, wxEXPAND, 0);
        if(isTempFile) { m_diffView->SetSaveFilepaths(false); }

        /*wxIconBundle b;
        {
            wxIcon icn;
            icn.CopyFromBitmap(gsgs_Image().LoadBitmap("diff"));
            b.AddIcon(icn);
        }
        SetIcons(b);*/
        CreateMenuBar();
        CenterOnScreen();
        Layout();

        m_diffView->DiffNew(left, right);
    }
    //----------------------------------------------------------------------------
    FileDiffFrame::~FileDiffFrame() {}
    //----------------------------------------------------------------------------
    void FileDiffFrame::CreateMenuBar()
    {
        //SetMenuBar(wxXmlResource::Get()->LoadMenuBar("diff_menu_bar"));
        /*Bind(wxEVT_MENU, &FileDiffPanel::OnSaveChanges, m_diffView, wxID_SAVE);
        Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnSaveChangesUI, m_diffView, wxID_SAVE);
        //Bind(wxEVT_MENU, &FileDiffPanel::OnFind, m_diffView, wxID_FIND);
        Bind(wxEVT_MENU, &FileDiffPanel::OnNextDiffSequence, m_diffView, XRCID("ID_DIFF_TOOL_NEXT"));
        Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnNextDiffUI, m_diffView, XRCID("ID_DIFF_TOOL_NEXT"));
        Bind(wxEVT_MENU, &FileDiffPanel::OnPrevDiffSequence, m_diffView, XRCID("ID_DIFF_TOOL_PREV"));
        Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnPrevDiffUI, m_diffView, XRCID("ID_DIFF_TOOL_PREV"));

        Bind(wxEVT_MENU, &FileDiffPanel::OnCopyLeftToRight, m_diffView, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
        Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyLeftToRightUI, m_diffView, XRCID("ID_DIFF_TOOL_COPY_RIGHT"));
        Bind(wxEVT_MENU, &FileDiffPanel::OnCopyRightToLeft, m_diffView, XRCID("ID_DIFF_TOOL_COPY_LEFT"));
        Bind(wxEVT_UPDATE_UI, &FileDiffPanel::OnCopyRightToLeftUI, m_diffView, XRCID("ID_DIFF_TOOL_COPY_LEFT"));

        Bind(wxEVT_MENU, &FileDiffPanel::OnCopyFileLeftToRight, m_diffView, XRCID("ID_DIFF_TOOL_COPY_ALL_LEFT_TO_RIGHT"));
        Bind(wxEVT_MENU, &FileDiffPanel::OnCopyFileFromRight, m_diffView, XRCID("ID_DIFF_TOOL_COPY_ALL_RIGHT_TO_LEFT"));
        */
        Bind(wxEVT_MENU, &FileDiffFrame::OnClose, this, wxID_CLOSE);
    }
    //----------------------------------------------------------------------------
    void FileDiffFrame::OnClose(wxCommandEvent&)
    {
        //wxUnusedVar(event);
        Close();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // DiffFoldersFrame
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    static int nCallCounter = 0;
    static std::atomic_bool checksumThreadStop;
    //-----------------------------------------------------------------------------------
    DiffFoldersFrame::DiffFoldersFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
        const wxSize& size, long style)
        : wxDialog(parent, id, title, pos, size, style)
    {
        wxBoxSizer* boxSizer2 = new wxBoxSizer(wxVERTICAL);
        this->SetSizer(boxSizer2);

        m_panel4 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        boxSizer2->Add(m_panel4, 1, wxEXPAND, WXC_FROM_DIP(5));

        wxBoxSizer* boxSizer6 = new wxBoxSizer(wxVERTICAL);
        m_panel4->SetSizer(boxSizer6);

        m_toolbar = new ToolBar(m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT);
        m_toolbar->SetToolBitmapSize(wxSize(16, 16));

        boxSizer6->Add(m_toolbar, 0, wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl = new ThemedListCtrl(m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxDV_ROW_LINES | wxDV_SINGLE);

        boxSizer6->Add(m_dvListCtrl, 1, wxEXPAND, WXC_FROM_DIP(5));

        m_dvListCtrl->AppendIconTextColumn(_("Left"), wxDATAVIEW_CELL_INERT, size.GetWidth()/2, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);
        m_dvListCtrl->AppendIconTextColumn(_("Right"), wxDATAVIEW_CELL_INERT, size.GetWidth() / 2, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE);

        if (GetParent()) {
            CentreOnParent(wxBOTH);
        }
        else {
            CentreOnScreen(wxBOTH);
        }

        // Connect events
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(DiffFoldersFrame::OnItemActivated), NULL, this);
        m_dvListCtrl->Connect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
            wxDataViewEventHandler(DiffFoldersFrame::OnItemContextMenu), NULL, this);

        ImageManager * imagemag = &gsgs_Image();
        checksumThreadStop.store(false);
        m_toolbar->SetMiniToolBar(false);
        m_toolbar->AddTool(wxID_NEW, _("New comparison"), imagemag->LoadBitmap("file_new"));
        m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), imagemag->LoadBitmap("file_reload"));
        m_toolbar->AddTool(XRCID("diff-intersection"), _("Show similar files only"),
            imagemag->LoadBitmap("intersection"), "", wxITEM_CHECK);
        m_toolbar->AddTool(XRCID("diff-up-folder"), _("Parent folder"), imagemag->LoadBitmap("up"));
        m_toolbar->AddTool(wxID_CLOSE, _("Close"), imagemag->LoadBitmap("file_close"));

        m_toolbar->Realize();
        m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnNewCmparison, this, wxID_NEW);
        m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnClose, this, wxID_CLOSE);
        m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnShowSimilarFiles, this, XRCID("diff-intersection"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnShowSimilarFilesUI, this, XRCID("diff-intersection"));
        m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnRefresh, this, wxID_REFRESH);
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnRefreshUI, this, wxID_REFRESH);
        m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnUpFolder, this, XRCID("diff-up-folder"));
        m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnUpFolderUI, this, XRCID("diff-up-folder"));

        //::SetTLWindowBestSizeAndPosition(this);

        // Load persistent items
        gsgs_Config().Read("DiffFolders/ShowSimilarItems", m_showSimilarItems, false);
    }
    //-----------------------------------------------------------------------------------
    DiffFoldersFrame::~DiffFoldersFrame()
    {
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,
            wxDataViewEventHandler(DiffFoldersFrame::OnItemActivated), NULL, this);
        m_dvListCtrl->Disconnect(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU,
            wxDataViewEventHandler(DiffFoldersFrame::OnItemContextMenu), NULL, this);

        gsgs_Config().Write("DiffFolders/ShowSimilarItems", m_showSimilarItems);
        StopChecksumThread();
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnClose(wxCommandEvent& event)
    {
        EndModal(wxID_OK);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnNewCmparison(wxCommandEvent& event)
    {
        wxString left, right;
        gsgs_Config().Read("DiffFolders/Left", left, wxString());
        gsgs_Config().Read("DiffFolders/Right", right, wxString());

        DiffSelectFoldersDlg dlg(this, left, right);
        if (dlg.ShowModal() == wxID_OK) {
            left = dlg.GetDirPickerLeft()->GetPath();
            right = dlg.GetDirPickerRight()->GetPath();
            gsgs_Config().Write("DiffFolders/Left", left);
            gsgs_Config().Write("DiffFolders/Right", right);
            m_depth = 0;
            CallAfter(&DiffFoldersFrame::BuildTrees, left, right);
        }
    }
    //-----------------------------------------------------------------------------------
    #define CLOSE_FP(fp)      \
        {                     \
            if(fp) {          \
                fclose(fp);   \
                fp = nullptr; \
            }                 \
        }
    //-----------------------------------------------------------------------------------
    static bool CompareFilesCheckSum(const wxString& fn1, const wxString& fn2)
    {
        // The sizes are the same
        unsigned char checksum1 = 0;
        unsigned char checksum2 = 0;

        FILE* fp1 = fopen(fn1.mb_str(), "rb");
        FILE* fp2 = fopen(fn2.mb_str(), "rb");
        if (!fp1 || !fp2) {
            CLOSE_FP(fp1);
            CLOSE_FP(fp2);
            return false;
        }

        while (!feof(fp1) && !ferror(fp1) && !feof(fp2) && !ferror(fp2)) {
            checksum1 ^= fgetc(fp1);
            checksum2 ^= fgetc(fp2);
            if (checksum1 != checksum2) { break; }
        }
        CLOSE_FP(fp1);
        CLOSE_FP(fp2);
        return (checksum1 == checksum2);
    }
    //-----------------------------------------------------------------------------------
    static void HelperThreadCalculateChecksum(int callId, const wxArrayString& items, const wxString& left,
        const wxString& right, DiffFoldersFrame* sink)
    {
        wxArrayString results;
        for (size_t i = 0; i < items.size(); ++i) {
            if (checksumThreadStop.load()) { break; }
            wxFileName fnLeft(left, items.Item(i));
            wxFileName fnRight(right, items.Item(i));
            if (fnLeft.IsOk() && fnLeft.FileExists() && fnRight.IsOk() && fnRight.FileExists()) {
                if (fnLeft.GetSize() != fnRight.GetSize()) {
                    // If the size is different, no need to go further
                    results.Add("different");
                }
                else {
                    bool isSame = CompareFilesCheckSum(fnLeft.GetFullPath(), fnRight.GetFullPath());
                    results.Add(isSame ? "same" : "different");
                }
            }
            else {
                results.Add("n/a"); // Dont know
            }
        }
        if (!checksumThreadStop.load()) { sink->CallAfter(&DiffFoldersFrame::OnChecksum, callId, results); }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::BuildTrees(const wxString& left, const wxString& right)
    {
        StopChecksumThread();
        wxBusyCursor bc;
        m_dvListCtrl->DeleteAllItems();
        m_entries.clear();
        m_dvListCtrl->SetSortFunction(nullptr);
        m_leftFolder = left;
        m_rightFolder = right;
        m_dvListCtrl->GetColumn(0)->SetLabel(left);
        m_dvListCtrl->GetColumn(1)->SetLabel(right);
        m_dvListCtrl->SetBitmaps(&gsgs_Image().GetBitmaps());

        // Set up the roots
        wxVector<wxVariant> cols;

        FileScan::EntryData::Vec_t leftFiles;
        FileScan::EntryData::Vec_t rightFiles;

        FileScan scanner;
        scanner.ScanNoRecurse(left, leftFiles);
        scanner.ScanNoRecurse(right, rightFiles);

        // Get list of all files in the given folders
        DiffView viewList;

        // Add all the files
        size_t count = wxMax(leftFiles.size(), rightFiles.size());
        for (size_t i = 0; i < count; ++i) {
            if (i < leftFiles.size()) {
                const FileScan::EntryData& entry = leftFiles[i];
                wxFileName fn(entry.fullpath);
                wxString fullname = fn.GetFullName();
                if (!viewList.HasFile(fullname)) { viewList.CreateEntry(entry, true); }
                viewList.GetEntry(fullname).SetLeft(entry);
            }

            if (i < rightFiles.size()) {
                const FileScan::EntryData& entry = rightFiles[i];
                wxFileName fn(entry.fullpath);
                wxString fullname = fn.GetFullName();
                if (!viewList.HasFile(fullname)) { viewList.CreateEntry(entry, false); }
                viewList.GetEntry(fullname).SetRight(entry);
            }
        }

        // Sort the merged list
        m_entries = viewList.ToSortedVector();
        wxArrayString displayedItems;
        for (size_t i = 0; i < m_entries.size(); ++i) {
            cols.clear();
            const DiffViewEntry& entry = m_entries[i];

            // If the "show similar files" button is clicked, display only files that exists in both lists
            if (m_showSimilarItems && !entry.IsExistsInBoth()) { continue; }

            // This will be passed to the checksum thread
            displayedItems.Add(entry.GetFullName());

            if (entry.IsExistsInLeft()) {
                cols.push_back(::MakeBitmapIndexText(entry.GetLeft().fullpath, entry.GetImageId(true)));
            }
            else {
                cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
            }

            if (entry.IsExistsInRight()) {
                cols.push_back(::MakeBitmapIndexText(entry.GetRight().fullpath, entry.GetImageId(false)));
            }
            else {
                cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
            }
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)&entry);
        }

        m_checksumThread = new std::thread(&HelperThreadCalculateChecksum, (++nCallCounter), displayedItems, m_leftFolder,
            m_rightFolder, this);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnItemActivated(wxDataViewEvent& event)
    {
        DiffViewEntry* entry = reinterpret_cast<DiffViewEntry*>(m_dvListCtrl->GetItemData(event.GetItem()));
        if (entry)
        {
            if (entry->IsExistsInBoth() && (entry->GetLeft().flags & FileScan::kIsFolder) &&
                (entry->GetRight().flags & FileScan::kIsFolder)) {
                // Refresh the view to the current folder
                wxFileName left(m_leftFolder, "");
                wxFileName right(m_rightFolder, "");
                left.AppendDir(entry->GetFullName());
                right.AppendDir(entry->GetFullName());
                m_leftFolder = left.GetPath();
                m_rightFolder = right.GetPath();
                ++m_depth;
                CallAfter(&DiffFoldersFrame::BuildTrees, m_leftFolder, m_rightFolder);
            }
            else {
                DoOpenDiff(event.GetItem());
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnItemContextMenu(wxDataViewEvent& event)
    {
        wxDataViewItem item = event.GetItem();
        wxString left = m_dvListCtrl->GetItemText(item, 0);
        wxString right = m_dvListCtrl->GetItemText(item, 1);

        wxMenu menu;
        if (!right.IsEmpty()) {
            menu.Append(XRCID("diff-copy-right-to-left"), _("Copy from Right to Left"));
            menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToLeft, this, XRCID("diff-copy-right-to-left"));
        }

        if (!left.IsEmpty()) {
            menu.Append(XRCID("diff-copy-left-to-right"), _("Copy from Left to Right"));
            menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToRight, this, XRCID("diff-copy-left-to-right"));
        }
        if (menu.GetMenuItemCount()) { menu.AppendSeparator(); }

        if (!right.IsEmpty() && !left.IsEmpty()) {
            menu.Append(XRCID("diff-open-diff"), _("Diff"));
            menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnMenuDiff, this, XRCID("diff-open-diff"));
        }
        if (menu.GetMenuItemCount() == 0) { return; }
        m_dvListCtrl->PopupMenu(&menu);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnShowSimilarFiles(wxCommandEvent& event)
    {
        event.Skip();
        m_showSimilarItems = event.IsChecked();
        BuildTrees(m_leftFolder, m_rightFolder);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnShowSimilarFilesUI(wxUpdateUIEvent& event)
    {
        if (m_leftFolder.IsEmpty() || m_rightFolder.IsEmpty()) {
            event.Check(false);
            event.Enable(false);
        }
        else {

            event.Enable(true);
            event.Check(m_showSimilarItems);
        }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::DoOpenDiff(const wxDataViewItem& item)
    {
        if (!item.IsOk()) { return; }
        wxString leftFile = m_dvListCtrl->GetItemText(item, 0);
        wxString rightFile = m_dvListCtrl->GetItemText(item, 1);
        if (leftFile.IsEmpty() || rightFile.IsEmpty()) { return; }

        wxFileName fnLeft(leftFile);
        wxFileName fnRight(rightFile);
        FileDiffFrame *diffFiles = new FileDiffFrame(this, fnLeft, fnRight, false);
        diffFiles->Show();
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnMenuDiff(wxCommandEvent& event) { DoOpenDiff(m_dvListCtrl->GetSelection()); }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnCopyToRight(wxCommandEvent& event)
    {
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        CHECK_ITEM_RET(item);
        wxString fullname = m_dvListCtrl->GetItemText(item, 0);
        wxFileName source(m_leftFolder, fullname);
        wxFileName target(m_rightFolder, fullname);
        if (::wxCopyFile(source.GetFullPath(), target.GetFullPath())) { m_dvListCtrl->SetItemText(item, fullname, 1); }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnCopyToLeft(wxCommandEvent& event)
    {
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        CHECK_ITEM_RET(item);
        wxString fullname = m_dvListCtrl->GetItemText(item, 1);
        wxFileName source(m_rightFolder, fullname);
        wxFileName target(m_leftFolder, fullname);
        if (::wxCopyFile(source.GetFullPath(), target.GetFullPath())) { m_dvListCtrl->SetItemText(item, fullname, 0); }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnChecksum(int callId, const wxArrayString& checksumArray)
    {
        if (callId != nCallCounter) { return; }
        bool isDark = Colour::IsDark(m_dvListCtrl->GetColours().GetBgColour());
        wxColour modifiedColour = isDark ? wxColour("rgb(255, 128, 64)") : *wxRED;
        for (size_t i = 0; i < checksumArray.size(); ++i) {
            const wxString& answer = checksumArray.Item(i);
            if (answer == "different") {
                wxDataViewItem item = m_dvListCtrl->RowToItem(i);
                if (item.IsOk()) {
                    m_dvListCtrl->SetItemTextColour(item, modifiedColour, 0);
                    m_dvListCtrl->SetItemTextColour(item, modifiedColour, 1);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnRefresh(wxCommandEvent&)
    {
        //wxUnusedVar(event);
        BuildTrees(m_leftFolder, m_rightFolder);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnRefreshUI(wxUpdateUIEvent& event)
    {
        event.Enable(!m_leftFolder.IsEmpty() && !m_rightFolder.IsEmpty());
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::StopChecksumThread()
    {
        checksumThreadStop.store(false);
        if (m_checksumThread)
        {
            m_checksumThread->join();
        }
        checksumThreadStop.store(false);
        wxDELETE(m_checksumThread);
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnUpFolder(wxCommandEvent& event)
    {
        if (!CanUp()) { return; }

        wxFileName fnLeft(m_leftFolder, "");
        wxFileName fnRight(m_rightFolder, "");

        fnLeft.RemoveLastDir();
        fnRight.RemoveLastDir();
        --m_depth;

        BuildTrees(fnLeft.GetPath(), fnRight.GetPath());
    }
    //-----------------------------------------------------------------------------------
    void DiffFoldersFrame::OnUpFolderUI(wxUpdateUIEvent& event)
    {
        event.Enable(CanUp());
    }
    //-----------------------------------------------------------------------------------
    bool DiffFoldersFrame::CanUp() const
    {
        wxFileName fnLeft(m_leftFolder, "");
        wxFileName fnRight(m_rightFolder, "");
        return m_depth && fnLeft.GetDirCount() && fnRight.GetDirCount();
    }
    //-----------------------------------------------------------------------------------

}