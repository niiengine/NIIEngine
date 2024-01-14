
#include "gsgsWindowManager.h"
#include "gsgsPathManager.h"
#include "gsgsFileUtil.h"
#include "NewProjectDialog.h"
#include "gsgsLogManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsStatusBar.h"
#include "gsgsNotebook.h"
#include "gsgsParseWorker.h"
//#include "gsgsManager.h"
#include "globals.h"
#include <wx/control.h>
#include <wx/aui/aui.h>
#include <wx/aui/floatpane.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/persist/bookctrl.h>
#include <wx/persist/toplevel.h>

#define CHECK_SHUTDOWN() { if(ParseThreadST::Get()->IsStop()) { return; } }

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // HotKeyData
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    class HotKeyData 
    {
    public:
        HotKeyData(){}
        ~HotKeyData(){}

        HotKeyData & Load()
        {
            JSONObject menus = gsgs_Config().GetObject("HotKey");
            if(menus.isValid())
                m_bindings.clear();

            int arrSize = menus.getArraySize();
            for(int i = 0; i < arrSize; ++i) {
                JSONObject item = menus.getArray(i);
                WindowManager::HotKey binding;
                item.getObject("description").getValue(binding.action);
                item.getObject("accelerator").getValue(binding.accel);
                item.getObject("parentMenu").getValue(binding.parentMenu);
                item.getObject("resourceID").getValue(binding.resourceID);
                if(binding.resourceID == "text_word_complete") {
                    // This entry was moved from Word Completion plugin to gsgs Edit menu entry
                    binding.resourceID = "simple_word_completion";
                    binding.parentMenu = "Edit";
                    binding.action = "Complete Word";
                } else if(binding.resourceID == "complete_word") {
                    // The "action" was changed
                    binding.action = "Code Complete";
                } else if(binding.resourceID == "word_complete") {
                    binding.resourceID = "complete_word";
                }
                m_bindings.insert(std::make_pair(binding.resourceID, binding));
            }
            return *this;
        }

        HotKeyData & Save()
        {
            JSONObject menuArr = gsgs_Config().AddObject("HotKey");
            for(WindowManager::HotKeyList::iterator iter = m_bindings.begin(); iter != m_bindings.end(); ++iter) {
                JSONObject binding = JSONObject::createObject();
                binding.addProperty("description", iter->second.action);
                binding.addProperty("accelerator", iter->second.accel);
                binding.addProperty("resourceID", iter->second.resourceID);
                binding.addProperty("parentMenu", iter->second.parentMenu);
                menuArr.arrayAppend(binding);
            }
            return *this;
        }

        bool Exists() const {
            return gsgs_Config().IsObjectExist("HotKey");
        }
    
        HotKeyData & SetBindings(const WindowManager::HotKeyList& menus, const WindowManager::HotKeyList& globals)
        {
            m_bindings = menus;
            m_bindings.insert(globals.begin(), globals.end());
            return *this;
        }
        const WindowManager::HotKeyList& GetBindings() const { return m_bindings; }
    private:
        WindowManager::HotKeyList m_bindings;
    };
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // DockablePane
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //const wxEventType wxEVT_CMD_NEW_DOCKPANE = XRCID("new_dockpane");
    //const wxEventType wxEVT_CMD_DELETE_DOCKPANE = XRCID("delete_dockpane");
    //-----------------------------------------------------------------------------------
    /*BEGIN_EVENT_TABLE(DockablePane, wxPanel)
    EVT_ERASE_BACKGROUND(DockablePane::OnEraseBg)
    EVT_PAINT(DockablePane::OnPaint)
    END_EVENT_TABLE()*/
    //-----------------------------------------------------------------------------------
    DockablePane::DockablePane(wxWindow* parent, Notebook* book, const wxString& title, 
        bool initFloat, const wxBitmap& bmp, const wxSize & size)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
        , m_child(NULL)
        , m_book(book)
        , m_text(title)
        , m_bmp(bmp)
        , m_notifiedDestroyed(false)
    {
        SetSizer(new wxBoxSizer(wxVERTICAL));
        //SetBackgroundStyle(wxBG_STYLE_PAINT);
        Connect(XRCID("close_pane"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DockablePane::ClosePane));
        wxAuiPaneInfo info;
        info.Name(title).Caption(title);
        if (initFloat)
        {
            info.Float();
        }
        GetManager()->GetAuiManager()->AddPane(this, info);
        //GetManager()->GetAuiManager()->Update();

        wxCommandEvent event(wxEVT_CMD_NEW_DOCKPANE);
        event.SetClientData(this);
        parent->GetEventHandler()->AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    DockablePane::~DockablePane() {}
    //-----------------------------------------------------------------------------------
    void DockablePane::ClosePane(wxCommandEvent&)
    {
        if (!m_notifiedDestroyed) {
            m_notifiedDestroyed = true;

            if (m_book) {
                // first detach the child from this pane
                wxSizer* sz = GetSizer();
                sz->Detach(m_child);

                // now we can add it to the noteook (it will be automatically be reparented to the notebook)
                m_book->AddPage(m_child, m_text, false, m_bmp);
            }

            wxCommandEvent event(wxEVT_CMD_DELETE_DOCKPANE);
            event.SetClientData(this);
            GetParent()->GetEventHandler()->AddPendingEvent(event);
        }
    }
    //-----------------------------------------------------------------------------------
        /*void DockablePane::OnEraseBg(wxEraseEvent&)
    {
    }
    //-----------------------------------------------------------------------------------
    void DockablePane::OnPaint(wxPaintEvent&)
    {
        wxBufferedPaintDC dc(this);

        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
        dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
        dc.SetPen(wxPen(gsgs_Style().GetAppBgColour()));
        dc.SetBrush(wxBrush(gsgs_Style().GetAppBgColour()));
        dc.DrawRectangle(GetClientSize());
    }*/
    //-----------------------------------------------------------------------------------
    void DockablePane::SetChildNoReparent(wxWindow* child)
    {
        m_child = child;
        if (!m_child->IsShown()) 
        {
            m_child->Show();
        }
        wxSizer* sz = GetSizer();
        sz->Add(m_child, 1, wxEXPAND | wxALL, 0);
        sz->Layout();
    }
    //-----------------------------------------------------------------------------------
    static wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
    {
        wxCoord x, y;

        // first check if the text fits with no problems
        dc.GetTextExtent(text, &x, &y);
        if (x <= max_size) return text;

        size_t i, len = text.Length();
        size_t last_good_length = 0;
        for (i = 0; i < len; ++i)
        {
            wxString s = text.Left(i);
            s += wxT("...");

            dc.GetTextExtent(s, &x, &y);
            if (x > max_size)
                break;

            last_good_length = i;
        }

        wxString ret = text.Left(last_good_length);
        ret += wxT("...");
        return ret;
    }
    //-----------------------------------------------------------------------------------
    static void DockGetColours(wxColour& bgColour, wxColour& penColour)
    {
        //bgColour = gsgs_Style().GetAppBgColour();
        //bgColour = bgColour.ChangeLightness(80);
        //penColour = gsgs_Style().GetAppBgColour();
        bgColour = gsgs_Style().GetActiveMarkerColour();
        penColour = gsgs_Style().GetAppActiveTextColour();
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // DockArt
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    DockArt::DockArt(IManager* manager) :
        m_manager(manager)
    {
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &DockArt::OnSettingsChanged, this);
        m_captionColour = gsgs_Style().GetCaptionColour();
        m_captionTextColour = gsgs_Style().GetCaptionTextColour();
        m_activeCaptionColour = gsgs_Style().GetActiveMarkerColour();
        m_inactiveCaptionColour = m_activeCaptionColour.MakeDisabled();
        gsgs_Config().Read("UseCustomCaptionsColour", m_useCustomCaptionColour, false);

        m_bgColour = gsgs_Style().GetAppBgColour();
        m_useCustomBgColour = gsgs_Style().IsCustomColour();
                
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, [&](Event& event)
        {
            event.Skip();
            m_bgColour = gsgs_Style().GetAppBgColour();
            m_useCustomBgColour = gsgs_Style().IsCustomColour();

            // Trigger a refresh
            m_manager->GetAuiManager()->Update();
        });
    }
    //-----------------------------------------------------------------------------------
    DockArt::~DockArt()
    {
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &DockArt::OnSettingsChanged, this);
    }
    //-----------------------------------------------------------------------------------
    #define AUI_BUTTON_SIZE 12
    void DockArt::DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect& _rect,
        wxAuiPaneInfo& pane)
    {
        wxRect buttonRect = _rect;

        // Make sure that the height and width of the button are equals
        if (buttonRect.GetWidth() != buttonRect.GetHeight())
        {
            buttonRect.SetHeight(wxMin(buttonRect.GetHeight(), buttonRect.GetWidth()));
            buttonRect.SetWidth(wxMin(buttonRect.GetHeight(), buttonRect.GetWidth()));
        }
        //else
        //{
            buttonRect.Deflate(2);
        //}
        buttonRect = buttonRect.CenterIn(_rect);
        Button::State buttonState = Button::State::BST_Normal;
        switch (button_state) {
        case wxAUI_BUTTON_STATE_HOVER:
            buttonState = Button::State::BST_Hover;
            break;
        case wxAUI_BUTTON_STATE_PRESSED:
            buttonState = Button::State::BST_Press;
            break;
        case wxAUI_BUTTON_STATE_NORMAL:
        default:
            buttonState = Button::State::BST_Normal;
            break;
        }

        // Prepare the colours
        wxColour bgColour, penColour;
        DockGetColours(bgColour, penColour);

        switch (button) {
        default:
        case wxAUI_BUTTON_CLOSE:
            gsgs_Style().DrawClose(dc, window, buttonRect, penColour, bgColour, buttonState, 2);
            break;
        case wxAUI_BUTTON_MAXIMIZE_RESTORE:
            gsgs_Style().DrawMinimize(dc, window, buttonRect, penColour, bgColour, buttonState, 2);
            break;
/*        default:
            pane.state |= wxAuiPaneInfo::optionActive;
            wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, button_state, _rect, pane);
            break;*/
        }
    }
    //-----------------------------------------------------------------------------------
    void DockArt::DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect,
        wxAuiPaneInfo& pane)
    {
        wxRect tmpRect(wxPoint(0, 0), rect.GetSize());

        // Hackishly prevent assertions on linux
        if (tmpRect.GetHeight() == 0) tmpRect.SetHeight(1);
        if (tmpRect.GetWidth() == 0) tmpRect.SetWidth(1);

    #if defined(__WXOSX__)
        tmpRect = rect;
        window->PrepareDC(dc);

        // Prepare the colours
        wxFont f = gsgs_Style().GetDefaultGuiFont();
        dc.SetFont(f);

        if (m_useCustomCaptionColour) {
            dc.SetPen(m_captionColour);
            dc.SetBrush(m_captionColour);
            dc.DrawRectangle(tmpRect);
        }
        else {
            gsgs_Style().FillMenuBarBgColour(dc, tmpRect);
        }

        int caption_offset = 0;
        if (pane.icon.IsOk()) {
            DrawIcon(dc, tmpRect, pane);
            caption_offset += pane.icon.GetWidth() + 3;
        }
        else {
            caption_offset = 3;
        }
        dc.SetTextForeground(m_captionTextColour);
        wxCoord w, h;
        dc.GetTextExtent(wxT("ABCDEFHXfgkj"), &w, &h);

        wxRect clip_rect = tmpRect;
        clip_rect.width -= 3; // text offset
        clip_rect.width -= 2; // button padding
        if (pane.HasCloseButton()) { clip_rect.width -= m_buttonSize; }
        if (pane.HasPinButton()) { clip_rect.width -= m_buttonSize; }
        if (pane.HasMaximizeButton()) { clip_rect.width -= m_buttonSize; }

        wxString draw_text = wxAuiChopText(dc, text, clip_rect.width);
        wxSize textSize = dc.GetTextExtent(draw_text);

        dc.DrawText(draw_text, tmpRect.x + 3 + caption_offset, tmpRect.y + ((tmpRect.height - textSize.y) / 2));
    #else
        wxBitmap bmp(tmpRect.GetSize());
        {
            wxMemoryDC memDc;
            memDc.SelectObject(bmp);

            wxGCDC gdc;
            wxDC* pDC = NULL;
            if (!gsgs_Style().GetGCDC(memDc, gdc)) {
                pDC = &memDc;
            }
            else {
                pDC = &gdc;
            }

            wxFont f = gsgs_Style().GetDefaultGuiFont();
            pDC->SetFont(f);

            wxColour captionBgColour, textColour;
            DockGetColours(captionBgColour, textColour);

            pDC->SetPen(captionBgColour);
            pDC->SetBrush(captionBgColour);
            pDC->DrawRectangle(tmpRect);

            int caption_offset = 5;

            wxRect clip_rect = tmpRect;
            clip_rect.width -= caption_offset; // text offset
            clip_rect.width -= 2;              // button padding
            if (pane.HasCloseButton()) clip_rect.width -= m_buttonSize;
            if (pane.HasPinButton()) clip_rect.width -= m_buttonSize;
            if (pane.HasMaximizeButton()) clip_rect.width -= m_buttonSize;

            // Truncate the text if needed
            wxString draw_text = wxAuiChopText(gdc, text, clip_rect.width);
            wxSize textSize = pDC->GetTextExtent(draw_text);
            wxRect textRect(textSize);
            textRect = textRect.CenterIn(clip_rect, wxVERTICAL);
            textRect.SetX(caption_offset);

            pDC->SetTextForeground(textColour);
            pDC->DrawText(draw_text, textRect.GetTopLeft());
            memDc.SelectObject(wxNullBitmap);
        }
        dc.DrawBitmap(bmp, rect.x, rect.y, false);
    #endif
    }
    //-----------------------------------------------------------------------------------
    void DockArt::DrawBackground(wxDC & dc, wxWindow * window, int orientation, const wxRect & rect)
    {
        wxUnusedVar(window);
        wxUnusedVar(orientation);
        //dc.SetPen(m_bgColour);
        //dc.SetBrush(m_bgColour);
        //dc.DrawRectangle(rect);
    }
    //-----------------------------------------------------------------------------------
    void DockArt::DrawBorder(wxDC & dc, wxWindow * window, const wxRect & rect, wxAuiPaneInfo & pane)
    {
        wxUnusedVar(window);
        //dc.SetPen(m_bgColour);
        //dc.SetBrush(*wxTRANSPARENT_BRUSH);
        //dc.SetPen(gsgs_Style().GetAppBgColour());
        //dc.SetBrush(gsgs_Style().GetAppBgColour());
        //dc.DrawRectangle(rect);
    }
    //-----------------------------------------------------------------------------------
    void DockArt::DrawSash(wxDC & dc, wxWindow * window, int orientation, const wxRect & rect)
    {
        wxUnusedVar(window);
        wxUnusedVar(orientation);
        //SetBackgroundColour(gsgs_Style().GetAppTextColour());
        //dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetPen(gsgs_Style().GetAppBgColour());
        dc.SetBrush(m_bgColour);
        dc.DrawRectangle(rect);
    }
    //-----------------------------------------------------------------------------------
    void DockArt::OnSettingsChanged(wxCommandEvent& event)
    {
        event.Skip();
        m_captionColour = gsgs_Style().GetCaptionColour();
        m_captionTextColour = gsgs_Style().GetCaptionTextColour();
        m_useCustomCaptionColour = false;
    }

    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    // DetachedPanesInfo
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    DetachedPanesInfo::DetachedPanesInfo(wxArrayString arr) : 
        m_panes(arr)
    {
    }
    //---------------------------------------------------------------------------
    DetachedPanesInfo::~DetachedPanesInfo()
    {
    }
    //---------------------------------------------------------------------------
    void DetachedPanesInfo::DeSerialize(Serializer& arch)
    {
	    arch.Read(wxT("m_panes"), m_panes);
    }
    //---------------------------------------------------------------------------
    void DetachedPanesInfo::Serialize(Serializer& arch)
    {
	    arch.Write(wxT("m_panes"), m_panes);
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // WindowManager
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    static WindowManager* m_mgr = NULL;
    WindowManager& WindowManager::singleton()
    {
        if(NULL == m_mgr) {
            m_mgr = new WindowManager();
        }
        return *m_mgr;
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::Release()
    {
        if(m_mgr) {
            delete m_mgr;
        }
        m_mgr = NULL;
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::HackShowPane(wxAuiPaneInfo & info, wxAuiManager * mag)
    {
	    if(info.IsOk() && mag)
        {
            //info.MinSize(info.best_size);    // saved while hiding
            info.MinSize(50, 50);
            info.Show();
		    //mag->Update();
            //info.MinSize(10,5);	// so it can't disappear if undocked
            mag->Update();
	    }
    }
    //---------------------------------------------------------------------------
    void WindowManager::HackHidePane(bool commit, wxAuiPaneInfo & info, wxAuiManager * mag)
    {
	    if(info.IsOk() && mag)
        {
		    int width = 0;
		    int height = 0;
            info.window->GetClientSize(&width, &height);
            info.BestSize(width, height);    // save for later subsequent show
            info.Hide();

		    if(commit) 
            {
                mag->Update();
		    }
	    }
    }
    //---------------------------------------------------------------------------
    void WindowManager::RemoveMenu(const wxString& name)
    {
        int itemId = wxXmlResource::GetXRCID(name.c_str());
        std::map<int, wxString>::iterator iter = m_id2nameMap.find(itemId);
        if (iter != m_id2nameMap.end()) 
        {
            m_id2nameMap.erase(iter);
        }
    #if 0
	    int idx = m_mb->FindMenu(wxT("View"));
	    if (idx != wxNOT_FOUND) {
		    wxMenu *menu = m_mb->GetMenu(idx);
		    menu->Destroy(itemId);

		    wxTheApp->Disconnect(itemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(WindowManager::OnDockpaneMenuItem), NULL, this);
		    wxTheApp->Disconnect(itemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(WindowManager::OnDockpaneMenuItemUI), NULL, this);
	    }
    #endif
    }
    //---------------------------------------------------------------------------
    void WindowManager::AddMenu(const wxString & name)
    {
        int itemId = wxXmlResource::GetXRCID(name.c_str());
        m_id2nameMap[itemId] = name;
    }
    //---------------------------------------------------------------------------
    void WindowManager::OnDockpaneMenuItem(wxCommandEvent& e)
    {
        wxString name;
        std::map<int, wxString>::iterator iter = m_id2nameMap.find(e.GetId());
        if (iter != m_id2nameMap.end())
        {
            name = iter->second;
        }
        else
        {
            name = wxEmptyString;
        }
	    wxAuiPaneInfo &info = GetPane(name);
	    if ( info.IsOk() ) 
        {
		    if ( e.IsChecked() ) 
            {
			    HackShowPane(info, this);
		    } 
            else 
            {
			    HackHidePane(true, info, this);
		    }
	    }
    }
    //---------------------------------------------------------------------------
    void WindowManager::OnDockpaneMenuItemUI(wxUpdateUIEvent& e)
    {
        wxString name;
        std::map<int, wxString>::iterator iter = m_id2nameMap.find(e.GetId());
        if (iter != m_id2nameMap.end())
        {
            name = iter->second;
        }
        else
        {
            name = wxEmptyString;
        }
	    wxAuiPaneInfo &info = GetPane(name);
	    if (info.IsOk()) 
        {
		    if ( info.IsShown() ) 
            {
			    e.Check(true);
		    }
            else 
            {
			    e.Check(false);
		    }
	    }
    }
    //---------------------------------------------------------------------------
    void WindowManager::getFloatingList(wxArrayString & out)
    {
	    std::map<int, wxString>::iterator iter = m_id2nameMap.begin();
	    for(; iter != m_id2nameMap.end(); iter++)
        {
		    out.Add(iter->second);
	    }
    }
    //---------------------------------------------------------------------------
    bool WindowManager::IsPaneDetached(const wxString& name)
    {
	    std::map<int, wxString>::iterator iter = m_id2nameMap.begin();
	    for(; iter != m_id2nameMap.end(); iter++)
        {
		    if(iter->second == name) 
            {
			    return true;
		    }
	    }
	    return false;
    }
    //-----------------------------------------------------------------------------------
    WindowManager::WindowManager()
        :mMainFrame(0)
    {
        Bind(wxEVT_AUI_PANE_BUTTON, &WindowManager::OnButtonClicked, this);
        Bind(wxEVT_AUI_RENDER, &WindowManager::OnRender, this);
        Bind(wxEVT_AUI_PANE_ACTIVATED, &WindowManager::OnAuiPaneActivated, this);
        Bind(wxEVT_INIT_DONE, &WindowManager::OnStartupCompleted, this);

        // A-Z
        for (size_t i = 65; i < 91; ++i) {
            char asciiCode = (char)i;
            m_keyCodes.insert(wxString() << asciiCode);
        }

        // 0-9
        for (size_t i = 48; i < 58; ++i) {
            char asciiCode = (char)i;
            m_keyCodes.insert(wxString() << asciiCode);
        }

        // Special chars
        m_keyCodes.insert("`");
        m_keyCodes.insert("-");
        m_keyCodes.insert("*");
        m_keyCodes.insert("=");
        m_keyCodes.insert("BACK");
        m_keyCodes.insert("TAB");
        m_keyCodes.insert("[");
        m_keyCodes.insert("]");
        m_keyCodes.insert("ENTER");
        m_keyCodes.insert("CAPITAL");
        m_keyCodes.insert("SCROLL_LOCK");
        m_keyCodes.insert("PASUE");
        m_keyCodes.insert(";");
        m_keyCodes.insert("'");
        m_keyCodes.insert("\\");
        m_keyCodes.insert(",");
        m_keyCodes.insert(".");
        m_keyCodes.insert("/");
        m_keyCodes.insert("SPACE");
        m_keyCodes.insert("INS");
        m_keyCodes.insert("HOME");
        m_keyCodes.insert("PGUP");
        m_keyCodes.insert("PGDN");
        m_keyCodes.insert("DEL");
        m_keyCodes.insert("END");
        m_keyCodes.insert("UP");
        m_keyCodes.insert("DOWN");
        m_keyCodes.insert("RIGHT");
        m_keyCodes.insert("LEFT");
        m_keyCodes.insert("F1");
        m_keyCodes.insert("F2");
        m_keyCodes.insert("F3");
        m_keyCodes.insert("F4");
        m_keyCodes.insert("F5");
        m_keyCodes.insert("F6");
        m_keyCodes.insert("F7");
        m_keyCodes.insert("F8");
        m_keyCodes.insert("F9");
        m_keyCodes.insert("F10");
        m_keyCodes.insert("F11");
        m_keyCodes.insert("F12");

        // There can be the following options:
        // Ctrl-Alt-Shift
        // Ctrl-Alt
        // Ctrl-Shift
        // Ctrl
        // Alt-Shift
        // Alt
        // Shift
        std::for_each(m_keyCodes.begin(), m_keyCodes.end(), [&](const wxString& keyCode) {
            m_allShorcuts.insert("Ctrl-Alt-Shift-" + keyCode);
            m_allShorcuts.insert("Ctrl-Alt-" + keyCode);
            m_allShorcuts.insert("Ctrl-Shift-" + keyCode);
            m_allShorcuts.insert("Ctrl-" + keyCode);
            m_allShorcuts.insert("Alt-Shift-" + keyCode);
            m_allShorcuts.insert("Alt-" + keyCode);
            m_allShorcuts.insert("Shift-" + keyCode);
        });
    }
    //-----------------------------------------------------------------------------------
    WindowManager::~WindowManager()
    {
        Save();
        Unbind(wxEVT_INIT_DONE, &WindowManager::OnStartupCompleted, this);
        Unbind(wxEVT_AUI_PANE_BUTTON, &WindowManager::OnButtonClicked, this);
        Unbind(wxEVT_AUI_RENDER, &WindowManager::OnRender, this);
        Unbind(wxEVT_AUI_PANE_ACTIVATED, &WindowManager::OnAuiPaneActivated, this);
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::OnRender(wxAuiManagerEvent& event) 
    { 
        event.Skip(); 
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::OnButtonClicked(wxAuiManagerEvent& event)
    {
        event.Skip();
        // if(event.GetButton() != wxAUI_BUTTON_PIN) {
        //     event.Skip();
        //     return;
        // }
        // event.Veto();
        // event.Skip(false);
        // 
        // if(event.GetPane()->name == OUTPUT_VIEW) {
        //     CallAfter(&WindowManager::ShowOutputViewOpenTabMenu);
        // } else {
        //     CallAfter(&WindowManager::ShowWorkspaceOpenTabMenu);
        // }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::OnAuiPaneActivated(wxAuiManagerEvent& e)
    {
        e.Skip();

        // If a pane is floating, copy the accelerator table to it.
        const auto pane = e.GetPane();
        if(pane && pane->frame && pane->IsFloating()) 
        {
            // wxGetRealTopLevelParent(pane->frame) doesn't work since the detached pane
            // has IsTopLevel() == true, so wxGetRealTopLevelParent() returns the parameter.
            const auto parent = ::GetManager()->GetTheApp()->GetTopWindow();
            if(parent) 
            {
                const auto accelTable = parent->GetAcceleratorTable();
                if(accelTable) 
                {
                    pane->frame->SetAcceleratorTable(*accelTable);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::ShowWorkspaceOpenTabMenu()
    {
        // Override the "pin" button to show a window list
        bool show;
        wxString selection = ShowMenu(GetPane(WORKSPACE_VIEW).window,
            ::GetManager()->GetWorkspaceTabs(), ::GetManager()->GetWorkspacePaneNotebook(), show);
        CHECK_COND_RET(!selection.IsEmpty());
        Event event(wxEVT_SHOW_WORKSPACE_TAB);
        event.SetSelected(show).SetString(selection);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::ShowOutputViewOpenTabMenu()
    {
        bool show;
        wxString selection = ShowMenu(GetPane(OUTPUT_VIEW).window, 
            ::GetManager()->GetOutputTabs(), ::GetManager()->GetOutputPaneNotebook(), show);
        CHECK_COND_RET(!selection.IsEmpty());
        Event event(wxEVT_SHOW_OUTPUT_TAB);
        event.SetSelected(show).SetString(selection);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::ShowPerspectiveViewOpenTabMenu()
    {
        bool show;
        wxString selection = ShowMenu(GetPane(Perspective_VIEW).window,
            ::GetManager()->GetPerspectiveTabs(), ::GetManager()->GetOutputPaneNotebook(), show);
        CHECK_COND_RET(!selection.IsEmpty());
        Event event(wxEVT_SHOW_Perspective_TAB);
        event.SetSelected(show).SetString(selection);
        gsgs_Event().AddPendingEvent(event);
    }
    //-----------------------------------------------------------------------------------
    wxString WindowManager::ShowMenu(wxWindow* win, const wxArrayString& tabs, Notebook* book, bool& checked)
    {
        DetachedPanesInfo dpi;
        gsgs_EditorConfig().ReadObject("DetachedPanesList", &dpi);

        std::map<int, wxString> tabsIds;
        wxMenu menu(_("Toggle Tabs"));
        for(size_t i = 0; i < tabs.size(); ++i) 
        {
            int tabId = ::wxNewId();
            const wxString & label = tabs.Item(i);
            tabsIds.insert(std::make_pair(tabId, label));
            wxMenuItem * item = new wxMenuItem(&menu, tabId, label, "", wxITEM_CHECK);
            menu.Append(item);
            item->Check((book->GetPageIndex(label) != wxNOT_FOUND));

            // Output pane does not support "detach"
            if((book != ::GetManager()->GetOutputPaneNotebook()) && dpi.GetPanes().Index(label) != wxNOT_FOUND) 
            {
                item->Enable(false);
            }
        }

        int sel = win->GetPopupMenuSelectionFromUser(menu);
        if(sel == wxID_NONE) 
            return "";
        const wxString & label = tabsIds.find(sel)->second;
        checked = (book->GetPageIndex(label) == wxNOT_FOUND);
        return label;
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::Load(wxTopLevelWindow* win)
    {
        if (win->GetName().IsEmpty()) { return; }

        // Is this object already registered?
        if (wxPersistenceManager::Get().Find(win)) {
            wxPersistenceManager::Get().Restore(win);
        }
        else {
            // Register and restore the object and recurse into its children
            wxPersistenceManager::Get().RegisterAndRestore(win);
        }
        DoLoad(win, win->GetName(), 0);
        if (win->GetParent()) { win->CentreOnParent(); }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::DoLoad(wxWindow* win, const wxString& parentName, int depth)
    {
        if (!win) return;
        int childIndex(0);
        wxWindowList::compatibility_iterator pclNode = win->GetChildren().GetFirst();
        while (pclNode) {
            ++childIndex;
            wxWindow* pclChild = pclNode->GetData();
            if (pclChild) {
                // Load the state of a notebook
                wxBookCtrlBase* bookbase = dynamic_cast<wxBookCtrlBase*>(pclChild);
                if (bookbase) {
                    // Make sure that the book control has a name (which is needed by the wxPersistenceManager)
                    wxString controlName;
                    if (bookbase->GetName().IsEmpty()) {
                        controlName << parentName << "_book_" << depth << "_" << childIndex;
                        bookbase->SetName(controlName);
                    }
                    else {
                        controlName = bookbase->GetName();
                    }
                    if (wxPersistenceManager::Get().Find(bookbase)) {
                        wxPersistenceManager::Get().Restore(bookbase);
                    }
                    else {
                        // Register and restore the object and recurse into its children
                        wxPersistenceManager::Get().RegisterAndRestore(bookbase);
                    }
                }

                // Recurse into this window children
                DoLoad(pclChild, parentName, depth + 1);
            }
            // Move on to the next sibling
            pclNode = pclNode->GetNext();
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::DoGetFrames(wxFrame* parent, WindowManager::FrameList_t& frames)
    {
        frames.push_back(parent);
        const wxWindowList& children = parent->GetChildren();
        wxWindowList::const_iterator iter = children.begin();
        for (; iter != children.end(); ++iter) {
            wxFrame* frameChild = dynamic_cast<wxFrame*>(*iter);
            if (frameChild) {
                if (std::find(frames.begin(), frames.end(), frameChild) == frames.end()) {
                    frames.push_back(frameChild);
                    DoGetFrames(frameChild, frames);
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::DoUpdateMenu(wxMenu* menu, HotKeyMap& accels, std::vector<wxAcceleratorEntry>& table)
    {
        wxMenuItemList items = menu->GetMenuItems();
        wxMenuItemList::iterator iter = items.begin();
        for (; iter != items.end(); iter++) {
            wxMenuItem* item = *iter;
            if (item->GetSubMenu()) {
                DoUpdateMenu(item->GetSubMenu(), accels, table);
                continue;
            }

            HotKeyMap::iterator where = accels.find(item->GetId());
            if (where != accels.end()) {
                wxString itemText = item->GetItemLabel();
                // remove the old shortcut
                itemText = itemText.BeforeFirst('\t');
                itemText << "\t" << where->second.accel;

                // Replace the item text (mnemonics + accel + label)
                item->SetItemLabel(itemText);

                // remove the matches entry from the accels map
                accels.erase(where);
            }

            wxAcceleratorEntry* a = wxAcceleratorEntry::Create(item->GetItemLabel());
            if (a) {
                a->Set(a->GetFlags(), a->GetKeyCode(), item->GetId());
                table.push_back(*a);
                wxDELETE(a);
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::DoUpdateFrame(wxFrame* frame, HotKeyMap& accels)
    {
        std::vector<wxAcceleratorEntry> table;

        // Update menus. If a match is found remove it from the 'accel' table
        wxMenuBar* menuBar = frame->GetMenuBar();
        if (!menuBar) return;
        for (size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
            wxMenu* menu = menuBar->GetMenu(i);
            DoUpdateMenu(menu, accels, table);
        }

        if (!table.empty() || !accels.empty()) {
            wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + accels.size()];
            // append the globals
            for (HotKeyMap::iterator iter = accels.begin(); iter != accels.end(); ++iter) {
                wxString dummyText;
                dummyText << iter->second.action << "\t" << iter->second.accel;
                wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
                if (entry) {
                    entry->Set(entry->GetFlags(), entry->GetKeyCode(), wxXmlResource::GetXRCID(iter->second.resourceID));
                    table.push_back(*entry);
                    wxDELETE(entry);
                }
            }

            for (size_t i = 0; i < table.size(); ++i) {
                entries[i] = table.at(i);
            }

            wxAcceleratorTable acceleTable(table.size(), entries);
            frame->SetAcceleratorTable(acceleTable);
            wxDELETEA(entries);
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::Save()
    {
        HotKeyData config;
        config.SetBindings(m_menuTable, m_globalTable).Save();
    }
    //-----------------------------------------------------------------
    void WindowManager::RegisterDockWindow(int menuItemId, const wxString& name)
    {
        m_panes[menuItemId] = name;
        mMainFrame->Bind(wxEVT_COMMAND_MENU_SELECTED, &WindowManager::OnViewPane, this, menuItemId);
        mMainFrame->Bind(wxEVT_UPDATE_UI, &WindowManager::OnViewPaneUI, this, menuItemId);
    }
    //-----------------------------------------------------------------
    void WindowManager::OnViewPane(wxCommandEvent& event)
    {
        std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
        if (iter != m_panes.end())
        {
            // In >wxGTK-2.9.4 event.GetChecked() is invalid when coming from an accelerator; instead examine the actual
            // state
            wxAuiPaneInfo& info = GetPane(iter->second);
            if (info.IsOk())
            {
                ViewPane(iter->second, !info.IsShown());
            }
        }
    }
    //-----------------------------------------------------------------
    void WindowManager::OnViewPaneUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        std::map<int, wxString>::iterator iter = m_panes.find(event.GetId());
        if (iter != m_panes.end())
        {
            wxAuiPaneInfo& info = GetPane(iter->second);
            if (info.IsOk())
            {
                event.Check(info.IsShown());
            }
        }
    }
    //-----------------------------------------------------------------
    void WindowManager::ViewPane(const wxString& paneName, bool checked)
    {
        wxAuiPaneInfo& info = GetPane(paneName);
        if (info.IsOk())
        {
            if (checked)
            {
                HackShowPane(info, this);
            }
            else
            {
                HackHidePane(true, info, this);
            }
        }
#ifndef __WXMSW__
        // This is needed in >=wxGTK-2.9, otherwise output pane doesn't fully expand, or on closing the auinotebook doesn't
        // occupy its space
        mMainFrame->SendSizeEvent(wxSEND_EVENT_POST);
#endif
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::updateImpl()
    {
        m_menuTable.clear();
        gsgs_Debug("Keyboard manager: Initializing keyboard manager");
        // Load old format
        HotKeyData config;
        if (!config.Exists()) {
            gsgs_Debug("Keyboard manager: No configurtion found - importing old settings");
            // Decide which file we want to load, take the user settings file first
            wxFileName fnOldSettings(gsgs_Path().GetUserDataDir(), "accelerators.conf");
            fnOldSettings.AppendDir("config");

            wxFileName fnDefaultOldSettings(gsgs_Path().GetDataDir(), "accelerators.conf.default");
            fnDefaultOldSettings.AppendDir("config");
            wxFileName fnFileToLoad;
            bool canDeleteOldSettings(false);
            if (fnOldSettings.Exists()) {
                fnFileToLoad = fnOldSettings;
                canDeleteOldSettings = true;
            }
            else {
                fnFileToLoad = fnDefaultOldSettings;
            }

            if (fnFileToLoad.Exists()) {
                gsgs_Debug("Keyboard manager: Importing settings from '%s'", fnFileToLoad.GetFullPath());

                // Apply the old settings to the menus
                wxString content;
                if (!FileUtil::ReadFileContent(fnFileToLoad, content)) return;
                wxArrayString lines = ::wxStringTokenize(content, "\r\n", wxTOKEN_STRTOK);
                for (size_t i = 0; i < lines.GetCount(); ++i) {
                    wxArrayString parts = ::wxStringTokenize(lines.Item(i), "|", wxTOKEN_RET_EMPTY);
                    if (parts.GetCount() < 3) continue;
                    HotKey binding;
                    binding.resourceID = parts.Item(0);
                    binding.parentMenu = parts.Item(1);
                    binding.action = parts.Item(2);
                    if (parts.GetCount() == 4) {
                        binding.accel = parts.Item(3);
                    }
                    m_menuTable.insert(std::make_pair(binding.resourceID, binding));
                }

                if (canDeleteOldSettings) {
                    wxLogNull noLog;
                    gsgsRemoveFile(fnFileToLoad.GetFullPath());
                }
            }
        }
        else {
            config.Load();
            m_menuTable = config.GetBindings();
        }

        // Load the default settings and add any new entries
        HotKeyList defaultEntries = DoLoadDefaultAccelerators();
        std::for_each(defaultEntries.begin(), defaultEntries.end(), [&](const HotKeyList::value_type& vt) {
            if (m_menuTable.count(vt.first) == 0) {
                m_menuTable.insert(vt);
            }
        });

        config.SetBindings(m_menuTable, m_globalTable).Save();
        UpdateAccels();
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::getHotKeyList(HotKeyList& accels) const
    {
        accels.insert(m_menuTable.begin(), m_menuTable.end());
        accels.insert(m_globalTable.begin(), m_globalTable.end());
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::SetAccelerators(const HotKeyList& accels)
    {
        // separate the globals from the menu accelerators
        // The process is done by checking each item's parentMenu
        // If the parentMenu is empty -> global accelerator
        HotKeyList globals, menus;
        HotKeyList::const_iterator iter = accels.begin();
        for (; iter != accels.end(); ++iter) {
            if (iter->second.parentMenu.IsEmpty()) {
                globals.insert(std::make_pair(iter->first, iter->second));
            }
            else {
                menus.insert(std::make_pair(iter->first, iter->second));
            }
        }

        m_menuTable.swap(menus);
        m_globalTable.swap(globals);
        UpdateAccels();
        Save();
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::UpdateAccels(wxFrame* frame)
    {
        // Since we keep the accelerators with their original resource ID in the form of string
        // we need to convert the map into a different integer with integer as the resource ID

        // Note that we place the items from the m_menuTable first and then we add the globals
        // this is because menu entries takes precedence over global accelerators
        HotKeyList accels = m_menuTable;
        accels.insert(m_globalTable.begin(), m_globalTable.end());

        HotKeyMap intAccels;
        DoConvertToIntMap(accels, intAccels);

        if (!frame) {
            // update all frames
            wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
            CHECK_PTR_RET(topFrame);

            FrameList_t frames;
            DoGetFrames(topFrame, frames);
            for (FrameList_t::iterator iter = frames.begin(); iter != frames.end(); ++iter) {

                DoUpdateFrame(*iter, intAccels);
            }
        }
        else {
            // update only the requested frame
            DoUpdateFrame(frame, intAccels);
        }
    }
    //-----------------------------------------------------------------------------------
    int WindowManager::showAddHotKeyDlg(wxWindow* parent, HotKey& data)
    {
        NewKeyShortcutDlg dlg(parent, data);
        if (dlg.ShowModal() == wxID_OK) {
            data.accel = dlg.GetAccel();
            return wxID_OK;
        }
        return wxID_CANCEL;
    }
    //-----------------------------------------------------------------------------------
    bool WindowManager::isHotKeyExist(const wxString& accel) const
    {
        if (accel.IsEmpty()) 
            return false;

        HotKeyList accels;
        getHotKeyList(accels);

        HotKeyList::const_iterator iter = accels.begin();
        for (; iter != accels.end(); ++iter) {
            if (iter->second.accel == accel) {
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::addHotKey(const wxString& resourceID, const wxString & accel, const wxString& description)
    {
        HotKey mid;
        mid.action = description;
        mid.accel = accel;
        mid.resourceID = resourceID;
        m_globalTable.insert(std::make_pair(mid.resourceID, mid));
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::clearAllHotKey()
    {
        // Decide which file we want to load, take the user settings file first
        wxFileName fnOldSettings(gsgs_Path().GetUserDataDir(), "accelerators.conf");
        fnOldSettings.AppendDir("config");

        wxLogNull nl;
        if (fnOldSettings.Exists()) {
            gsgsRemoveFile(fnOldSettings.GetFullPath());
        }

        updateImpl();
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::OnStartupCompleted(wxCommandEvent& event)
    {
        event.Skip();
        updateImpl();
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::DoConvertToIntMap(const HotKeyList& strMap, HotKeyMap& intMap)
    {
        // Convert the string map into int based map
        HotKeyList::const_iterator iter = strMap.begin();
        for (; iter != strMap.end(); ++iter) {
            intMap.insert(std::make_pair(wxXmlResource::GetXRCID(iter->second.resourceID), iter->second));
        }
    }
    //-----------------------------------------------------------------------------------
    void WindowManager::getFreeHotKeyList(wxArrayString & allUnasigned) const
    {
        HotKeyList accels;
        getHotKeyList(accels);

        gsgsStringSet usedShortcuts;
        std::for_each(accels.begin(), accels.end(), [&](const std::pair<wxString, HotKey>& p) {
            if (!p.second.accel.IsEmpty()) {
                usedShortcuts.insert(p.second.accel);
            }
        });

        // Remove all duplicate entries
        std::set_difference(m_allShorcuts.begin(), m_allShorcuts.end(),
            usedShortcuts.begin(), usedShortcuts.end(), std::back_inserter(allUnasigned));
    }
    //-----------------------------------------------------------------------------------
    WindowManager::HotKeyList WindowManager::DoLoadDefaultAccelerators()
    {
        HotKeyList entries;
        wxFileName fnDefaultOldSettings(gsgs_Path().GetDataDir(), "accelerators.conf.default");
        fnDefaultOldSettings.AppendDir("config");

        if (fnDefaultOldSettings.Exists()) {
            wxString content;
            if (!FileUtil::ReadFileContent(fnDefaultOldSettings, content)) {
                return entries;
            }
            wxArrayString lines = ::wxStringTokenize(content, "\r\n", wxTOKEN_STRTOK);
            for (size_t i = 0; i < lines.GetCount(); ++i) {
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), "|", wxTOKEN_RET_EMPTY);
                if (parts.GetCount() < 3)
                    continue;
                HotKey binding;
                binding.resourceID = parts.Item(0);
                binding.parentMenu = parts.Item(1);
                binding.action = parts.Item(2);
                if (parts.GetCount() == 4) {
                    binding.accel = parts.Item(3);
                }
                entries.insert(std::make_pair(binding.resourceID, binding));
            }
        }
        return entries;
    }
    //-----------------------------------------------------------------------------------
}