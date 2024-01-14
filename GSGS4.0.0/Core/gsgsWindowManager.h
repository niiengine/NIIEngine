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
#ifndef GSGS_WINDOWMANAGER_H_
#define GSGS_WINDOWMANAGER_H_

#include "gsgsPreInclude.h"
#include "imanager.h"
#include <wx/frame.h>
#include <wx/accel.h>
#include <wx/menu.h>
#include <wx/toplevel.h>
#include <wx/aui/aui.h>
#include <wx/dc.h>

namespace gsgs
{
    class _gsgsAPI DockablePane : public wxPanel
    {
    public:
        DockablePane(wxWindow * parent, Notebook * book, const wxString & title,
            bool initFloat = true, const wxBitmap & bmp = wxNullBitmap, const wxSize & size = wxDefaultSize);
        virtual ~DockablePane();

        inline Notebook * GetBook() const { return m_book; }

        wxString GetName() const { return m_text; }

        void SetChildNoReparent(wxWindow * child);
    protected:
        //DECLARE_EVENT_TABLE();
        void ClosePane(wxCommandEvent& e);

        //void OnEraseBg(wxEraseEvent&);
        //void OnPaint(wxPaintEvent& e);
    private:
        wxWindow* m_child;
        Notebook* m_book;
        wxString m_text;
        wxBitmap m_bmp;
        bool m_notifiedDestroyed;
    };

    class _gsgsAPI DetachedPanesInfo : public ObjectData
    {
    public:
        DetachedPanesInfo(wxArrayString arr);
        DetachedPanesInfo() {}
        virtual ~DetachedPanesInfo();

        virtual void DeSerialize(Serializer &arch);
        virtual void Serialize(Serializer &arch);

        inline void SetPanes(const wxArrayString & panes)       { m_panes = panes;}
        inline const wxArrayString & GetPanes() const           { return m_panes; }
    protected:
        wxArrayString m_panes;
    };

    class _gsgsAPI DockArt : public wxEvtHandler, public wxAuiDefaultDockArt
    {
    public:
        DockArt(IManager* manager);
        virtual ~DockArt();

        virtual void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane);
        virtual void DrawCaption(wxDC& dc, wxWindow* window, const wxString& text, const wxRect& rect, wxAuiPaneInfo& pane);

        virtual void DrawPaneButton(wxDC& dc, wxWindow* window, int button, int button_state, const wxRect & _rect, wxAuiPaneInfo & pane);
        virtual void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect);

        virtual void DrawSash(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect);

        void OnSettingsChanged(wxCommandEvent& event);
    private:
        IManager * m_manager;
        wxColour m_captionColour;
        wxColour m_captionTextColour;
        wxColour m_bgColour;
        bool m_useCustomCaptionColour;
        bool m_useCustomBgColour;
    };

    class _gsgsAPI WindowManager : public wxAuiManager
    {
    public:
        struct _gsgsAPI HotKey
        {
            wxString parentMenu;
            wxString resourceID;
            wxString accel;
            wxString action;

            struct ClearParentMenu {
                void operator()(std::pair<const int, HotKey>& iter) { iter.second.parentMenu.Clear(); }
            };

            struct PrependPrefix {
                wxString m_prefix;
                PrependPrefix(const wxString& prefix)
                    : m_prefix(prefix)
                {
                }
                void operator()(std::pair<const int, HotKey>& iter) { iter.second.action.Prepend(m_prefix); }
            };
        };

        typedef std::unordered_map<wxString, HotKey> HotKeyList;
        typedef std::unordered_map<int, HotKey> HotKeyMap;

        typedef std::list<wxFrame*> FrameList_t;
    public:
        WindowManager();
        virtual ~WindowManager();

        static void Release();
        static WindowManager& singleton();

        void SetMainFrame(wxFrame * frame)      { mMainFrame = frame; }
        wxFrame * GetMainFrame() const          { return mMainFrame; }
        void Save();

        void RegisterDockWindow(int menuItemId, const wxString& name);

        int showAddHotKeyDlg(wxWindow * parent, HotKey & data);

        void AddMenu(const wxString & name);

        void RemoveMenu(const wxString & name);

        void getFloatingList(wxArrayString & out);

        bool IsPaneDetached(const wxString & name);

        /**
         * @brief add keyboard shortcut by specifying the action ID + the shortcut combination
         * For example: AddAccelerator("wxID_COPY", "Ctrl-Shift-C", "Copy the current selection");
         * @return true if the action succeeded, false otherwise
         */
        void addHotKey(const wxString& resourceID, const wxString& accel, const wxString& description);

        bool isHotKeyExist(const wxString & accel) const;

        void SetAccelerators(const HotKeyList & accels);

        void getHotKeyList(HotKeyList& ls) const;

        void getFreeHotKeyList(wxArrayString & ls) const;

        void clearAllHotKey();

        void UpdateAccels(wxFrame* frame = NULL);

        static void HackShowPane(wxAuiPaneInfo & info, wxAuiManager * mag);

        static void HackHidePane(bool commit, wxAuiPaneInfo & info, wxAuiManager * mag);

        static void Load(wxTopLevelWindow * win);
    protected:
        void updateImpl();

        void OnDockpaneMenuItem(wxCommandEvent & e);
        void OnDockpaneMenuItemUI(wxUpdateUIEvent & e);
        void DoGetFrames(wxFrame * parent, WindowManager::FrameList_t & frames);
        void DoUpdateMenu(wxMenu * menu, HotKeyMap & accels, std::vector<wxAcceleratorEntry> & table);
        void DoUpdateFrame(wxFrame * frame, HotKeyMap & accels);
        void DoConvertToIntMap(const HotKeyList & strMap, HotKeyMap & intMap);
        HotKeyList DoLoadDefaultAccelerators();

        void OnStartupCompleted(wxCommandEvent & event);

        void OnRender(wxAuiManagerEvent& event);
        void OnButtonClicked(wxAuiManagerEvent& event);
        void OnAuiPaneActivated(wxAuiManagerEvent& e);
        void OnViewPane(wxCommandEvent& event);
        void OnViewPaneUI(wxUpdateUIEvent& event);

        void ShowWorkspaceOpenTabMenu();
        void ShowOutputViewOpenTabMenu();
        void ShowPerspectiveViewOpenTabMenu();

        void ViewPane(const wxString& paneName, bool checked);
        /**
         * @brief show a popup menu and return the selected string
         * return an empty string if no selection was made
         */
        wxString ShowMenu(wxWindow * win, const wxArrayString & tabs, Notebook * book, bool & checked);

        static void DoLoad(wxWindow * win, const wxString & parentName, int depth);
    private:
        std::map<int, wxString> m_id2nameMap;
        std::map<int, wxString> m_panes;
        wxFrame * mMainFrame;
        HotKeyList m_menuTable;
        HotKeyList m_globalTable;
        gsgsStringSet m_keyCodes;
        gsgsStringSet m_allShorcuts;
    };
}
#define gsgs_Window() gsgs::WindowManager::singleton()
#endif
