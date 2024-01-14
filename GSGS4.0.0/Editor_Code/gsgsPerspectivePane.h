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
#ifndef gsgsPERSPECTIVE_PANE_H
#define gsgsPERSPECTIVE_PANE_H

#include "gsgsPreInclude.h"
#include "gsgsCommonHandler.h"
#include "gsgsNotebook.h"
#include "gsgsScrolledPanel.h"
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/propgrid/propgrid.h>

namespace gsgs
{
    class SpacePanel : public ScrolledWindow
    {
    public:
        SpacePanel(wxWindow * parent, const wxString& caption, wxWindowID id = wxID_ANY,
            const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = 0);
        virtual ~SpacePanel();

        bool Create(wxWindow* parent, const wxString& caption, wxWindowID id = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

        wxPropertyGrid * getGrid() const        { return mGrid; }

        void OnSize(wxSizeEvent& event);

        void OnClientSize(wxSizeEvent& event);

        /**
         * @brief scroll to set 'firstLine' as the first visible line in the view
         */
        virtual void ScrollToRow(int firstLine);

        /**
         * @brief scroll to set 'firstColumn' as the first column in the view
         */
        virtual void ScollToColumn(int firstColumn);

        bool SetBackgroundColour(const wxColour& colour);

        const wxString & GetCaption() const     { return m_caption; }
    protected:
        SpacePanel() {}
        virtual void CreateViewImpl();
    protected:
        wxString m_caption;
        wxPropertyGrid * mGrid;
    };

    class MaterialPanel : public SpacePanel
    {
    public:
        MaterialPanel(wxWindow* parent, const wxString& caption);
        ~MaterialPanel();
    protected:
        void CreateViewImpl();
    };

    class EventPanel : public SpacePanel
    {
    public:
        EventPanel(wxWindow* parent, const wxString& caption);
        ~EventPanel();
    protected:
        void CreateViewImpl();
    };

    class PropertyPanel : public SpacePanel
    {
    public:
        PropertyPanel(wxWindow* parent, const wxString& caption);
        ~PropertyPanel();

    protected:
        void CreateViewImpl();
    };

    class PerspectivePane : public wxPanel
    {
    public:
        PerspectivePane(wxWindow * parent, const wxString & caption, wxAuiManager* mgr);
        ~PerspectivePane();

        bool IsTabVisible(int flag);

        const wxString & GetCaption() const             { return m_caption; }
        inline Notebook* GetNotebook() const            { return m_book; }
        inline wxWindow * GetSpacePanel() const         { return m_Pane1; }
        inline wxWindow * GetMaterialPanel() const      { return m_Pane2; }
        inline wxWindow * GetGPUProgramPanel() const    { return m_Pane3; }
        /**
         * @brief set an active tab by its title
         * @param tabTitle the tab to select
         */
        void SelectTab(const wxString& tabTitle);
    protected:
        void CreateViewImpl();

        void DoShowTab(bool show, const wxString& title);
        wxWindow* DoGetControlByName(const wxString& title);
        TabViewPtr_t GetNotebookRenderer();

        void OnInitDone(wxCommandEvent& event);
        void OnSettingsChanged(wxCommandEvent& event);
        void OnTogglePerspectiveTab(Event& event);
        void OnWorkspaceBookFileListMenu(ContextMenuEvent& event);
    protected:
        struct Tab 
        {
            wxString m_label;
            wxWindow * m_window;
            wxBitmap m_bmp;

            Tab(const wxString & label, wxWindow * win, const wxBitmap & bmp = wxNullBitmap)
                : m_label(label), m_window(win), m_bmp(bmp){}

            Tab() : m_window(NULL){}
        };

        std::map<wxString, Tab> m_tabs;
    private:
        wxString m_caption;
        wxAuiManager * m_mgr;
        Notebook * m_book;
        wxWindow * m_Pane1;
        wxWindow * m_Pane2;
        wxWindow * m_Pane3;
        wxWindow * m_Pane4;
        AuiCaptionEnabler m_captionEnabler;
    };
}
#endif
