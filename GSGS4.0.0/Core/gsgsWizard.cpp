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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "gsgsWizard.h"
#include "wx/dynarray.h"
#include "wx/intl.h"
#include "wx/statbmp.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/sizer.h"

#include "wx/statline.h"

#include "wx/scrolwin.h"
#include "wx/wizard.h"
#include "wx/dcmemory.h"

namespace gsgs
{
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // WizardSizer
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    class WizardSizer : public wxSizer
    {
    public:
        WizardSizer(Wizard *owner);

        virtual wxSizerItem *Insert(size_t index, wxSizerItem *item);

        virtual void RecalcSizes();
        virtual wxSize CalcMin();

        // get the max size of all wizard pages
        wxSize GetMaxChildSize();

        // return the border which can be either set using Wizard::SetBorder() or
        // have default value
        int GetBorder() const;

        // hide the pages which we temporarily "show" when they're added to this
        // sizer (see Insert())
        void HidePages();

    private:
        wxSize SiblingSize(wxSizerItem *child);

        Wizard *m_owner;
        wxSize m_childSize;
    };

    // ----------------------------------------------------------------------------
    // event tables and such
    // ----------------------------------------------------------------------------

    wxDEFINE_EVENT(wxEVT_WIZARD_PAGE_CHANGED, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_PAGE_CHANGING, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_BEFORE_PAGE_CHANGED, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_CANCEL, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_FINISHED, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_HELP, WizardEvent);
    wxDEFINE_EVENT(wxEVT_WIZARD_PAGE_SHOWN, WizardEvent);
    // ----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(Wizard, wxDialog)
        EVT_BUTTON(wxID_CANCEL, Wizard::OnCancel)
        EVT_BUTTON(wxID_BACKWARD, Wizard::OnBackOrNext)
        EVT_BUTTON(wxID_FORWARD, Wizard::OnBackOrNext)
        EVT_BUTTON(wxID_HELP, Wizard::OnHelp)

        EVT_WIZARD_PAGE_CHANGED(wxID_ANY, Wizard::OnWizEvent)
        EVT_WIZARD_PAGE_CHANGING(wxID_ANY, Wizard::OnWizEvent)
        EVT_WIZARD_CANCEL(wxID_ANY, Wizard::OnWizEvent)
        EVT_WIZARD_FINISHED(wxID_ANY, Wizard::OnWizEvent)
        EVT_WIZARD_HELP(wxID_ANY, Wizard::OnWizEvent)
    END_EVENT_TABLE()

        IMPLEMENT_DYNAMIC_CLASS(Wizard, wxDialog)

        /*
            TODO PROPERTIES :
            Wizard
                extstyle
                title
        */

        IMPLEMENT_ABSTRACT_CLASS(WizardPage, wxPanel)
        IMPLEMENT_DYNAMIC_CLASS(WizardPageSimple, WizardPage)
        IMPLEMENT_DYNAMIC_CLASS(WizardEvent, wxNotifyEvent)

        // ============================================================================
        // implementation
        // ============================================================================
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // WizardPage
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    void WizardPage::Init()
    {
        m_bitmap = wxNullBitmap;
    }
    // ----------------------------------------------------------------------------
    WizardPage::WizardPage(Wizard *parent,
        const wxBitmap& bitmap)
    {
        Create(parent, bitmap);
    }
    // ----------------------------------------------------------------------------
    bool WizardPage::Create(Wizard *parent,
        const wxBitmap& bitmap)
    {
        if (!wxPanel::Create(parent, wxID_ANY))
            return false;

        m_bitmap = bitmap;

        // initially the page is hidden, it's shown only when it becomes current
        Hide();

        return true;
    }
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // WizardPageSimple
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    WizardPage *WizardPageSimple::GetPrev() const
    {
        return m_prev;
    }
    // ----------------------------------------------------------------------------
    WizardPage *WizardPageSimple::GetNext() const
    {
        return m_next;
    }
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // WizardSizer
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    WizardSizer::WizardSizer(Wizard *owner)
        : m_owner(owner),
        m_childSize(wxDefaultSize)
    {
    }
    // ----------------------------------------------------------------------------
    wxSizerItem *WizardSizer::Insert(size_t index, wxSizerItem *item)
    {
        m_owner->m_usingSizer = true;

        if (item->IsWindow())
        {
            // we must pretend that the window is shown as otherwise it wouldn't be
            // taken into account for the layout -- but avoid really showing it, so
            // just set the internal flag instead of calling wxWindow::Show()
            item->GetWindow()->wxWindowBase::Show();
        }

        return wxSizer::Insert(index, item);
    }
    // ----------------------------------------------------------------------------
    void WizardSizer::HidePages()
    {
        for (wxSizerItemList::compatibility_iterator node = GetChildren().GetFirst();
            node;
            node = node->GetNext())
        {
            wxSizerItem * const item = node->GetData();
            if (item->IsWindow())
                item->GetWindow()->wxWindowBase::Show(false);
        }
    }
    // ----------------------------------------------------------------------------
    void WizardSizer::RecalcSizes()
    {
        // Effect of this function depends on m_owner->m_page and
        // it should be called whenever it changes (Wizard::ShowPage)
        if (m_owner->m_page)
        {
            m_owner->m_page->SetSize(wxRect(m_position, m_size));
        }
    }
    // ----------------------------------------------------------------------------
    wxSize WizardSizer::CalcMin()
    {
        return m_owner->GetPageSize();
    }
    // ----------------------------------------------------------------------------
    wxSize WizardSizer::GetMaxChildSize()
    {
        wxSize maxOfMin;

        for (wxSizerItemList::compatibility_iterator childNode = m_children.GetFirst();
            childNode;
            childNode = childNode->GetNext())
        {
            wxSizerItem *child = childNode->GetData();
            maxOfMin.IncTo(child->CalcMin());
            maxOfMin.IncTo(SiblingSize(child));
        }

        if (m_owner->m_started)
        {
            m_childSize = maxOfMin;
        }

        return maxOfMin;
    }
    // ----------------------------------------------------------------------------
    int WizardSizer::GetBorder() const
    {
        return m_owner->m_border;
    }
    // ----------------------------------------------------------------------------
    wxSize WizardSizer::SiblingSize(wxSizerItem *child)
    {
        wxSize maxSibling;

        if (child->IsWindow())
        {
            WizardPage *page = wxDynamicCast(child->GetWindow(), WizardPage);
            if (page)
            {
                for (WizardPage *sibling = page->GetNext();
                    sibling;
                    sibling = sibling->GetNext())
                {
                    if (sibling->GetSizer())
                    {
                        maxSibling.IncTo(sibling->GetSizer()->CalcMin());
                    }
                }
            }
        }

        return maxSibling;
    }
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // generic Wizard implementation
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    void Wizard::Init()
    {
        m_posWizard = wxDefaultPosition;
        m_page = NULL;
        m_btnPrev = m_btnNext = NULL;
        m_statbmp = NULL;
        m_sizerBmpAndPage = NULL;
        m_sizerPage = NULL;
        m_border = 5;
        m_started = false;
        m_wasModal = false;
        m_usingSizer = false;
        m_bitmapBackgroundColour = *wxWHITE;
        m_bitmapPlacement = 0;
        m_bitmapMinimumWidth = 115;
    }
    // ----------------------------------------------------------------------------
    bool Wizard::Create(wxWindow *parent,
        int id,
        const wxString& title,
        const wxBitmap& bitmap,
        const wxPoint& pos,
        long style)
    {
        bool result = wxDialog::Create(parent, id, title, pos, wxDefaultSize, style);

        m_posWizard = pos;
        m_bitmap = bitmap;

        DoCreateControls();

        return result;
    }
    // ----------------------------------------------------------------------------
    Wizard::~Wizard()
    {
        // normally we don't have to delete this sizer as it's deleted by the
        // associated window but if we never used it or didn't set it as the window
        // sizer yet, do delete it manually
        if (!m_usingSizer || !m_started)
            delete m_sizerPage;
    }
    // ----------------------------------------------------------------------------
    void Wizard::AddBitmapRow(wxBoxSizer *mainColumn)
    {
        m_sizerBmpAndPage = new wxBoxSizer(wxHORIZONTAL);
        mainColumn->Add(
            m_sizerBmpAndPage,
            1, // Vertically stretchable
            wxEXPAND // Horizontal stretching, no border
        );
        mainColumn->Add(0, 5,
            0, // No vertical stretching
            wxEXPAND // No border, (mostly useless) horizontal stretching
        );

#if wxUSE_STATBMP
        if (m_bitmap.IsOk())
        {
            wxSize bitmapSize(wxDefaultSize);
            if (GetBitmapPlacement())
                bitmapSize.x = GetMinimumBitmapWidth();

            m_statbmp = new wxStaticBitmap(this, wxID_ANY, m_bitmap, wxDefaultPosition, bitmapSize);
            m_sizerBmpAndPage->Add(
                m_statbmp,
                0, // No horizontal stretching
                wxALL, // Border all around, top alignment
                5 // Border width
            );
            m_sizerBmpAndPage->Add(
                5, 0,
                0, // No horizontal stretching
                wxEXPAND // No border, (mostly useless) vertical stretching
            );
        }
#endif

        // Added to m_sizerBmpAndPage later
        m_sizerPage = new WizardSizer(this);
    }
    // ----------------------------------------------------------------------------
    void Wizard::AddStaticLine(wxBoxSizer *mainColumn)
    {
#if wxUSE_STATLINE
        mainColumn->Add(
            new wxStaticLine(this, wxID_ANY),
            0, // Vertically unstretchable
            wxEXPAND | wxALL, // Border all around, horizontally stretchable
            5 // Border width
        );
        mainColumn->Add(0, 5,
            0, // No vertical stretching
            wxEXPAND // No border, (mostly useless) horizontal stretching
        );
#else
        (void)mainColumn;
#endif // wxUSE_STATLINE
    }
    // ----------------------------------------------------------------------------
    void Wizard::AddBackNextPair(wxBoxSizer *buttonRow)
    {
        wxASSERT_MSG(m_btnNext && m_btnPrev,
            wxT("You must create the buttons before calling ")
            wxT("Wizard::AddBackNextPair"));

        wxBoxSizer *backNextPair = new wxBoxSizer(wxHORIZONTAL);
        buttonRow->Add(
            backNextPair,
            0, // No horizontal stretching
            wxALL, // Border all around
            5 // Border width
        );

        backNextPair->Add(m_btnPrev);
        backNextPair->Add(10, 0,
            0, // No horizontal stretching
            wxEXPAND // No border, (mostly useless) vertical stretching
        );
        backNextPair->Add(m_btnNext);
    }
    // ----------------------------------------------------------------------------
    void Wizard::AddButtonRow(wxBoxSizer *mainColumn)
    {
        // the order in which the buttons are created determines the TAB order - at least under MSWindows...
        // although the 'back' button appears before the 'next' button, a more userfriendly tab order is
        // to activate the 'next' button first (create the next button before the back button).
        // The reason is: The user will repeatedly enter information in the wizard pages and then wants to
        // press 'next'. If a user uses mostly the keyboard, he would have to skip the 'back' button
        // every time. This is annoying. There is a second reason: RETURN acts as TAB. If the 'next'
        // button comes first in the TAB order, the user can enter information very fast using the RETURN
        // key to TAB to the next entry field and page. This would not be possible, if the 'back' button
        // was created before the 'next' button.

        bool isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
        int buttonStyle = isPda ? wxBU_EXACTFIT : 0;

        wxBoxSizer *buttonRow = new wxBoxSizer(wxHORIZONTAL);
#ifdef __WXMAC__
        if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
            mainColumn->Add(
                buttonRow,
                0, // Vertically unstretchable
                wxGROW | wxALIGN_CENTRE
            );
        else
#endif
            mainColumn->Add(
                buttonRow,
                0, // Vertically unstretchable
                wxALIGN_RIGHT // Right aligned, no border
            );

        // Desired TAB order is 'next', 'cancel', 'help', 'back'. This makes the 'back' button the last control on the page.
        // Create the buttons in the right order...
        Button *btnHelp = 0;
#ifdef __WXMAC__
        if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
            btnHelp = new Button(this, wxID_HELP, wxEmptyString, wxDefaultPosition, wxDefaultSize, buttonStyle);
#endif

        m_btnNext = new Button(this, wxID_FORWARD, _("&Next >"));
        Button *btnCancel = new Button(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, buttonStyle);
#ifndef __WXMAC__
        if (GetExtraStyle() & wxWIZARD_EX_HELPBUTTON)
            btnHelp = new Button(this, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, buttonStyle);
#endif
        m_btnPrev = new Button(this, wxID_BACKWARD, _("< &Back"), wxDefaultPosition, wxDefaultSize, buttonStyle);

        if (btnHelp)
        {
            buttonRow->Add(
                btnHelp,
                0, // Horizontally unstretchable
                wxALL, // Border all around, top aligned
                5 // Border width
            );
#ifdef __WXMAC__
            // Put stretchable space between help button and others
            buttonRow->Add(0, 0, 1, wxALIGN_CENTRE, 0);
#endif
        }

        AddBackNextPair(buttonRow);

        buttonRow->Add(
            btnCancel,
            0, // Horizontally unstretchable
            wxALL, // Border all around, top aligned
            5 // Border width
        );
    }
    // ----------------------------------------------------------------------------
    void Wizard::DoCreateControls()
    {
        // do nothing if the controls were already created
        if (WasCreated())
            return;

        bool isPda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

        // Horizontal stretching, and if not PDA, border all around
        int mainColumnSizerFlags = isPda ? wxEXPAND : wxALL | wxEXPAND;

        // wxWindow::SetSizer will be called at end
        wxBoxSizer *windowSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer *mainColumn = new wxBoxSizer(wxVERTICAL);
        windowSizer->Add(
            mainColumn,
            1, // Vertical stretching
            mainColumnSizerFlags,
            5 // Border width
        );

        AddBitmapRow(mainColumn);

        //if (!isPda)
        //    AddStaticLine(mainColumn);

        AddButtonRow(mainColumn);

        SetSizer(windowSizer);
    }
    // ----------------------------------------------------------------------------
    void Wizard::SetPageSize(const wxSize& size)
    {
        wxCHECK_RET(!m_started, wxT("Wizard::SetPageSize after RunWizard"));
        m_sizePage = size;
    }
    // ----------------------------------------------------------------------------
    void Wizard::FitToPage(const WizardPage *page)
    {
        wxCHECK_RET(!m_started, wxT("Wizard::FitToPage after RunWizard"));

        while (page)
        {
            wxSize size = page->GetBestSize();

            m_sizePage.IncTo(size);

            page = page->GetNext();
        }
    }
    // ----------------------------------------------------------------------------
    bool Wizard::ShowPage(WizardPage *page, bool goingForward)
    {
        wxASSERT_MSG(page != m_page, wxT("this is useless"));

        wxSizerFlags flags(1);
        flags.Border(wxALL, m_border).Expand();

        if (!m_started)
        {
            if (m_usingSizer)
            {
                m_sizerBmpAndPage->Add(m_sizerPage, flags);

                // now that our layout is computed correctly, hide the pages
                // artificially shown in WizardSizer::Insert() back again
                m_sizerPage->HidePages();
            }
        }


        // remember the old bitmap (if any) to compare with the new one later
        wxBitmap bmpPrev;

        // check for previous page
        if (m_page)
        {
            // send the event to the old page
            WizardEvent event(wxEVT_WIZARD_PAGE_CHANGING, GetId(),
                goingForward, m_page);
            if (m_page->GetEventHandler()->ProcessEvent(event) &&
                !event.IsAllowed())
            {
                // vetoed by the page
                return false;
            }

            m_page->Hide();

            bmpPrev = m_page->GetBitmap();

            if (!m_usingSizer)
                m_sizerBmpAndPage->Detach(m_page);
        }

        // is this the end?
        if (!page)
        {
            // terminate successfully
            if (IsModal())
            {
                EndModal(wxID_OK);
            }
            else
            {
                SetReturnCode(wxID_OK);
                Hide();
            }

            // and notify the user code (this is especially useful for modeless
            // wizards)
            WizardEvent event(wxEVT_WIZARD_FINISHED, GetId(), false, m_page);
            (void)GetEventHandler()->ProcessEvent(event);

            m_page = NULL;

            return true;
        }

        // notice that we change m_page only here so that wxEVT_WIZARD_FINISHED
        // event above could still use the correct (i.e. old) value of m_page
        m_page = page;

        // position and show the new page
        (void)m_page->TransferDataToWindow();

        if (m_usingSizer)
        {
            // WizardSizer::RecalcSizes wants to be called when m_page changes
            m_sizerPage->RecalcSizes();
        }
        else // pages are not managed by the sizer
        {
            m_sizerBmpAndPage->Add(m_page, flags);
            m_sizerBmpAndPage->SetItemMinSize(m_page, GetPageSize());
        }

#if wxUSE_STATBMP
        // update the bitmap if:it changed
        wxBitmap bmp;
        if (m_statbmp)
        {
            bmp = m_page->GetBitmap();
            if (!bmp.IsOk())
                bmp = m_bitmap;

            if (!bmpPrev.IsOk())
                bmpPrev = m_bitmap;

            if (!GetBitmapPlacement())
            {
                if (!bmp.IsSameAs(bmpPrev))
                    m_statbmp->SetBitmap(bmp);
            }
        }
#endif // wxUSE_STATBMP


        // and update the buttons state
        m_btnPrev->Enable(HasPrevPage(m_page));

        const bool hasNext = HasNextPage(m_page);
        const wxString label = hasNext ? _("&Next >") : _("&Finish");
        if (label != m_btnNext->GetLabel())
            m_btnNext->SetLabel(label);

        m_btnNext->SetDefault();


        // send the change event to the new page now
        WizardEvent event(wxEVT_WIZARD_PAGE_CHANGED, GetId(), goingForward, m_page);
        (void)m_page->GetEventHandler()->ProcessEvent(event);

        // and finally show it
        m_page->Show();
        m_page->SetFocus();

        if (!m_usingSizer)
            m_sizerBmpAndPage->Layout();

        if (!m_started)
        {
            m_started = true;

            DoWizardLayout();
        }

        if (GetBitmapPlacement() && m_statbmp)
        {
            ResizeBitmap(bmp);

            if (!bmp.IsSameAs(bmpPrev))
                m_statbmp->SetBitmap(bmp);

            if (m_usingSizer)
                m_sizerPage->RecalcSizes();
        }

        WizardEvent pageShownEvent(wxEVT_WIZARD_PAGE_SHOWN, GetId(),
            goingForward, m_page);
        m_page->GetEventHandler()->ProcessEvent(pageShownEvent);

        return true;
    }
    // ----------------------------------------------------------------------------
    /// Do fit, and adjust to screen size if necessary
    void Wizard::DoWizardLayout()
    {
        if (wxSystemSettings::GetScreenType() > wxSYS_SCREEN_PDA)
        {
            if (CanDoLayoutAdaptation())
                DoLayoutAdaptation();
            else
                GetSizer()->SetSizeHints(this);

            if (m_posWizard == wxDefaultPosition)
                CentreOnScreen();
        }

        SetLayoutAdaptationDone(true);
    }
    // ----------------------------------------------------------------------------
    bool Wizard::RunWizard(WizardPage *firstPage)
    {
        wxCHECK_MSG(firstPage, false, wxT("can't run empty wizard"));

        // can't return false here because there is no old page
        (void)ShowPage(firstPage, true /* forward */);

        m_wasModal = true;

        return ShowModal() == wxID_OK;
    }
    // ----------------------------------------------------------------------------
    WizardPage *Wizard::GetCurrentPage() const
    {
        return m_page;
    }
    // ----------------------------------------------------------------------------
    wxSize Wizard::GetPageSize() const
    {
        // default width and height of the page
        int DEFAULT_PAGE_WIDTH,
            DEFAULT_PAGE_HEIGHT;
        if (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA)
        {
            // Make the default page size small enough to fit on screen
            DEFAULT_PAGE_WIDTH = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) / 2;
            DEFAULT_PAGE_HEIGHT = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) / 2;
        }
        else // !PDA
        {
            DEFAULT_PAGE_WIDTH =
                DEFAULT_PAGE_HEIGHT = 270;
        }

        // start with default minimal size
        wxSize pageSize(DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT);

        // make the page at least as big as specified by user
        pageSize.IncTo(m_sizePage);

        if (m_statbmp)
        {
            // make the page at least as tall as the bitmap
            pageSize.IncTo(wxSize(0, m_bitmap.GetHeight()));
        }

        if (m_usingSizer)
        {
            // make it big enough to contain all pages added to the sizer
            pageSize.IncTo(m_sizerPage->GetMaxChildSize());
        }

        return pageSize;
    }
    // ----------------------------------------------------------------------------
    wxSizer *Wizard::GetPageAreaSizer() const
    {
        return m_sizerPage;
    }
    // ----------------------------------------------------------------------------
    void Wizard::SetBorder(int border)
    {
        wxCHECK_RET(!m_started, wxT("Wizard::SetBorder after RunWizard"));

        m_border = border;
    }
    // ----------------------------------------------------------------------------
    void Wizard::OnCancel(wxCommandEvent& WXUNUSED(eventUnused))
    {
        // this function probably can never be called when we don't have an active
        // page, but a small extra check won't hurt
        wxWindow *win = m_page ? (wxWindow *)m_page : (wxWindow *)this;

        WizardEvent event(wxEVT_WIZARD_CANCEL, GetId(), false, m_page);
        if (!win->GetEventHandler()->ProcessEvent(event) || event.IsAllowed())
        {
            // no objections - close the dialog
            if (IsModal())
            {
                EndModal(wxID_CANCEL);
            }
            else
            {
                SetReturnCode(wxID_CANCEL);
                Hide();
            }
        }
        //else: request to Cancel ignored
    }
    // ----------------------------------------------------------------------------
    void Wizard::OnBackOrNext(wxCommandEvent& event)
    {
        wxASSERT_MSG((event.GetEventObject() == m_btnNext) ||
            (event.GetEventObject() == m_btnPrev),
            wxT("unknown button"));

        wxCHECK_RET(m_page, wxT("should have a valid current page"));

        // ask the current page first: notice that we do it before calling
        // GetNext/Prev() because the data transferred from the controls of the page
        // may change the value returned by these methods
        if (!m_page->Validate() || !m_page->TransferDataFromWindow())
        {
            // the page data is incorrect, don't do anything
            return;
        }

        bool forward = event.GetEventObject() == m_btnNext;

        // Give the application a chance to set state which may influence GetNext()/GetPrev()
        WizardEvent eventPreChanged(wxEVT_WIZARD_BEFORE_PAGE_CHANGED, GetId(), forward, m_page);
        (void)m_page->GetEventHandler()->ProcessEvent(eventPreChanged);

        if (!eventPreChanged.IsAllowed())
            return;

        WizardPage *page;
        if (forward)
        {
            page = m_page->GetNext();
        }
        else // back
        {
            page = m_page->GetPrev();

            wxASSERT_MSG(page, wxT("\"<Back\" button should have been disabled"));
        }

        // just pass to the new page (or maybe not - but we don't care here)
        (void)ShowPage(page, forward);
    }
    // ----------------------------------------------------------------------------
    void Wizard::OnHelp(wxCommandEvent& WXUNUSED(event))
    {
        // this function probably can never be called when we don't have an active
        // page, but a small extra check won't hurt
        if (m_page != NULL)
        {
            // Create and send the help event to the specific page handler
            // event data contains the active page so that context-sensitive
            // help is possible
            WizardEvent eventHelp(wxEVT_WIZARD_HELP, GetId(), true, m_page);
            (void)m_page->GetEventHandler()->ProcessEvent(eventHelp);
        }
    }
    // ----------------------------------------------------------------------------
    void Wizard::OnWizEvent(WizardEvent& event)
    {
        // the dialogs have wxWS_EX_BLOCK_EVENTS style on by default but we want to
        // propagate wxEVT_WIZARD_XXX to the parent (if any), so do it manually
        if (!(GetExtraStyle() & wxWS_EX_BLOCK_EVENTS))
        {
            // the event will be propagated anyhow
            event.Skip();
        }
        else
        {
            wxWindow *parent = GetParent();

            if (!parent || !parent->GetEventHandler()->ProcessEvent(event))
            {
                event.Skip();
            }
        }

        if ((!m_wasModal) &&
            event.IsAllowed() &&
            (event.GetEventType() == wxEVT_WIZARD_FINISHED ||
                event.GetEventType() == wxEVT_WIZARD_CANCEL
                )
            )
        {
            Destroy();
        }
    }
    // ----------------------------------------------------------------------------
    void Wizard::SetBitmap(const wxBitmap& bitmap)
    {
        m_bitmap = bitmap;
        if (m_statbmp)
            m_statbmp->SetBitmap(m_bitmap);
    }
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    // WizardEvent
    // ----------------------------------------------------------------------------
    // ----------------------------------------------------------------------------
    WizardEvent::WizardEvent(wxEventType type, int id, bool direction, WizardPage* page)
        : wxNotifyEvent(type, id)
    {
        // Modified 10-20-2001 Robert Cavanaugh
        // add the active page to the event data
        m_direction = direction;
        m_page = page;
    }
    // ----------------------------------------------------------------------------
    /// Do the adaptation
    bool Wizard::DoLayoutAdaptation()
    {
        wxWindowList windows;
        wxWindowList pages;

        // Make all the pages (that use sizers) scrollable
        for (wxSizerItemList::compatibility_iterator node = m_sizerPage->GetChildren().GetFirst(); node; node = node->GetNext())
        {
            wxSizerItem * const item = node->GetData();
            if (item->IsWindow())
            {
                WizardPage* page = wxDynamicCast(item->GetWindow(), WizardPage);
                if (page)
                {
                    while (page)
                    {
                        if (!pages.Find(page) && page->GetSizer())
                        {
                            // Create a scrolled window and reparent
                            wxScrolledWindow* scrolledWindow = new wxScrolledWindow(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxVSCROLL | wxHSCROLL | wxBORDER_NONE);
                            wxSizer* oldSizer = page->GetSizer();

                            wxSizer* newSizer = new wxBoxSizer(wxVERTICAL);
                            newSizer->Add(scrolledWindow, 1, wxEXPAND, 0);

                            page->SetSizer(newSizer, false /* don't delete the old sizer */);

                            scrolledWindow->SetSizer(oldSizer);

                            wxStandardDialogLayoutAdapter::DoReparentControls(page, scrolledWindow);

                            pages.Append(page);
                            windows.Append(scrolledWindow);
                        }
                        page = page->GetNext();
                    }
                }
            }
        }

        wxStandardDialogLayoutAdapter::DoFitWithScrolling(this, windows);

        // Size event doesn't get sent soon enough on wxGTK
        DoLayout();

        SetLayoutAdaptationDone(true);

        return true;
    }
    // ----------------------------------------------------------------------------
    bool Wizard::ResizeBitmap(wxBitmap& bmp)
    {
        if (!GetBitmapPlacement())
            return false;

        if (bmp.IsOk())
        {
            wxSize pageSize = m_sizerPage->GetSize();
            if (pageSize == wxSize(0, 0))
                pageSize = GetPageSize();
            int bitmapWidth = wxMax(bmp.GetWidth(), GetMinimumBitmapWidth());
            int bitmapHeight = pageSize.y;

            if (!m_statbmp->GetBitmap().IsOk() || m_statbmp->GetBitmap().GetHeight() != bitmapHeight)
            {
                wxBitmap bitmap(bitmapWidth, bitmapHeight);
                {
                    wxMemoryDC dc;
                    dc.SelectObject(bitmap);
                    dc.SetBackground(wxBrush(m_bitmapBackgroundColour));
                    dc.Clear();

                    if (GetBitmapPlacement() & wxWIZARD_TILE)
                    {
                        TileBitmap(wxRect(0, 0, bitmapWidth, bitmapHeight), dc, bmp);
                    }
                    else
                    {
                        int x, y;

                        if (GetBitmapPlacement() & wxWIZARD_HALIGN_LEFT)
                            x = 0;
                        else if (GetBitmapPlacement() & wxWIZARD_HALIGN_RIGHT)
                            x = bitmapWidth - bmp.GetWidth();
                        else
                            x = (bitmapWidth - bmp.GetWidth()) / 2;

                        if (GetBitmapPlacement() & wxWIZARD_VALIGN_TOP)
                            y = 0;
                        else if (GetBitmapPlacement() & wxWIZARD_VALIGN_BOTTOM)
                            y = bitmapHeight - bmp.GetHeight();
                        else
                            y = (bitmapHeight - bmp.GetHeight()) / 2;

                        dc.DrawBitmap(bmp, x, y, true);
                        dc.SelectObject(wxNullBitmap);
                    }
                }

                bmp = bitmap;
            }
        }

        return true;
    }
    // ----------------------------------------------------------------------------
    bool Wizard::TileBitmap(const wxRect& rect, wxDC& dc, const wxBitmap& bitmap)
    {
        int w = bitmap.GetWidth();
        int h = bitmap.GetHeight();

        wxMemoryDC dcMem;

        dcMem.SelectObjectAsSource(bitmap);

        int i, j;
        for (i = rect.x; i < rect.x + rect.width; i += w)
        {
            for (j = rect.y; j < rect.y + rect.height; j += h)
                dc.Blit(i, j, bitmap.GetWidth(), bitmap.GetHeight(), &dcMem, 0, 0);
        }
        dcMem.SelectObject(wxNullBitmap);

        return true;
    }
    // ----------------------------------------------------------------------------
}
