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

#ifndef _gsgs_WIZARD_H_
#define _gsgs_WIZARD_H_

#include "gsgsPreInclude.h"
#include "gsgsButton.h"
#include <wx/wizard.h>

namespace gsgs
{
    // forward declarations
    class Wizard;

    // ----------------------------------------------------------------------------
    // WizardPage is one of the wizards screen: it must know what are the
    // following and preceding pages (which may be NULL for the first/last page).
    //
    // Other than GetNext/Prev() functions, WizardPage is just a panel and may be
    // used as such (i.e. controls may be placed directly on it &c).
    // ----------------------------------------------------------------------------

    class _gsgsAPI WizardPage : public wxPanel
    {
    public:
        WizardPage() { Init(); }

            // ctor accepts an optional bitmap which will be used for this page instead
            // of the default one for this wizard (should be of the same size). Notice
            // that no other parameters are needed because the wizard will resize and
            // reposition the page anyhow
        WizardPage(Wizard *parent, const wxBitmap& bitmap = wxNullBitmap);

        bool Create(Wizard *parent, const wxBitmap& bitmap = wxNullBitmap);

        // these functions are used by the wizard to show another page when the
        // user chooses "Back" or "Next" button
        virtual WizardPage *GetPrev() const = 0;
        virtual WizardPage *GetNext() const = 0;

        // default GetBitmap() will just return m_bitmap which is ok in 99% of
        // cases - override this method if you want to create the bitmap to be used
        // dynamically or to do something even more fancy. It's ok to return
        // wxNullBitmap from here - the default one will be used then.
        virtual wxBitmap GetBitmap() const { return m_bitmap; }

    #if wxUSE_VALIDATORS
        // Override the base functions to allow a validator to be assigned to this page.
        virtual bool TransferDataToWindow()
        {
            return GetValidator() ? GetValidator()->TransferToWindow()
                : wxPanel::TransferDataToWindow();
        }

        virtual bool TransferDataFromWindow()
        {
            return GetValidator() ? GetValidator()->TransferFromWindow()
                : wxPanel::TransferDataFromWindow();
        }

        virtual bool Validate()
        {
            return GetValidator() ? GetValidator()->Validate(this) : wxPanel::Validate();
        }
    #endif // wxUSE_VALIDATORS

    protected:
        // common part of ctors:
        void Init();

        wxBitmap m_bitmap;

    private:
        DECLARE_DYNAMIC_CLASS_NO_COPY(WizardPage)
    };

        // ----------------------------------------------------------------------------
        // WizardPageSimple just returns the pointers given to the ctor and is useful
        // to create a simple wizard where the order of pages never changes.
        //
        // OTOH, it is also possible to dynamically decide which page to return (i.e.
        // depending on the user's choices) as the wizard sample shows - in order to do
        // this, you must derive from WizardPage directly.
        // ----------------------------------------------------------------------------

    class _gsgsAPI WizardPageSimple : public WizardPage
    {
    public:
        WizardPageSimple() { Init(); }

        // ctor takes the previous and next pages
        WizardPageSimple(Wizard *parent,
                WizardPage *prev = NULL,
                WizardPage *next = NULL,
                const wxBitmap& bitmap = wxNullBitmap)
        {
            Create(parent, prev, next, bitmap);
        }

        bool Create(Wizard *parent = NULL, // let it be default ctor too
                WizardPage *prev = NULL,
                WizardPage *next = NULL,
                const wxBitmap& bitmap = wxNullBitmap)
        {
            m_prev = prev;
            m_next = next;
            return WizardPage::Create(parent, bitmap);
        }

            // the pointers may be also set later - but before starting the wizard
        void SetPrev(WizardPage *prev) { m_prev = prev; }
        void SetNext(WizardPage *next) { m_next = next; }

            // Convenience functions to make the pages follow each other without having
            // to call their SetPrev() or SetNext() explicitly.
        WizardPageSimple& Chain(WizardPageSimple* next)
        {
            SetNext(next);
            next->SetPrev(this);
            return *next;
        }

        static void Chain(WizardPageSimple *first, WizardPageSimple *second)
        {
            wxCHECK_RET(first && second, wxT("NULL passed to WizardPageSimple::Chain"));

            first->SetNext(second);
            second->SetPrev(first);
        }

            // base class pure virtuals
        virtual WizardPage *GetPrev() const;
        virtual WizardPage *GetNext() const;

    private:
            // common part of ctors:
        void Init()
        {
            m_prev = m_next = NULL;
        }

            // pointers are private, the derived classes shouldn't mess with them -
            // just derive from WizardPage directly to implement different behaviour
        WizardPage *m_prev,
            *m_next;

        DECLARE_DYNAMIC_CLASS_NO_COPY(WizardPageSimple)
    };

        // ----------------------------------------------------------------------------
        // Wizard
        // ----------------------------------------------------------------------------

    class _gsgsAPI WizardBase : public wxDialog
    {
    public:
            /*
               The derived class (i.e. the real Wizard) has a ctor and Create()
               function taking the following arguments:

                Wizard(wxWindow *parent,
                         int id = wxID_ANY,
                         const wxString& title = wxEmptyString,
                         const wxBitmap& bitmap = wxNullBitmap,
                         const wxPoint& pos = wxDefaultPosition,
                         long style = wxDEFAULT_DIALOG_STYLE);
            */
        WizardBase() { }

            // executes the wizard starting from the given page, returns true if it was
            // successfully finished, false if user cancelled it
        virtual bool RunWizard(WizardPage *firstPage) = 0;

            // get the current page (NULL if RunWizard() isn't running)
        virtual WizardPage *GetCurrentPage() const = 0;

            // set the min size which should be available for the pages: a
            // wizard will take into account the size of the bitmap (if any)
            // itself and will never be less than some predefined fixed size
        virtual void SetPageSize(const wxSize& size) = 0;

            // get the size available for the page
        virtual wxSize GetPageSize() const = 0;

            // set the best size for the wizard, i.e. make it big enough to contain all
            // of the pages starting from the given one
            //
            // this function may be called several times and possible with different
            // pages in which case it will only increase the page size if needed (this
            // may be useful if not all pages are accessible from the first one by
            // default)
        virtual void FitToPage(const WizardPage *firstPage) = 0;

            // Adding pages to page area sizer enlarges wizard
        virtual wxSizer *GetPageAreaSizer() const = 0;

            // Set border around page area. Default is 0 if you add at least one
            // page to GetPageAreaSizer and 5 if you don't.
        virtual void SetBorder(int border) = 0;

            // the methods below may be overridden by the derived classes to provide
            // custom logic for determining the pages order

        virtual bool HasNextPage(WizardPage *page)
        {
                return page->GetNext() != NULL;
        }

        virtual bool HasPrevPage(WizardPage *page)
        {
                return page->GetPrev() != NULL;
        }

            /// Override these functions to stop InitDialog from calling TransferDataToWindow
            /// for _all_ pages when the wizard starts. Instead 'ShowPage' will call
            /// TransferDataToWindow for the first page only.
        bool TransferDataToWindow() { return true; }
        bool TransferDataFromWindow() { return true; }
        bool Validate() { return true; }

    private:
        wxDECLARE_NO_COPY_CLASS(WizardBase);
    };

    // ----------------------------------------------------------------------------
    // WizardEvent class represents an event generated by the wizard: this event
    // is first sent to the page itself and, if not processed there, goes up the
    // window hierarchy as usual
    // ----------------------------------------------------------------------------

    class _gsgsAPI WizardEvent : public wxNotifyEvent
    {
    public:
        WizardEvent(wxEventType type = wxEVT_NULL,
            int id = wxID_ANY,
            bool direction = true,
            WizardPage* page = NULL);

            // for EVT_WIZARD_PAGE_CHANGING, return true if we're going forward or
            // false otherwise and for EVT_WIZARD_PAGE_CHANGED return true if we came
            // from the previous page and false if we returned from the next one
            // (this function doesn't make sense for CANCEL events)
        bool GetDirection() const { return m_direction; }

        WizardPage*   GetPage() const { return m_page; }

        virtual wxEvent *Clone() const { return new WizardEvent(*this); }

    private:
        bool m_direction;
        WizardPage*    m_page;

        DECLARE_DYNAMIC_CLASS_NO_ASSIGN(WizardEvent)
    };

    // ----------------------------------------------------------------------------
    // macros for handling wxWizardEvents
    // ----------------------------------------------------------------------------

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_PAGE_CHANGED, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_PAGE_CHANGING, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_CANCEL, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_HELP, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_FINISHED, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_PAGE_SHOWN, WizardEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_WIZARD_BEFORE_PAGE_CHANGED, WizardEvent);

    typedef void (wxEvtHandler::*WizardEventFunction)(WizardEvent&);

    #define WizardEventHandler(func) \
        wxEVENT_HANDLER_CAST(WizardEventFunction, func)

    #define wx__DECLARE_WIZARDEVT(evt, id, fn) \
        wx__DECLARE_EVT1(wxEVT_WIZARD_ ## evt, id, WizardEventHandler(fn))

        // notifies that the page has just been changed (can't be vetoed)
    #define EVT_WIZARD_PAGE_CHANGED(id, fn) wx__DECLARE_WIZARDEVT(PAGE_CHANGED, id, fn)

    // the user pressed "<Back" or "Next>" button and the page is going to be
    // changed - unless the event handler vetoes the event
    #define EVT_WIZARD_PAGE_CHANGING(id, fn) wx__DECLARE_WIZARDEVT(PAGE_CHANGING, id, fn)

    // Called before GetNext/GetPrev is called, so that the handler can change state that will be
    // used when GetNext/GetPrev is called. PAGE_CHANGING is called too late to influence GetNext/GetPrev.
    #define EVT_WIZARD_BEFORE_PAGE_CHANGED(id, fn) wx__DECLARE_WIZARDEVT(BEFORE_PAGE_CHANGED, id, fn)

    // the user pressed "Cancel" button and the wizard is going to be dismissed -
    // unless the event handler vetoes the event
    #define EVT_WIZARD_CANCEL(id, fn) wx__DECLARE_WIZARDEVT(CANCEL, id, fn)

    // the user pressed "Finish" button and the wizard is going to be dismissed -
    #define EVT_WIZARD_FINISHED(id, fn) wx__DECLARE_WIZARDEVT(FINISHED, id, fn)

    // the user pressed "Help" button
    #define EVT_WIZARD_HELP(id, fn) wx__DECLARE_WIZARDEVT(HELP, id, fn)

    // the page was just shown and laid out
    #define EVT_WIZARD_PAGE_SHOWN(id, fn) wx__DECLARE_WIZARDEVT(PAGE_SHOWN, id, fn)

    class WizardSizer;

    class _gsgsAPI Wizard : public WizardBase
    {
    public:
        // ctor
        Wizard() { Init(); }
        Wizard(wxWindow *parent,
                int id = wxID_ANY,
                const wxString& title = wxEmptyString,
                const wxBitmap& bitmap = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                long style = wxDEFAULT_DIALOG_STYLE)
        {
            Init();
            Create(parent, id, title, bitmap, pos, style);
        }
        bool Create(wxWindow *parent,
                int id = wxID_ANY,
                const wxString& title = wxEmptyString,
                const wxBitmap& bitmap = wxNullBitmap,
                const wxPoint& pos = wxDefaultPosition,
                long style = wxDEFAULT_DIALOG_STYLE);
        void Init();
        virtual ~Wizard();

            // implement base class pure virtuals
        virtual bool RunWizard(WizardPage *firstPage);
        virtual WizardPage *GetCurrentPage() const;
        virtual void SetPageSize(const wxSize& size);
        virtual wxSize GetPageSize() const;
        virtual void FitToPage(const WizardPage *firstPage);
        virtual wxSizer *GetPageAreaSizer() const;
        virtual void SetBorder(int border);

        /// set/get bitmap
        const wxBitmap& GetBitmap() const { return m_bitmap; }
        void SetBitmap(const wxBitmap& bitmap);

        // implementation only from now on
        // -------------------------------

        // is the wizard running?
        bool IsRunning() const { return m_page != NULL; }

        // show the prev/next page, but call TransferDataFromWindow on the current
        // page first and return false without changing the page if
        // TransferDataFromWindow() returns false - otherwise, returns true
        virtual bool ShowPage(WizardPage *page, bool goingForward = true);

        // do fill the dialog with controls
        // this is app-overridable to, for example, set help and tooltip text
        virtual void DoCreateControls();

        // Do the adaptation
        virtual bool DoLayoutAdaptation();

        // Set/get bitmap background colour
        void SetBitmapBackgroundColour(const wxColour& colour) { m_bitmapBackgroundColour = colour; }
        const wxColour& GetBitmapBackgroundColour() const { return m_bitmapBackgroundColour; }

        // Set/get bitmap placement (centred, tiled etc.)
        void SetBitmapPlacement(int placement) { m_bitmapPlacement = placement; }
        int GetBitmapPlacement() const { return m_bitmapPlacement; }

        // Set/get minimum bitmap width
        void SetMinimumBitmapWidth(int w) { m_bitmapMinimumWidth = w; }
        int GetMinimumBitmapWidth() const { return m_bitmapMinimumWidth; }

        // Tile bitmap
        static bool TileBitmap(const wxRect& rect, wxDC& dc, const wxBitmap& bitmap);
    protected:
        // for compatibility only, doesn't do anything any more
        void FinishLayout() { }

        // Do fit, and adjust to screen size if necessary
        virtual void DoWizardLayout();

        // Resize bitmap if necessary
        virtual bool ResizeBitmap(wxBitmap& bmp);

        // was the dialog really created?
        bool WasCreated() const { return m_btnPrev != NULL; }

        // event handlers
        void OnCancel(wxCommandEvent& event);
        void OnBackOrNext(wxCommandEvent& event);
        void OnHelp(wxCommandEvent& event);

        void OnWizEvent(WizardEvent& event);

        void AddBitmapRow(wxBoxSizer *mainColumn);
        void AddStaticLine(wxBoxSizer *mainColumn);
        void AddBackNextPair(wxBoxSizer *buttonRow);
        void AddButtonRow(wxBoxSizer *mainColumn);

        // the page size requested by user
        wxSize m_sizePage;

        // the dialog position from the ctor
        wxPoint m_posWizard;

        // wizard state
        WizardPage *m_page;       // the current page or NULL
        wxBitmap      m_bitmap;     // the default bitmap to show

        // wizard controls
        Button    *m_btnPrev,     // the "<Back" button
                *m_btnNext;     // the "Next>" or "Finish" button
        wxStaticBitmap *m_statbmp;  // the control for the bitmap

        // Border around page area sizer requested using SetBorder()
        int m_border;

        // Whether RunWizard() was called
        bool m_started;

        // Whether was modal (modeless has to be destroyed on finish or cancel)
        bool m_wasModal;

        // True if pages are laid out using the sizer
        bool m_usingSizer;

        // Page area sizer will be inserted here with padding
        wxBoxSizer *m_sizerBmpAndPage;

        // Actual position and size of pages
        WizardSizer *m_sizerPage;

        // Bitmap background colour if resizing bitmap
        wxColour    m_bitmapBackgroundColour;

        // Bitmap placement flags
        int         m_bitmapPlacement;

        // Minimum bitmap width
        int         m_bitmapMinimumWidth;

        friend class WizardSizer;

        DECLARE_DYNAMIC_CLASS(Wizard)
        DECLARE_EVENT_TABLE()
        wxDECLARE_NO_COPY_CLASS(Wizard);
    };
}
#endif // _WX_GENERIC_WIZARD_H_
