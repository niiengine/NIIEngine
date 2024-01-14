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
#ifndef gsgs_StatusBar_h_
#define gsgs_StatusBar_h_

#include "gsgsPreInclude.h"
#include "gsgsGotoEntryManager.h"
#include "wxPNGAnimation.h"
#include "gsgsStyleManager.h"
#include <wx/statusbr.h>
#include <wx/colour.h>
#include <wx/bitmap.h>
#include <wx/timer.h>

namespace gsgs
{
    class CustomStatusBar;
    class _gsgsAPI StatusBarView //: public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<StatusBarView> Ptr_t;
    public:
        StatusBarView(const wxString& name);
        virtual ~StatusBarView() {}

        virtual void DrawText(wxDC & dc, wxCoord x, wxCoord y, const wxString & text);
        virtual void DrawFieldSeparator(wxDC& dc, const wxRect& fieldRect);

        virtual wxColour GetBgColour() const                    { return gsgs_Style().GetAppBgColour(); }
        virtual wxColour GetPenColour() const                   { return gsgs_Style().GetColour(wxSYS_COLOUR_3DSHADOW); }
        virtual wxColour GetTextColour() const                  { return gsgs_Style().GetAppTextColour(); }
        virtual wxColour GetSeparatorColour() const             { return gsgs_Style().GetColour(wxSYS_COLOUR_3DSHADOW); }
        void SetName(const wxString& name)                      { m_name = name; }
        const wxString& GetName() const                         { return m_name; }
    protected:
        wxString m_name;
    };

    class _gsgsAPI StatusBarField : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<StatusBarField> Ptr_t;
        typedef std::vector<Ptr_t> Vect_t;

        StatusBarField(CustomStatusBar* parent)
            : m_parent(parent),
            mLeftAlignment(false)
        {
        }
        virtual ~StatusBarField() {}

        /**
         * @brief render the field content
         * @param dc the device content
         * @param rect the field bounding rect
         */
        virtual void Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art) = 0;

        /**
         * @brief return the field length
         */
        virtual size_t GetWidth() const = 0;

        /**
         * @brief return true if this field requires frequest refreshes
         * e.g. animation field needs it
         */
        virtual bool IsRequireFrequentRefresh() const           { return false; }

        /**
         * @brief return true if 'point' is inside the current field
         * @param point position in parent coordinates
         */
        bool HitTest(const wxPoint& point) const;
        inline void SetLeftAlignment(bool set)                  { mLeftAlignment = set; }
        inline bool GetLeftAlignment() const                    { return mLeftAlignment; }
        virtual void SetTooltip(const wxString& tooltip)        { m_tooltip = tooltip; }
        inline const wxString& GetTooltip() const               { return m_tooltip; }
        inline void SetRect(const wxRect& rect)                 { m_rect = rect; }
        inline const wxRect& GetRect() const                    { return m_rect; }
        template <typename T> T* Cast() const                   { return static_cast<T*>(const_cast<StatusBarField*>(this)); }
    protected:
        wxRect m_rect;
        wxString m_tooltip;
        CustomStatusBar* m_parent;
        bool mLeftAlignment;
    };

    class _gsgsAPI StatusBarTextView : public StatusBarField
    {
    public:
        StatusBarTextView(CustomStatusBar* parent, size_t width)
            : StatusBarField(parent)
            , m_width(width)
            , m_textAlign(wxALIGN_CENTER)
        {
        }
        virtual ~StatusBarTextView() {}
        virtual void Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art);
        void SetText(const wxString& text);
        inline const wxString& GetText() const                  { return m_text; }
        inline void SetWidth(size_t width)                      { m_width = width; }
        inline size_t GetWidth() const                          { return m_width; }
        inline void SetTextAlignment(wxAlignment align)         { m_textAlign = align; }
    private:
        wxString m_text;
        size_t m_width;
        wxAlignment m_textAlign;
    };

    class _gsgsAPI StatusBarBitmapView : public StatusBarField
    {
    public:
        StatusBarBitmapView(CustomStatusBar* parent, size_t width)
            : StatusBarField(parent)
            , m_width(width)
        {
        }
        virtual ~StatusBarBitmapView() {}
        virtual void Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art);
        inline void SetWidth(size_t width)                      { m_width = width; }
        inline size_t GetWidth() const                          { return m_width; }
        inline void SetBitmap(const wxBitmap& bitmap)           { m_bitmap = bitmap; }
        inline const wxBitmap& GetBitmap() const                { return m_bitmap; }
    private:
        size_t m_width;
        wxBitmap m_bitmap;
    };

    class _gsgsAPI StatusBarAnimationView : public StatusBarField
    {
    public:
        /**
         * @brief construct animation field.
         */
        StatusBarAnimationView(
            CustomStatusBar* parent, const wxBitmap& sprite, wxOrientation spriteOrientation, const wxSize& animSize);

        virtual ~StatusBarAnimationView();
        virtual void Render(wxDC& dc, const wxRect& rect, StatusBarView::Ptr_t art);
        inline void SetWidth(size_t width)     { m_width = width; }
        inline size_t GetWidth() const         { return m_width; }
        /**
         * @brief start the animation with a given refresh rate (milliseconds)
         */
        void Start(long refreshRate = 50);
        void Stop();
        /**
         * @brief is the animation running?
         */
        inline bool IsRunning() const          { return m_animation->IsRunning(); }

        /**
         * @brief set the tooltip to the animation as well
         */
        virtual void SetTooltip(const wxString& tooltip)
        {
            this->m_tooltip = tooltip;
            if(m_animation) {
                m_animation->SetToolTip(tooltip);
            }
        }
    protected:
        void OnAnimationClicked(wxMouseEvent& event);
    private:
        size_t m_width;
        wxPNGAnimation* m_animation;
    };

    class _gsgsAPI CustomStatusBar : public wxStatusBar
    {
    public:
        CustomStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY, long style = 0);
        virtual ~CustomStatusBar();

        inline void SetArt(StatusBarView::Ptr_t art)            { m_art = art; Refresh(); }

        inline StatusBarView::Ptr_t GetArt()                    { return m_art; }

        inline void AddField(StatusBarField::Ptr_t field)       { m_fields.push_back(field); }


        void SetGlint(const wxColour & c, int msecondsToGlint = 300, int secondsToLive = 5);

        /**
         * @brief set text in the main status bar text area
         */
        void SetText(const wxString& message, int secondsToLive = 5);
        
        /**
         * @brief return the main status bar area text message
         */
        const wxString& GetText() const                         { return m_text; }
        
        /**
         * @brief clear the main text
         */
        void ClearText();
        /**
         * @brief return pointer to the field at given index
         * if index is out of bounds, return NULL
         */
        StatusBarField::Ptr_t GetField(size_t index);

        /**
         * @brief remove status bar field by index
         */
        void RemoveField(size_t index);

        /**
         * @brief return the number of fields
         */
        size_t GetFieldsCount() const                           { return m_fields.size(); }

        /**
         * @brief animation control owned by 'field' was clicked
         */
        void AnimationClicked(StatusBarField* field);

        void SetLastArtNameUsedForPaint(const wxString& art)    { m_lastArtNameUsedForPaint = art; }

        const wxString& GetLastArtNameUsedForPaint() const      { return m_lastArtNameUsedForPaint; }
    protected:
        size_t DoGetFieldsWidth();

        // Event handlers
        void OnPaint(wxPaintEvent& event);
        void OnEraseBackround(wxEraseEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnMouseMotion(wxMouseEvent& event);
        void OnTimer(wxTimerEvent& event);
        void OnGlint(wxTimerEvent& event);
        wxRect DoGetMainFieldRect();
    protected:
        StatusBarView::Ptr_t m_art;
        StatusBarField::Vect_t m_fields;
        StatusBarField::Ptr_t m_mainText;
        wxColour m_glintColour;
        wxColour m_CurrentColour;
        wxString m_text;
        wxString m_lastArtNameUsedForPaint;
        wxTimer* m_timer;
        wxTimer* m_glinttimer;
        bool mAutoStopGlint;
    };
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_STATUSBAR_CLICKED, Event);
    
    class _gsgsAPI StatusBar : public CustomStatusBar
    {
    public:
        StatusBar(wxWindow* parent, IManager* mgr);
        virtual ~StatusBar();

        /**
         * @brief clear all text fields from the status bar
         */
        void Clear();

        /**
         * @brief set a status bar message
         */
        void SetMessage(const wxString& message, int secondsToLive = 5) { SetText(message, secondsToLive); }

        /**
         * @brief set the whitespace information (Tabs vs Spaces)
         */
        void SetWhitespaceInfo();

        /**
         * @brief clear the whitespace info fields
         */
        void ClearWhitespaceInfo();

        /**
         * @brief update the language field
         */
        void SetState(const wxString& state);

        /**
         * @brief update the language field
         */
        void SetLanguage(const wxString& lang);

        /**
         * @brief Update the encoding field
         */
        void SetEncoding(const wxString& enc);

        /**
         * @brief update the line / column / pos field
         */
        void SetLinePosColumn(const wxString& lineCol);

        /**
         * @brief start the animation
         * @param refreshRate refresh rate in milliseconds
         * @param tooltip set the tooltip for this field
         */
        void StartAnimation(long refreshRate, const wxString& tooltip);

        /**
         * @brief stop the animation
         */
        void StopAnimation();

        /**
         * @brief set a bitmap (16x16) in the source control section
         * and optionally, provide an output tab name to toggle when the bitmap is clicked
         */
        void SetSourceControlBitmap(const wxBitmap& bmp, const wxString& outputTabName, const wxString& tooltip);

    protected:
        void OnPageChanged(wxCommandEvent& event);
        void OnTheme(wxCommandEvent& event);
        void OnAllEditorsClosed(wxCommandEvent& event);
        void OnBuildStarted(BuildEvent& event);
        void OnBuildEnded(BuildEvent& event);
        void OnWorkspaceClosed(wxCommandEvent& event);
        void OnFieldClicked(Event& event);
        void OnEditorSettingsChanged(wxCommandEvent& event);
        void OnGotoAnythingShowing(GotoEvent& e);
        void OnActionSelected(GotoEvent& e);
        void DoUpdateColour();
        void DoSetLinePosColumn(const wxString& message);

        void SetBuildBitmap(const wxBitmap& bmp, const wxString& tooltip);

        void DoUpdateView();
        void DoFieldClicked(int fieldIndex);
        int GetTextWidth(const wxString& text) const;

    private:
        IManager* m_mgr;
        wxBitmap m_bmpBuildError;
        wxBitmap m_bmpBuildWarnings;
        wxBitmap m_bmpSourceControl;
        wxString m_sourceControlTabName;
        gsgsStringMap m_gotoAnythingTableThemes;
        gsgsStringMap m_gotoAnythingTableSyntax;
    };
}
#endif