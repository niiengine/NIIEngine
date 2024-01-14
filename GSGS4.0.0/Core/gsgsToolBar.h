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
#ifndef gsgsTOOLBAR_H
#define gsgsTOOLBAR_H

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include "gsgsTreeCtrl.h"
#include <wx/menu.h>
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/dataview.h>

#ifdef __WXOSX__
#define TOOL_BAR_DROPDOWN_ARROW_SIZE 5
#else
#define TOOL_BAR_DROPDOWN_ARROW_SIZE 7
#endif

namespace gsgs
{
    #define OPEN_CONFIG_MGR_STR _("Configuration Manager...")
    class _gsgsAPI ConfigurationPanel : public wxPanel
    {
    public:
        ConfigurationPanel(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
        virtual ~ConfigurationPanel();

        /**
         * @brief update the list of projects and configurations available
         */
        void Update(const wxArrayString& projects, const wxArrayString& cfg);

        /**
         * @brief clear everything
         */
        void Clear();

        void SetActiveConfiguration(const wxString& activeConfiguration);
        void SetActiveProject(const wxString& activeProject);
        void SetConfigurations(const wxArrayString& cfg);
        void SetProjects(const wxArrayString& projects) { this->m_projects = projects; }
        const wxString& GetActiveConfiguration() const { return m_activeConfiguration; }
        const wxString& GetActiveProject() const { return m_activeProject; }
        const wxArrayString& GetConfigurations() const { return m_configurations; }
        const wxArrayString& GetProjects() const { return m_projects; }
    protected:
        void OnConfigChoice(wxCommandEvent& event);

        void OnMachineChoice(wxCommandEvent& event);

        void OnStrategyChoice(wxCommandEvent& event);

        void OnSize(wxSizeEvent& event);
    protected:
        wxArrayString m_projects;
        wxArrayString m_configurations;
        wxString m_activeProject;
        wxString m_activeConfiguration;
        DefaultChoice* m_cfgChoice = nullptr;
        DefaultChoice* m_machineChoice = nullptr;
        DefaultChoice* m_strategyChoice = nullptr;
    };

    class _gsgsAPI CustomiseToolBarDlg : public wxDialog
    {
    public:
        CustomiseToolBarDlg(wxWindow* parent, ToolBar* tb, wxWindowID id = wxID_ANY, const wxString& title = _("Customise"),
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = gsgsMinDlgSize, long style = gsgsDlgStyle);
        
        virtual ~CustomiseToolBarDlg();
        
        DataViewListCtrl* GetDvListCtrlItems() { return m_dvListCtrlItems; }
    protected:
        virtual void OnOK(wxCommandEvent& event);
    protected:
        //ToolBar* m_toolbar;
        std::vector<ToolBarButton*>& m_buttons;
        DataViewListCtrl* m_dvListCtrlItems;
        DialogButtonSizer* m_stdBtnSizer4;
        Button* m_button6;
        Button* m_button8;
    };

    class _gsgsAPI ToolBarButton
    {
    public:
        enum eFlags {
            kHasMenu            = (1 << 0),
            kToggleButton       = (1 << 1),
            kChecked            = (1 << 2),
            kDisabled           = (1 << 3),
            kSeparator          = (1 << 4),
            kControl            = (1 << 5),
            kHidden             = (1 << 6),
            kSpacer             = (1 << 7),
            kStretchalbeSpace   = (1 << 8),
        };

        enum eRenderFlags {
            kHover = (1 << 0),
            kPressed = (1 << 1),
        };

    public:
        ToolBarButton(ToolBar* parent, wxWindowID id, int bmp, const wxString& label = "", size_t flags = 0);
        virtual ~ToolBarButton();

        /**
         * @brief calculate the size needed for drawing this button
         * @return
         */
        virtual wxSize CalculateSize(wxDC& dc) const = 0;

        /**
         * @brief draw the button in a given wxRect
         */
        virtual void Render(wxDC& dc, const wxRect& rect);

        inline void SetBmpIdx(int idx)              { m_bmpidx = idx; }

        inline int GetBmpIdx() const                { return m_bmpidx; }

        inline void SetLabel(const wxString& label) { m_label = label; }

        inline const wxString& GetLabel() const     { return m_label; }

        inline bool IsMenuButton() const            { return m_flags & kHasMenu; }

        inline wxWindowID GetId() const             { return m_id; }

        inline ToolBar * GetToolbar()               { return m_toolbar; }

        inline void SetMenu(wxMenu * menu)          { wxDELETE(m_menu); m_menu = menu; }

        inline wxMenu * GetMenu() const             { return m_menu; }

        virtual bool Contains(const wxPoint& pt) const { return m_buttonRect.Contains(pt); }

        bool InsideMenuButton(const wxPoint& pt) const { return IsMenuButton() && (m_dropDownArrowRect.Contains(pt)); }

        inline void SetRenderFlags(size_t flags)    { m_renderFlags = flags; }
        inline bool IsHover() const                 { return m_renderFlags & kHover; }
        inline bool IsPressed() const               { return m_renderFlags & kPressed; }
        inline void SetHover(bool b)                { EnableRenderFlag(kHover, b); }
        inline void SetPressed(bool b)              { ClearRenderFlags(); EnableRenderFlag(kPressed, b); }
        inline void ClearRenderFlags()              { m_renderFlags = 0; }
        inline wxRect& GetButtonRect()              { return m_buttonRect; }
        inline bool IsChecked() const               { return (m_flags & kChecked); }
        inline void Check(bool b)                   { EnableFlag(kChecked, b); SetPressed(b); }
        inline bool IsToggle() const                { return (m_flags & kToggleButton); }
        inline bool IsSeparator() const             { return m_flags & kSeparator; }
        inline bool IsSpacer() const                { return m_flags & kSpacer; }
        inline bool IsStretchableSpace() const      { return m_flags & kStretchalbeSpace; }
        inline bool IsEnabled() const               { return !(m_flags & kDisabled); }
        inline bool IsControl() const               { return m_flags & kControl; }
        inline bool Enable(bool b)                  { EnableFlag(kDisabled, !b); return true; }
        inline bool IsHidden() const                { return m_flags & kHidden; }
        inline bool IsShown() const                 { return !IsHidden(); }
        virtual void Show(bool b);

        template <typename T> T* DCast()            { return dynamic_cast<T*>(this); }
        template <typename T> T* Cast()             { return static_cast<T*>(this); }
    protected:
        void EnableRenderFlag(ToolBarButton::eRenderFlags f, bool b)
        {
            if (b) {
                m_renderFlags |= f;
            }
            else {
                m_renderFlags &= ~f;
            }
        }

        void EnableFlag(ToolBarButton::eFlags f, bool b)
        {
            if (b) {
                m_flags |= f;
            }
            else {
                m_flags &= ~f;
            }
        }
    protected:
        ToolBar * m_toolbar;
        wxWindowID m_id;
        int m_bmpidx;
        wxString m_label;
        wxRect m_dropDownArrowRect;
        wxRect m_buttonRect;
        size_t m_flags;
        size_t m_renderFlags;
        wxMenu* m_menu;
    };

    class _gsgsAPI DefaultToolBarButton : public ToolBarButton
    {
    public:
        DefaultToolBarButton(ToolBar* parent, wxWindowID winid, int bmp, const wxString& label = "");
        virtual ~DefaultToolBarButton();

        virtual wxSize CalculateSize(wxDC& dc) const;
    };

    class _gsgsAPI ToolBarMenuButton : public ToolBarButton
    {
    public:
        ToolBarMenuButton(ToolBar* parent, wxWindowID winid, int bmp, const wxString& label = "");
        virtual ~ToolBarMenuButton();

        virtual wxSize CalculateSize(wxDC& dc) const;
    };

    class _gsgsAPI ToolBarSpacer : public ToolBarButton
    {
    public:
        ToolBarSpacer(ToolBar* parent);
        virtual ~ToolBarSpacer();

        virtual wxSize CalculateSize(wxDC& dc) const;

        void Render(wxDC& dc, const wxRect& rect);
    };

    class _gsgsAPI ToolBarStretchSpace : public ToolBarButton
    {
    public:
        ToolBarStretchSpace(ToolBar* parent);
        virtual ~ToolBarStretchSpace();

        virtual wxSize CalculateSize(wxDC& dc) const;
        virtual void Render(wxDC& dc, const wxRect& rect);

        inline void SetWidth(size_t width)  { m_width = width; }
        inline size_t GetWidth() const      { return m_width; }
    private:
        size_t m_width = 0;
    };

    class _gsgsAPI ToolBarToggleButton : public DefaultToolBarButton
    {
    public:
        ToolBarToggleButton(ToolBar* parent, wxWindowID id, int bmp, const wxString& label = "");
        virtual ~ToolBarToggleButton();
    };

    class _gsgsAPI ToolBarSeparator : public ToolBarButton
    {
    public:
        ToolBarSeparator(ToolBar* parent);
        virtual ~ToolBarSeparator();

        void Render(wxDC& dc, const wxRect& rect);
    public:
        virtual wxSize CalculateSize(wxDC& dc) const;
    };

    class _gsgsAPI ToolBarControl : public ToolBarButton
    {
    public:
        ToolBarControl(ToolBar * parent, wxWindow * control);
        virtual ~ToolBarControl();

        void Render(wxDC & dc, const wxRect & rect);

        wxWindow * GetControl() { return m_ctrl; }

        virtual wxSize CalculateSize(wxDC & dc) const;

        void Show(bool b);
    private:
        wxWindow* m_ctrl;
    };

    typedef std::vector<ToolBarButton*> ToolBarButtonList;

    class _gsgsAPI ToolBar : public wxPanel
    {
    public:
        enum eFlags {
            kShowLabels = (1 << 0),
            kThemedColour = (1 << 1),
            kShowCustomiseMenu = (1 << 2),
            kMiniToolBar = (1 << 3),
        };
    public:
        ToolBar(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = "ToolBar");
        virtual ~ToolBar();

        void EnableFlag(eFlags f, bool b)
        {
            if (b) {
                m_flags |= f;
            }
            else {
                m_flags &= ~f;
            }
        }
        bool HasFlag(eFlags flag) const { return m_flags & flag; }

        int GetXSpacer() const;
        int GetYSpacer() const;

        /**
         * @brief set a drop down menu for a button
         */
        void SetDropdownMenu(wxWindowID buttonID, wxMenu* menu);

        /**
         * @brief find the menu for a given button
         */
        wxMenu* FindMenuById(wxWindowID buttonID) const;

        /**
         * @brief return all the buttons
         */
        ToolBarButtonList& GetButtons() { return m_buttons; };
        const ToolBarButtonList& GetButtons() const { return m_buttons; }

        /**
         * @brief display labels next to the bitmap icon
         */
        void ShowLabels(bool show) { EnableFlag(kShowLabels, show); }
        bool IsShowLabels() const { return m_flags & kShowLabels; }
        void EnableCustomisation(bool b) { EnableFlag(kShowCustomiseMenu, b); }
        bool IsCustomisationEnabled() const { return HasFlag(kShowCustomiseMenu); }
        void SetMiniToolBar(bool b) { EnableFlag(kMiniToolBar, b); }
        bool IsMiniToolBar() const { return HasFlag(kMiniToolBar); }
        void SetGroupSpacing(int spacing) { m_groupSpacing = spacing; }
        int GetGroupSpacing() const { return m_groupSpacing; }

        /**
         * @brief add toolbar button
         */
        ToolBarButton* Add(ToolBarButton* button);
        /**
         * @brief insert a button after the button identified by 'otherButton'
         * @param where insert the button after this button.
         * if where can not be found, this function returns null
         */
        ToolBarButton* InsertBefore(wxWindowID where, ToolBarButton* button);
        /**
         * @brief insert a button before the button identified by 'otherButton'
         * @param where insert the button after this button.
         * if where can not be found, this function returns null     */
        ToolBarButton* InsertAfter(wxWindowID where, ToolBarButton* button);

        ToolBarButton* AddButton(wxWindowID id, int bmp, const wxString& label = "");
        ToolBarButton* AddMenuButton(wxWindowID id, int bmp, const wxString& label = "");
        ToolBarButton* AddToggleButton(wxWindowID id, int bmp, const wxString& label = "");
        ToolBarButton* AddSeparator();
        ToolBarButton* AddStretchableSpace();
        ToolBarButton* AddSpacer();

        /**
         * @brief add control to the toolbar
         * @param control
         */
        ToolBarButton* AddControl(wxWindow* control);

        ToolBarButton* AddTool(wxWindowID id, const wxString& label, const wxBitmap& bmp,
            const wxString& helpString = "", wxItemKind kind = wxITEM_NORMAL);

        // Compatibility API with wxToolBar
        ToolBarButton* AddTool(wxWindowID id, const wxString& label, int bmp,
            const wxString& helpString = "", wxItemKind kind = wxITEM_NORMAL)
        {
            wxUnusedVar(helpString);
            switch (kind) {
            case wxITEM_DROPDOWN:
                return AddMenuButton(id, bmp, label);
            case wxITEM_CHECK:
                return AddToggleButton(id, bmp, label);
            case wxITEM_NORMAL:
            default:
                return AddButton(id, bmp, label);
            }
        }

        ToolBarButton* AddToolInter(wxWindowID id, const wxString& label, const wxBitmap& bmp,
            const wxString& helpString = "", wxItemKind kind = wxITEM_NORMAL);

        ToolBarButton* AddTool(wxWindowID id, const wxString& label, const wxBitmap& bitmap,
            const wxBitmap& bmpDisabled, wxItemKind kind = wxITEM_NORMAL,
            const wxString& shortHelp = wxEmptyString, const wxString& longHelp = wxEmptyString,
            wxObject* data = NULL)
        {
            wxUnusedVar(bmpDisabled);
            wxUnusedVar(longHelp);
            wxUnusedVar(data);
            return AddTool(id, label, bitmap, shortHelp, kind);
        }

        ToolBarButton* AddTool(wxWindowID id, const wxString& label, int bitmap,
            int bmpDisabled, wxItemKind kind = wxITEM_NORMAL,
            const wxString& shortHelp = wxEmptyString, const wxString& longHelp = wxEmptyString,
            wxObject* data = NULL)
        {
            wxUnusedVar(bmpDisabled);
            wxUnusedVar(longHelp);
            wxUnusedVar(data);
            return AddTool(id, label, bitmap, shortHelp, kind);
        }

        ToolBarButton* AddToolInter(wxWindowID id, const wxString& label, const wxBitmap& bitmap,
            const wxBitmap& bmpDisabled, wxItemKind kind = wxITEM_NORMAL,
            const wxString& shortHelp = wxEmptyString, const wxString& longHelp = wxEmptyString,
            wxObject* data = NULL)
        {
            wxUnusedVar(bmpDisabled);
            wxUnusedVar(longHelp);
            wxUnusedVar(data);
            return AddToolInter(id, label, bitmap, shortHelp, kind);
        }

        void SetToolBitmapSize(const wxSize& size) { wxUnusedVar(size); }

        void ToggleTool(wxWindowID buttonID, bool toggle);

        /**
         * @brief This function should be called after you have added tools.
         */
        void Realize();

        /**
         * @brief show a drop down menu for a button
         */
        void ShowMenuForButton(wxWindowID buttonID, wxMenu* menu);

        /**
         * @brief display a menu for a button and return the user selected menu item ID
         */
        int GetMenuSelectionFromUser(wxWindowID buttonID, wxMenu* menu);

        /**
         * @brief find a button by ID
         */
        ToolBarButton* FindById(wxWindowID id) const;

        /**
         * @brief delete a button identified by 'id'
         * @param id the button id
         * @return true on success, false otherwise
         */
        bool DeleteById(wxWindowID id);
        // Compatibility API
        bool DeleteTool(wxWindowID id) { return DeleteById(id); }
    protected:
        void OnPaint(wxPaintEvent& event);
        void OnEraseBackground(wxEraseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnMotion(wxMouseEvent& event);
        //void OnSize(wxSizeEvent& event);
        void OnLeaveWindow(wxMouseEvent& event);
        void OnOverflowItem(wxCommandEvent& event);
        virtual void UpdateWindowUI(long flags = wxUPDATE_UI_NONE) override;
        wxRect CalculateRect(wxDC& dc) const;
        void DoShowOverflowMenu();
        void PrepareForDrawings(wxDC& dc, std::vector<ToolBarButtonList>& G, const wxRect& rect);
        void RenderGroup(int& xx, const ToolBarButtonList& G, wxDC& gcdc, bool isLastGroup);
        void OnColoursChanged(gsgs::Event& event);
    private:
        ToolBarButtonList m_buttons;
        ToolBarButtonList m_overflowButtons;
        ToolBarButtonList m_visibleButtons;
        wxRect mDDBRect;
        wxColour m_bgColour;
        size_t m_flags = 0;
        int m_groupSpacing = 30;
        bool m_useCustomBgColour = false;
        bool m_popupShown = false;
    };
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TOOLBAR_CUSTOMISE, wxCommandEvent);
}
#endif
