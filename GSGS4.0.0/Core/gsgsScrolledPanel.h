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
#ifndef gsgsSCROLLEDPANEL_H
#define gsgsSCROLLEDPANEL_H

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include <wx/bitmap.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/scrolbar.h>
#include <wx/scrolwin.h>
#include <wx/dialog.h>
#include <wx/srchctrl.h>
#include <wx/dataview.h>
#include <wx/treebase.h>

namespace gsgs
{
    class Notebook;
    /// a wxCommandEvent that takes ownership of the clientData
    class _gsgsAPI ScrollEvent : public wxCommandEvent
    {
    public:
        ScrollEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        ScrollEvent(const ScrollEvent& event);
        ScrollEvent& operator=(const ScrollEvent& src);
        virtual ~ScrollEvent();
        
        virtual wxEvent* Clone() const;
        
        inline void SetPosition(int pos)    { m_position = pos; }
        inline int GetPosition() const      { return m_position; }
    protected:
        int m_position = wxNOT_FOUND;
    };

    typedef void (wxEvtHandler::*clScrollEventFunction)(ScrollEvent&);
    #define clScrollEventHandler(func) wxEVENT_HANDLER_CAST(clScrollEventFunction, func)
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_CUSTOM_SCROLL, ScrollEvent);

    #if 1 // defined(__WXMSW__)||defined(__WXGTK__)
    #define USE_NATIVE_SCROLLBAR 0
    #define CL_USE_CUSTOM_SCROLLBAR 1
    #else
    #define USE_NATIVE_SCROLLBAR 1
    #define CL_USE_CUSTOM_SCROLLBAR 0
    #endif

    class _gsgsAPI ScrollBar : public wxScrollBar
    {
    public:
        ScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
        virtual ~ScrollBar();

        void SetScrollbar(int position, int thumbSize, int range, int pageSize, bool notify = true);
        inline bool ShouldShow() const      { return (m_thumb_size < m_range_size); }
        /**
         * @brief can we scroll up or left?
         */
        inline bool CanScrollUp() const     { return GetThumbPosition() > 0; }
        /**
         * @brief can we scroll down or right?
         */
        inline bool CanScollDown() const    { return (GetThumbPosition() + GetThumbSize()) < GetRange(); }
        
        void SetPosition(int pos, bool)     { SetThumbPosition(pos); }

        void SetColours(const Colour&) {}
        void SetColour(const wxColour&) {}
    protected:
        int m_thumb_size = 0;
        int m_range_size = 0;
    };
    enum CornerType
    {
        CT_LT = 0x01,
        CT_RT = 0x02,
        CT_LB = 0x04,
        CT_RB = 0x08
    };
    class _gsgsAPI CustomScrollBar : public wxPanel
    {
    public:
        CustomScrollBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxSB_VERTICAL);
        virtual ~CustomScrollBar();

        bool ShouldShow() const             { return ((m_thumbSize > 0) && (m_thumbSize < m_range)); }
        bool CanScollDown() const           { return (GetThumbPosition() + GetThumbSize()) < GetRange(); }

        void SetAlwaysShow(bool set)        { mAlwaysShow = set; }
        bool IsAlwaysShow() const           { return mAlwaysShow; }
        void SetPosition(int pos, bool notify = true);
        void UpdateScroll(int thumbSize, int range, int pageSize, int position, bool refesh = false);

        bool IsVertical() const             { return m_sbStyle & wxSB_VERTICAL; }
        bool IsHorizontal() const           { return m_sbStyle & wxSB_HORIZONTAL; }

        /// Accessors
        void SetPageSize(int pageSize)      { m_pageSize = pageSize; }
        int GetPageSize() const             { return m_pageSize; }
        
        void SetRange(int range)            { m_range = range; }
        int GetRange() const                { return m_range; }
        
        void SetThumbPosition(int pos)      { m_thumbPosition = pos; }
        int GetThumbPosition() const        { return m_thumbPosition; }
        
        void SetThumbSize(int thumbSize)    { this->m_thumbSize = thumbSize; }
        int GetThumbSize() const            { return m_thumbSize; }

        void SetThumbPendingPosition(int thumbSize)     { mThumbPendingPos = thumbSize; }
        int GetThumbPendingPosition() const             { return mThumbPendingPos; }

        /// Override's parent
        virtual void SetColours(const Colour& colours);
        virtual void SetColour(const wxColour& colours);
        virtual void SetConrnerColour(int cornermark, const wxColour& colours);
    protected:
        void OnIdle(wxIdleEvent& event);
        void OnPaint(wxPaintEvent& e);
        void OnMouseLeftDown(wxMouseEvent& e);
        void OnMouseLeftUp(wxMouseEvent& e);
        void OnMouseCaptureLost(wxMouseCaptureLostEvent & e);
        void OnMotion(wxMouseEvent& e);
        void UpdateDrag(const wxPoint& pt);
        int GetPositionFromPoint(const wxPoint& pt) const;
    private:
        float m_thumbSize = 0.0;
        float m_pageSize = 0.0;
        float m_range = 0.0;
        float m_thumbPosition = 0.0;
        int mThumbPendingPos = 0;
        long m_sbStyle = wxSB_VERTICAL;
        int mCornerTypeMark;
        wxRect m_thumbRect;
        wxPoint m_mouseCapturePoint;
        wxPoint m_thumbCapturePoint;
        Colour m_colours;
        wxColour mCornerColour;
        bool mDragging = false;
        bool mClick = false;
        bool mNotifyContent = false;
        bool mAlwaysShow = false;
        bool mCornerSet = false;
    };

    #if USE_NATIVE_SCROLLBAR
    typedef ScrollBar ScrollBar_t;
    #else
    typedef CustomScrollBar ScrollBar_t;
    #endif

    class _gsgsAPI ScrolledWindow : public wxWindow
    {
    public:
        ScrolledWindow();
        ScrolledWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);
        virtual ~ScrolledWindow();

        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr);

        ScrollBar_t* GetHScrollBar() const          { return m_hsb; }
        ScrollBar_t* GetVScrollBar() const          { return m_vsb; }

        virtual void setClient(wxWindow * obj, bool sizeEvent = true);

        wxWindow * getClient() const                { return mClient; }

        void SetScrollRate(int xrate, int yrate)    { mScrollRateX = xrate; mScrollRateY = yrate; }

        void SetHColour(const wxColour& clr)        { m_hsb->SetColour(clr); }
        void SetVColour(const wxColour& clr)        { m_vsb->SetColour(clr); }
        void SetColour(const wxColour& clr)         { m_vsb->SetColour(clr); m_hsb->SetColour(clr); SetBackgroundColour(clr); }
        void SetConrnerColour(const wxColour& clr);
        void SetHAlwaysShow(bool set)               { m_hsb->SetAlwaysShow(set); }
        bool IsHAlwaysShow()const                   { return m_hsb->IsAlwaysShow(); }
        void SetVAlwaysShow(bool set)               { m_vsb->SetAlwaysShow(set); }
        bool IsVAlwaysShow()const                   { return m_vsb->IsAlwaysShow(); }
        void SetHThumbPosition(int pos, bool notify = true)     { m_hsb->SetPosition(pos, notify); }
        int GetHThumbPosition()const                { return m_hsb->GetThumbPosition(); }
        void SetVThumbPosition(int pos, bool notify = true)     {  m_vsb->SetPosition(pos, notify); }
        int GetVThumbPosition()const                { return m_vsb->GetThumbPosition(); }
        void SetHThumbPendingPosition(int pos)      { return m_hsb->SetThumbPendingPosition(pos); }
        int GetHThumbPendingPosition()const         { return m_hsb->GetThumbPendingPosition(); }
        void SetVThumbPendingPosition(int pos)      { return m_vsb->SetThumbPendingPosition(pos); }
        int GetVThumbPendingPosition()const         { return m_vsb->GetThumbPendingPosition(); }

        void SetScrollEvent(bool set)               { mScrollEvent = set; }
        bool GetScrollEvent() const                 { return mScrollEvent; }
        /**
         * @brief return the system default font
         */
        static const wxFont & GetDefaultFont()      { return gsgs_Style().GetDefaultGuiFont(); }
        static const wxFont & GetDefaultActiveFont(){ return gsgs_Style().GetDefaultActiveGuiFont(); }
        /**
         * @brief when enabled, the scrollbar will only be shown (if needed at all) when this window has the focus (or any
         * of its decendants)
         */
        void SetShowScrollBarOnFocus(bool b)        { m_showSBOnFocus = b; }
        wxDC& GetTempDC() const                     { return *m_gcdc; }

        /**
         * @brief return the number lines can fit into the page (vertically)
         */
        int GetPageSize() const;

        /**
         * @brief return the client area, taking scrollbars into consideration
         */
        wxRect GetClientArea() const;

        /**
         * @brief return the client area, taking scrollbars into consideration
         */
        void GetClientAreaSize(int * width, int * height) const;

        /**
         * @brief whenver the view changes (i.e. there is a new top line) call this method so the scrollbar
         * will adjust its position
         */
        void UpdateVScrollBar(int position, int thumbSize, int rangeSize, int pageSize, bool clientsize = false, bool notify = false);
        void UpdateHScrollBar(int position, int thumbSize, int rangeSize, int pageSize, bool clientsize = false, bool notify = false);

        //===----------------------------------------------------
        // Overridables
        //===----------------------------------------------------

        /**
         * @brief override this method to scroll the view
         * @param steps number of lines to scroll. If 'steps' is set to 0, then scroll to top or bottom of the view
         * depending on the direction. otherwise, scroll 'steps' into the correct 'direction'
         * @param direction direction to scroll
         */
        virtual void ScrollRows(int steps, wxDirection direction)
        {
            wxUnusedVar(steps);
            wxUnusedVar(direction);
        }

        /**
         * @brief override this method to scroll the view
         * @param steps number of columns to scroll. If 'steps' is set to 0, then scroll to right or left of the view
         * depending on the direction. otherwise, scroll 'steps' into the correct 'direction'
         * @param direction direction to scroll
         */
        virtual void ScrollColumns(int steps, wxDirection direction)
        {
            wxUnusedVar(steps);
            wxUnusedVar(direction);
        }

        /**
         * @brief scroll to set 'firstLine' as the first visible line in the view
         */
        virtual void ScrollToRow(int firstLine)         { wxUnusedVar(firstLine); }

        /**
         * @brief scroll to set 'firstColumn' as the first column in the view
         */
        virtual void ScollToColumn(int firstColumn)     { wxUnusedVar(firstColumn); }

        /**
         * @brief called by the scrolled window whenver a key is down
         * return true if the key was processed and we should stop the processing of this event
         */
        virtual bool DoKeyDown(const wxKeyEvent& event)
        {
            wxUnusedVar(event);
            return false;
        }

        // Process idle events. Override this in the subclass
        virtual void ProcessIdle() {}
    protected:
    #if USE_NATIVE_SCROLLBAR
        virtual void OnVScroll(wxScrollEvent& event);
        virtual void OnHScroll(wxScrollEvent& event);
    #else
        // custom scrollbar events
        virtual void OnVCustomScroll(ScrollEvent& event);
        virtual void OnHCustomScroll(ScrollEvent& event);
    #endif
        virtual void OnMouseWheel(wxMouseEvent& event);
        virtual void OnCharHook(wxKeyEvent& event);
        virtual void OnIdle(wxIdleEvent& event);
        virtual void OnSize(wxSizeEvent& event);
        virtual void OnClientSize(wxSizeEvent& event);
        virtual void OnLeftDown(wxMouseEvent& event);
        virtual void OnLeftUp(wxMouseEvent& event);
        virtual void OnMotion(wxMouseEvent& event);
        virtual void OnLeaveWindow(wxMouseEvent& event);
        void DoBeginDrag();
        void DoCancelDrag();
        virtual void DoPositionVScrollbar();
        virtual void DoPositionHScrollbar();
    protected:
        bool ShouldShowScrollBar() const;
        void DoInitialize();

        /**
         * @brief return true row from a position
         */
        virtual wxTreeItemId GetRow(const wxPoint& pt) const { return wxTreeItemId(); }
    protected:
        ScrollBar_t* m_vsb = nullptr;
        ScrollBar_t* m_hsb = nullptr;
        wxWindow * mClient;
        int m_pageSize = 0;
        int m_position = 0;
        int m_thumbSize = 0;
        int m_rangeSize = 0;
        int m_wheelRotation = 0;
        int mScrollRateX = 1;
        int mScrollRateY = 1;
        wxBitmap m_tmpBmp;
        wxMemoryDC* m_memDC = nullptr;
        wxGCDC* m_gcdc = nullptr;
        wxDateTime m_dragStartTime;
        wxPoint m_dragStartPos;
        wxColour mCornerColour;
        bool mClientSizeEvent = false;
        bool m_showSBOnFocus = false;
        bool mDragging = false;
        bool mMutexSizeDisable = true;
        bool mCornerSet = false;
        bool mHsbNeedShow = false;
        bool mVsbNeedShow = false;
        bool mScrollEvent = false;
    };

    class _gsgsAPI DefaultScrolledWindow : public ScrolledWindow
    {
    public:
        DefaultScrolledWindow();
        DefaultScrolledWindow(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxPanelNameStr);

        /// copydetails ScrolledWindow::ScrollToRow
        virtual void ScrollToRow(int firstLine);

        /// @copydetails ScrolledWindow::ScollToColumn
        virtual void ScollToColumn(int firstColumn);

        /// @copydetails ScrolledWindow::OnSize
        virtual void OnSize(wxSizeEvent& event);

        /// @copydetails ScrolledWindow::OnClientSize
        virtual void OnClientSize(wxSizeEvent& event);
    private:
        bool mMuteDisable;
    };

    class _gsgsAPI SingleChoiceDialog : public wxDialog
    {
    public:
        SingleChoiceDialog();
        SingleChoiceDialog(wxWindow* parent);
        virtual ~SingleChoiceDialog();

        void initItem(const wxArrayString& options, const WindowList & optionPanels, int selection = 0);
        void initItem(const wxArrayString& options, int selection = 0);
        bool Create(wxWindow* parent);
        wxString GetSelection() const;

        void SetNotebookSelection(int selete);
        int GetNotebookSelection() const;

        wxSearchCtrl* GetSearchCtrl() const         { return m_searchCtrl; }
        Notebook* GetListCtrl() const               { return mNotebook; }

        Button* GetOkButton() const                 { return mOkButton; }
        Button* GetCancelButton() const             { return mCancelButton; }
    protected:
        void DoInitialise();

    protected:
        virtual void OnFilter(wxCommandEvent& event);
        virtual void OnActivated(wxDataViewEvent& event);
        virtual void OnItemActivated(wxCommandEvent& event);
        virtual void OnOKUI(wxUpdateUIEvent& event);
    protected:
        wxArrayString m_options;
        WindowList m_optionPanels;
        wxSearchCtrl* m_searchCtrl;
        Notebook* mNotebook;
        DialogButtonSizer* m_stdBtnSizer183;
        Button* mOkButton;
        Button* mCancelButton;
    };

    class _gsgsAPI ListChoiceDialog : public wxDialog
    {
    public:
        ListChoiceDialog();
        ListChoiceDialog(wxWindow* parent, const wxArrayString& options, const WindowList & optionPanels, int initialSelection = 0);
        virtual ~ListChoiceDialog();

        bool Create(wxWindow* parent, const wxArrayString& options, const WindowList & optionPanels, int initialSelection = 0);
        wxString GetSelection() const;

        wxSearchCtrl* GetSearchCtrl()       const   { return m_searchCtrl; }
        wxDataViewListCtrl* GetListCtrl()   const   { return m_dvListCtrl; }
    protected:
        void DoInitialise();

    protected:
        virtual void OnFilter(wxCommandEvent& event);
        virtual void OnActivated(wxDataViewEvent& event);
        virtual void OnItemActivated(wxCommandEvent& event);
        virtual void OnOKUI(wxUpdateUIEvent& event);
    protected:
        wxArrayString m_options;
        WindowList m_optionPanels;
        wxSearchCtrl* m_searchCtrl;
        wxDataViewListCtrl* m_dvListCtrl;
        DialogButtonSizer* m_stdBtnSizer183;
        Button* mOkButton;
        Button* mCancelButton;
    };
}
#endif
